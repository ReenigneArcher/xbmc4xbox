#include "../../stdafx.h"
#include "DirectoryNodeAlbumRecentlyPlayedSong.h"
#include "../../MusicDatabase.h"

using namespace DIRECTORY::MUSICDATABASEDIRECTORY;

CDirectoryNodeAlbumRecentlyPlayedSong::CDirectoryNodeAlbumRecentlyPlayedSong(const CStdString& strName, CDirectoryNode* pParent)
  : CDirectoryNode(NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS, strName, pParent)
{

}

bool CDirectoryNodeAlbumRecentlyPlayedSong::GetContent(CFileItemList& items)
{
  CMusicDatabase musicdatabase;
  if (!musicdatabase.Open())
    return false;

  bool bSuccess=musicdatabase.GetRecentlyPlayedAlbumSongs(BuildPath(), items);

  musicdatabase.Close();

  return bSuccess;
}
