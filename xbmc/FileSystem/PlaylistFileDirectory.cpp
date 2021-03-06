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
#include "PlaylistFileDirectory.h"
#include "utils/log.h"
#include "PlayListFactory.h"
#include "MusicInfoTag.h"
#include "FileSystem/File.h"
#include "PlayList.h"

using namespace std;
using namespace PLAYLIST;

namespace XFILE
{
  CPlaylistFileDirectory::CPlaylistFileDirectory()
  {
  }

  CPlaylistFileDirectory::~CPlaylistFileDirectory()
  {
  }

  bool CPlaylistFileDirectory::GetDirectory(const CStdString& strPath, CFileItemList& items)
  {
    auto_ptr<CPlayList> pPlayList (CPlayListFactory::Create(strPath));
    if ( NULL != pPlayList.get())
    {
      // load it
      if (!pPlayList->Load(strPath))
        return false; //hmmm unable to load playlist?

      CPlayList playlist = *pPlayList;
      // convert playlist items to songs
      for (int i = 0; i < (int)playlist.size(); ++i)
      {
        CFileItemPtr item = playlist[i];
        item->m_iprogramCount = i;  // hack for playlist order
        items.Add(item);
      }
    }
    return true;
  }

  bool CPlaylistFileDirectory::ContainsFiles(const CStdString& strPath)
  {
    auto_ptr<CPlayList> pPlayList (CPlayListFactory::Create(strPath));
    if ( NULL != pPlayList.get())
    {
      // load it
      if (!pPlayList->Load(strPath))
        return false; //hmmm unable to load playlist?

      return (pPlayList->size() > 1);
    }
    return false;
  }

  bool CPlaylistFileDirectory::Remove(const char *strPath) 
  { 
    return XFILE::CFile::Delete(strPath); 
  }
}

