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
#include "LastFmManager.h"
#include "Album.h"
#include "Artist.h"
#include "Application.h"
#include "ApplicationRenderer.h"
#include "PlayListPlayer.h"
#include "utils/URIUtils.h"
#include "PlayListFactory.h"
#include "Picture.h"
#include "utils/md5.h"
#include "FileSystem/File.h"
#include "FileSystem/FileCurl.h"
#include "utils/GUIInfoManager.h"
#include "MusicDatabase.h"
#include "MusicInfoTag.h"
#include "URL.h"
#include "GUIWindowManager.h"
#include "GUIDialogProgress.h"
#include "GUIDialogYesNo.h"
#include "Settings.h"
#include "PlayList.h"
#include "Crc32.h"
#include "AdvancedSettings.h"

#include <sstream>

using namespace std;
using namespace MUSIC_INFO;
using namespace PLAYLIST;
using namespace XFILE;

#define XBMC_LASTFM_VERSION "0.2"
#ifdef _DEBUG
  #define XBMC_LASTFM_ID  "tst"
#else
  #define XBMC_LASTFM_ID  "xbmc"
#endif
#define XBMC_LASTFM_MINTRACKS 3

CLastFmManager* CLastFmManager::m_pInstance=NULL;

CLastFmManager::CLastFmManager()
{
  m_hWorkerEvent = CreateEvent(NULL, false, false, NULL);
  m_RadioTrackQueue = new CPlayList;
}

CLastFmManager::~CLastFmManager()
{
  StopRadio(true);
  CloseHandle(m_hWorkerEvent);
  StopThread();
  CLog::Log(LOGINFO,"lastfm destroyed");
  delete m_RadioTrackQueue;
}

void CLastFmManager::RemoveInstance()
{
  if (m_pInstance)
  {
    delete m_pInstance;
    m_pInstance=NULL;
  }
}

CLastFmManager* CLastFmManager::GetInstance()
{
  if (!m_pInstance)
    m_pInstance=new CLastFmManager;

  return m_pInstance;
}

void CLastFmManager::Parameter(const CStdString& key, const CStdString& data, CStdString& value)
{
  value = "";
  vector<CStdString> params;
  StringUtils::SplitString(data, "\n", params);
  for (int i = 0; i < (int)params.size(); i++)
  {
    CStdString tmp = params[i];
    if (int pos = tmp.Find(key) >= 0)
    {
      tmp.Delete(pos - 1, key.GetLength() + 1);
      value = tmp;
      break;
    }
  }
  CLog::Log(LOGDEBUG, "Parameter %s -> %s", key.c_str(), value.c_str());
}

bool CLastFmManager::RadioHandShake()
{
  if (!m_RadioSession.IsEmpty()) return true; //already signed in

  if (dlgProgress)
  {
    dlgProgress->SetLine(2, 15251);//Connecting to Last.fm..
    dlgProgress->Progress();
  }

  m_RadioSession = "";

  CFileCurl http;
  CStdString html;

  CStdString strPassword = g_guiSettings.GetString("scrobbler.lastfmpassword");
  CStdString strUserName = g_guiSettings.GetString("scrobbler.lastfmusername");
  if (strUserName.IsEmpty() || strPassword.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm stream selected but no username or password set.");
    return false;
  }

  CStdString passwordmd5;
  CreateMD5Hash(strPassword, passwordmd5);

  CStdString url;
  CURL::Encode(strUserName);
  url.Format("http://ws.audioscrobbler.com/radio/handshake.php?version=%s&platform=%s&username=%s&passwordmd5=%s&debug=%i&partner=%s", XBMC_LASTFM_VERSION, XBMC_LASTFM_ID, strUserName, passwordmd5, 0, "");
  if (!http.Get(url, html))
  {
    CLog::Log(LOGERROR, "Connect to Last.fm radio failed.");
    return false;
  }
  //CLog::DebugLog("Handshake: %s", html.c_str());

  Parameter("session",    html, m_RadioSession);
  Parameter("base_url",   html, m_RadioBaseUrl);
  Parameter("base_path",  html, m_RadioBasePath);
  Parameter("subscriber", html, m_RadioSubscriber);
  Parameter("banned",     html, m_RadioBanned);

  if (m_RadioSession.CompareNoCase("failed") == 0)
  {
    CLog::Log(LOGERROR, "Last.fm return failed response, possible bad username or password?");
    m_RadioSession = "";
  }
  return !m_RadioSession.IsEmpty();
}

void CLastFmManager::InitProgressDialog(const CStdString& strUrl)
{
  if (m_RadioSession.IsEmpty())
  {
    dlgProgress = (CGUIDialogProgress*)g_windowManager.GetWindow(WINDOW_DIALOG_PROGRESS);
    if (dlgProgress)
    {
      dlgProgress->SetHeading(15200);
      dlgProgress->SetLine(0, 259);
      CStdString strUrlDec = strUrl;
      CURL::Decode(strUrlDec);
      dlgProgress->SetLine(1, strUrlDec);
      dlgProgress->SetLine(2, "");
      if (!dlgProgress->IsDialogRunning())
        dlgProgress->StartModal();
    }
  }
  else
  {
    g_ApplicationRenderer.SetBusy(true);
  }
}

void CLastFmManager::UpdateProgressDialog(const int iStringID)
{
  if (dlgProgress)
  {
    dlgProgress->SetLine(2, iStringID);
    dlgProgress->Progress();
  }
}

void CLastFmManager::CloseProgressDialog()
{
  if (dlgProgress)
  {
    dlgProgress->Close();
    dlgProgress = NULL;
  }
  else
  {
    g_ApplicationRenderer.SetBusy(false);
  }
}

bool CLastFmManager::ChangeStation(const CURL& stationUrl)
{
  DWORD start = timeGetTime();

  InitProgressDialog(stationUrl.Get());

  StopRadio(false);
  if (!RadioHandShake())
  {
    CloseProgressDialog();
    return false;
  }

  UpdateProgressDialog(15252); // Selecting station...

  CFileCurl http;
  CStdString url;
  CStdString html;

  url.Format("http://" + m_RadioBaseUrl + m_RadioBasePath + "/adjust.php?session=%s&url=%s&debug=%i", m_RadioSession, stationUrl.Get().c_str(), 0);
  if (!http.Get(url, html))
  {
    CLog::Log(LOGERROR, "Connect to Last.fm to change station failed.");
    CloseProgressDialog();
    return false;
  }
  //CLog::DebugLog("ChangeStation: %s", html.c_str());

  CStdString strErrorCode;
  Parameter("error", html,  strErrorCode);
  if (strErrorCode != "")
  {
    CLog::Log(LOGERROR, "Last.fm returned an error (%s) response for change station request.", strErrorCode.c_str());
    CloseProgressDialog();
    return false;
  }

  UpdateProgressDialog(261); //Waiting for start....

  g_playlistPlayer.ClearPlaylist(PLAYLIST_MUSIC);
  RequestRadioTracks();
  CacheTrackThumb(XBMC_LASTFM_MINTRACKS);
  AddToPlaylist(XBMC_LASTFM_MINTRACKS);
  Create(); //start thread
  SetEvent(m_hWorkerEvent); //kickstart the thread

  CSingleLock lock(m_lockPlaylist);
  CPlayList& playlist = g_playlistPlayer.GetPlaylist(PLAYLIST_MUSIC);
  if ((int)playlist.size())
  {
    g_application.m_strPlayListFile = stationUrl.Get(); //needed to highlight the playing item
    g_playlistPlayer.SetCurrentPlaylist(PLAYLIST_MUSIC);
    g_playlistPlayer.Play(0);
    CLog::Log(LOGDEBUG, "%s: Done (time: %i ms)", __FUNCTION__, (int)(timeGetTime() - start));
    CloseProgressDialog();
    return true;
  }
  CloseProgressDialog();
  return false;
}

bool CLastFmManager::RequestRadioTracks()
{
  DWORD start = timeGetTime();
  CStdString url;
  CStdString html;
  url.Format("http://" + m_RadioBaseUrl + m_RadioBasePath + "/xspf.php?sk=%s&discovery=0&desktop=", m_RadioSession);
  {
    CFileCurl http;
    if (!http.Get(url, html))
    {
      m_RadioSession.empty();
      CLog::Log(LOGERROR, "LastFmManager: Connect to Last.fm to request tracks failed.");
      return false;
    }
  }
  //CLog::DebugLog("RequestRadioTracks: %s", html.c_str());

  //parse playlist
  TiXmlDocument xmlDoc;

  xmlDoc.Parse(html);
  if (xmlDoc.Error())
  {
    m_RadioSession.empty();
    CLog::Log(LOGERROR, "LastFmManager: Unable to parse tracklist Error: %s", xmlDoc.ErrorDesc());
    return false;
  }

  TiXmlElement* pRootElement = xmlDoc.RootElement();
  if (!pRootElement )
  {
    CLog::Log(LOGWARNING, "LastFmManager: No more tracks received");
    m_RadioSession.empty();
    return false;
  }

  TiXmlElement* pBodyElement = pRootElement->FirstChildElement("trackList");
  if (!pBodyElement )
  {
    CLog::Log(LOGWARNING, "LastFmManager: No more tracks received, no tracklist");
    m_RadioSession.empty();
    return false;
  }

  TiXmlElement* pTrackElement = pBodyElement->FirstChildElement("track");

  if (!pTrackElement)
  {
    CLog::Log(LOGWARNING, "LastFmManager: No more tracks received, empty tracklist");
    m_RadioSession.empty();
    return false;
  }
  while (pTrackElement)
  {
    CFileItemPtr newItem(new CFileItem);

    TiXmlElement* pElement = pTrackElement->FirstChildElement("location");
    if (pElement)
    {
      TiXmlNode* child = pElement->FirstChild();
      if (child)
      {
        CStdString url = child->Value();
        url.Replace("http:", "lastfm:");
        newItem->SetPath(url);
      }
    }
    pElement = pTrackElement->FirstChildElement("title");
    if (pElement)
    {
      TiXmlNode* child = pElement->FirstChild();
      if (child)
      {
        newItem->SetLabel(child->Value());
        newItem->GetMusicInfoTag()->SetTitle(child->Value());
      }
    }
    pElement = pTrackElement->FirstChildElement("creator");
    if (pElement)
    {
      TiXmlNode* child = pElement->FirstChild();
      if (child)
      {
        newItem->GetMusicInfoTag()->SetArtist(child->Value());
      }
    }
    pElement = pTrackElement->FirstChildElement("album");
    if (pElement)
    {
      TiXmlNode* child = pElement->FirstChild();
      if (child)
      {
        newItem->GetMusicInfoTag()->SetAlbum(child->Value());
      }
    }
   
    pElement = pTrackElement->FirstChildElement("duration");
    if (pElement)
    {
      TiXmlNode* child = pElement->FirstChild();
      if (child)
      {
        int iDuration = atoi(child->Value())/1000;
        newItem->GetMusicInfoTag()->SetDuration(iDuration);
      }
    }
    newItem->FillInDefaultIcon();
    pElement = pTrackElement->FirstChildElement("image");
    if (pElement)
    {
      TiXmlNode* child = pElement->FirstChild();
      if (child)
      {
        CStdString coverUrl = child->Value();
        if ((coverUrl != "") && (coverUrl.Find("noimage") == -1) && (coverUrl.Right(1) != "/"))
        {
          newItem->SetThumbnailImage(coverUrl);
        }
      }
    }
    //trackauth is needed for validating the track when scrobbling
    pElement = pTrackElement->FirstChildElement("lastfm:trackauth");
    if (pElement)
    {
      TiXmlNode* child = pElement->FirstChild();
      if (child)
      {
        CStdString trackAuth = child->Value();
        //abuse comment field for the track authcode
        newItem->GetMusicInfoTag()->SetComment(trackAuth);
      }
    }

    {
      CSingleLock lock(m_lockCache);
      m_RadioTrackQueue->Add(newItem);
    }
    pTrackElement = pTrackElement->NextSiblingElement();
  }
  //end parse
  CSingleLock lock(m_lockCache);
  int iNrCachedTracks = m_RadioTrackQueue->size();
  CLog::Log(LOGDEBUG, "%s: Done (time: %i ms)", __FUNCTION__, (int)(timeGetTime() - start));
  return iNrCachedTracks > 0;
}

void CLastFmManager::CacheTrackThumb(const int nrInitialTracksToAdd)
{
  DWORD start = timeGetTime();
  CSingleLock lock(m_lockCache);
  int iNrCachedTracks = m_RadioTrackQueue->size();
  CPicture pic;
  CFileCurl http;
  for (int i = 0; i < nrInitialTracksToAdd && i < iNrCachedTracks; i++)
  {
    CFileItemPtr item = (*m_RadioTrackQueue)[i];
    if (!item->GetMusicInfoTag()->Loaded())
    {
      //cache albumthumb, GetThumbnailImage contains the url to cache
      if (item->HasThumbnail())
      {
        CStdString coverUrl = item->GetThumbnailImage();
        CStdString crcFile;
        CStdString cachedFile;
        CStdString thumbFile;

        Crc32 crc;
        crc.ComputeFromLowerCase(coverUrl);
        crcFile.Format("%08x.tbn", (__int32)crc);
        URIUtils::AddFileToFolder(g_advancedSettings.m_cachePath, crcFile, cachedFile);
        URIUtils::AddFileToFolder(g_settings.GetLastFMThumbFolder(), crcFile, thumbFile);
        item->SetThumbnailImage("");
        try
        {
          //download to temp, then make a thumb
          if (CFile::Exists(thumbFile) || (http.Download(coverUrl, cachedFile) && pic.CreateThumbnail(cachedFile, thumbFile)))
          {
            if (CFile::Exists(cachedFile))
              CFile::Delete(cachedFile);
            item->SetThumbnailImage(thumbFile);
          }
        }
        catch(...)
        {
          CLog::Log(LOGERROR, "LastFmManager: exception while caching %s to %s.", coverUrl.c_str(), thumbFile.c_str());
        }
      }
      if (!item->HasThumbnail())
      {
        item->SetThumbnailImage("DefaultAlbumCover.png");
      }
      item->GetMusicInfoTag()->SetLoaded();
    }
  }
  CLog::Log(LOGDEBUG, "%s: Done (time: %i ms)", __FUNCTION__, (int)(timeGetTime() - start));
}

void CLastFmManager::AddToPlaylist(const int nrTracks)
{
  CPlayList& playlist = g_playlistPlayer.GetPlaylist(PLAYLIST_MUSIC);
  for (int i = 0; i < nrTracks; i++)
  {
    int iNrCachedTracks = m_RadioTrackQueue->size();
    if (iNrCachedTracks > 0)
    {
      CFileItemPtr item = (*m_RadioTrackQueue)[0];
      if (item->GetMusicInfoTag()->Loaded())
      {
        CMusicDatabase database;
        database.Open();
        database.SetPropertiesForFileItem(*item);
        CSingleLock lock(m_lockCache);
        m_RadioTrackQueue->Remove(0);
        CSingleLock lock2(m_lockPlaylist);
        playlist.Add(item);
      }
      else 
      {
        break;
      }
    }
  }
}


void CLastFmManager::OnSongChange(CFileItem& newSong)
{
  if (IsRadioEnabled())
  {
    if (!newSong.IsLastFM())
    {
      StopRadio(true);
    }
    else
    { 
      DWORD start = timeGetTime();
      ReapSongs();
      MovePlaying();
      Update();
      SendUpdateMessage();
      
      CLog::Log(LOGDEBUG, "%s: Done (time: %i ms)", __FUNCTION__, (int)(timeGetTime() - start));
    }
  }
  m_CurrentSong.IsLoved = false;
  m_CurrentSong.IsBanned = false;
  m_CurrentSong.CurrentSong = &newSong;
}

void CLastFmManager::Update()
{
  int iNrTracks = 0;
  {
    CSingleLock lock(m_lockPlaylist);
    CPlayList& playlist = g_playlistPlayer.GetPlaylist(PLAYLIST_MUSIC);
    iNrTracks = playlist.size();
  }
  if (iNrTracks < XBMC_LASTFM_MINTRACKS)
  {
    AddToPlaylist(XBMC_LASTFM_MINTRACKS - iNrTracks);
    int iNrCachedTracks = 0;
    {
      CSingleLock lock(m_lockCache);
      iNrCachedTracks = m_RadioTrackQueue->size();
    }
    if (iNrCachedTracks == 0)
    {
      //get more tracks
      if (ThreadHandle() != NULL)
      {
        SetEvent(m_hWorkerEvent);
      }
    }
  }
}

bool CLastFmManager::ReapSongs()
{
  CSingleLock lock(m_lockPlaylist);
  CPlayList& playlist = g_playlistPlayer.GetPlaylist(PLAYLIST_MUSIC);

  // reap any played songs
  int iCurrentSong = g_playlistPlayer.GetCurrentSong();
  int i=0;
  while (i < playlist.size())
  {
    if (i < iCurrentSong)
    {
      playlist.Remove(i);
      iCurrentSong--;
    }
    else
      i++;
  }

  g_playlistPlayer.SetCurrentSong(iCurrentSong);
  return true;
}

bool CLastFmManager::MovePlaying()
{
  CSingleLock lock(m_lockPlaylist);
  // move current song to the top if its not there
  int iCurrentSong = g_playlistPlayer.GetCurrentSong();
  if (iCurrentSong > 0)
  {
    CLog::Log(LOGINFO,"LastFmManager: Moving currently playing song from %i to 0", iCurrentSong);
    CPlayList &playlist = g_playlistPlayer.GetPlaylist(PLAYLIST_MUSIC);
    CPlayList playlistTemp;
    playlistTemp.Add(playlist[iCurrentSong]);
    playlist.Remove(iCurrentSong);
    for (int i=0; i<playlist.size(); i++)
      playlistTemp.Add(playlist[i]);
    playlist.Clear();
    for (int i=0; i<playlistTemp.size(); i++)
      playlist.Add(playlistTemp[i]);
  }
  g_playlistPlayer.SetCurrentSong(0);
  return true;
}

void CLastFmManager::SendUpdateMessage()
{
  CGUIMessage msg(GUI_MSG_PLAYLIST_CHANGED, 0, 0);
  g_windowManager.SendThreadMessage(msg);
}

void CLastFmManager::OnStartup()
{
  SetPriority(THREAD_PRIORITY_NORMAL);
}

void CLastFmManager::Process()
{
  bool bLastShuffleState = g_playlistPlayer.IsShuffled(PLAYLIST_MUSIC);
  PLAYLIST::REPEAT_STATE LastRepeatState = g_playlistPlayer.GetRepeat(PLAYLIST_MUSIC);
  
  g_playlistPlayer.SetShuffle(PLAYLIST_MUSIC, false);
  g_playlistPlayer.SetRepeat(PLAYLIST_MUSIC, PLAYLIST::REPEAT_NONE);
  
  while (!m_bStop)
  {
    WaitForSingleObject(m_hWorkerEvent, INFINITE);
    if (m_bStop)
      break;
    int iNrCachedTracks = m_RadioTrackQueue->size();
    if (iNrCachedTracks == 0)
    {
      RequestRadioTracks();
    }
    CSingleLock lock(m_lockCache);
    iNrCachedTracks = m_RadioTrackQueue->size();
    CacheTrackThumb(iNrCachedTracks);
  }
	
	g_playlistPlayer.SetShuffle(PLAYLIST_MUSIC, bLastShuffleState);
  g_playlistPlayer.SetRepeat(PLAYLIST_MUSIC, LastRepeatState);
  
  CLog::Log(LOGINFO,"LastFM thread terminated");
}

void CLastFmManager::StopRadio(bool bKillSession /*= true*/)
{
  if (bKillSession) 
  {
	m_RadioSession = "";
  }
  if (m_ThreadHandle)
  {
    m_bStop = true;
    SetEvent(m_hWorkerEvent);
    StopThread();
  }
  m_CurrentSong.CurrentSong = NULL;
  m_RadioTrackQueue->Clear();
  {
    CSingleLock lock(m_lockPlaylist);
    //all last.fm tracks are now invalid, remove them from the playlist
    CPlayList &playlist = g_playlistPlayer.GetPlaylist(PLAYLIST_MUSIC);
    for (int i = playlist.size() - 1; i >= 0; i--)
    {
      if (playlist[i]->IsLastFM())
      {
        playlist.Remove(i);
      }
    }
  }
  
  if (!bKillSession)
  {
    SendUpdateMessage();
  }
}

void CLastFmManager::CreateMD5Hash(const CStdString& bufferToHash, CStdString& hash)
{
  XBMC::MD5 md5state;
  md5state.append(bufferToHash);
  md5state.getDigest(hash);
  hash.ToLower();
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<methodCall>
<methodName>method</methodName>
<params>
<param><value><string>user</string></value></param>
<param><value><string>challenge</string></value></param>
<param><value><string>auth</string></value></param>
<param><value><string>artist</string></value></param>
<param><value><string>title</string></value></param>
</params>
</methodCall>
*/
bool CLastFmManager::CallXmlRpc(const CStdString& action, const CStdString& artist, const CStdString& title)
{
  CStdString strUserName = g_guiSettings.GetString("scrobbler.lastfmusername");
  CStdString strPassword = g_guiSettings.GetString("scrobbler.lastfmpassword");
  if (strUserName.IsEmpty() || strPassword.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm CallXmlRpc no username or password set.");
    return false;
  }
  if (artist.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm CallXmlRpc no artistname provided.");
    return false;
  }
  if (title.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm CallXmlRpc no tracktitle provided.");
    return false;
  }
  
  char ti[20];
  time_t rawtime;
  time ( &rawtime );
  struct tm *now = gmtime(&rawtime);
  strftime(ti, sizeof(ti), "%Y-%m-%d %H:%M:%S", now);
  CStdString strChallenge = ti;

  CStdString strAuth;
  CreateMD5Hash(strPassword, strAuth);
  strAuth.append(strChallenge);
  CreateMD5Hash(strAuth, strAuth);

  //create request xml
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );
	
	TiXmlElement * elMethodCall = new TiXmlElement( "methodCall" );
	doc.LinkEndChild( elMethodCall );

  TiXmlElement * elMethodName = new TiXmlElement( "methodName" );
	elMethodCall->LinkEndChild( elMethodName );
	TiXmlText * txtAction = new TiXmlText( action );
	elMethodName->LinkEndChild( txtAction );

  TiXmlElement * elParams = new TiXmlElement( "params" );
	elMethodCall->LinkEndChild( elParams );

  TiXmlElement * elParam = new TiXmlElement( "param" );
	elParams->LinkEndChild( elParam );
  TiXmlElement * elValue = new TiXmlElement( "value" );
	elParam->LinkEndChild( elValue );
  TiXmlElement * elString = new TiXmlElement( "string" );
	elValue->LinkEndChild( elString );
	TiXmlText * txtParam = new TiXmlText( strUserName );
	elString->LinkEndChild( txtParam );

  elParam = new TiXmlElement( "param" );
	elParams->LinkEndChild( elParam );
  elValue = new TiXmlElement( "value" );
	elParam->LinkEndChild( elValue );
  elString = new TiXmlElement( "string" );
	elValue->LinkEndChild( elString );
	txtParam = new TiXmlText( strChallenge );
	elString->LinkEndChild( txtParam );

  elParam = new TiXmlElement( "param" );
	elParams->LinkEndChild( elParam );
  elValue = new TiXmlElement( "value" );
	elParam->LinkEndChild( elValue );
  elString = new TiXmlElement( "string" );
	elValue->LinkEndChild( elString );
	txtParam = new TiXmlText( strAuth );
	elString->LinkEndChild( txtParam );

  elParam = new TiXmlElement( "param" );
	elParams->LinkEndChild( elParam );
  elValue = new TiXmlElement( "value" );
	elParam->LinkEndChild( elValue );
  elString = new TiXmlElement( "string" );
	elValue->LinkEndChild( elString );
	txtParam = new TiXmlText( artist );
	elString->LinkEndChild( txtParam );

  elParam = new TiXmlElement( "param" );
	elParams->LinkEndChild( elParam );
  elValue = new TiXmlElement( "value" );
	elParam->LinkEndChild( elValue );
  elString = new TiXmlElement( "string" );
	elValue->LinkEndChild( elString );
	txtParam = new TiXmlText( title );
	elString->LinkEndChild( txtParam );

  CStdString strBody;
  strBody << doc;

  CFileCurl http;
  CStdString html;
  CStdString url = "http://ws.audioscrobbler.com/1.0/rw/xmlrpc.php";
  http.SetMimeType("text/xml");
  if (!http.Post(url, strBody, html))
  {
    CLog::Log(LOGERROR, "Last.fm action %s failed.", action.c_str());
    return false;
  }

  if (html.Find("fault") >= 0)
  {
    CLog::Log(LOGERROR, "Last.fm return failed response: %s", html.c_str());
    return false;
  }
  return true;
}

bool CLastFmManager::Love(bool askConfirmation)
{
  if (IsLastFmEnabled() && CanLove())
  {
    const CMusicInfoTag* infoTag = g_infoManager.GetCurrentSongTag();
    if (infoTag)
    {
      CStdString strTitle = infoTag->GetTitle();
      CStdString strArtist = infoTag->GetArtist();

      CStdString strInfo;
      strInfo.Format("%s - %s", strArtist, strTitle);
      if (!askConfirmation || CGUIDialogYesNo::ShowAndGetInput(g_localizeStrings.Get(15200), g_localizeStrings.Get(15287), strInfo, ""))
      {
        CStdString strMessage;
        if (Love(*infoTag))
        {
          strMessage.Format(g_localizeStrings.Get(15289), strTitle);
          g_application.m_guiDialogKaiToast.QueueNotification(CGUIDialogKaiToast::Info, g_localizeStrings.Get(15200), strMessage, 7000, false);
          return true;
        }
        else 
        {
          strMessage.Format(g_localizeStrings.Get(15290), strTitle);
          g_application.m_guiDialogKaiToast.QueueNotification(CGUIDialogKaiToast::Error, g_localizeStrings.Get(15200), strMessage, 7000, false);
          return false;
        }
      }
    }
  }
  return false;
}

bool CLastFmManager::Ban(bool askConfirmation)
{
  if (IsLastFmEnabled() && IsRadioEnabled() && CanBan())
  {
    const CMusicInfoTag* infoTag = g_infoManager.GetCurrentSongTag();
    if (infoTag)
    {
      CStdString strTitle = infoTag->GetTitle();
      CStdString strArtist = infoTag->GetArtist();

      CStdString strInfo;
      strInfo.Format("%s - %s", strArtist, strTitle);
      if (!askConfirmation || CGUIDialogYesNo::ShowAndGetInput(g_localizeStrings.Get(15200), g_localizeStrings.Get(15288), strInfo, ""))
      {
        CStdString strMessage;
        if (Ban(*infoTag))
        {
          strMessage.Format(g_localizeStrings.Get(15291), strTitle);
          g_application.m_guiDialogKaiToast.QueueNotification(CGUIDialogKaiToast::Info, g_localizeStrings.Get(15200), strMessage, 7000, false);
          return true;
        }
        else 
        {
          strMessage.Format(g_localizeStrings.Get(15292), strTitle);
          g_application.m_guiDialogKaiToast.QueueNotification(CGUIDialogKaiToast::Warning, g_localizeStrings.Get(15200), strMessage, 7000, false);
          return false;
        }
      }
    }
  }
  return false;
}

bool CLastFmManager::Love(const CMusicInfoTag& musicinfotag)
{
  if (!IsLastFmEnabled())
  {
    CLog::Log(LOGERROR, "LastFmManager Love, lastfm is not enabled.");
    return false;
  }
  
  CStdString strTitle = musicinfotag.GetTitle();
  CStdString strArtist = musicinfotag.GetArtist();
  
  CStdString strFilePath;
  if (m_CurrentSong.CurrentSong && !m_CurrentSong.CurrentSong->IsLastFM())
  {
    //path to update the rating for
    strFilePath = m_CurrentSong.CurrentSong->GetPath();
  }
  if (CallXmlRpc("loveTrack",strArtist, strTitle))
  {
    m_CurrentSong.IsLoved = true;
    //update the rating to 5, we loved it.
    CMusicInfoTag newTag(musicinfotag);
    newTag.SetRating('5');
    g_infoManager.SetCurrentSongTag(newTag);
    //try updating the rating in the database if it's a local file.
    CMusicDatabase musicdatabase;
    if (musicdatabase.Open())
    {
      CSong song;
      //update if the song exists in our database and there is no rating yet.
      if (musicdatabase.GetSongByFileName(strFilePath, song) && song.rating == '0')
      {
        musicdatabase.SetSongRating(strFilePath, '5');
      }
      musicdatabase.Close();
    }
    return true;
  }
  return false;
}

bool CLastFmManager::Ban(const CMusicInfoTag& musicinfotag)
{
  if (!IsRadioEnabled())
  {
    CLog::Log(LOGERROR, "LastFmManager Ban, radio is not active");
    return false;
  }
  
  if (CallXmlRpc("banTrack", musicinfotag.GetArtist(), musicinfotag.GetTitle()))
  {
    //we banned this track so skip to the next track
    g_applicationMessenger.ExecBuiltIn("playercontrol(next)");
    m_CurrentSong.IsBanned = true;
    return true;
  }
  return false;
}

bool CLastFmManager::Unlove(const CMusicInfoTag& musicinfotag, bool askConfirmation /*= true*/)
{
  if (!IsLastFmEnabled())
  {
    CLog::Log(LOGERROR, "LastFmManager Unlove, lasfm is not enabled");
    return false;
  }

  CStdString strTitle = musicinfotag.GetTitle();
  CStdString strArtist = musicinfotag.GetArtist();

  if (strArtist.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm Unlove no artistname provided.");
    return false;
  }
  if (strTitle.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm Unlove no tracktitle provided.");
    return false;
  }

  CStdString strInfo;
  strInfo.Format("%s - %s", strArtist, strTitle);
  if (!askConfirmation || CGUIDialogYesNo::ShowAndGetInput(g_localizeStrings.Get(15200), g_localizeStrings.Get(15297), strInfo, ""))
  {
    if (CallXmlRpc("unLoveTrack", strArtist, strTitle))
    {
      //update our local rating, now this is tricky because we only have an artist and title
      //and don't know if it was a local or radio song.
      //So we're going to try to get it from the database and check if the rating is 5,
      //if it is we can assume this was the song we loved before.
      CMusicDatabase musicdatabase;
      if (musicdatabase.Open())
      {
        long songid = musicdatabase.GetSongByArtistAndAlbumAndTitle(strArtist, "%", strTitle);
        if (songid > 0)
        {
          CSong song;
          musicdatabase.GetSongById(songid, song);
          if (song.rating == '5')
          {
            //reset the rating
            musicdatabase.SetSongRating(song.strFileName, '0');
          }
        }
        musicdatabase.Close();
      }

      return true;
    }
  }
  return false;
}

bool CLastFmManager::Unban(const CMusicInfoTag& musicinfotag, bool askConfirmation /*= true*/)
{
  if (!IsLastFmEnabled())
  {
    CLog::Log(LOGERROR, "LastFmManager Unban, lasfm is not enabled");
    return false;
  }
  
  CStdString strTitle = musicinfotag.GetTitle();
  CStdString strArtist = musicinfotag.GetArtist();

  if (strArtist.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm Unban no artistname provided.");
    return false;
  }
  if (strTitle.IsEmpty())
  {
    CLog::Log(LOGERROR, "Last.fm Unban no tracktitle provided.");
    return false;
  }

  CStdString strInfo;
  strInfo.Format("%s - %s", strArtist, strTitle);
  if (!askConfirmation || CGUIDialogYesNo::ShowAndGetInput(g_localizeStrings.Get(15200), g_localizeStrings.Get(15298), strInfo, ""))
  {
    return CallXmlRpc("unBanTrack", strArtist, strTitle);
  }
  return false;
}

bool CLastFmManager::IsLastFmEnabled()
{
  return (
    !g_guiSettings.GetString("scrobbler.lastfmusername").IsEmpty() && 
    !g_guiSettings.GetString("scrobbler.lastfmpassword").IsEmpty()
  );
}

bool CLastFmManager::CanLove()
{
  return (
    m_CurrentSong.CurrentSong &&
    !m_CurrentSong.IsLoved &&
    IsLastFmEnabled() &&
    (m_CurrentSong.CurrentSong->IsLastFM() || 
    (
      m_CurrentSong.CurrentSong->HasMusicInfoTag() && 
      m_CurrentSong.CurrentSong->GetMusicInfoTag()->Loaded() &&
      !m_CurrentSong.CurrentSong->GetMusicInfoTag()->GetArtist().IsEmpty() && 
      !m_CurrentSong.CurrentSong->GetMusicInfoTag()->GetTitle().IsEmpty()
    ))
  );
}

bool CLastFmManager::CanBan()
{
  return (m_CurrentSong.CurrentSong && !m_CurrentSong.IsBanned && m_CurrentSong.CurrentSong->IsLastFM());
}

bool CLastFmManager::CanScrobble(const CFileItem &fileitem)
{
  return (
    (!fileitem.IsInternetStream() && g_guiSettings.GetBool("scrobbler.lastfmsubmit")) || 
    (fileitem.IsLastFM() && g_guiSettings.GetBool("scrobbler.lastfmlastfmsubmitradio"))
  );
}
