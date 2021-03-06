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
#include "GUIWindowMusicNav.h"
#include "Util.h"
#include "utils/GUIInfoManager.h"
#include "PlayListM3U.h"
#include "PlayListPlayer.h"
#include "GUIPassword.h"
#include "GUIDialogFileBrowser.h"
#include "GUIDialogContentSettings.h"
#include "Picture.h"
#include "FileSystem/MusicDatabaseDirectory.h"
#include "FileSystem/VideoDatabaseDirectory.h"
#include "PartyModeManager.h"
#include "PlayListFactory.h"
#include "GUIDialogMusicScan.h"
#include "VideoDatabase.h"
#include "GUIWindowVideoNav.h"
#include "MusicInfoTag.h"
#include "GUIWindowManager.h"
#include "GUIWindowFileManager.h"
#include "GUIDialogOK.h"
#include "GUIDialogKeyboard.h"
#include "GUIEditControl.h"
#include "FileSystem/File.h"
#include "FileItem.h"
#include "AdvancedSettings.h"
#include "utils/URIUtils.h"

using namespace std;
using namespace XFILE;
using namespace PLAYLIST;
using namespace MUSICDATABASEDIRECTORY;

#define CONTROL_BTNVIEWASICONS     2
#define CONTROL_BTNSORTBY          3
#define CONTROL_BTNSORTASC         4
#define CONTROL_BTNTYPE            5
#define CONTROL_LIST              50
#define CONTROL_THUMBS            51
#define CONTROL_BIGLIST           52
#define CONTROL_LABELFILES        12

#define CONTROL_SEARCH             8
#define CONTROL_FILTER            15
#define CONTROL_BTNPARTYMODE      16
#define CONTROL_BTNMANUALINFO     17
#define CONTROL_BTN_FILTER        19
#define CONTROL_LABELEMPTY        18

CGUIWindowMusicNav::CGUIWindowMusicNav(void)
    : CGUIWindowMusicBase(WINDOW_MUSIC_NAV, "MyMusicNav.xml")
{
  m_vecItems->SetPath("?");
  m_bDisplayEmptyDatabaseMessage = false;
  m_thumbLoader.SetObserver(this);
  m_searchWithEdit = false;
}

CGUIWindowMusicNav::~CGUIWindowMusicNav(void)
{
}

bool CGUIWindowMusicNav::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
  case GUI_MSG_WINDOW_RESET:
    m_vecItems->SetPath("?");
    break;
  case GUI_MSG_WINDOW_DEINIT:
    if (m_thumbLoader.IsLoading())
      m_thumbLoader.StopThread();
    break;
  case GUI_MSG_WINDOW_INIT:
    {
      // check for valid quickpath parameter
      CStdString strDestination = message.GetNumStringParams() ? message.GetStringParam(0) : "";
      CStdString strReturn = message.GetNumStringParams() > 1 ? message.GetStringParam(1) : "";
      bool returning = strReturn.CompareNoCase("return") == 0;

      if (!strDestination.IsEmpty())
      {
        message.SetStringParam("");
        CLog::Log(LOGINFO, "Attempting to quickpath to: %s", strDestination.c_str());
      }

      // is this the first time the window is opened?
      if (m_vecItems->GetPath() == "?" && strDestination.IsEmpty())
      {
        strDestination = g_settings.m_defaultMusicLibSource;
        m_vecItems->SetPath(strDestination);
        CLog::Log(LOGINFO, "Attempting to default to: %s", strDestination.c_str());
      }

      CStdString destPath;
      if (!strDestination.IsEmpty())
      {
        if (strDestination.Equals("$ROOT") || strDestination.Equals("Root"))
          destPath = "";
        else if (strDestination.Equals("Genres"))
          destPath = "musicdb://1/";
        else if (strDestination.Equals("Artists"))
          destPath = "musicdb://2/";
        else if (strDestination.Equals("Albums"))
          destPath = "musicdb://3/";
        else if (strDestination.Equals("Singles"))
          destPath = "musicdb://10/";
        else if (strDestination.Equals("Songs"))
          destPath = "musicdb://4/";
        else if (strDestination.Equals("Top100"))
          destPath = "musicdb://5/";
        else if (strDestination.Equals("Top100Songs"))
          destPath = "musicdb://5/2/";
        else if (strDestination.Equals("Top100Albums"))
          destPath = "musicdb://5/1/";
        else if (strDestination.Equals("RecentlyAddedAlbums"))
          destPath = "musicdb://6/";
        else if (strDestination.Equals("RecentlyPlayedAlbums"))
          destPath = "musicdb://7/";
        else if (strDestination.Equals("Compilations"))
          destPath = "musicdb://8/";
        else if (strDestination.Equals("Playlists"))
          destPath = "special://musicplaylists/";
        else if (strDestination.Equals("Years"))
          destPath = "musicdb://9/";
        else if (strDestination.Equals("Plugins"))
          destPath = "plugin://music/";
        else
        {
          CLog::Log(LOGWARNING, "Warning, destination parameter (%s) may not be valid", strDestination.c_str());
          destPath = strDestination;
        }
        if (!returning || m_vecItems->GetPath().Left(destPath.GetLength()) != destPath)
        { // we're not returning to the same path, so set our directory to the requested path
          m_vecItems->SetPath(destPath);
        }
        SetHistoryForPath(m_vecItems->GetPath());
      }

      DisplayEmptyDatabaseMessage(false); // reset message state

      if (!CGUIWindowMusicBase::OnMessage(message))
        return false;
      
      if (message.GetParam1() != WINDOW_INVALID)
      { // first time to this window - make sure we set the root path
        m_startDirectory = returning ? destPath : "";
      }

      //  base class has opened the database, do our check
      DisplayEmptyDatabaseMessage(m_musicdatabase.GetSongsCount() <= 0);

      if (m_bDisplayEmptyDatabaseMessage)
      {
        // no library - make sure we focus on a known control, and default to the root.
        SET_CONTROL_FOCUS(CONTROL_BTNTYPE, 0);
        m_vecItems->SetPath("");
        SetHistoryForPath("");
        Update("");
      }

      return true;
    }
    break;

  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      if (iControl == CONTROL_BTNPARTYMODE)
      {
        if (g_partyModeManager.IsEnabled())
          g_partyModeManager.Disable();
        else
        {
          if (!g_partyModeManager.Enable())
          {
            SET_CONTROL_SELECTED(GetID(),CONTROL_BTNPARTYMODE,false);
            return false;
          }

          // Playlist directory is the root of the playlist window
          if (m_guiState.get()) m_guiState->SetPlaylistDirectory("playlistmusic://");

          return true;
        }
        UpdateButtons();
      }
      else if (iControl == CONTROL_BTNMANUALINFO)
      {
        OnManualAlbumInfo();
        return true;
      }
      else if (iControl == CONTROL_SEARCH)
      {
        if (m_searchWithEdit)
        {
          // search updated - reset timer
          m_searchTimer.StartZero();
          // grab our search string
          CGUIMessage selected(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_SEARCH);
          OnMessage(selected);
          SetProperty("search", selected.GetLabel());
          return true;
        }
        CStdString search(GetProperty("search"));
        CGUIDialogKeyboard::ShowAndGetFilter(search, true);
        SetProperty("search", search);
        return true;
      }
    }
    break;
  case GUI_MSG_PLAYBACK_STOPPED:
  case GUI_MSG_PLAYBACK_ENDED:
  case GUI_MSG_PLAYLISTPLAYER_STOPPED:
  case GUI_MSG_PLAYBACK_STARTED:
    {
      SET_CONTROL_SELECTED(GetID(),CONTROL_BTNPARTYMODE, g_partyModeManager.IsEnabled());
    }
    break;
  case GUI_MSG_NOTIFY_ALL:
    {
      if (message.GetParam1() == GUI_MSG_SEARCH_UPDATE && IsActive())
      {
        // search updated - reset timer
        m_searchTimer.StartZero();
        SetProperty("search", message.GetStringParam());
      }
    }
  }
  return CGUIWindowMusicBase::OnMessage(message);
}

bool CGUIWindowMusicNav::OnAction(const CAction& action)
{
  if (action.id == ACTION_PARENT_DIR)
  {
    if (g_advancedSettings.m_bUseEvilB && m_vecItems->GetPath() == m_startDirectory)
    {
      g_windowManager.PreviousWindow();
      return true;
    }
  }
  if (action.id == ACTION_SCAN_ITEM)
  {
    int item = m_viewControl.GetSelectedItem();
    CMusicDatabaseDirectory dir;
    if (item > -1 && m_vecItems->Get(item)->m_bIsFolder
                  && (dir.HasAlbumInfo(m_vecItems->Get(item)->GetPath())||
                      dir.IsArtistDir(m_vecItems->Get(item)->GetPath())))
      OnContextButton(item,CONTEXT_BUTTON_INFO);

    return true;
  }
  
  return CGUIWindowMusicBase::OnAction(action);
}

CStdString CGUIWindowMusicNav::GetQuickpathName(const CStdString& strPath) const
{
  if (strPath.Equals("musicdb://1/"))
    return "Genres";
  else if (strPath.Equals("musicdb://2/"))
    return "Artists";
  else if (strPath.Equals("musicdb://3/"))
    return "Albums";
  else if (strPath.Equals("musicdb://4/"))
    return "Songs";
  else if (strPath.Equals("musicdb://5/"))
    return "Top100";
  else if (strPath.Equals("musicdb://5/2/"))
    return "Top100Songs";
  else if (strPath.Equals("musicdb://5/1/"))
    return "Top100Albums";
  else if (strPath.Equals("musicdb://6/"))
    return "RecentlyAddedAlbums";
  else if (strPath.Equals("musicdb://7/"))
    return "RecentlyPlayedAlbums";
  else if (strPath.Equals("musicdb://8/"))
    return "Compilations";
  else if (strPath.Equals("musicdb://9/"))
    return "Years";
  else if (strPath.Equals("musicdb://10/"))
    return "Singles";
  else if (strPath.Equals("special://musicplaylists/"))
    return "Playlists";
  else
  {
    CLog::Log(LOGERROR, "  CGUIWindowMusicNav::GetQuickpathName: Unknown parameter (%s)", strPath.c_str());
    return strPath;
  }
}

bool CGUIWindowMusicNav::OnClick(int iItem)
{
  if (iItem < 0 || iItem >= m_vecItems->Size()) return false;

  CFileItemPtr item = m_vecItems->Get(iItem);
  if (item->GetPath().Left(14) == "musicsearch://")
  {
    if (m_searchWithEdit)
      OnSearchUpdate();
    else
    {
      CStdString search(GetProperty("search"));
      CGUIDialogKeyboard::ShowAndGetFilter(search, true);
      SetProperty("search", search);
    }
    return true;
  }
  if (item->IsMusicDb() && !item->m_bIsFolder)
    m_musicdatabase.SetPropertiesForFileItem(*item);
    
  return CGUIWindowMusicBase::OnClick(iItem);
}

bool CGUIWindowMusicNav::GetDirectory(const CStdString &strDirectory, CFileItemList &items)
{
  if (m_bDisplayEmptyDatabaseMessage)
    return true;

  if (strDirectory.IsEmpty())
    AddSearchFolder();

  if (m_thumbLoader.IsLoading())
    m_thumbLoader.StopThread();

  bool bResult = CGUIWindowMusicBase::GetDirectory(strDirectory, items);
  if (bResult)
  {
    if (items.IsPlayList())
      OnRetrieveMusicInfo(items);
    if (!items.IsMusicDb())
    {
      items.SetCachedMusicThumbs();
      m_thumbLoader.Load(*m_vecItems);
    }
  }

  // update our content in the info manager
  if (strDirectory.Left(10).Equals("videodb://"))
  {
    CVideoDatabaseDirectory dir;
    VIDEODATABASEDIRECTORY::NODE_TYPE node = dir.GetDirectoryChildType(strDirectory);
    if (node == VIDEODATABASEDIRECTORY::NODE_TYPE_TITLE_MUSICVIDEOS)
      items.SetContent("musicvideos");
  }
  else if (strDirectory.Left(10).Equals("musicdb://"))
  {
    CMusicDatabaseDirectory dir;
    NODE_TYPE node = dir.GetDirectoryChildType(strDirectory);
    if (node == NODE_TYPE_ALBUM ||
        node == NODE_TYPE_ALBUM_RECENTLY_ADDED ||
        node == NODE_TYPE_ALBUM_RECENTLY_PLAYED ||
        node == NODE_TYPE_ALBUM_TOP100 ||
        node == NODE_TYPE_ALBUM_COMPILATIONS ||
        node == NODE_TYPE_YEAR_ALBUM)
      items.SetContent("albums");
    else if (node == NODE_TYPE_ARTIST)
      items.SetContent("artists");
    else if (node == NODE_TYPE_SONG ||
             node == NODE_TYPE_SONG_TOP100 ||
             node == NODE_TYPE_SINGLES)
      items.SetContent("songs");
    else if (node == NODE_TYPE_GENRE)
      items.SetContent("genres");
    else if (node == NODE_TYPE_YEAR)
      items.SetContent("years");
  }
  else if (strDirectory.Equals("special://musicplaylists"))
    items.SetContent("playlists");
  else if (strDirectory.Equals("plugin://music/"))
  {
    items.SetContent("plugins");
    items.SetLabel(g_localizeStrings.Get(24001)); 
  }
  else if (items.IsPlayList())
    items.SetContent("songs");
  else if (strDirectory.IsEmpty())
    items.SetLabel("");

  return bResult;
}

void CGUIWindowMusicNav::UpdateButtons()
{
  CGUIWindowMusicBase::UpdateButtons();

  // Update object count
  int iItems = m_vecItems->Size();
  if (iItems)
  {
    // check for parent dir and "all" items
    // should always be the first two items
    for (int i = 0; i <= (iItems>=2 ? 1 : 0); i++)
    {
      CFileItemPtr pItem = m_vecItems->Get(i);
      if (pItem->IsParentFolder()) iItems--;
      if (pItem->GetPath().Left(4).Equals("/-1/")) iItems--;
    }
    // or the last item
    if (m_vecItems->Size() > 2 &&
      m_vecItems->Get(m_vecItems->Size()-1)->GetPath().Left(4).Equals("/-1/"))
      iItems--;
  }
  CStdString items;
  items.Format("%i %s", iItems, g_localizeStrings.Get(127).c_str());
  SET_CONTROL_LABEL(CONTROL_LABELFILES, items);

  // set the filter label
  CStdString strLabel;

  // "Playlists"
  if (m_vecItems->GetPath().Equals("special://musicplaylists/"))
    strLabel = g_localizeStrings.Get(136);
  // "{Playlist Name}"
  else if (m_vecItems->IsPlayList())
  {
    // get playlist name from path
    CStdString strDummy;
    URIUtils::Split(m_vecItems->GetPath(), strDummy, strLabel);
  }
  // everything else is from a musicdb:// path
  else
  {
    CMusicDatabaseDirectory dir;
    dir.GetLabel(m_vecItems->GetPath(), strLabel);
  }

  SET_CONTROL_LABEL(CONTROL_FILTER, strLabel);

  SET_CONTROL_SELECTED(GetID(),CONTROL_BTNPARTYMODE, g_partyModeManager.IsEnabled());
}

void CGUIWindowMusicNav::PlayItem(int iItem)
{
  // unlike additemtoplaylist, we need to check the items here
  // before calling it since the current playlist will be stopped
  // and cleared!

  // root is not allowed
  if (m_vecItems->IsVirtualDirectoryRoot())
    return;

  CGUIWindowMusicBase::PlayItem(iItem);
}

void CGUIWindowMusicNav::OnWindowLoaded()
{
  const CGUIControl *control = GetControl(CONTROL_SEARCH);
  m_searchWithEdit = (control && control->GetControlType() == CGUIControl::GUICONTROL_EDIT);

  CGUIWindowMusicBase::OnWindowLoaded();

  if (m_searchWithEdit)
  {
    SendMessage(GUI_MSG_SET_TYPE, CONTROL_SEARCH, CGUIEditControl::INPUT_TYPE_SEARCH);
    SET_CONTROL_LABEL2(CONTROL_SEARCH, GetProperty("search"));
  }
}

void CGUIWindowMusicNav::GetContextButtons(int itemNumber, CContextButtons &buttons)
{
  CGUIWindowMusicBase::GetContextButtons(itemNumber, buttons);

  CGUIDialogMusicScan *musicScan = (CGUIDialogMusicScan *)g_windowManager.GetWindow(WINDOW_DIALOG_MUSIC_SCAN);
  CFileItemPtr item;
  if (itemNumber >= 0 && itemNumber < m_vecItems->Size())
    item = m_vecItems->Get(itemNumber);
  if (item && (item->GetExtraInfo().Find("lastfm") < 0))
  {
    // are we in the playlists location?
    bool inPlaylists = m_vecItems->GetPath().Equals(CUtil::MusicPlaylistsLocation()) ||
                       m_vecItems->GetPath().Equals("special://musicplaylists/");

    CMusicDatabaseDirectory dir;
    SScraperInfo info;
    m_musicdatabase.GetScraperForPath(item->GetPath(),info);
    // enable music info button on an album or on a song.
    if (item->IsAudio() && !item->IsPlayList() && !item->IsSmartPlayList() &&
       !item->IsLastFM() && !item->IsShoutCast() && !item->m_bIsFolder)
    {
      buttons.Add(CONTEXT_BUTTON_SONG_INFO, 658);
    }
    else if (item->IsVideoDb())
    {
      if (!item->m_bIsFolder) // music video
       buttons.Add(CONTEXT_BUTTON_INFO, 20393);
      if (item->GetPath().Left(14).Equals("videodb://3/4/") &&
          item->GetPath().size() > 14 && item->m_bIsFolder)
      {
        long idArtist = m_musicdatabase.GetArtistByName(m_vecItems->Get(itemNumber)->GetLabel());
        if (idArtist > - 1)
          buttons.Add(CONTEXT_BUTTON_INFO,21891);
      }
    }
    else if (!inPlaylists && (dir.HasAlbumInfo(item->GetPath())||
                              dir.IsArtistDir(item->GetPath())   )      &&
             !dir.IsAllItem(item->GetPath()) && !item->IsParentFolder() &&
             !item->IsLastFM() && !item->IsShoutCast()                  &&
             !item->GetPath().Left(14).Equals("musicsearch://"))
    {
      if (dir.IsArtistDir(item->GetPath()))
        buttons.Add(CONTEXT_BUTTON_INFO, 21891);
      else
        buttons.Add(CONTEXT_BUTTON_INFO, 13351);
    }

    // enable query all albums button only in album view
    if (dir.HasAlbumInfo(item->GetPath()) && !dir.IsAllItem(item->GetPath()) &&
        item->m_bIsFolder && !item->IsVideoDb() && !item->IsParentFolder()   &&
       !item->IsLastFM() &&  !item->IsShoutCast()                            &&
       !item->IsPluginRoot() && !item->IsPlugin()                            &&
       !item->GetPath().Left(14).Equals("musicsearch://"))
    {
      buttons.Add(CONTEXT_BUTTON_INFO_ALL, 20059);
    }

    // enable query all artist button only in album view
    if (dir.IsArtistDir(item->GetPath())        && !dir.IsAllItem(item->GetPath()) &&
        item->m_bIsFolder && !item->IsVideoDb() && !info.strContent.IsEmpty())
    {
      buttons.Add(CONTEXT_BUTTON_INFO_ALL, 21884);
    }

    if (m_vecItems->GetPath().Equals("plugin://music/"))
      buttons.Add(CONTEXT_BUTTON_SET_PLUGIN_THUMB, 1044);

    //Set default or clear default
    NODE_TYPE nodetype = dir.GetDirectoryType(item->GetPath());
    if (!item->IsParentFolder() && !inPlaylists &&
        (nodetype == NODE_TYPE_ROOT     ||
         nodetype == NODE_TYPE_OVERVIEW ||
         nodetype == NODE_TYPE_TOP100))
    {
      if (!item->GetPath().Equals(g_settings.m_defaultMusicLibSource))
        buttons.Add(CONTEXT_BUTTON_SET_DEFAULT, 13335); // set default
      if (strcmp(g_settings.m_defaultMusicLibSource, ""))
        buttons.Add(CONTEXT_BUTTON_CLEAR_DEFAULT, 13403); // clear default
    }
    NODE_TYPE childtype = dir.GetDirectoryChildType(item->GetPath());
    if (childtype == NODE_TYPE_ALBUM               ||
        childtype == NODE_TYPE_ARTIST              ||
        nodetype == NODE_TYPE_GENRE                ||
        nodetype == NODE_TYPE_ALBUM                ||
        nodetype == NODE_TYPE_ALBUM_RECENTLY_ADDED ||
        nodetype == NODE_TYPE_ALBUM_COMPILATIONS)
    {
      // we allow the user to set content for
      // 1. general artist and album nodes
      // 2. specific per genre
      // 3. specific per artist
      // 4. specific per album
      buttons.Add(CONTEXT_BUTTON_SET_CONTENT,20195);
    }
    if (item->HasMusicInfoTag() && item->GetMusicInfoTag()->GetArtist().size() > 0)
    {
      CVideoDatabase database;
      database.Open();
      if (database.GetMatchingMusicVideo(item->GetMusicInfoTag()->GetArtist()) > -1)
        buttons.Add(CONTEXT_BUTTON_GO_TO_ARTIST, 20400);
    }
    if (item->HasMusicInfoTag() && item->GetMusicInfoTag()->GetArtist().size() > 0 &&
        item->GetMusicInfoTag()->GetAlbum().size() > 0 &&
        item->GetMusicInfoTag()->GetTitle().size() > 0)
    {
      CVideoDatabase database;
      database.Open();
      if (database.GetMatchingMusicVideo(item->GetMusicInfoTag()->GetArtist(),item->GetMusicInfoTag()->GetAlbum(),item->GetMusicInfoTag()->GetTitle()) > -1)
        buttons.Add(CONTEXT_BUTTON_PLAY_OTHER, 20401);
    }
    if (item->HasVideoInfoTag() && !item->m_bIsFolder && !item->IsShoutCast())
    {
      if (item->GetVideoInfoTag()->m_playCount > 0)
        buttons.Add(CONTEXT_BUTTON_MARK_UNWATCHED, 16104); //Mark as UnWatched
      else
        buttons.Add(CONTEXT_BUTTON_MARK_WATCHED, 16103);   //Mark as Watched
      if ((g_settings.m_vecProfiles[g_settings.m_iLastLoadedProfileIndex].canWriteDatabases() || g_passwordManager.bMasterUser) &&
          !item->IsPluginRoot() && !item->IsPlugin())
      {
        buttons.Add(CONTEXT_BUTTON_RENAME, 16105);
        buttons.Add(CONTEXT_BUTTON_DELETE, 646);
      }
    }
    if (inPlaylists && !URIUtils::GetFileName(item->GetPath()).Equals("PartyMode.xsp")
                    && (item->IsPlayList() || item->IsSmartPlayList()))
      buttons.Add(CONTEXT_BUTTON_DELETE, 117);
  }
  // noncontextual buttons

  if (musicScan && musicScan->IsScanning())
    buttons.Add(CONTEXT_BUTTON_STOP_SCANNING, 13353);     // Stop Scanning
  else if (musicScan)
    buttons.Add(CONTEXT_BUTTON_UPDATE_LIBRARY, 653);

  CGUIWindowMusicBase::GetNonContextButtons(buttons);
}

bool CGUIWindowMusicNav::OnContextButton(int itemNumber, CONTEXT_BUTTON button)
{
  CFileItemPtr item;
  if (itemNumber >= 0 && itemNumber < m_vecItems->Size())
    item = m_vecItems->Get(itemNumber);

  switch (button)
  {
  case CONTEXT_BUTTON_INFO:
    {
      if (!item->IsVideoDb())
        return CGUIWindowMusicBase::OnContextButton(itemNumber,button);

      // music videos - artists
      if (item->GetPath().Left(14).Equals("videodb://3/4/"))
      {
        long idArtist = m_musicdatabase.GetArtistByName(item->GetLabel());
        if (idArtist == -1)
          return false;
        CStdString path;
        path.Format("musicdb://2/%ld/", idArtist);
        item->SetPath(path);
        CGUIWindowMusicBase::OnContextButton(itemNumber,button);
        Update(m_vecItems->GetPath());
        m_viewControl.SetSelectedItem(itemNumber);
        return true;
      }

      // music videos - albums
      if (item->GetPath().Left(14).Equals("videodb://3/5/"))
      {
        long idAlbum = m_musicdatabase.GetAlbumByName(item->GetLabel());
        if (idAlbum == -1)
          return false;
        CStdString path;
        path.Format("musicdb://3/%ld/", idAlbum);
        item->SetPath(path);
        CGUIWindowMusicBase::OnContextButton(itemNumber,button);
        Update(m_vecItems->GetPath());
        m_viewControl.SetSelectedItem(itemNumber);
        return true;
      }

      if (item->HasVideoInfoTag() && !item->GetVideoInfoTag()->m_strTitle.IsEmpty())
      {
        CGUIWindowVideoNav* pWindow = (CGUIWindowVideoNav*)g_windowManager.GetWindow(WINDOW_VIDEO_NAV);
        if (pWindow)
        { 
          SScraperInfo info;
          pWindow->OnInfo(item.get(),info);
          Update(m_vecItems->GetPath());
        }
      }
      return true;
    }

  case CONTEXT_BUTTON_INFO_ALL:
    OnInfoAll(itemNumber);
    return true;

  case CONTEXT_BUTTON_SET_PLUGIN_THUMB:
    SetPluginThumb(itemNumber, g_settings.m_musicSources);
    return true;

  case CONTEXT_BUTTON_UPDATE_LIBRARY:
    {
      CGUIDialogMusicScan *scanner = (CGUIDialogMusicScan *)g_windowManager.GetWindow(WINDOW_DIALOG_MUSIC_SCAN);
      if (scanner)
        scanner->StartScanning("");
      return true;
    }

  case CONTEXT_BUTTON_SET_DEFAULT:
    g_settings.m_defaultMusicLibSource = GetQuickpathName(item->GetPath());
    g_settings.Save();
    return true;

  case CONTEXT_BUTTON_CLEAR_DEFAULT:
    g_settings.m_defaultMusicLibSource.Empty();
    g_settings.Save();
    return true;

  case CONTEXT_BUTTON_GO_TO_ARTIST:
    {
      CStdString strPath;
      CVideoDatabase database;
      database.Open();
      strPath.Format("videodb://3/4/%ld/",database.GetMatchingMusicVideo(item->GetMusicInfoTag()->GetArtist()));
      g_windowManager.ActivateWindow(WINDOW_VIDEO_NAV,strPath);
      return true;
    }

  case CONTEXT_BUTTON_PLAY_OTHER:
    {
      CVideoDatabase database;
      database.Open();
      CVideoInfoTag details;
      database.GetMusicVideoInfo("",details,database.GetMatchingMusicVideo(item->GetMusicInfoTag()->GetArtist(),item->GetMusicInfoTag()->GetAlbum(),item->GetMusicInfoTag()->GetTitle()));
      g_applicationMessenger.PlayFile(CFileItem(details));
      return true;
    }

  case CONTEXT_BUTTON_MARK_WATCHED:
    CGUIWindowVideoBase::MarkWatched(item,true);
    CUtil::DeleteVideoDatabaseDirectoryCache();
    Update(m_vecItems->GetPath());
    return true;

  case CONTEXT_BUTTON_MARK_UNWATCHED:
    CGUIWindowVideoBase::MarkWatched(item,false);
    CUtil::DeleteVideoDatabaseDirectoryCache();
    Update(m_vecItems->GetPath());
    return true;

  case CONTEXT_BUTTON_RENAME:
    CGUIWindowVideoBase::UpdateVideoTitle(item.get());
    CUtil::DeleteVideoDatabaseDirectoryCache();
    Update(m_vecItems->GetPath());
    return true;

  case CONTEXT_BUTTON_DELETE:
    if (item->IsPlayList() || item->IsSmartPlayList())
    {
      item->m_bIsFolder = false;
      CGUIWindowFileManager::DeleteItem(item.get());
    }
    else
    {
      CGUIWindowVideoNav::DeleteItem(item.get());
      CUtil::DeleteVideoDatabaseDirectoryCache();
    }
    Update(m_vecItems->GetPath());
    return true;

  case CONTEXT_BUTTON_SET_CONTENT:
    {
      bool bScan=false;
      SScraperInfo info;
      CStdString path(item->GetPath());
      CQueryParams params;
      CDirectoryNode::GetDatabaseInfo(item->GetPath(), params);
      if (params.GetAlbumId() != -1)
        path.Format("musicdb://3/%i/",params.GetAlbumId());
      else if (params.GetArtistId() != -1)
        path.Format("musicdb://2/%i/",params.GetArtistId());

      if (!m_musicdatabase.GetScraperForPath(path,info))
        info.strContent = "albums";

      int iLabel=132;
      // per genre or for all artists
      if (m_vecItems->GetPath().Left(12).Equals("musicdb://1/") || item->GetPath().Left(12).Equals("musicdb://2/"))
      {
        iLabel = 133;
      }

      if (CGUIDialogContentSettings::Show(info, bScan,iLabel))
      {
        m_musicdatabase.SetScraperForPath(path,info);
        if (bScan)
          OnInfoAll(itemNumber,true);
      }
      return true;
    }

  default:
    break;
  }

  return CGUIWindowMusicBase::OnContextButton(itemNumber, button);
}

void CGUIWindowMusicNav::SetPluginThumb(int iItem, const VECSOURCES &sources)
{
  CFileItemList items;

  CStdString itemPath = m_vecItems->Get(iItem)->GetPath();
  itemPath.Replace("plugin://", "special://home/plugins/");
  CStdString picturePath = itemPath;
  CFileItem item(picturePath, true);
  CStdString cachedThumb = item.GetCachedProgramThumb();

  if (XFILE::CFile::Exists(cachedThumb))
  {
    CFileItemPtr item(new CFileItem("thumb://Current", false));
    item->SetThumbnailImage(cachedThumb);
    item->SetLabel(g_localizeStrings.Get(20016));
    items.Add(item);
  }
  else
  {
    CFileItem item2(URIUtils::AddFileToFolder(picturePath, "default.py"), false);
      if (XFILE::CFile::Exists(item2.GetCachedProgramThumb()))
      {
        CFileItemPtr item(new CFileItem("thumb://Current", false));
        item->SetThumbnailImage(item2.GetCachedProgramThumb());
        item->SetLabel(g_localizeStrings.Get(20016));
        items.Add(item);
      }
    }

  CStdString localThumb = URIUtils::AddFileToFolder(picturePath, "default.tbn");
  if (XFILE::CFile::Exists(localThumb))
    {
    CFileItemPtr item(new CFileItem(localThumb, false));
    item->SetThumbnailImage(localThumb);
      item->SetLabel(g_localizeStrings.Get(20017));
      items.Add(item);
    }
  else
  {
    CFileItemPtr nItem(new CFileItem("thumb://None", false));
  nItem->SetLabel(g_localizeStrings.Get(20018));
    nItem->SetIconImage("DefaultFolder.png");
  items.Add(nItem);
  }
  
  if (CGUIDialogFileBrowser::ShowAndGetImage(items, sources,
                                             g_localizeStrings.Get(20019), picturePath))
  {
    CPicture picture;
    if (picturePath.Equals("thumb://Current"))
      return;

    if (picturePath.Equals("thumb://None"))
    {
      XFILE::CFile::Delete(cachedThumb);
      CFileItem item2(URIUtils::AddFileToFolder(itemPath, "default.py"), false);
        XFILE::CFile::Delete(item2.GetCachedProgramThumb());
      }
    else
      XFILE::CFile::Cache(picturePath, cachedThumb);

    if (picturePath.Equals("thumb://None") ||
        picture.CreateThumbnail(picturePath, cachedThumb))
    {
      CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_REFRESH_THUMBS);
      g_windowManager.SendMessage(msg);
      Update(m_vecItems->GetPath());
    }
    else
      CLog::Log(LOGERROR, " %s Could not cache plugin thumb: %s", __FUNCTION__, picturePath.c_str());
  }
}

bool CGUIWindowMusicNav::GetSongsFromPlayList(const CStdString& strPlayList, CFileItemList &items)
{
  CStdString strParentPath=m_history.GetParentPath();

  if (m_guiState.get() && !m_guiState->HideParentDirItems())
  {
    CFileItemPtr pItem(new CFileItem(".."));
    pItem->SetPath(strParentPath);
    items.Add(pItem);
  }

  items.SetPath(strPlayList);
  CLog::Log(LOGDEBUG,"CGUIWindowMusicNav, opening playlist [%s]", strPlayList.c_str());

  auto_ptr<CPlayList> pPlayList (CPlayListFactory::Create(strPlayList));
  if ( NULL != pPlayList.get())
  {
    // load it
    if (!pPlayList->Load(strPlayList))
    {
      CGUIDialogOK::ShowAndGetInput(6, 0, 477, 0);
      return false; //hmmm unable to load playlist?
    }
    CPlayList playlist = *pPlayList;
    // convert playlist items to songs
    for (int i = 0; i < (int)playlist.size(); ++i)
    {
      items.Add(playlist[i]);
    }
  }

  return true;
}

void CGUIWindowMusicNav::DisplayEmptyDatabaseMessage(bool bDisplay)
{
  m_bDisplayEmptyDatabaseMessage = bDisplay;
}

void CGUIWindowMusicNav::OnSearchUpdate()
{
  CStdString search(GetProperty("search"));
  CURL::Encode(search);
  // send using a thread message as we may be called from Render, and this function may re-call
  // the render loop (slow dir fetch pops up busy dialog)
  if (!search.IsEmpty())
  {
    m_history.ClearPathHistory();
    CGUIMessage message(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE);
    message.SetStringParam("musicsearch://" + search + "/");
    g_windowManager.SendThreadMessage(message, GetID());
  }
  else if (m_vecItems->IsVirtualDirectoryRoot())
  {
    CGUIMessage message(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE);
    g_windowManager.SendThreadMessage(message, GetID());
  }
}

void CGUIWindowMusicNav::FrameMove()
{
  static const int search_timeout = 2000;
  // update our searching
  if (m_searchTimer.IsRunning() && m_searchTimer.GetElapsedMilliseconds() > search_timeout)
  {
    OnSearchUpdate();
    m_searchTimer.Stop();
  }
  if (m_bDisplayEmptyDatabaseMessage)
    SET_CONTROL_LABEL(CONTROL_LABELEMPTY,g_localizeStrings.Get(745)+'\n'+g_localizeStrings.Get(746));
  else
    SET_CONTROL_LABEL(CONTROL_LABELEMPTY,"");
  CGUIWindowMusicBase::FrameMove();
}

void CGUIWindowMusicNav::OnPrepareFileItems(CFileItemList &items)
{
  CGUIWindowMusicBase::OnPrepareFileItems(items);
  // set fanart
  SetupFanart(items);
}

void CGUIWindowMusicNav::AddSearchFolder()
{
  // we use a general viewstate (and not our member) here as our
  // current viewstate may be specific to some other folder, and
  // we know we're in the root here
  CFileItemList items;
  CGUIViewState* viewState = CGUIViewState::GetViewState(GetID(), items);
  if (viewState)
  {
    // add our remove the musicsearch source
    VECSOURCES &sources = viewState->GetSources();
    bool haveSearchSource = false;
    bool needSearchSource = !GetProperty("search").IsEmpty() || !m_searchWithEdit; // we always need it if we don't have the edit control
    for (IVECSOURCES it = sources.begin(); it != sources.end(); ++it)
    {
      CMediaSource& share = *it;
      if (share.strPath == "musicsearch://")
      {
        haveSearchSource = true;
        if (!needSearchSource)
        { // remove it
          sources.erase(it);
          break;
        }
      }
    }
    if (!haveSearchSource && needSearchSource)
    {
      // add earch share
      CMediaSource share;
      share.strName=g_localizeStrings.Get(137); // Search
      share.strPath = "musicsearch://";
      share.m_iDriveType = CMediaSource::SOURCE_TYPE_LOCAL;
      sources.push_back(share);
    }
    m_rootDir.SetSources(sources);
    delete viewState;
  }
}
