#pragma once
/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://xbmc.org
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

#include "tinyXML/tinyxml.h"
#include "XMLUtils.h"
#include "RegExp.h"
#include "FileItem.h"
#include "PlayerCoreFactory.h"

class CPlayerSelectionRule
{
public:
  CPlayerSelectionRule(TiXmlElement* rule);
  virtual ~CPlayerSelectionRule();

  //bool Matches(const CFileItem& item) const;
  //CStdString GetPlayerName() const;
  void GetPlayers(const CFileItem& item, VECPLAYERCORES &vecCores);

private:
  int GetTristate(const char* szValue) const;
  void Initialize(TiXmlElement* pRule);
  PLAYERCOREID GetPlayerCore();

  CStdString m_name;

  int m_tAudio;
  int m_tVideo;
  int m_tInternetStream;

  int m_tDVD;
  int m_tDVDFile;
  int m_tDVDImage;

  CStdString m_protocols;
  CStdString m_fileTypes;
  CStdString m_mimeTypes;
  CStdString m_fileName;

  CStdString m_playerName;
  PLAYERCOREID m_playerCoreId;

  std::vector<CPlayerSelectionRule *> vecSubRules;
};
