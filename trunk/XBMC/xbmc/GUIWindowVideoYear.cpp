// Todo:
//  - directory history
//  - if movie does not exists when play movie is called then show dialog asking to insert the correct CD
//  - show if movie has subs

#include "stdafx.h"
#include "GUIWindowVideoYear.h"
#include "Util.h"
#include "Utils/imdb.h"
#include "GUIWindowVideoInfo.h"
#include "NFOFile.h"
#include "PlayListPlayer.h"
#include "GUIThumbnailPanel.h"
#include "GUIPassword.h"
#include "SortFileItem.h"

#define CONTROL_BTNVIEWASICONS     2 
#define CONTROL_BTNSORTBY          3
#define CONTROL_BTNSORTASC         4
#define CONTROL_BTNTYPE            5
#define CONTROL_LIST              50
#define CONTROL_THUMBS            51
#define CONTROL_LABELFILES        12

#define CONTROL_PLAY_DVD           6
#define CONTROL_STACK              7
#define CONTROL_IMDB        9
#define CONTROL_BTNSHOWMODE       10
#define LABEL_YEAR              100


//****************************************************************************************************************************
CGUIWindowVideoYear::CGUIWindowVideoYear()
: CGUIWindowVideoBase(WINDOW_VIDEO_YEAR, "MyVideoYear.xml")
{
  m_Directory.m_strPath = "";
}

//****************************************************************************************************************************
CGUIWindowVideoYear::~CGUIWindowVideoYear()
{
}

//****************************************************************************************************************************
bool CGUIWindowVideoYear::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_WINDOW_INIT:
    {
      m_iShowMode = g_stSettings.m_iMyVideoYearShowMode;
    }
    break;
  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      if (iControl == CONTROL_BTNSORTBY) // sort by
      {
        if (!m_Directory.IsVirtualDirectoryRoot())
        {
          g_stSettings.m_iMyVideoYearSortMethod++;
          if (g_stSettings.m_iMyVideoYearSortMethod >= 3)
            g_stSettings.m_iMyVideoYearSortMethod = 0;
          g_settings.Save();
        }
        UpdateButtons();
        OnSort();
      }
      else if (iControl == CONTROL_BTNSORTASC) // sort asc
      {
        if (m_Directory.IsVirtualDirectoryRoot())
          g_stSettings.m_bMyVideoYearRootSortAscending = !g_stSettings.m_bMyVideoYearRootSortAscending;
        else
          g_stSettings.m_bMyVideoYearSortAscending = !g_stSettings.m_bMyVideoYearSortAscending;

        g_settings.Save();
        UpdateButtons();
        OnSort();
      }
      else if (iControl == CONTROL_BTNSHOWMODE)
	    {
        m_iShowMode++;
		    if (m_iShowMode > VIDEO_SHOW_WATCHED) m_iShowMode = VIDEO_SHOW_ALL;
		    g_stSettings.m_iMyVideoYearShowMode = m_iShowMode;
        g_settings.Save();
		    Update(m_Directory.m_strPath);
        return true;
      }
      else
        return CGUIWindowVideoBase::OnMessage(message);
    }
  }
  return CGUIWindowVideoBase::OnMessage(message);
}

//****************************************************************************************************************************
void CGUIWindowVideoYear::FormatItemLabels()
{
  for (int i = 0; i < m_vecItems.Size(); i++)
  {
    CFileItem* pItem = m_vecItems[i];
    if (g_stSettings.m_iMyVideoYearSortMethod == 0 || g_stSettings.m_iMyVideoYearSortMethod == 2)
    {
      if (pItem->m_bIsFolder) pItem->SetLabel2("");
      else
      {
        CStdString strRating;
        strRating.Format("%2.2f", pItem->m_fRating);
        pItem->SetLabel2(strRating);
      }
    }
    else
    {
      if (pItem->m_stTime.wYear)
      {
        CStdString strDateTime;
        strDateTime.Format("%i", pItem->m_stTime.wYear);
        pItem->SetLabel2(strDateTime);
      }
      else
        pItem->SetLabel2("");
    }
  }
}

void CGUIWindowVideoYear::SortItems(CFileItemList& items)
{
  int sortMethod;
  bool sortAscending;
  if (m_Directory.IsVirtualDirectoryRoot())
  {
    sortMethod = g_stSettings.m_iMyVideoYearRootSortMethod;
    sortAscending = g_stSettings.m_bMyVideoYearRootSortAscending;
  }
  else
  {
    sortMethod = g_stSettings.m_iMyVideoYearSortMethod;
    sortAscending = g_stSettings.m_bMyVideoYearSortAscending;
    if (g_stSettings.m_iMyVideoYearSortMethod == 1)
      sortAscending = !sortAscending;
  }
  switch (sortMethod)
  {
  case 1:
    items.Sort(sortAscending ? SSortFileItem::MovieYearAscending : SSortFileItem::MovieYearDescending); break;
  case 2:
    items.Sort(sortAscending ? SSortFileItem::MovieRatingAscending : SSortFileItem::MovieRatingDescending); break;
  default:
    if (g_guiSettings.GetBool("MyVideos.IgnoreTheWhenSorting"))
      items.Sort(sortAscending ? SSortFileItem::LabelAscendingNoThe : SSortFileItem::LabelDescendingNoThe);
    else
      items.Sort(sortAscending ? SSortFileItem::LabelAscending : SSortFileItem::LabelDescending);
    break;
  }
}

//****************************************************************************************************************************
bool CGUIWindowVideoYear::Update(const CStdString &strDirectory)
{
  // get selected item
  int iItem = m_viewControl.GetSelectedItem();
  CStdString strSelectedItem = "";
  if (iItem >= 0 && iItem < m_vecItems.Size())
  {
    CFileItem* pItem = m_vecItems[iItem];
    if (!pItem->IsParentFolder())
    {
      strSelectedItem = pItem->m_strPath;
      m_history.Set(strSelectedItem, m_Directory.m_strPath);
    }
  }
  ClearFileItems();
  m_Directory.m_strPath = strDirectory;
  if (m_Directory.IsVirtualDirectoryRoot())
  {
    VECMOVIEYEARS years;
    m_database.GetYears(years, m_iShowMode);
    // Display an error message if the database doesn't contain any years
    DisplayEmptyDatabaseMessage(years.empty());
    for (int i = 0; i < (int)years.size(); ++i)
    {
      CFileItem *pItem = new CFileItem(years[i]);
      pItem->m_strPath = years[i];
      pItem->m_bIsFolder = true;
      pItem->m_bIsShareOrDrive = false;
      m_vecItems.Add(pItem);
    }
    SET_CONTROL_LABEL(LABEL_YEAR, "");
  }
  else
  {
    if (!g_guiSettings.GetBool("MyVideos.HideParentDirItems"))
    {
      CFileItem *pItem = new CFileItem("..");
      pItem->m_strPath = "";
      pItem->m_bIsFolder = true;
      pItem->m_bIsShareOrDrive = false;
      m_vecItems.Add(pItem);
    }
    m_strParentPath = "";
    VECMOVIES movies;
    m_database.GetMoviesByYear(m_Directory.m_strPath, movies);
    for (int i = 0; i < (int)movies.size(); ++i)
    {
      CIMDBMovie movie = movies[i];
      // add the appropiate movies to m_vecItems based on the showmode
      if (
        (m_iShowMode == VIDEO_SHOW_ALL) ||
        (m_iShowMode == VIDEO_SHOW_WATCHED && movie.m_bWatched == true) ||
        (m_iShowMode == VIDEO_SHOW_UNWATCHED && movie.m_bWatched == false)
        )
      {
        // mark watched movies when showing all
        CStdString strTitle = movie.m_strTitle;
        if (m_iShowMode == VIDEO_SHOW_ALL && movie.m_bWatched == true)
          strTitle += " [W]";
        CFileItem *pItem = new CFileItem(strTitle);
        pItem->m_strPath = movie.m_strSearchString;
        pItem->m_bIsFolder = false;
        pItem->m_bIsShareOrDrive = false;

        CStdString strThumb;
        CUtil::GetVideoThumbnail(movie.m_strIMDBNumber, strThumb);
        if (CFile::Exists(strThumb))
          pItem->SetThumbnailImage(strThumb);
        pItem->m_fRating = movie.m_fRating;
        pItem->m_stTime.wYear = movie.m_iYear;
        m_vecItems.Add(pItem);
      }
    }
    SET_CONTROL_LABEL(LABEL_YEAR, m_Directory.m_strPath);
  }
  m_vecItems.SetThumbs();
  SetIMDBThumbs(m_vecItems);

  // Fill in default icons
  CStdString strPath;
  for (int i = 0; i < (int)m_vecItems.Size(); i++)
  {
    CFileItem* pItem = m_vecItems[i];
    strPath = pItem->m_strPath;
    // Fake a videofile
    pItem->m_strPath = pItem->m_strPath + ".avi";

    pItem->FillInDefaultIcon();
    pItem->m_strPath = strPath;
  }

  OnSort();
  UpdateButtons();
  strSelectedItem = m_history.Get(m_Directory.m_strPath);
  for (int i = 0; i < (int)m_vecItems.Size(); ++i)
  {
    CFileItem* pItem = m_vecItems[i];
    if (pItem->m_strPath == strSelectedItem)
    {
      m_viewControl.SetSelectedItem(i);
      break;
    }
  }

  return true;
}

//****************************************************************************************************************************
void CGUIWindowVideoYear::OnClick(int iItem)
{
  if ( iItem < 0 || iItem >= (int)m_vecItems.Size() ) return ;
  CFileItem* pItem = m_vecItems[iItem];
  CStdString strPath = pItem->m_strPath;

  CStdString strExtension;
  CUtil::GetExtension(pItem->m_strPath, strExtension);

  if (pItem->m_bIsFolder)
  {
    m_iItemSelected = -1;
    if ( pItem->m_bIsShareOrDrive )
    {
      if ( !g_passwordManager.IsItemUnlocked( pItem, "video" ) )
        return ;

      if ( !HaveDiscOrConnection( pItem->m_strPath, pItem->m_iDriveType ) )
        return ;
    }
    Update(strPath);
  }
  else
  {
    m_iItemSelected = m_viewControl.GetSelectedItem();
    PlayMovie(pItem);
  }
}

void CGUIWindowVideoYear::OnInfo(int iItem)
{
  if ( iItem < 0 || iItem >= (int)m_vecItems.Size() ) return ;
  if ( m_Directory.IsVirtualDirectoryRoot() ) return ;
  CGUIWindowVideoBase::OnInfo(iItem);
}

void CGUIWindowVideoYear::LoadViewMode()
{
  m_iViewAsIconsRoot = g_stSettings.m_iMyVideoYearRootViewAsIcons;
  m_iViewAsIcons = g_stSettings.m_iMyVideoYearViewAsIcons;
}

void CGUIWindowVideoYear::SaveViewMode()
{
  g_stSettings.m_iMyVideoYearRootViewAsIcons = m_iViewAsIconsRoot;
  g_stSettings.m_iMyVideoYearViewAsIcons = m_iViewAsIcons;
  g_settings.Save();
}

int CGUIWindowVideoYear::SortMethod()
{
  if (m_Directory.IsVirtualDirectoryRoot())
    return g_stSettings.m_iMyVideoYearRootSortMethod + 365;
  else
    return g_stSettings.m_iMyVideoYearSortMethod + 365;
}

bool CGUIWindowVideoYear::SortAscending()
{
  if (m_Directory.IsVirtualDirectoryRoot())
    return g_stSettings.m_bMyVideoYearRootSortAscending;
  else
    return g_stSettings.m_bMyVideoYearSortAscending;
}
