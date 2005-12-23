#pragma once
#include "GUIWindowMusicBase.h"

class CGUIWindowMusicSongs : public CGUIWindowMusicBase
{
public:
  CGUIWindowMusicSongs(void);
  virtual ~CGUIWindowMusicSongs(void);

  virtual bool OnMessage(CGUIMessage& message);

protected:
  virtual void UpdateButtons();
  virtual void OnFileItemFormatLabel(CFileItem* pItem);
  virtual void OnClick(int iItem);
  virtual void OnPopupMenu(int iItem);
  virtual void DoSearch(const CStdString& strSearch, CFileItemList& items);
  virtual void GetDirectoryHistoryString(const CFileItem* pItem, CStdString& strHistoryString);
  virtual void OnRetrieveMusicInfo(CFileItemList& items);
  virtual void OnScan();

  // new method
  virtual void PlayItem(int iItem);

  void LoadDirectoryCache(const CStdString& strDirectory, MAPFILEITEMS& items);
  void SaveDirectoryCache(const CStdString& strDirectory, CFileItemList& items);
  void DeleteDirectoryCache();
  void DeleteRemoveableMediaDirectoryCache();
};
