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


#include "MusicFileDirectory.h"
#include "cores/paplayer/DllSidplay2.h"

namespace XFILE
{
  class CSIDFileDirectory : public CMusicFileDirectory
  {
  public:
    CSIDFileDirectory(void);
    virtual ~CSIDFileDirectory(void);
  private:
    DllSidplay2 m_dll;
    virtual int GetTrackCount(const CStdString& strPath); 
  };
}
