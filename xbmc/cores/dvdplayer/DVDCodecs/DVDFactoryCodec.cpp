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

#include "system.h"
#include "DVDFactoryCodec.h"
#include "Video/DVDVideoCodec.h"
#include "Audio/DVDAudioCodec.h"
#include "Overlay/DVDOverlayCodec.h"

#include "Video/DVDVideoCodecFFmpeg.h"
#include "Video/DVDVideoCodecLibMpeg2.h"
#include "Audio/DVDAudioCodecFFmpeg.h"
#ifdef USE_LIBA52_DECODER
  #include "Audio/DVDAudioCodecLiba52.h"
#endif
#ifdef USE_LIBDTS_DECODER
  #include "Audio/DVDAudioCodecLibDts.h"
#endif
#ifdef USE_LIBMAD
#include "Audio/DVDAudioCodecLibMad.h"
#endif
#ifdef USE_LIBFAAD
#include "Audio/DVDAudioCodecLibFaad.h"
#endif
#include "Audio/DVDAudioCodecPcm.h"
#include "Audio/DVDAudioCodecLPcm.h"
#if defined(USE_LIB52_DECODER) || defined(USE_LIBDTS_DECODER)
  #include "Audio/DVDAudioCodecPassthrough.h"
#endif
#include "Audio/DVDAudioCodecPassthroughFFmpeg.h"
#include "Overlay/DVDOverlayCodecSSA.h"
#include "Overlay/DVDOverlayCodecText.h"
#include "Overlay/DVDOverlayCodecFFmpeg.h"

#include "DVDStreamInfo.h"


CDVDVideoCodec* CDVDFactoryCodec::OpenCodec(CDVDVideoCodec* pCodec, CDVDStreamInfo &hints, CDVDCodecOptions &options )
{  
  try
  {
    CLog::Log(LOGDEBUG, "FactoryCodec - Video: %s - Opening", pCodec->GetName());
    if( pCodec->Open( hints, options ) )
    {
      CLog::Log(LOGDEBUG, "FactoryCodec - Video: %s - Opened", pCodec->GetName());
      return pCodec;
    }

    CLog::Log(LOGDEBUG, "FactoryCodec - Video: %s - Failed", pCodec->GetName());
    pCodec->Dispose();
    delete pCodec;
  }
  catch(...)
  {
    CLog::Log(LOGERROR, "FactoryCodec - Video: Failed with exception");
  }
  return NULL;
}

CDVDAudioCodec* CDVDFactoryCodec::OpenCodec(CDVDAudioCodec* pCodec, CDVDStreamInfo &hints, CDVDCodecOptions &options )
{    
  try
  {
    CLog::Log(LOGDEBUG, "FactoryCodec - Audio: %s - Opening", pCodec->GetName());
    if( pCodec->Open( hints, options ) )
    {
      CLog::Log(LOGDEBUG, "FactoryCodec - Audio: %s - Opened", pCodec->GetName());
      return pCodec;
    }

    CLog::Log(LOGDEBUG, "FactoryCodec - Audio: %s - Failed", pCodec->GetName());
    pCodec->Dispose();
    delete pCodec;
  }
  catch(...)
  {
    CLog::Log(LOGERROR, "FactoryCodec - Audio: Failed with exception");
  }
  return NULL;
}

CDVDOverlayCodec* CDVDFactoryCodec::OpenCodec(CDVDOverlayCodec* pCodec, CDVDStreamInfo &hints, CDVDCodecOptions &options )
{  
  try
  {
    CLog::Log(LOGDEBUG, "FactoryCodec - Overlay: %s - Opening", pCodec->GetName());
    if( pCodec->Open( hints, options ) )
    {
      CLog::Log(LOGDEBUG, "FactoryCodec - Overlay: %s - Opened", pCodec->GetName());
      return pCodec;
    }

    CLog::Log(LOGDEBUG, "FactoryCodec - Overlay: %s - Failed", pCodec->GetName());
    pCodec->Dispose();
    delete pCodec;
  }
  catch(...)
  {
    CLog::Log(LOGERROR, "FactoryCodec - Audio: Failed with exception");
  }
  return NULL;
}


CDVDVideoCodec* CDVDFactoryCodec::CreateVideoCodec( CDVDStreamInfo &hint )
{
  CDVDVideoCodec* pCodec = NULL;
  CDVDCodecOptions options;

  // dvd's have weird still-frames in it, which is not fully supported in ffmpeg
  if(hint.stills && (hint.codec == CODEC_ID_MPEG2VIDEO || hint.codec == CODEC_ID_MPEG1VIDEO))
  {
    if( (pCodec = OpenCodec(new CDVDVideoCodecLibMpeg2(), hint, options)) ) return pCodec;
  }

  // try to decide if we want to try halfres decoding - note some codecs such as vp8 may return bad data for fpsrate (1000)/fpsscale (1),
  // so check for sensible values, as well as for vp8 codec as it doesn't support the lowres flag.
  float pixelrate = (float)hint.width*hint.height*hint.fpsrate/hint.fpsscale;
  if( pixelrate > 1400.0f*720.0f*30.0f && (hint.fpsrate/hint.fpsscale) < 100 && hint.codec != CODEC_ID_VP8)
  {
    CLog::Log(LOGINFO, "CDVDFactoryCodec - High video resolution detected %dx%d, trying half resolution decoding ", hint.width, hint.height);    
    options.push_back(CDVDCodecOption("lowres","1"));    
  }

  if( (pCodec = OpenCodec(new CDVDVideoCodecFFmpeg(), hint, options)) ) return pCodec;

  return NULL;
}

CDVDAudioCodec* CDVDFactoryCodec::CreateAudioCodec( CDVDStreamInfo &hint, bool passthrough /* = true */)
{
  CDVDAudioCodec* pCodec = NULL;
  CDVDCodecOptions options;

  if (passthrough)
  {
#if defined(USE_LIBA52_DECODER) || defined(USE_LIBDTS_DECODER)
    pCodec = OpenCodec( new CDVDAudioCodecPassthrough(), hint, options );
    if( pCodec ) return pCodec;
#endif

    pCodec = OpenCodec( new CDVDAudioCodecPassthroughFFmpeg(), hint, options);
    if ( pCodec ) return pCodec;
  }

  switch (hint.codec)
  {
#ifdef USE_LIBA52_DECODER
  case CODEC_ID_AC3:
    {
      pCodec = OpenCodec( new CDVDAudioCodecLiba52(), hint, options );
      if( pCodec ) return pCodec;
      break;
    }
#endif
#ifdef USE_LIBDTS_DECODER
  case CODEC_ID_DTS:
    {
      pCodec = OpenCodec( new CDVDAudioCodecLibDts(), hint, options );
      if( pCodec ) return pCodec;
      break;
    }
#endif
#ifdef USE_LIBMAD
  case CODEC_ID_MP2:
  case CODEC_ID_MP3:
    {
      pCodec = OpenCodec( new CDVDAudioCodecLibMad(), hint, options );
      if( pCodec ) return pCodec;
      break;
    }
#endif
#ifdef USE_LIBFAAD
  case CODEC_ID_AAC:
  //case CODEC_ID_MPEG4AAC:
    {
      pCodec = OpenCodec( new CDVDAudioCodecLibFaad(), hint, options );
      if( pCodec ) return pCodec;
      break;
    }
#endif
  case CODEC_ID_PCM_S32LE:
  case CODEC_ID_PCM_S32BE:
  case CODEC_ID_PCM_U32LE:
  case CODEC_ID_PCM_U32BE:
  case CODEC_ID_PCM_S24LE:
  case CODEC_ID_PCM_S24BE:
  case CODEC_ID_PCM_U24LE:
  case CODEC_ID_PCM_U24BE:
  case CODEC_ID_PCM_S24DAUD:
  case CODEC_ID_PCM_S16LE:
  case CODEC_ID_PCM_S16BE:
  case CODEC_ID_PCM_U16LE:
  case CODEC_ID_PCM_U16BE:
  case CODEC_ID_PCM_S8:
  case CODEC_ID_PCM_U8:
  case CODEC_ID_PCM_ALAW:
  case CODEC_ID_PCM_MULAW:
    {
      pCodec = OpenCodec( new CDVDAudioCodecPcm(), hint, options );
      if( pCodec ) return pCodec;
      break;
    }
#if 0
  //case CODEC_ID_LPCM_S16BE:
  //case CODEC_ID_LPCM_S20BE:
  case CODEC_ID_LPCM_S24BE:
    {
      pCodec = OpenCodec( new CDVDAudioCodecLPcm(), hint, options );
      if( pCodec ) return pCodec;
      break;
    }
#endif
  default:
    {
      pCodec = NULL;
      break;
    }
  }

  pCodec = OpenCodec( new CDVDAudioCodecFFmpeg(), hint, options );
  if( pCodec ) return pCodec;

  return NULL;
}

CDVDOverlayCodec* CDVDFactoryCodec::CreateOverlayCodec( CDVDStreamInfo &hint )
{
  CDVDOverlayCodec* pCodec = NULL;
  CDVDCodecOptions options;

  switch (hint.codec)
  {
    case CODEC_ID_TEXT:
      pCodec = OpenCodec(new CDVDOverlayCodecText(), hint, options);
      if( pCodec ) return pCodec;

    case CODEC_ID_SSA:
      pCodec = OpenCodec(new CDVDOverlayCodecSSA(), hint, options);
      if( pCodec ) return pCodec;

      pCodec = OpenCodec(new CDVDOverlayCodecText(), hint, options);
      if( pCodec ) return pCodec;

    default:
      pCodec = OpenCodec(new CDVDOverlayCodecFFmpeg(), hint, options);
      if( pCodec ) return pCodec;
  }

  return NULL;
}
