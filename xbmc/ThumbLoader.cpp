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
#include "ThumbLoader.h"
#include "Util.h"
#include "URL.h"
#include "Picture.h"
#include "FileSystem/File.h"
#include "FileItem.h"
#include "GUISettings.h"
#include "TextureManager.h"
#include "VideoInfoTag.h"
#include "VideoDatabase.h"

#include "cores/dvdplayer/DVDFileInfo.h"

using namespace XFILE;

CThumbLoader::CThumbLoader(int nThreads) :
  CBackgroundInfoLoader(nThreads)
{
}

CThumbLoader::~CThumbLoader()
{
}

bool CThumbLoader::LoadRemoteThumb(CFileItem *pItem)
{
  // look for remote thumbs
  CStdString thumb(pItem->GetThumbnailImage());
  if (!g_TextureManager.CanLoad(thumb))
  {
    CStdString cachedThumb(pItem->GetCachedVideoThumb());
    if (CFile::Exists(cachedThumb))
      pItem->SetThumbnailImage(cachedThumb);
    else
    {
      CPicture pic;
      if(pic.CreateThumbnail(thumb, cachedThumb))
        pItem->SetThumbnailImage(cachedThumb);
      else
        pItem->SetThumbnailImage("");
    }
  }
  return pItem->HasThumbnail();
}

CVideoThumbLoader::CVideoThumbLoader() :  
  CThumbLoader(), m_pStreamDetailsObs(NULL)
{
}

CVideoThumbLoader::~CVideoThumbLoader()
{
  StopThread();
}

void CVideoThumbLoader::OnLoaderStart()
{
}

void CVideoThumbLoader::OnLoaderFinish()
{
}

/**
* Reads watched status from the database and sets the watched overlay accordingly
*/
void CVideoThumbLoader::SetWatchedOverlay(CFileItem *item)
{
  // do this only for video files and exclude everything else.
  if (item->IsVideo() && !item->IsVideoDb() && !item->IsInternetStream()
      && !item->IsFileFolder() && !item->IsPlugin())
  {
    CVideoDatabase dbs;
    if (dbs.Open())
    {
      int playCount = dbs.GetPlayCount(*item);
      if (playCount >= 0)
        item->SetOverlayImage(CGUIListItem::ICON_OVERLAY_UNWATCHED, playCount > 0);

      dbs.Close();
    }
  }
}

/**
 * Look for a thumbnail for pItem.  If one does not exist, look for an autogenerated
 * thumbnail. Finally, check  * for the existance of fanart and set properties
 * accordingly.
 * @return: true if pItem has been modified 
 */
bool CVideoThumbLoader::LoadItem(CFileItem* pItem)
{
  if (pItem->m_bIsShareOrDrive
  ||  pItem->IsParentFolder())
    return false;

  SetWatchedOverlay(pItem);

  CFileItem item(*pItem);
  CStdString cachedThumb(item.GetCachedVideoThumb());

  if (!pItem->HasThumbnail())
  {
    item.SetUserVideoThumb();
    if (CFile::Exists(cachedThumb))
      pItem->SetThumbnailImage(cachedThumb);
    else
    {
      CStdString strPath, strFileName;
      CUtil::Split(cachedThumb, strPath, strFileName);

      // create unique thumb for auto generated thumbs
      cachedThumb = strPath + "auto-" + strFileName;
      if (CFile::Exists(cachedThumb))
      {
        // this is abit of a hack to avoid loading zero sized images
        // which we know will fail. They will just display empty image
        // we should really have some way for the texture loader to
        // do fallbacks to default images for a failed image instead
        struct __stat64 stat;
        if(CFile::Stat(cachedThumb, &stat) == 0 && stat.st_size > 0)
        {
          pItem->SetProperty("HasAutoThumb", "1");
          pItem->SetProperty("AutoThumbImage", cachedThumb);
          pItem->SetThumbnailImage(cachedThumb);
        }
      }
    }
  }
  else if (!pItem->GetThumbnailImage().Left(10).Equals("special://"))
    LoadRemoteThumb(pItem);

  if (!pItem->HasProperty("fanart_image"))
  {
    if (pItem->CacheLocalFanart())
      pItem->SetProperty("fanart_image",pItem->GetCachedFanart());
  }

  return true;
}

CProgramThumbLoader::CProgramThumbLoader()
{
}

CProgramThumbLoader::~CProgramThumbLoader()
{
}

bool CProgramThumbLoader::LoadItem(CFileItem *pItem)
{
  if (pItem->m_bIsShareOrDrive) return true;
  if (!pItem->HasThumbnail())
    pItem->SetUserProgramThumb();
  else
    LoadRemoteThumb(pItem);
  return true;
}

CMusicThumbLoader::CMusicThumbLoader()
{
}

CMusicThumbLoader::~CMusicThumbLoader()
{
}

bool CMusicThumbLoader::LoadItem(CFileItem* pItem)
{
  if (pItem->m_bIsShareOrDrive) return true;
  if (!pItem->HasThumbnail())
    pItem->SetUserMusicThumb();
  else
    LoadRemoteThumb(pItem);
  return true;
}

