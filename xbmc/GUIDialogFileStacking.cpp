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
#include "GUIDialogFileStacking.h"
#include "LocalizeStrings.h"
#include "FileItem.h"
#include "Settings.h" // for the ifdef

using namespace std;

#define STACK_LIST 450

CGUIDialogFileStacking::CGUIDialogFileStacking(void)
    : CGUIDialog(WINDOW_DIALOG_FILESTACKING, "DialogFileStacking.xml")
{
  m_iSelectedFile = -1;
  m_iNumberOfFiles = 0;
  m_stackItems = new CFileItemList;
}

CGUIDialogFileStacking::~CGUIDialogFileStacking(void)
{
  delete m_stackItems;
}

bool CGUIDialogFileStacking::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_WINDOW_DEINIT:
    CGUIDialog::OnMessage(message);
    if (m_stackItems)
      m_stackItems->Clear();
    return true;
  case GUI_MSG_WINDOW_INIT:
    {
      CGUIDialog::OnMessage(message);
      m_iSelectedFile = -1;
      if (GetControl(STACK_LIST))
      { // have the new stack list instead - fill it up
        SendMessage(GUI_MSG_LABEL_RESET, GetID(), STACK_LIST);
        for (int i = 0; i < m_iNumberOfFiles; i++)
        {
          CStdString label;
          label.Format(g_localizeStrings.Get(23051).c_str(), i+1);
          CFileItemPtr item(new CFileItem(label));
          m_stackItems->Add(item);
        }
        CGUIMessage msg(GUI_MSG_LABEL_BIND, GetID(), STACK_LIST, 0, 0, m_stackItems);
        OnMessage(msg);
      }
      return true;
    }
    break;

  case GUI_MSG_CLICKED:
    {
      if (message.GetSenderId() == STACK_LIST && (message.GetParam1() == ACTION_SELECT_ITEM ||
                                                  message.GetParam1() == ACTION_MOUSE_LEFT_CLICK))
      {
        // grab the selected item
        CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), STACK_LIST);
        OnMessage(msg);
        m_iSelectedFile = msg.GetParam1() + 1;
      }
      Close();
      return true;
    }
    break;
  }
  return CGUIDialog::OnMessage(message);
}

int CGUIDialogFileStacking::GetSelectedFile() const
{
  return m_iSelectedFile;
}

void CGUIDialogFileStacking::SetNumberOfFiles(int iFiles)
{
  m_iNumberOfFiles = iFiles;
}
