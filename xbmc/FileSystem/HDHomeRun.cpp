/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "stdafx.h"
#include "Util.h"
#include "utils/URIUtils.h"
#include "URL.h"
#include "FileItem.h"
#include "HDHomeRun.h"
#include "utils/log.h"

using namespace XFILE;
using namespace XFILE;
using namespace std;

class CUrlOptions 
  : public map<CStdString, CStdString>
{  
public:
  CUrlOptions(const CStdString& data)
  {    
    vector<CStdString> options;    
    CUtil::Tokenize(data, options, "&");
    for(vector<CStdString>::iterator it = options.begin();it != options.end(); it++)
    {
      CStdString name, value;
      unsigned int pos = it->find_first_of('=');
      if(pos != CStdString::npos)
      {
        name = it->substr(0, pos);
        value = it->substr(pos+1);
      }
      else
      {
        name = *it;
        value = "";
      }

      CURL::Decode(name);
      CURL::Decode(value);
      insert(value_type(name, value));
    }
  }  
};


// -------------------------------------------
// ---------------- Directory ----------------
// -------------------------------------------

CDirectoryHomeRun::CDirectoryHomeRun()
{
  m_dll.Load();
}

CDirectoryHomeRun::~CDirectoryHomeRun()
{
  m_dll.Unload();
}

bool CDirectoryHomeRun::GetDirectory(const CStdString& strPath, CFileItemList &items)
{
  if(!m_dll.IsLoaded())
    return false;

  CURL url(strPath);

  if(url.GetHostName().IsEmpty())
  {
    // no hostname, list all available devices
	  struct hdhomerun_discover_device_t result_list[64];
    int count = m_dll.discover_find_devices(HDHOMERUN_DEVICE_TYPE_TUNER, result_list, 64);
	  if (count < 0)
      return false;

    for(int i=0;i<count;i++)
    {
      CStdString device, ip;
      CFileItemPtr item;
      unsigned int ip_addr = result_list[i].ip_addr;

      device.Format("%x", result_list[i].device_id);
      ip.Format("%u.%u.%u.%u",
		    (unsigned int)(ip_addr >> 24) & 0xFF, (unsigned int)(ip_addr >> 16) & 0xFF,
		    (unsigned int)(ip_addr >> 8) & 0xFF, (unsigned int)(ip_addr >> 0) & 0xFF);

      item.reset(new CFileItem("hdhomerun://" + device + "/tuner0/", true));
      item->SetLabel(device + "-0 On " + ip);
      item->SetLabelPreformated(true);
      items.Add(item);

      item.reset(new CFileItem("hdhomerun://" + device + "/tuner1/", true));
      item->SetLabel(device + "-1 On " + ip);
      item->SetLabelPreformated(true);
      items.Add(item);
    }
    return true;
  }
  else
  {    
    hdhomerun_device_t* device = m_dll.device_create_from_str(url.GetHostName().c_str());
    if(!device)
      return false;

    m_dll.device_set_tuner_from_str(device, url.GetFileName().c_str());

    hdhomerun_tuner_status_t status;
    if(!m_dll.device_get_tuner_status(device, NULL, &status))
    {
      m_dll.device_destroy(device);
      return true;
    }

    CStdString label;
    if(status.signal_present)
      label.Format("Current Stream: N/A");
    else
      label.Format("Current Stream: Channel %s, SNR %d", status.channel, status.signal_to_noise_quality);

    CStdString path = "hdhomerun://" + url.GetHostName() + "/" + url.GetFileName();
    URIUtils::RemoveSlashAtEnd(path);
    CFileItemPtr item(new CFileItem(path, false));
    item->SetLabel(label);
    item->SetLabelPreformated(true);
    items.Add(item);

    m_dll.device_destroy(device);
    return true;
  }

  return false;
}


// -------------------------------------------
// ------------------ File -------------------
// -------------------------------------------
CFileHomeRun::CFileHomeRun() 
{
  m_device = NULL;
  m_dll.Load();
}

CFileHomeRun::~CFileHomeRun()
{
  Close();
}

bool CFileHomeRun::Exists(const CURL& url)
{
  CStdString path(url.GetFileName());

  /*
   * HDHomeRun URLs are of the form hdhomerun://1014F6D1/tuner0?channel=qam:108&program=10
   * The filename starts with "tuner" and has no extension. This check will cover off requests
   * for *.tbn, *.jpg, *.jpeg, *.edl etc. that do not exist.
   */
  if(path.Left(5) == "tuner"
  && URIUtils::GetExtension(path).IsEmpty())
    return true;

  return false;
}

__int64 CFileHomeRun::Seek(__int64 iFilePosition, int iWhence)
{ 
  return -1;
}

int CFileHomeRun::Stat(const CURL& url, struct __stat64* buffer)
{
  memset(buffer, 0, sizeof(struct __stat64));
  return 0;
}

__int64 CFileHomeRun::GetPosition()
{
  return 0;
}

__int64 CFileHomeRun::GetLength()
{
  return 0;
}

bool CFileHomeRun::Open(const CURL &url)
{
  if(!m_dll.IsLoaded())
    return false;
  
  m_device = m_dll.device_create_from_str(url.GetHostName().c_str());
  if(!m_device)
    return false;

  m_dll.device_set_tuner_from_str(m_device, url.GetFileName().c_str());

  CUrlOptions options(url.GetOptions().Mid(1));
  CUrlOptions::iterator it;

  if( (it = options.find("channel")) != options.end() )
    m_dll.device_set_tuner_channel(m_device, it->second.c_str());

  if( (it = options.find("program")) != options.end() )
    m_dll.device_set_tuner_program(m_device, it->second.c_str());

  // start streaming from selected device and tuner
  if( m_dll.device_stream_start(m_device) <= 0 )
    return false;

  return true;
}

unsigned int CFileHomeRun::Read(void* lpBuf, __int64 uiBufSize)
{
  unsigned int datasize;

  if(uiBufSize < VIDEO_DATA_PACKET_SIZE)
    CLog::Log(LOGWARNING, "CFileHomeRun::Read - buffer size too small, will most likely fail");

  // for now, let it it time out after 5 seconds,
  // neither of the players can be forced to 
  // continue even if read return 0 as can happen
  // on live streams.
  DWORD timestamp = GetTickCount() + 5000;
  while(1) 
  {
    datasize = (unsigned int)min((unsigned int) uiBufSize,UINT_MAX);
    uint8_t* ptr = m_dll.device_stream_recv(m_device, datasize, &datasize);
    if(ptr)
    {
      memcpy(lpBuf, ptr, datasize);
      return datasize;
    }

    if(GetTickCount() > timestamp)
      return 0;

    Sleep(64);
  }
  return datasize;
}

void CFileHomeRun::Close()
{  
  if(m_device)
  {
    m_dll.device_stream_stop(m_device);
    m_dll.device_destroy(m_device);
    m_device = NULL;
  }
}

int CFileHomeRun::GetChunkSize()
{
  return VIDEO_DATA_PACKET_SIZE;
}
