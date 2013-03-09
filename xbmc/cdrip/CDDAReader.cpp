/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#include "system.h"
#include "utils/log.h"
#include "CDDAReader.h"
#include "lib/libcdio/cdio.h"

#define SECTOR_COUNT 55

#ifndef PRId64
#ifdef _MSC_VER
#define PRId64 "I64d"
#else
#define PRId64 "lld"
#endif
#endif

CCDDAReader::CCDDAReader()
{
  m_sRipBuffer[0].pbtStream = NULL;
  m_sRipBuffer[1].pbtStream = NULL;

  m_iCurrentBuffer = 0;

  m_hReadEvent = CreateEvent(NULL, false, false, NULL);
  m_hDataReadyEvent = CreateEvent(NULL, false, false, NULL);
}

CCDDAReader::~CCDDAReader()
{
  m_bStop = true;
  StopThread();

  CloseHandle(m_hReadEvent);
  CloseHandle(m_hDataReadyEvent);

  if (m_sRipBuffer[0].pbtStream) delete []m_sRipBuffer[0].pbtStream;
  if (m_sRipBuffer[1].pbtStream) delete []m_sRipBuffer[1].pbtStream;
}

bool CCDDAReader::Init(const char* strFileName)
{
  // Open the Ripper session
  if (!m_fileCdda.Open(strFileName))
    return false;

  CLog::Log(LOGINFO, "%s, Sectors %"PRId64, strFileName, m_fileCdda.GetLength() / CDIO_CD_FRAMESIZE_RAW);

  // allocate 2 buffers
  // read around 128k per chunk. This makes the cd reading less noisy.
  m_sRipBuffer[0].pbtStream = new BYTE[CDIO_CD_FRAMESIZE_RAW * SECTOR_COUNT];
  m_sRipBuffer[1].pbtStream = new BYTE[CDIO_CD_FRAMESIZE_RAW * SECTOR_COUNT];

  Create(false, THREAD_MINSTACKSIZE);

  return true;
}

bool CCDDAReader::DeInit()
{
  m_bStop = true;
  StopThread();

  m_iCurrentBuffer = 0;

  // free buffers
  if (m_sRipBuffer[0].pbtStream) delete []m_sRipBuffer[0].pbtStream;
  m_sRipBuffer[0].pbtStream = NULL;
  if (m_sRipBuffer[1].pbtStream) delete []m_sRipBuffer[1].pbtStream;
  m_sRipBuffer[1].pbtStream = NULL;

  // Close the Ripper session
  m_fileCdda.Close();

  return true;
}

int CCDDAReader::GetPercent()
{
  return (int)((m_fileCdda.GetPosition()*100) / m_fileCdda.GetLength());
}

int CCDDAReader::ReadChunk()
{
  // Read data
  DWORD dwBytesRead = m_fileCdda.Read(m_sRipBuffer[m_iCurrentBuffer].pbtStream, SECTOR_COUNT * CDIO_CD_FRAMESIZE_RAW);
  if (dwBytesRead == 0)
  {
    m_sRipBuffer[m_iCurrentBuffer].lBytesRead = 0;
    return CDDARIP_ERR;
  }

  m_sRipBuffer[m_iCurrentBuffer].lBytesRead = dwBytesRead;

  if (m_fileCdda.GetPosition() == m_fileCdda.GetLength()) return CDDARIP_DONE;

  return CDDARIP_OK;
}

void CCDDAReader::Process()
{
  // fill first buffer
  m_iCurrentBuffer = 0;

  m_sRipBuffer[0].iRipError = ReadChunk();
  SetEvent(m_hDataReadyEvent);

  while (!m_bStop)
  {
    // wait until someone called GetData()
    if (WaitForSingleObject(m_hReadEvent, INFINITE) == WAIT_OBJECT_0)
    {
      // event generated by m_hReadEvent, continue
      // switch buffer and start reading in this one
      m_iCurrentBuffer = m_iCurrentBuffer ? 0 : 1;
      m_sRipBuffer[m_iCurrentBuffer].iRipError = ReadChunk();

      SetEvent(m_hDataReadyEvent);
    }
  }
}

int CCDDAReader::GetData(BYTE** stream, long& lBytes)
{
  // wait until we are sure we have a buffer that is filled
  WaitForSingleObject(m_hDataReadyEvent, INFINITE);

  int iError = m_sRipBuffer[m_iCurrentBuffer].iRipError;
  *stream = m_sRipBuffer[m_iCurrentBuffer].pbtStream;
  lBytes = m_sRipBuffer[m_iCurrentBuffer].lBytesRead;

  // got data buffer, signal thread so it can start filling the other buffer
  SetEvent(m_hReadEvent);
  return iError;
}
