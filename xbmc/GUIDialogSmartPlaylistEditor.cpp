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
#include "GUIDialogSmartPlaylistEditor.h"
#include "GUIDialogKeyboard.h"
#include "Util.h"
#include "utils/URIUtils.h"
#include "GUIDialogSmartPlaylistRule.h"
#include "GUIWindowManager.h"
#include "FileSystem/File.h"
#include "GUISettings.h"
#include "Settings.h"
#include "FileItem.h"

using namespace std;

#define CONTROL_HEADING         2
#define CONTROL_RULE_LIST       10
#define CONTROL_NAME            12
#define CONTROL_RULE_ADD        13
#define CONTROL_RULE_REMOVE     14
#define CONTROL_RULE_EDIT       15
#define CONTROL_MATCH           16
#define CONTROL_LIMIT           17
#define CONTROL_ORDER_FIELD     18
#define CONTROL_ORDER_DIRECTION 19

#define CONTROL_OK              20
#define CONTROL_CANCEL          21
#define CONTROL_TYPE            22

typedef struct
{
  CGUIDialogSmartPlaylistEditor::PLAYLIST_TYPE type;
  char string[13];
  int localizedString;
} translateType;

static const translateType types[] = { { CGUIDialogSmartPlaylistEditor::TYPE_SONGS, "songs", 134 },
                                       { CGUIDialogSmartPlaylistEditor::TYPE_ALBUMS, "albums", 132 },
                                       { CGUIDialogSmartPlaylistEditor::TYPE_MIXED, "mixed", 20395 },
                                       { CGUIDialogSmartPlaylistEditor::TYPE_MUSICVIDEOS, "musicvideos", 20389 },
                                       { CGUIDialogSmartPlaylistEditor::TYPE_MOVIES, "movies", 20342 },
                                       { CGUIDialogSmartPlaylistEditor::TYPE_TVSHOWS, "tvshows", 20343 },
                                       { CGUIDialogSmartPlaylistEditor::TYPE_EPISODES, "episodes", 20360 }
                                     };

#define NUM_TYPES (sizeof(types) / sizeof(translateType))

CGUIDialogSmartPlaylistEditor::CGUIDialogSmartPlaylistEditor(void)
    : CGUIDialog(WINDOW_DIALOG_SMART_PLAYLIST_EDITOR, "SmartPlaylistEditor.xml")
{
  m_cancelled = false;
  m_ruleLabels = new CFileItemList;
}

CGUIDialogSmartPlaylistEditor::~CGUIDialogSmartPlaylistEditor()
{
  delete m_ruleLabels;
}

bool CGUIDialogSmartPlaylistEditor::OnAction(const CAction &action)
{
  if (action.id == ACTION_PREVIOUS_MENU)
    m_cancelled = true;
  return CGUIDialog::OnAction(action);
}

bool CGUIDialogSmartPlaylistEditor::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      int iAction = message.GetParam1();
      if (iControl == CONTROL_RULE_LIST && (iAction == ACTION_SELECT_ITEM || iAction == ACTION_MOUSE_LEFT_CLICK))
        OnRuleList(GetSelectedItem());
      else if (iControl == CONTROL_RULE_ADD)
        OnRuleAdd();
      else if (iControl == CONTROL_RULE_EDIT)
        OnRuleList(GetSelectedItem());
      else if (iControl == CONTROL_RULE_REMOVE)
        OnRuleRemove(GetSelectedItem());
      else if (iControl == CONTROL_NAME)
        OnEditChanged(iControl, m_playlist.m_playlistName);
      else if (iControl == CONTROL_OK)
        OnOK();
      else if (iControl == CONTROL_CANCEL)
        OnCancel();
      else if (iControl == CONTROL_MATCH)
        OnMatch();
      else if (iControl == CONTROL_LIMIT)
        OnLimit();
      else if (iControl == CONTROL_ORDER_FIELD)
        OnOrder();
      else if (iControl == CONTROL_ORDER_DIRECTION)
        OnOrderDirection();
      else if (iControl == CONTROL_TYPE)
        OnType();
      else
        return CGUIDialog::OnMessage(message);
      return true;
    }
    break;
  case GUI_MSG_WINDOW_INIT:
    {
      m_cancelled = false;
      UpdateButtons();
    }
    break;
  case GUI_MSG_WINDOW_DEINIT:
    {
      CGUIDialog::OnMessage(message);
      // clear the rule list
      CGUIMessage msg(GUI_MSG_LABEL_RESET, GetID(), CONTROL_RULE_LIST);
      OnMessage(msg);
      m_ruleLabels->Clear();
    }
    break;
  case GUI_MSG_FOCUSED:
    if (message.GetControlId() == CONTROL_RULE_REMOVE ||
        message.GetControlId() == CONTROL_RULE_EDIT)
      HighlightItem(GetSelectedItem());
    else
      HighlightItem(-1);
    break;
  }
  return CGUIDialog::OnMessage(message);
}

void CGUIDialogSmartPlaylistEditor::OnRuleList(int item)
{
  if (item < 0 || item >= (int)m_playlist.m_playlistRules.size()) return;

  CSmartPlaylistRule rule = m_playlist.m_playlistRules[item];

  if (CGUIDialogSmartPlaylistRule::EditRule(rule,m_playlist.GetType()))
    m_playlist.m_playlistRules[item] = rule;

  UpdateButtons();
}

void CGUIDialogSmartPlaylistEditor::OnOK()
{
  // save our playlist
  if (m_path.IsEmpty())
  {
    CStdString filename(m_playlist.m_playlistName);
    CStdString path;
    if (CGUIDialogKeyboard::ShowAndGetInput(filename, g_localizeStrings.Get(16013), false))
    {
      path = URIUtils::AddFileToFolder(g_guiSettings.GetString("system.playlistspath"),m_playlist.GetSaveLocation());
      path = URIUtils::AddFileToFolder(path, filename);
    }
    else
      return;
    if (URIUtils::GetExtension(path) != ".xsp")
      path += ".xsp";

    // should we check whether we should overwrite?
    m_path = path;
  }
  else
  {
    // check if we need to actually change the save location for this playlist
    // this occurs if the user switches from music video <> songs <> mixed
    if (m_path.Left(g_guiSettings.GetString("system.playlistspath").size()).Equals(g_guiSettings.GetString("system.playlistspath"))) // fugly, well aware
    {
      CStdString filename = URIUtils::GetFileName(m_path);
      CStdString strFolder = m_path.Mid(g_guiSettings.GetString("system.playlistspath").size(),m_path.size()-filename.size()-g_guiSettings.GetString("system.playlistspath").size()-1);
      if (strFolder != m_playlist.GetSaveLocation())
      { // move to the correct folder
        XFILE::CFile::Delete(m_path);
        m_path = URIUtils::AddFileToFolder(g_guiSettings.GetString("system.playlistspath"),m_playlist.GetSaveLocation());
        m_path = URIUtils::AddFileToFolder(m_path, filename);
      }
    }
  }

  m_playlist.Save(m_path);

  m_cancelled = false;
  Close();
}

void CGUIDialogSmartPlaylistEditor::OnCancel()
{
  m_cancelled = true;
  Close();
}

void CGUIDialogSmartPlaylistEditor::OnMatch()
{
  CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_MATCH);
  OnMessage(msg);
  m_playlist.m_matchAllRules = (msg.GetParam1() == 0);
  UpdateButtons();
}

void CGUIDialogSmartPlaylistEditor::OnLimit()
{
  CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_LIMIT);
  OnMessage(msg);
  m_playlist.m_limit = msg.GetParam1();
  UpdateButtons();
}

void CGUIDialogSmartPlaylistEditor::OnType()
{
  CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_TYPE);
  OnMessage(msg);
  m_playlist.SetType(ConvertType((PLAYLIST_TYPE)msg.GetParam1()));
  UpdateButtons();
}

void CGUIDialogSmartPlaylistEditor::OnOrder()
{
  CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_ORDER_FIELD);
  OnMessage(msg);
  m_playlist.m_orderField = (CSmartPlaylistRule::DATABASE_FIELD)msg.GetParam1();
  UpdateButtons();
}

void CGUIDialogSmartPlaylistEditor::OnOrderDirection()
{
  m_playlist.m_orderAscending = !m_playlist.m_orderAscending;
  UpdateButtons();
}

void CGUIDialogSmartPlaylistEditor::UpdateButtons()
{
  CONTROL_ENABLE(CONTROL_OK); // always enabled since we can have no rules -> match everything (as we do with default partymode playlists)

  if (m_playlist.m_playlistRules.size() <= 1)
  {
    CONTROL_DISABLE(CONTROL_RULE_REMOVE);
    CONTROL_DISABLE(CONTROL_MATCH);
  }
  else
  {
    CONTROL_ENABLE(CONTROL_RULE_REMOVE);
    CONTROL_ENABLE(CONTROL_MATCH);
  }
  // name
  if (m_mode == "partyvideo" || m_mode == "partymusic")
  {
    SET_CONTROL_LABEL2(CONTROL_NAME, g_localizeStrings.Get(16035));
    CONTROL_DISABLE(CONTROL_NAME);
  }
  else
  {
  SET_CONTROL_LABEL2(CONTROL_NAME, m_playlist.m_playlistName);
  }

  int currentItem = GetSelectedItem();
  CGUIMessage msgReset(GUI_MSG_LABEL_RESET, GetID(), CONTROL_RULE_LIST);
  OnMessage(msgReset);
  m_ruleLabels->Clear();
  for (unsigned int i = 0; i < m_playlist.m_playlistRules.size(); i++)
  {
    CFileItemPtr item(new CFileItem("", false));
    if (m_playlist.m_playlistRules[i].m_field == CSmartPlaylistRule::FIELD_NONE)
      item->SetLabel(g_localizeStrings.Get(21423));
    else
      item->SetLabel(m_playlist.m_playlistRules[i].GetLocalizedRule());
    m_ruleLabels->Add(item);
  }
  CGUIMessage msg(GUI_MSG_LABEL_BIND, GetID(), CONTROL_RULE_LIST, 0, 0, m_ruleLabels);
  OnMessage(msg);
  SendMessage(GUI_MSG_ITEM_SELECT, GetID(), CONTROL_RULE_LIST, currentItem);

  if (m_playlist.m_orderAscending)
  {
    CONTROL_SELECT(CONTROL_ORDER_DIRECTION);
  }
  else
  {
    CONTROL_DESELECT(CONTROL_ORDER_DIRECTION);
  }

  // sort out the order fields
  {
    CGUIMessage msg(GUI_MSG_LABEL_RESET, GetID(), CONTROL_ORDER_FIELD);
    OnMessage(msg);
  }
  vector<CSmartPlaylistRule::DATABASE_FIELD> fields = CSmartPlaylistRule::GetFields(m_playlist.GetType(), true);
  for (unsigned int i = 0; i < fields.size(); i++)
  {
    CGUIMessage msg(GUI_MSG_LABEL_ADD, GetID(), CONTROL_ORDER_FIELD, fields[i]);
    msg.SetLabel(CSmartPlaylistRule::GetLocalizedField(fields[i]));
    OnMessage(msg);
  }
  {
    CGUIMessage msg(GUI_MSG_ITEM_SELECT, GetID(), CONTROL_ORDER_FIELD, m_playlist.m_orderField);
    OnMessage(msg);
  }
}

void CGUIDialogSmartPlaylistEditor::OnWindowLoaded()
{
  CGUIDialog::OnWindowLoaded();
  ChangeButtonToEdit(CONTROL_NAME, true); // true for single label
  SendMessage(GUI_MSG_SET_TYPE, CONTROL_NAME, 0, 16012);
  // setup the match spinner
  {
    CGUIMessage msg(GUI_MSG_LABEL_ADD, GetID(), CONTROL_MATCH, 0);
    msg.SetLabel(21425);
    OnMessage(msg);
  }
  {
    CGUIMessage msg(GUI_MSG_LABEL_ADD, GetID(), CONTROL_MATCH, 1);
    msg.SetLabel(21426);
    OnMessage(msg);
  }
  SendMessage(GUI_MSG_ITEM_SELECT, CONTROL_MATCH, m_playlist.m_matchAllRules ? 0 : 1);
  // and now the limit spinner
  {
    CGUIMessage msg(GUI_MSG_LABEL_ADD, GetID(), CONTROL_LIMIT, 0);
    msg.SetLabel(21428);
    OnMessage(msg);
  }
  const int limits[] = { 10, 25, 50, 100, 250, 500, 1000 };
  for (unsigned int i = 0; i < sizeof(limits) / sizeof(int); i++)
  {
    CGUIMessage msg(GUI_MSG_LABEL_ADD, GetID(), CONTROL_LIMIT, limits[i]);
    CStdString label; label.Format(g_localizeStrings.Get(21436).c_str(), limits[i]);
    msg.SetLabel(label);
    OnMessage(msg);
  }
  SendMessage(GUI_MSG_ITEM_SELECT, CONTROL_LIMIT, m_playlist.m_limit);

  vector<PLAYLIST_TYPE> allowedTypes;
  if (m_mode.Equals("partymusic"))
  {
    allowedTypes.push_back(TYPE_SONGS);
    allowedTypes.push_back(TYPE_MIXED);
  }
  else if (m_mode.Equals("partyvideo"))
  {
    allowedTypes.push_back(TYPE_MUSICVIDEOS);
    allowedTypes.push_back(TYPE_MIXED);
  }
  else if (m_mode.Equals("music"))
  { // music types + mixed
    allowedTypes.push_back(TYPE_SONGS);
    allowedTypes.push_back(TYPE_ALBUMS);
    allowedTypes.push_back(TYPE_MIXED);
  }
  else if (m_mode.Equals("video"))
  { // general category for videos
    allowedTypes.push_back(TYPE_MOVIES);
    allowedTypes.push_back(TYPE_TVSHOWS);
    allowedTypes.push_back(TYPE_EPISODES);
    allowedTypes.push_back(TYPE_MUSICVIDEOS);
    allowedTypes.push_back(TYPE_MIXED);
  }
  // add to the spinner
  for (unsigned int i = 0; i < allowedTypes.size(); i++)
  {
    CGUIMessage msg(GUI_MSG_LABEL_ADD, GetID(), CONTROL_TYPE, allowedTypes[i]);
    msg.SetLabel(GetLocalizedType(allowedTypes[i]));
    OnMessage(msg);
  }
  // check our playlist type is allowed
  PLAYLIST_TYPE type = ConvertType(m_playlist.GetType());
  bool allowed = false;
  for (unsigned int i = 0; i < allowedTypes.size(); i++)
    if (type == allowedTypes[i])
      allowed = true;
  if (!allowed && allowedTypes.size())
    type = allowedTypes[0];

  SendMessage(GUI_MSG_ITEM_SELECT, CONTROL_TYPE, type);
  m_playlist.SetType(ConvertType(type));
}

CGUIDialogSmartPlaylistEditor::PLAYLIST_TYPE CGUIDialogSmartPlaylistEditor::ConvertType(const CStdString &type)
{
  for (unsigned int i = 0; i < NUM_TYPES; i++)
    if (type.Equals(types[i].string))
      return types[i].type;
  assert(false);
  return TYPE_SONGS;
}

int CGUIDialogSmartPlaylistEditor::GetLocalizedType(PLAYLIST_TYPE type)
{
  for (unsigned int i = 0; i < NUM_TYPES; i++)
    if (types[i].type == type)
      return types[i].localizedString;
  assert(false);
  return 0;
}

CStdString CGUIDialogSmartPlaylistEditor::ConvertType(PLAYLIST_TYPE type)
{
  for (unsigned int i = 0; i < NUM_TYPES; i++)
    if (types[i].type == type)
      return types[i].string;
  assert(false);
  return "songs";
}

int CGUIDialogSmartPlaylistEditor::GetSelectedItem()
{
  CGUIMessage message(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_RULE_LIST);
  OnMessage(message);
  return message.GetParam1();
}

void CGUIDialogSmartPlaylistEditor::HighlightItem(int item)
{
  for (int i = 0; i < m_ruleLabels->Size(); i++)
    (*m_ruleLabels)[i]->Select(false);
  if (item >= 0 && item < m_ruleLabels->Size())
    (*m_ruleLabels)[item]->Select(true);
  CGUIMessage msg(GUI_MSG_ITEM_SELECT, GetID(), CONTROL_RULE_LIST, item);
  OnMessage(msg);
}

void CGUIDialogSmartPlaylistEditor::OnRuleRemove(int item)
{
  if (item < 0 || item >= (int)m_playlist.m_playlistRules.size()) return;
  m_playlist.m_playlistRules.erase(m_playlist.m_playlistRules.begin() + item);

  UpdateButtons();
  if (item >= m_ruleLabels->Size())
    HighlightItem(m_ruleLabels->Size() - 1);
  else
    HighlightItem(item);
  if (m_ruleLabels->Size() <= 1)
  {
    SET_CONTROL_FOCUS(CONTROL_RULE_ADD, 0);
  }
}

void CGUIDialogSmartPlaylistEditor::OnRuleAdd()
{
  CSmartPlaylistRule rule;
  if (CGUIDialogSmartPlaylistRule::EditRule(rule,m_playlist.GetType()))
  {
    if (m_playlist.m_playlistRules.size() == 1 && m_playlist.m_playlistRules[0].m_field == CSmartPlaylistRule::FIELD_NONE)
      m_playlist.m_playlistRules[0] = rule;
    else
      m_playlist.m_playlistRules.push_back(rule);
  }
  UpdateButtons();
}

bool CGUIDialogSmartPlaylistEditor::NewPlaylist(const CStdString &type)
{
  CGUIDialogSmartPlaylistEditor *editor = (CGUIDialogSmartPlaylistEditor *)g_windowManager.GetWindow(WINDOW_DIALOG_SMART_PLAYLIST_EDITOR);
  if (!editor) return false;

  editor->m_path = "";
  editor->m_playlist = CSmartPlaylist();
  editor->m_playlist.m_playlistRules.push_back(CSmartPlaylistRule());
  editor->m_mode = type;
  editor->Initialize();
  editor->DoModal(g_windowManager.GetActiveWindow());
  return !editor->m_cancelled;
}

bool CGUIDialogSmartPlaylistEditor::EditPlaylist(const CStdString &path, const CStdString &type)
{
  CGUIDialogSmartPlaylistEditor *editor = (CGUIDialogSmartPlaylistEditor *)g_windowManager.GetWindow(WINDOW_DIALOG_SMART_PLAYLIST_EDITOR);
  if (!editor) return false;

  editor->m_mode = type;
  if (path.Equals(g_settings.GetUserDataItem("PartyMode.xsp")))
    editor->m_mode = "partymusic";
  if (path.Equals(g_settings.GetUserDataItem("PartyMode-Video.xsp")))
    editor->m_mode = "partyvideo";

  CSmartPlaylist playlist;
  bool loaded(playlist.Load(path));
  if (!loaded)
  { // failed to load
    if (!editor->m_mode.Left(5).Equals("party"))
      return false; // only edit normal playlists that exist
    // party mode playlists can be editted even if they don't exist
    playlist.m_playlistRules.push_back(CSmartPlaylistRule());
    playlist.SetType(editor->m_mode == "partymusic" ? "songs" : "musicvideos");
  }

  editor->m_playlist = playlist;
  editor->m_path = path;
  editor->Initialize();
  editor->DoModal(g_windowManager.GetActiveWindow());
  return !editor->m_cancelled;
}

