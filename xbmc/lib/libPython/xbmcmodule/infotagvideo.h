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
 
#include "lib/libPython/python/Include/Python.h"
#include "VideoInfoTag.h"
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

namespace PYXBMC
{
  typedef struct {
    PyObject_HEAD
    CVideoInfoTag infoTag;
  } InfoTagVideo;

  extern PyTypeObject InfoTagVideo_Type;
  extern InfoTagVideo* InfoTagVideo_FromCVideoInfoTag(const CVideoInfoTag& infoTag);
  void initInfoTagVideo_Type();
}

#ifdef __cplusplus
}
#endif
