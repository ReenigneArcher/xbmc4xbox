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
#include "MusicInfoTagLoaderASAP.h"
#include "Util.h"
#include "utils/URIUtils.h"
#include "MusicInfoTag.h"
#include "FileSystem/File.h"

using namespace MUSIC_INFO;

CMusicInfoTagLoaderASAP::CMusicInfoTagLoaderASAP()
{
}

CMusicInfoTagLoaderASAP::~CMusicInfoTagLoaderASAP()
{
}

bool CMusicInfoTagLoaderASAP::Load(const CStdString &strFile, CMusicInfoTag &tag)
{
  tag.SetLoaded(false);

  if (!m_dll.Load())
    return false;

  CStdString strFileToLoad = strFile;
  int song = -1;
  CStdString strExtension;
  URIUtils::GetExtension(strFile, strExtension);
  strExtension.MakeLower();
  if (strExtension == ".asapstream")
  {
    CStdString strFileName = URIUtils::GetFileName(strFile);
    int iStart = strFileName.ReverseFind('-') + 1;
    song = atoi(strFileName.substr(iStart, strFileName.size() - iStart - 11).c_str()) - 1;
    CStdString strPath = strFile;
    URIUtils::GetDirectory(strPath, strFileToLoad);
    URIUtils::RemoveSlashAtEnd(strFileToLoad);
  }

  ASAP_SongInfo songInfo;
  if (!m_dll.asapGetInfo(strFileToLoad.c_str(), song, &songInfo))
    return false;

  tag.SetURL(strFileToLoad);
  if (songInfo.name[0] != '\0')
    tag.SetTitle(songInfo.name);
  if (songInfo.author[0] != '\0')
    tag.SetArtist(songInfo.author);
  if (song >= 0)
    tag.SetTrackNumber(song + 1);
  if (songInfo.duration >= 0)
    tag.SetDuration(songInfo.duration / 1000);
  if (songInfo.year > 0)
  {
    SYSTEMTIME dateTime = { songInfo.year, songInfo.month, 0, songInfo.day, 0, 0, 0, 0 };
    tag.SetReleaseDate(dateTime);
  }
  tag.SetLoaded(true);
  return true;
}
