#ifndef _ENCODERLAME_H
#define _ENCODERLAME_H

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

#include "Encoder.h"
#include "DllLameenc.h"

class CEncoderLame : public CEncoder
{
public:
  CEncoderLame();
  virtual ~CEncoderLame() {}
  bool Init(const char* strFile, int iInChannels, int iInRate, int iInBits);
  int Encode(int nNumBytesRead, BYTE* pbtStream);
  bool Close();
  void AddTag(int key, const char* value);

protected:
  int parse_args_from_string(lame_global_flags * const gfp, const char *p,
                              char *inPath, char *outPath);
  lame_global_flags* m_pGlobalFlags;

  unsigned char m_buffer[48160]; // mp3buf_size in bytes = 1.25*(chunk size / 4) + 7200
  char m_inPath[XBMC_MAX_PATH + 1];
  char m_outPath[XBMC_MAX_PATH + 1];

  DllLameEnc m_dll;
};

#endif // _ENCODERLAME_H
