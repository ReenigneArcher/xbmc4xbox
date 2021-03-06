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
#include "StackDirectory.h"
#include "utils/log.h"
#include "Util.h"
#include "FileItem.h"
#include "AdvancedSettings.h"
#include "utils/URIUtils.h"

#define PRE_2_1_STACK_COMPATIBILITY

using namespace std;
namespace XFILE
{
  CStackDirectory::CStackDirectory()
  {
  }

  CStackDirectory::~CStackDirectory()
  {
  }

  bool CStackDirectory::GetDirectory(const CStdString& strPath, CFileItemList& items)
  {
    items.Clear();
    // format is:
    // stack://file1 , file2 , file3 , file4
    // filenames with commas are double escaped (ie replaced with ,,), thus the " , " separator used.
    //CStdString folder, file;
    //URIUtils::Split(strPath, folder, file);
    // split files on the single comma
    CStdStringArray files;
    StringUtils::SplitString(strPath, " , ", files);
    if (files.empty())
      return false;   // error in path
    // remove "stack://" from the folder
    for (unsigned int i = 0; i < files.size(); i++)
    {
      CStdString file = files[i];
      if (i == 0)
        file = file.Mid(8);
#ifdef PRE_2_1_STACK_COMPATIBILITY
      if (i > 0 && file.Find("\\") == -1 && file.Find('/') == -1)
      {
        CStdString strPath;
        URIUtils::GetDirectory(items[0]->GetPath(),strPath);
        CStdString strFile = file;
        URIUtils::AddFileToFolder(strPath,strFile,file);
      }
#endif
      // replace double comma's with single ones.
      file.Replace(",,", ",");
      CFileItemPtr item(new CFileItem(file));
      //URIUtils::AddFileToFolder(folder, file, item->GetPath());
      item->SetPath(file);
      item->m_bIsFolder = false;
      items.Add(item);
    }
    return true;
  }

  CStdString CStackDirectory::GetStackedTitlePath(const CStdString &strPath)
  {
    // Load up our REs
    VECCREGEXP  RegExps;
    CRegExp     tempRE(true);
    const CStdStringArray& strRegExps = g_advancedSettings.m_videoStackRegExps;
    CStdStringArray::const_iterator itRegExp = strRegExps.begin();
    vector<pair<int, CStdString> > badStacks;
    while (itRegExp != strRegExps.end())
    {
      tempRE.RegComp(*itRegExp);
      if (tempRE.GetCaptureTotal() == 4)
        RegExps.push_back(tempRE);
      else
        CLog::Log(LOGERROR, "Invalid video stack RE (%s). Must have exactly 4 captures.", itRegExp->c_str());
      itRegExp++;
    }
    return GetStackedTitlePath(strPath, RegExps);
  }

  CStdString CStackDirectory::GetStackedTitlePath(const CStdString &strPath, VECCREGEXP& RegExps)
  {
    CStackDirectory stack;
    CFileItemList   files;
    CStdString      File1,
                    File2,
                    strStackTitlePath,
                    strStackTitle,
                    strCommonDir        = URIUtils::GetParentPath(strPath);
    
    stack.GetDirectory(strPath, files);

    if (files.Size() > 1)
    {

      File1 = URIUtils::GetFileName(files[0]->GetPath());
      File2 = URIUtils::GetFileName(files[1]->GetPath());

      std::vector<CRegExp>::iterator itRegExp = RegExps.begin();
      int offset = 0;

      while (itRegExp != RegExps.end())
      {
        if (itRegExp->RegFind(File1, offset) != -1)
        {
          CStdString Title1     = itRegExp->GetMatch(1),
                     Volume1    = itRegExp->GetMatch(2),
                     Ignore1    = itRegExp->GetMatch(3),
                     Extension1 = itRegExp->GetMatch(4);
          if (offset)
            Title1 = File1.substr(0, itRegExp->GetSubStart(2));
          if (itRegExp->RegFind(File2, offset) != -1)
          {
            CStdString Title2     = itRegExp->GetMatch(1),
                       Volume2    = itRegExp->GetMatch(2),
                       Ignore2    = itRegExp->GetMatch(3),
                       Extension2 = itRegExp->GetMatch(4);
            if (offset)
              Title2 = File2.substr(0, itRegExp->GetSubStart(2));
            if (Title1.Equals(Title2))
            {
              if (!Volume1.Equals(Volume2))
              {
                if (Ignore1.Equals(Ignore2) && Extension1.Equals(Extension2))
                {
                  // got it
                  strStackTitle = Title1 + Ignore1 + Extension1;
                  itRegExp = RegExps.end();
                  break;
                }
                else // Invalid stack
                  break;
              }
              else // Early match, retry with offset
              {
                offset = itRegExp->GetSubStart(3);
                continue;
              }
            }
          }
        }
        offset = 0;
        itRegExp++;
      }
      if (!strCommonDir.empty() && !strStackTitle.empty())
        strStackTitlePath = strCommonDir + strStackTitle;
    }

    return strStackTitlePath;
  }

  CStdString CStackDirectory::GetFirstStackedFile(const CStdString &strPath)
  {
    // the stacked files are always in volume order, so just get up to the first filename
    // occurence of " , "
    CStdString path, file, folder;
    int pos = strPath.Find(" , ");
    if (pos > 0)
      URIUtils::Split(strPath.Left(pos), folder, file);
    else
      URIUtils::Split(strPath, folder, file); // single filed stacks - should really not happen

    // remove "stack://" from the folder    
    folder = folder.Mid(8);
    file.Replace(",,", ",");
    URIUtils::AddFileToFolder(folder, file, path);
    
    return path;
  }

  CStdString CStackDirectory::ConstructStackPath(const CFileItemList &items, const vector<int> &stack)
  {
    // no checks on the range of stack here.
    // we replace all instances of comma's with double comma's, then separate
    // the files using " , ".
    CStdString stackedPath = "stack://";
    CStdString folder, file;
    URIUtils::Split(items[stack[0]]->GetPath(), folder, file);
    stackedPath += folder;
    // double escape any occurence of commas
    file.Replace(",", ",,");
    stackedPath += file;
    for (unsigned int i = 1; i < stack.size(); ++i)
    {
      stackedPath += " , ";
      file = items[stack[i]]->GetPath();
      
      // double escape any occurence of commas
      file.Replace(",", ",,");
      stackedPath += file;
    }
    return stackedPath;
  }

  bool CStackDirectory::ConstructStackPath(const vector<CStdString> &paths, CStdString& stackedPath)
  {
    if (paths.size() < 2)
      return false;
    stackedPath = "stack://";
    CStdString folder, file;
    URIUtils::Split(paths[0], folder, file);
    stackedPath += folder;
    // double escape any occurence of commas
    file.Replace(",", ",,");
    stackedPath += file;
    for (unsigned int i = 1; i < paths.size(); ++i)
    {
      stackedPath += " , ";
      file = paths[i];
      
      // double escape any occurence of commas
      file.Replace(",", ",,");
      stackedPath += file;
    }
    return true;
  }
}

