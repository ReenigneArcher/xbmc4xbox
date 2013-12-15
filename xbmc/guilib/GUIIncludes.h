#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#include <map>
#include <set>

// forward definitions
class TiXmlElement;
namespace INFO
{
  class CSkinVariableString;
}

class CGUIIncludes
{
public:
  CGUIIncludes();
  ~CGUIIncludes();

  void ClearIncludes();
  bool LoadIncludes(const CStdString &includeFile);
  void ResolveIncludes(TiXmlElement *node, const CStdString &type);
  const INFO::CSkinVariableString* CreateSkinVariable(const CStdString& name);
  bool ResolveConstant(const CStdString &constant, float &value) const;
  bool LoadIncludesFromXML(const TiXmlElement *root);

private:
  bool HasIncludeFile(const CStdString &includeFile) const;
  std::map<CStdString, TiXmlElement> m_includes;
  std::map<CStdString, TiXmlElement> m_defaults;
  std::map<CStdString, TiXmlElement> m_skinvariables;
  std::map<CStdString, float> m_constants;
  std::vector<CStdString> m_files;
  typedef std::vector<CStdString>::const_iterator iFiles;
};

