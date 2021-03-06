#pragma once

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

class CFileItem;
class CMediaSource;

#include <vector>
#include <map>

typedef std::vector<CMediaSource> VECSOURCES;

typedef std::map<CStdString, CStdString> MAPPASSWORDS;
typedef std::map<CStdString, CStdString>::iterator IMAPPASSWORDS;

typedef enum
{
  LOCK_MODE_UNKNOWN            = -1,
  LOCK_MODE_EVERYONE           =  0,
  LOCK_MODE_NUMERIC            =  1,
  LOCK_MODE_GAMEPAD            =  2,
  LOCK_MODE_QWERTY             =  3,
  LOCK_MODE_SAMBA              =  4,
  LOCK_MODE_EEPROM_PARENTAL    =  5
} LockType;

class CGUIPassword
{
public:
  CGUIPassword(void);
  virtual ~CGUIPassword(void);
  bool IsItemUnlocked(CFileItem* pItem, const CStdString &strType);
  bool IsItemUnlocked(CMediaSource* pItem, const CStdString &strType);
  bool CheckLock(LockType btnType, const CStdString& strPassword, int iHeading);
  bool CheckLock(LockType btnType, const CStdString& strPassword, int iHeading, bool& bCanceled);
  bool IsProfileLockUnlocked(int iProfile=-1);
  bool IsProfileLockUnlocked(int iProfile, bool& bCanceled);
  bool IsMasterLockUnlocked(bool bPromptUser);
  bool IsMasterLockUnlocked(bool bPromptUser, bool& bCanceled);
  
  void UpdateMasterLockRetryCount(bool bResetCount);
  bool GetSMBShareUserPassword();
  void SetSMBShare(const CStdString &strShare);
  CStdString GetSMBShare();
  bool CheckStartUpLock();
  bool CheckMenuLock(int iWindowID);
  bool SetMasterLockMode(bool bDetails=true);
  CStdString GetSMBAuthFilename(const CStdString& strAuth);
  bool LockSource(const CStdString& strType, const CStdString& strName, bool bState);
  void LockSources(bool lock);
  void RemoveSourceLocks();
  bool IsDatabasePathUnlocked(CStdString& strPath, VECSOURCES& VECSOURCES);

	MAPPASSWORDS			m_mapSMBPasswordCache; // SMB share password cache

  bool bMasterUser;
  int iMasterLockRetriesLeft;
protected:
  CStdString m_SMBShare;
private:
  int VerifyPassword(LockType btnType, const CStdString& strPassword, const CStdString& strHeading);
};

extern CGUIPassword g_passwordManager;

