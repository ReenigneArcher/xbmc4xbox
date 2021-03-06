/*
   MPlayer video driver for DirectFramebuffer device
  
   (C) 2001
   
   Written by  Jiri Svoboda <Jiri.Svoboda@seznam.cz>

   Inspired by vo_sdl and vo_fbdev.    
  
   To get second head working delete line 120
   from fbdev.c (from DirectFB sources version 0.9.7)
   Line contains following:
        fbdev->fd = open( "/dev/fb0", O_RDWR );

   Parts of this code taken from DirectFB examples:
   (c) Copyright 2000  convergence integrated media GmbH.
   All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// directfb includes

#include <directfb.h>

// other things

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <linux/fb.h>

#include "config.h"
#include "video_out.h"
#include "video_out_internal.h"
#include "fastmemcpy.h"
#include "sub.h"
#include "libswscale/rgb2rgb.h"

#include "aspect.h"

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

static vo_info_t info = {
	"Direct Framebuffer Device",
	"directfb",
	"Jiri Svoboda Jiri.Svoboda@seznam.cz",
	""
};

LIBVO_EXTERN(directfb)

extern int verbose;

/******************************
*	   directfb 	      *
******************************/

 /*
 * (Globals)
 */
static IDirectFB *dfb = NULL;
static IDirectFBSurface *primary = NULL;
static IDirectFBInputDevice *keyboard = NULL;
static IDirectFBDisplayLayer       *videolayer = NULL;
static DFBDisplayLayerConfig        dlc;
static unsigned int screen_width  = 0;
static unsigned int screen_height = 0;
static DFBSurfacePixelFormat frame_format;
static unsigned int frame_pixel_size = 0;
static unsigned int source_pixel_size = 0;
static int xoffset=0,yoffset=0;
#define DFBCHECK(x...)                                         \
  {                                                            \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
      {                                                        \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBErrorFatal( #x, err );                         \
      }                                                        \
  }

/*
 * The frame is to be loaded into a surface that we can blit from.
 */

static IDirectFBSurface *frame = NULL;

/*
 * A buffer for input events.
 */

#if DIRECTFBVERSION > 908
static IDirectFBEventBuffer *buffer = NULL;
#else
static IDirectFBInputBuffer *buffer = NULL;
#endif

/******************************
*	    vo_directfb       *
******************************/

/* command line/config file options */
#ifdef HAVE_FBDEV
extern char *fb_dev_name;
#else
char *fb_dev_name;
#endif

static void (*draw_alpha_p)(int w, int h, unsigned char *src,
		unsigned char *srca, int stride, unsigned char *dst,
		int dstride);

static uint32_t in_width;
static uint32_t in_height;
static uint32_t out_width=1;
static uint32_t out_height=1;
static uint32_t pixel_format;
static int fs;
static int flip;
static int stretch=0;
struct modes_t {
        int valid;
        unsigned int width;
        unsigned int height;
        int overx,overy;
        } modes [4];
static unsigned int best_bpp=5;
// videolayer stuff
static int videolayeractive=0;
static int videolayerpresent=0;
//some info about videolayer - filled on preinit
struct vlayer_t { 
        int iv12;
	int i420;
	int yuy2;
	int uyvy;
	int brightness;
	int saturation;
	int contrast;
	int hue;
	int hwscale;
        } videolayercaps;
// workabout for DirectFB bug
static int buggyYV12BitBlt=0;
static int memcpyBitBlt=0;
// 
static int hwstretchblit=0;
#define DIRECTRENDER
#ifdef DIRECTRENDER
static int dr_enabled=0;
static int framelocked=0;
#endif
#define FLIPPING
#ifdef FLIPPING
static int do_flipping=1; // turn (on) off flipping - prepared for cmd line switch
static int wait_vsync_after_flip=0; 
static int flipping=0; // flipping is active
static int invram=0; // backbuffer in video memory
static int blitperformed=0; // in case of temporary frame we will blit before drawing osd
#endif
// primary & frame stuff
static int frameallocated=0;
static int primaryallocated=0;

DFBEnumerationResult enum_modes_callback( unsigned int width,unsigned int height,unsigned int bpp, void *data)
{
int overx=0,overy=0,closer=0,over=0;
unsigned int index=bpp/8-1;
int we_are_under=0;

if (verbose) printf("DirectFB: Validator entered %i %i %i\n",width,height,bpp);

overx=width-out_width;
overy=height-out_height;
if (!modes[index].valid) {
        modes[index].valid=1;
        modes[index].width=width;
        modes[index].height=height;
        modes[index].overx=overx;
        modes[index].overy=overy;
	if (verbose) printf("DirectFB: Mode added %i %i %i\n",width,height,bpp);
        }
if ((modes[index].overy<0)||(modes[index].overx<0)) we_are_under=1; // stored mode is smaller than req mode
if (abs(overx*overy)<abs(modes[index].overx * modes[index].overy)) closer=1; // current mode is closer to desired res
if ((overx>=0)&&(overy>=0)) over=1; // current mode is bigger or equaul to desired res
if ((closer && (over || we_are_under)) || (we_are_under && over)) {
                modes[index].valid=1;
                modes[index].width=width;
                modes[index].height=height;
                modes[index].overx=overx;
                modes[index].overy=overy;
		if (verbose) printf("DirectFB: Better mode added %i %i %i\n",width,height,bpp);
                };

return DFENUM_OK;
}


#if DIRECTFBVERSION > 912
DFBEnumerationResult enum_layers_callback( DFBDisplayLayerID            id,
                                           DFBDisplayLayerDescription   desc,
                                           void                        *data )
{
     DFBDisplayLayerCapabilities  caps = desc.caps;
#else
DFBEnumerationResult enum_layers_callback( unsigned int                 id,
                                           DFBDisplayLayerCapabilities  caps,
                                           void                        *data )
{
#endif
     IDirectFBDisplayLayer **layer = (IDirectFBDisplayLayer **)data;
if (verbose) { 
     printf("\nDirectFB: Layer %d:\n", id );

     if (caps & DLCAPS_SURFACE)
          printf( "  - Has a surface.\n" );

     if (caps & DLCAPS_ALPHACHANNEL)
          printf( "  - Supports blending based on alpha channel.\n" );

#if DIRECTFBVERSION > 909
     if (caps & DLCAPS_SRC_COLORKEY)
          printf( "  - Supports source based color keying.\n" );

     if (caps & DLCAPS_DST_COLORKEY)
          printf( "  - Supports destination based color keying.\n" );
#else
     if (caps & DLCAPS_COLORKEYING)
          printf( "  - Supports color keying.\n" );
#endif

     if (caps & DLCAPS_FLICKER_FILTERING)
          printf( "  - Supports flicker filtering.\n" );

/*   renamed in dfb v. 0.9.13
     if (caps & DLCAPS_INTERLACED_VIDEO)
          printf( "  - Can natively display interlaced video.\n" );
*/
     if (caps & DLCAPS_OPACITY)
          printf( "  - Supports blending based on global alpha factor.\n" );

     if (caps & DLCAPS_SCREEN_LOCATION)
          printf( "  - Can be positioned on the screen.\n" );

     if (caps & DLCAPS_BRIGHTNESS)
          printf( "  - Brightness can be adjusted.\n" );

     if (caps & DLCAPS_CONTRAST)
          printf( "  - Contrast can be adjusted.\n" );

     if (caps & DLCAPS_HUE)
          printf( "  - Hue can be adjusted.\n" );

     if (caps & DLCAPS_SATURATION)
          printf( "  - Saturation can be adjusted.\n" );

     printf("\n");
}
     /* We take the first layer not being the primary */
     if (id != DLID_PRIMARY) {
          DFBResult ret;

          ret = dfb->GetDisplayLayer( dfb, id, layer );
          if (ret)
               DirectFBError( "dfb->GetDisplayLayer failed", ret );
          else
               return DFENUM_CANCEL;
     }

     return DFENUM_OK;
}

static uint32_t preinit(const char *arg)
{
     DFBSurfaceDescription dsc;
     DFBResult             ret;
     DFBDisplayLayerConfigFlags   failed;

  /*
   * (Initialize)
   */
	
if (verbose) printf("DirectFB: Preinit entered\n");

        DFBCHECK (DirectFBInit (NULL,NULL));

	if (!((directfb_major_version <= 0) &&
	    (directfb_minor_version <= 9) &&
	    (directfb_micro_version < 7)))
	{
    	    if (!fb_dev_name && !(fb_dev_name = getenv("FRAMEBUFFER"))) fb_dev_name = strdup("/dev/fb0");
    	    DFBCHECK (DirectFBSetOption ("fbdev",fb_dev_name));
	}

	// use own bitblt for YV12 beacuse bug in dfb till 0.9.11 
	if ((directfb_major_version <= 0) &&
	    (directfb_minor_version <= 9) &&
	    (directfb_micro_version <= 11)) {
	    buggyYV12BitBlt=1;
	    if (verbose) printf("DirectFB: Buggy YV12BitBlt!\n");
	}

#ifdef FLIPPING
	// activate flipping from release 0.9.11
	if ((directfb_major_version <= 0) &&
	    (directfb_minor_version <= 9) &&
	    (directfb_micro_version <= 10)) {
	    do_flipping=0;
	} else {
//	(de)activated by default - should be overwritten by cmd line option
//	    do_flipping=1;
	}
	// wait for vsync if ver <0.9.13
	if ((directfb_major_version <= 0) &&
	    (directfb_minor_version <= 9) &&
	    (directfb_micro_version <= 12)) {
	    wait_vsync_after_flip=1;
            if (verbose) printf("DirectFB: Manual wait for vsync enabled!\n");
	} else {
	    wait_vsync_after_flip=0; 
	}
#endif

//	uncomment this if you do not wish to create a new vt for DirectFB
//       DFBCHECK (DirectFBSetOption ("no-vt-switch",""));

//	uncomment this if you want to allow vt switching
//       DFBCHECK (DirectFBSetOption ("vt-switching",""));
#if DIRECTFBVERSION > 908
//	uncomment this if you want to hide gfx cursor (req dfb >=0.9.9)
       DFBCHECK (DirectFBSetOption ("no-cursor",""));
#endif

        DFBCHECK (DirectFBSetOption ("bg-color","00000000"));

        DFBCHECK (DirectFBCreate (&dfb));
        DFBCHECK (dfb->SetCooperativeLevel (dfb, DFSCL_FULLSCREEN));

  // let's try to get YUY2 layer - borrowed from DirectFb examples

     /* Enumerate display layers */
        DFBCHECK (dfb->EnumDisplayLayers( dfb, enum_layers_callback, &videolayer ));

        if (!videolayer) {
	    if (verbose) printf("DirectFB: No videolayer found\n");
          // no videolayer found
//          printf( "\nNo additional layers have been found.\n" );
            videolayeractive=0;

        } else {
	// just to be sure that layer is hidden during config
		videolayer->SetOpacity(videolayer,0);

        // there is an additional layer so test it for YUV formats
	// some videolayers support RGB formats - not used now
		if (verbose) printf("DirectFB: Testing videolayer caps\n");
	
                dlc.flags       = DLCONF_PIXELFORMAT;
#if DIRECTFBVERSION > 908
                dlc.pixelformat = DSPF_YV12;
                ret = videolayer->TestConfiguration( videolayer, &dlc, &failed );
                if (ret==DFB_OK) {
		    videolayercaps.iv12=1; 
		    if (verbose) printf("DirectFB: Videolayer supports YV12 format\n");
		} else {
		    videolayercaps.iv12=0;
		    if (verbose) printf("DirectFB: Videolayer doesn't support YV12 format\n");
		};

                dlc.pixelformat = DSPF_I420;
                ret = videolayer->TestConfiguration( videolayer, &dlc, &failed );
                if (ret==DFB_OK) {
		    videolayercaps.i420=1; 
		    if (verbose) printf("DirectFB: Videolayer supports I420 format\n");
		} else {
		    videolayercaps.i420=0;
		    if (verbose) printf("DirectFB: Videolayer doesn't support I420 format\n");
		};
#else
	        videolayercaps.yuy2=0;
#endif

		dlc.pixelformat = DSPF_YUY2;
                ret = videolayer->TestConfiguration( videolayer, &dlc, &failed );
		if (ret==DFB_OK) {
		    videolayercaps.yuy2=1; 
		    if (verbose) printf("DirectFB: Videolayer supports YUY2 format\n");
		} else {
		    videolayercaps.yuy2=0;
		    if (verbose) printf("DirectFB: Videolayer doesn't support YUY2 format\n");
		};

		dlc.pixelformat = DSPF_UYVY;
                ret = videolayer->TestConfiguration( videolayer, &dlc, &failed );
                if (ret==DFB_OK) {
		    videolayercaps.uyvy=1; 
		    if (verbose) printf("DirectFB: Videolayer supports UYVY format\n");
		} else {
		    videolayercaps.uyvy=0;
		    if (verbose) printf("DirectFB: Videolayer doesn't support UYVY format\n");
		};
		
		// test for color caps
		{
                DFBDisplayLayerCapabilities  caps;
#if DIRECTFBVERSION > 912
                DFBDisplayLayerDescription  desc;
                videolayer->GetDescription(videolayer,&desc);
                caps = desc.caps;
#else
		videolayer->GetCapabilities(videolayer,&caps);
#endif
	        if (caps & DLCAPS_BRIGHTNESS) {
		    videolayercaps.brightness=1;
		} else {
		    videolayercaps.brightness=0;
		};

		if (caps & DLCAPS_CONTRAST) {
		    videolayercaps.contrast=1;
		} else {
		    videolayercaps.contrast=0;
		};

    		if (caps & DLCAPS_HUE) {
		    videolayercaps.hue=1;
		} else {
		    videolayercaps.hue=0;
		};

    		if (caps & DLCAPS_SATURATION) {
		    videolayercaps.saturation=1;
		} else {
		    videolayercaps.saturation=0;
		};
		
		// test if layer can change size/position
    		if (caps & DLCAPS_SCREEN_LOCATION) {
		    videolayercaps.hwscale=1;
		} else {
		    videolayercaps.hwscale=0;
		};
		
	  
		}


	// is there a working yuv ? if no we will not use videolayer
		if ((videolayercaps.iv12==0)&&(videolayercaps.i420==0)&&(videolayercaps.yuy2==0)&&(videolayercaps.uyvy==0)) {
		    // videolayer doesn't work with yuv so release it
		    videolayerpresent=0;
		    videolayer->SetOpacity(videolayer,0);
		    videolayer->Release(videolayer);
		} else {
		    videolayerpresent=1;
		};
        }

// check generic card capabilities (for hw scaling)
    {
    DFBCardCapabilities caps;    
    DFBCHECK (dfb->GetCardCapabilities(dfb,&caps));
    if (caps.acceleration_mask & DFXL_STRETCHBLIT) hwstretchblit=1; else hwstretchblit=0;
    if (verbose && hwstretchblit) printf("DirectFB: Card supports hw stretch blit\n");
    }

// just look at RGB things for main layer
        modes[0].valid=0;
        modes[1].valid=0;
        modes[2].valid=0;
        modes[3].valid=0;
        DFBCHECK (dfb->EnumVideoModes(dfb,enum_modes_callback,NULL));


  /*
   * (Get keyboard)
   */
  ret = dfb->GetInputDevice (dfb, DIDID_KEYBOARD, &keyboard);

  if (ret==DFB_OK) {
    if (verbose) {
    printf("DirectFB: Keyboard init OK\n");
    }
  } else {
    keyboard = NULL;
    printf("DirectFB: Keyboard init FAILED\n");
  }

  /*
   * Create an input buffer for the keyboard.
   */
#if DIRECTFBVERSION > 908
  if (keyboard) DFBCHECK (keyboard->CreateEventBuffer (keyboard, &buffer));
#else
  if (keyboard) DFBCHECK (keyboard->CreateInputBuffer (keyboard, &buffer));
#endif
  // just to start with clean ...
  if (buffer) buffer->Reset(buffer);
  return 0;

}


static uint32_t config(uint32_t width, uint32_t height, uint32_t d_width,
		uint32_t d_height, uint32_t fullscreen, char *title,
		uint32_t format)
{
  /*
   * (Locals)
   */
	DFBSurfaceDescription dsc;
        DFBResult             ret;


        int vm = fullscreen & 0x02;
	int zoom = fullscreen & 0x04;

	if (verbose) printf("DirectFB: Config entered [%ix%i]\n",width,height);
	if (verbose) printf("DirectFB: With requested format: %s\n",vo_format_name(format));

	fs = fullscreen & 0x01;
	flip = fullscreen & 0x08;

	pixel_format=format;

	in_width = width;
	in_height = height;

        if (d_width) {
		out_width = d_width;
		out_height = d_height;
	} else {
		d_width = out_width = in_width;
		d_height = out_height = in_height;
	}

// 	just look at RGB things for main layer - once again - now we now desired screen size
        modes[0].valid=0;
        modes[1].valid=0;
        modes[2].valid=0;
        modes[3].valid=0;
        DFBCHECK (dfb->EnumVideoModes(dfb,enum_modes_callback,NULL));

  if (vm) {
        // need better algorithm just hack
	    switch (format) {
		    case IMGFMT_RGB32: 
    		    case IMGFMT_BGR32: 
				if (modes[3].valid) {
				    dfb->SetVideoMode(dfb,modes[3].width,modes[3].height,32);
				    if (verbose) printf("DirectFB: Trying to set videomode [%ix%i 32 bpp]\n",modes[3].width,modes[3].height);
				};
				break;
        	    case IMGFMT_RGB24: 
		    case IMGFMT_BGR24: 
				if (modes[2].valid) {
				    dfb->SetVideoMode(dfb,modes[2].width,modes[2].height,24);
				    if (verbose) printf("DirectFB: Trying to set videomode [%ix%i 24 bpp]\n",modes[2].width,modes[2].height);
				};
				break;
		    case IMGFMT_RGB16:
    		    case IMGFMT_BGR16:
            	    case IMGFMT_RGB15:
	    	    case IMGFMT_BGR15:
				if (modes[1].valid) {
				    dfb->SetVideoMode(dfb,modes[1].width,modes[1].height,16);
				    if (verbose) printf("DirectFB: Trying to set videomode [%ix%i 16 bpp]\n",modes[1].width,modes[1].height);
				};
				break;
		
        	    default: // try all of them in order 24bit 16bit 32bit 8bit
				if (modes[2].valid) {
				    dfb->SetVideoMode(dfb,modes[2].width,modes[2].height,24);
				    if (verbose) printf("DirectFB: Trying to set videomode [%ix%i 24 bpp]\n",modes[2].width,modes[2].height);
				    }
				else if (modes[1].valid) {
				    dfb->SetVideoMode(dfb,modes[1].width,modes[1].height,16);
				    if (verbose) printf("DirectFB: Trying to set videomode [%ix%i 16 bpp]\n",modes[1].width,modes[1].height);
				    }
				else if (modes[3].valid) {
				    dfb->SetVideoMode(dfb,modes[3].width,modes[3].height,32);
				    if (verbose) printf("DirectFB: Trying to set videomode [%ix%i 32 bpp]\n",modes[3].width,modes[3].height);
				    }
				else if (modes[0].valid) {
				    dfb->SetVideoMode(dfb,modes[0].width,modes[0].height,8);
				    if (verbose) printf("DirectFB: Trying to set videomode [%ix%i 8 bpp]\n",modes[0].width,modes[0].height);
				    }
				break;
            };

        }

  // release primary if it is already allocated
  if (primaryallocated) {
    if (verbose ) printf("DirectFB: Release primary\n");
    primary->Release (primary);
    primaryallocated=0;
  };

     videolayeractive=0; // will be enabled on succes later

     if (videolayerpresent) {
     
        // try to set proper w a h values matching image size
        dlc.flags       = DLCONF_WIDTH | DLCONF_HEIGHT;
        dlc.width       = in_width;
        dlc.height      = in_height;

        ret = videolayer->SetConfiguration( videolayer, &dlc );

	if (ret) {
	if (verbose) printf("DirectFB: Set layer size failed\n");
	};

	// try to set correct pixel format (closest to required)
	
        dlc.flags       = DLCONF_PIXELFORMAT;
	dlc.pixelformat = 0;
        switch (pixel_format) {
	    case IMGFMT_YV12: 
#if DIRECTFBVERSION > 908
			      if (videolayercaps.i420==1) { 
				dlc.pixelformat=DSPF_I420;
			        break;
			      } else if (videolayercaps.iv12==1) { 
				dlc.pixelformat=DSPF_YV12;
			        break;
			      };
			      
#endif
	    case IMGFMT_YUY2: if (videolayercaps.yuy2==1) {
				    dlc.pixelformat=DSPF_YUY2;
				    break;
// temporary disabled - do not have conv tool to uyvy
/*			      }	else if (videolayercaps.uyvy==1) {
			    	    dlc.pixelformat=DSPF_UYVY;
				    break;
*/
#if DIRECTFBVERSION > 908
			      } else if (videolayercaps.i420==1) { 
				    dlc.pixelformat=DSPF_I420;
			    	    break;
			      } else if (videolayercaps.iv12==1) { 
				    dlc.pixelformat=DSPF_YV12;
			    	    break;
#endif
			      }; 
			      // shouldn't happen - if it reaches here -> bug

            case IMGFMT_RGB32: dlc.pixelformat =  DSPF_ARGB; break;
	    case IMGFMT_BGR32: dlc.pixelformat =  DSPF_ARGB; break;
    	    case IMGFMT_RGB24: dlc.pixelformat =  DSPF_RGB24; break;
	    case IMGFMT_BGR24: dlc.pixelformat =  DSPF_RGB24; break;
            case IMGFMT_RGB16: dlc.pixelformat =  DSPF_RGB16; break;
            case IMGFMT_BGR16: dlc.pixelformat =  DSPF_RGB16; break;
            case IMGFMT_RGB15: dlc.pixelformat =  DSPF_RGB15; break;
            case IMGFMT_BGR15: dlc.pixelformat =  DSPF_RGB15; break;
            default: dlc.pixelformat =  DSPF_RGB24; break;
	}

	if (verbose) switch (dlc.pixelformat) {
                case DSPF_ARGB:  printf("DirectFB: layer format ARGB\n");
                                 break;
                case DSPF_RGB32: printf("DirectFB: layer format RGB32\n");
                                 break;
                case DSPF_RGB24: printf("DirectFB: layer format RGB24\n");
                                 break;
                case DSPF_RGB16: printf("DirectFB: layer format RGB16\n");
                                 break;
                case DSPF_RGB15: printf("DirectFB: layer format RGB15\n");
                                 break;
                case DSPF_YUY2:  printf("DirectFB: layer format YUY2\n");
                                 break;
                case DSPF_UYVY:  printf("DirectFB: layer format UYVY\n");
                                 break;
#if DIRECTFBVERSION > 908
                case DSPF_YV12:  printf("DirectFB: layer format YV12\n");
                                 break;
                case DSPF_I420:  printf("DirectFB: layer format I420\n");
                                 break;
#endif
                default: printf("DirectFB:  - unknown format ->exit\n"); return 1;
        }

	ret =videolayer->SetConfiguration( videolayer, &dlc );
        if (!ret) {
             if (verbose) printf("DirectFB: SetConfiguration for layer OK\n");

#ifdef FLIPPING
	     // try to set flipping for videolayer
	     if (do_flipping) {
	        dlc.flags = DLCONF_BUFFERMODE;
	        dlc.buffermode = DLBM_BACKVIDEO;
		invram = 1;
	        flipping = 1;
	        ret =videolayer->SetConfiguration( videolayer, &dlc );
	        if (ret!=DFB_OK) {
	    	    invram = 0;
		    if (!((directfb_major_version <= 0) &&
			(directfb_minor_version <= 9) &&
			(directfb_micro_version <= 11))) {
		    
	    		dlc.buffermode = DLBM_BACKSYSTEM;
			ret =videolayer->SetConfiguration( videolayer, &dlc );
	        	if (ret!=DFB_OK) {
			    flipping = 0;
			}
		    } else { flipping = 0; };
	        }
	        if (verbose) if (flipping) {
	    	    printf("DirectFB: SetFlipping for layer: OK (vram=%i)\n",invram);
	        } else {
	            printf("DirectFB: SetFlipping for layer: FAILED\n");
		}
	     } else flipping=0;
#endif
	     ret = videolayer->GetSurface( videolayer, &primary );
	     if (!ret){
                videolayeractive=1;
                if (verbose) printf("DirectFB: Get surface for layer: OK\n");
		primaryallocated=1;
		// set videolayer to be visible
		videolayer->SetOpacity(videolayer,0xFF);
              } else {
	      videolayeractive=0;
	      if (videolayer) videolayer->SetOpacity(videolayer,0);
	      };
        } else {
	videolayeractive=0;
	if (videolayer) videolayer->SetOpacity(videolayer,0);
	};

      }

// for flipping we will use BitBlt not integrated directfb flip (if necessary)
  dsc.flags = DSDESC_CAPS | DSDESC_PIXELFORMAT;
  dsc.caps  = DSCAPS_PRIMARY /*| DSCAPS_VIDEOONLY*/;

  switch (format) {
        case IMGFMT_RGB32: dsc.pixelformat =  DSPF_ARGB; source_pixel_size= 4; break;
        case IMGFMT_BGR32: dsc.pixelformat =  DSPF_ARGB; source_pixel_size= 4;  break;
        case IMGFMT_RGB24: dsc.pixelformat =  DSPF_RGB24; source_pixel_size= 3;  break;
        case IMGFMT_BGR24: dsc.pixelformat =  DSPF_RGB24; source_pixel_size= 3;  break;
        case IMGFMT_RGB16: dsc.pixelformat =  DSPF_RGB16; source_pixel_size= 2; break;
        case IMGFMT_BGR16: dsc.pixelformat =  DSPF_RGB16; source_pixel_size= 2; break;
        case IMGFMT_RGB15: dsc.pixelformat =  DSPF_RGB15; source_pixel_size= 2; break;
        case IMGFMT_BGR15: dsc.pixelformat =  DSPF_RGB15; source_pixel_size= 2; break;
        default: dsc.pixelformat =  DSPF_RGB24; source_pixel_size=2; break; //YUV formats
        };

  if (!videolayeractive) {
#ifdef FLIPPING
      if (do_flipping) {
          flipping = 1;
          invram = 0;
          dsc.caps |= DSCAPS_FLIPPING;
          ret = dfb->CreateSurface( dfb, &dsc, &primary );
          if (ret!=DFB_OK) {
	    dsc.caps &= ~DSCAPS_FLIPPING;
      DFBCHECK (dfb->CreateSurface( dfb, &dsc, &primary ));
	    flipping = 0;
          } else {
         // test flipping again
    	    DFBCHECK (primary->GetCapabilities(primary,&dsc.caps));
    	    if (!(dsc.caps & DSCAPS_FLIPPING)) {
		printf("DirectFB: Error - surface sucesfully created with flipping flag, but doesn't support it.\n");
		flipping = 0;
    	    } else {
	    // test fliping in real - sometimes flips fails even if shouldn't
		DFBResult ret;
		ret = primary->Flip(primary,NULL,0);
		if (ret!=DFB_OK) {
		    // recreate surface as non flipping
    		    printf("DirectFB: Error - surface sucesfully created with flipping flag, but test flip failed.\n");
		    flipping = 0;
	            dsc.caps &= ~DSCAPS_FLIPPING;
		    primary->Release(primary);
	            DFBCHECK (dfb->CreateSurface( dfb, &dsc, &primary ));
		}
		
	    }	
          }
/*         if (verbose) if (flipping) {
	        printf("DirectFB: Flipping for primary: OK\n");
	 } else {
	        printf("DirectFB: Flipping for primary: FAILED\n");
	 }
*/    } else {
        flipping = 0;
#endif
      DFBCHECK (dfb->CreateSurface( dfb, &dsc, &primary ));
#ifdef FLIPPING
      }
#endif
      
      if (verbose) printf("DirectFB: Get primary surface: OK\n");

      primaryallocated=1;
  } 

#ifdef FLIPPING
//	     final check for flipping - based on real caps

	    	    DFBCHECK (primary->GetCapabilities(primary,&dsc.caps));
	    	    if (!(dsc.caps & DSCAPS_FLIPPING)) {
			printf("DirectFB: Flipping si NOT active.\n");
			flipping = 0;
	            } else {
			printf("DirectFB: Flipping is active.\n");
			flipping = 1;
		    }
//	        }	
#endif		

  DFBCHECK (primary->GetSize (primary, &screen_width, &screen_height));

  DFBCHECK (primary->GetPixelFormat (primary, &frame_format));

// temporary frame buffer
  dsc.flags = DSDESC_CAPS | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_WIDTH;

  dsc.width = in_width;
  dsc.height = in_height;

  // at this time use pixel req format or format of main disp

  switch (format) {
        case IMGFMT_RGB32: dsc.pixelformat =  DSPF_ARGB; break;
        case IMGFMT_BGR32: dsc.pixelformat =  DSPF_ARGB; break;
        case IMGFMT_RGB24: dsc.pixelformat =  DSPF_RGB24; break;
        case IMGFMT_BGR24: dsc.pixelformat =  DSPF_RGB24; break;
        case IMGFMT_RGB16: dsc.pixelformat =  DSPF_RGB16; break;
        case IMGFMT_BGR16: dsc.pixelformat =  DSPF_RGB16; break;
        case IMGFMT_RGB15: dsc.pixelformat =  DSPF_RGB15; break;
        case IMGFMT_BGR15: dsc.pixelformat =  DSPF_RGB15; break;
        default: dsc.pixelformat =  frame_format; break;  // uknown or YUV ->  retain layer format eg. RGB or YUY2
        };


  /*
   * Create a surface based on the description of the source frame
   */
#if DIRECTFBVERSION > 908
  if (((dsc.pixelformat==DSPF_YV12)||(dsc.pixelformat==DSPF_I420)) && buggyYV12BitBlt) {
    memcpyBitBlt = 1;
  } else {
   memcpyBitBlt = 0;
  };
#else
   memcpyBitBlt = 0;
#endif   


  // release frame if it is already allocated
  if (frameallocated) {
    if (verbose ) printf("DirectFB: Release frame\n");
    frame->Release (frame);
    frameallocated=0;
  };

// picture size and position

 aspect_save_orig(in_width,in_height);
 aspect_save_prescale(d_width,d_height);
 if (videolayeractive) {//  try to set pos for YUY2 layer and proper aspect ratio
		aspect_save_screenres(10000,10000);
		aspect(&out_width,&out_height,A_ZOOM);

                ret = videolayer->SetScreenLocation(videolayer,(1-(float)out_width/10000)/2,(1-(float)out_height/10000)/2,((float)out_width/10000),((float)out_height/10000));

		xoffset = 0;
		yoffset = 0;
  } else {
                // aspect ratio correction for zoom to fullscreen
		aspect_save_screenres(screen_width,screen_height);
	
		if(fs) /* -fs */
			aspect(&out_width,&out_height,A_ZOOM);
		else
			aspect(&out_width,&out_height,A_NOZOOM);


    		xoffset = (screen_width - out_width) / 2;
	        yoffset = (screen_height - out_height) / 2;
  }

 if (((out_width != in_width) || (out_height != in_height)) && (!videolayeractive)) {stretch = 1;} else stretch=0; //yuy doesn't like strech and should not be needed

#ifdef FLIPPING
    // frame will not be allocated in case of overlay or nonstrech blit on primary
    if (flipping && (!stretch)) {
     frame = primary;
     frameallocated = 0;
     if (verbose) printf("DirectFB: Frame is NOT used (flipping is active).\n");
    } else {
#endif
    /*
    dsc.caps  = DSCAPS_SYSTEMONLY;
      //let dfb decide where frame should be - preparation for AGP support
    */
     dsc.flags &=~DSDESC_CAPS;
     
    DFBCHECK (dfb->CreateSurface( dfb, &dsc, &frame));
    frameallocated=1;
#ifdef FLIPPING
     if (verbose) if (flipping) {
	printf("DirectFB: Frame created (flipping&stretch is active).\n");
     } else {
	printf("DirectFB: Frame created (flipping is NOT active).\n");
     };
    };
#endif
  
  DFBCHECK (frame->GetPixelFormat (frame, &frame_format));

  switch (frame_format) {
                case DSPF_ARGB:  if (verbose) printf("DirectFB: frame format ARGB\n");
                                 frame_pixel_size = 4;
                                 break;
                case DSPF_RGB32: if (verbose) printf("DirectFB: frame format RGB32\n");
                                 frame_pixel_size = 4;
                                 break;
                case DSPF_RGB24: if (verbose) printf("DirectFB: frame format RGB24\n");
                                 frame_pixel_size = 3;
                                 break;
                case DSPF_RGB16: if (verbose) printf("DirectFB: frame format RGB16\n");
                                 frame_pixel_size = 2;
                                 break;
                case DSPF_RGB15: if (verbose) printf("DirectFB: frame format RGB15\n");
                                 frame_pixel_size = 2;
                                 break;
                case DSPF_YUY2:  if (verbose) printf("DirectFB: frame format YUY2\n");
                                 frame_pixel_size = 2;
                                 break;
                case DSPF_UYVY:  if (verbose) printf("DirectFB: frame format UYVY\n");
                                 frame_pixel_size = 2;
                                 break;
#if DIRECTFBVERSION > 908
                case DSPF_YV12:  if (verbose) printf("DirectFB: frame format YV12\n");
                                 frame_pixel_size = 1;
                                 break;
                case DSPF_I420:  if (verbose) printf("DirectFB: frame format I420\n");
                                 frame_pixel_size = 1;
                                 break;
#endif
                default: printf("DirectFB: - unknown format ->exit\n"); return 1;
        }

	if ((out_width < in_width || out_height < in_height) && (!fs)) {
		printf("Screensize is smaller than video size !\n");
//		return 1;  // doesn't matter we will rescale
	}



// yuv2rgb transform init

 if (((format == IMGFMT_YV12) || (format == IMGFMT_YUY2)) && (!videolayeractive)){ yuv2rgb_init(frame_pixel_size * 8,MODE_RGB);};

 if ((verbose)&&(memcpyBitBlt)) printf("DirectFB: Using memcpyBitBlt\n");
#ifdef DIRECTRENDER
//direct rendering is enabled in case of same buffer and img format
 if ((format==IMGFMT_RGB32)&&(frame_format ==DSPF_ARGB) ||
     (format==IMGFMT_BGR32)&&(frame_format ==DSPF_ARGB) ||
     (format==IMGFMT_RGB24)&&(frame_format ==DSPF_RGB24) ||
     (format==IMGFMT_BGR24)&&(frame_format ==DSPF_RGB24) ||
     (format==IMGFMT_RGB16)&&(frame_format ==DSPF_RGB16) ||
     (format==IMGFMT_BGR16)&&(frame_format ==DSPF_RGB16) ||
     (format==IMGFMT_RGB15)&&(frame_format ==DSPF_RGB15) ||
     (format==IMGFMT_BGR15)&&(frame_format ==DSPF_RGB15) ||
#if DIRECTFBVERSION > 908
     (format==IMGFMT_YUY2)&&(frame_format ==DSPF_YUY2) ||
     (format==IMGFMT_YV12)&&(frame_format ==DSPF_I420) ||
     (format==IMGFMT_YV12)&&(frame_format ==DSPF_YV12)){
#else     
     (format==IMGFMT_YUY2)&&(frame_format ==DSPF_YUY2)){
#endif
     	dr_enabled=1;
	if (verbose) printf("DirectFB: Direct rendering supported\n");
 } else {
      	dr_enabled=0;
	if (verbose) printf("DirectFB: Direct rendering not supported\n");
 };
#endif 
	

 if (verbose) printf("DirectFB: Config finished [%ix%i]\n",out_width,out_height);

return 0;
}

static uint32_t query_format(uint32_t format)
{
	int ret = VFCAP_CSP_SUPPORTED|VFCAP_OSD; /* osd/sub is supported on every bpp */

//        preinit(NULL);

	if (verbose ) printf("DirectFB: Format query: %s\n",vo_format_name(format));

	switch (format) {
		// primary
                case IMGFMT_RGB32:
                case IMGFMT_BGR32: 
                case IMGFMT_RGB24:
                case IMGFMT_BGR24:
                case IMGFMT_RGB16:
                case IMGFMT_BGR16:
                case IMGFMT_RGB15:
                case IMGFMT_BGR15:  if (hwstretchblit) ret |=VFCAP_HWSCALE_UP|VFCAP_HWSCALE_DOWN;
				     break;
		// overlay		     
                case IMGFMT_YUY2:
        	case IMGFMT_YV12:   if (videolayercaps.hwscale) ret |=VFCAP_HWSCALE_UP|VFCAP_HWSCALE_DOWN;
                                    break;
 	}
	

	switch (format) {

// RGB mode works only if color depth is same as on screen and this driver doesn't know before init
// so we couldn't report supported formats well

// Just support those detected by preinit
                case IMGFMT_RGB32:
                case IMGFMT_BGR32: if (modes[3].valid) return ret;
                                   break;
                case IMGFMT_RGB24:
                case IMGFMT_BGR24: if (modes[2].valid) return ret;
                                   break;
                case IMGFMT_RGB16:
                case IMGFMT_BGR16:
                case IMGFMT_RGB15:
                case IMGFMT_BGR15: if (modes[1].valid) return ret;
                                   break;
                case IMGFMT_YUY2: if (videolayerpresent) {
				    if (videolayercaps.hwscale) ret |=VFCAP_HWSCALE_UP|VFCAP_HWSCALE_DOWN;
				    if (videolayercaps.yuy2) {
					return ret|VFCAP_CSP_SUPPORTED_BY_HW;
// disabled - MPlayer will do conversion automatically
/*				    } else {
				    	return ret;
*/				    };
				   };				    
                                   break;
        	case IMGFMT_YV12:  if ((videolayerpresent) &&
				       (videolayercaps.i420 || videolayercaps.iv12))
				    return ret|VFCAP_CSP_SUPPORTED_BY_HW;
// disabled - MPlayer will do conversion automatically
/*				    else return ret;
*/                                   break;
 	}

	return 0;
}

static void draw_alpha(int x0, int y0, int w, int h, unsigned char *src,
		unsigned char *srca, int stride)
{
        void *dst;
        int pitch;
	int len;
	static IDirectFBSurface *surface = NULL;
	

#ifdef DIRECTRENDER
	if(framelocked) {
	    frame->Unlock(frame);
	    framelocked=0;
	};
#endif
#ifdef FLIPPING
	if (flipping && stretch) {
        DFBCHECK (primary->Lock(primary,DSLF_WRITE,&dst,&pitch));
	} else {
#endif
        DFBCHECK (frame->Lock(frame,DSLF_WRITE,&dst,&pitch));
#ifdef FLIPPING
	}
#endif
	switch(frame_format) {
                case DSPF_RGB32:
                case DSPF_ARGB:
                        vo_draw_alpha_rgb32(w,h,src,srca,stride,((uint8_t *) dst)+pitch*y0 + frame_pixel_size*x0,pitch);
                        break;

                case DSPF_RGB24:
                        vo_draw_alpha_rgb24(w,h,src,srca,stride,((uint8_t *) dst)+pitch*y0 + frame_pixel_size*x0,pitch);
                        break;

                case DSPF_RGB16:
                        vo_draw_alpha_rgb16(w,h,src,srca,stride,((uint8_t *) dst)+pitch*y0 + frame_pixel_size*x0,pitch);
                        break;

                case DSPF_RGB15:
                        vo_draw_alpha_rgb15(w,h,src,srca,stride,((uint8_t *) dst)+pitch*y0 + frame_pixel_size*x0,pitch);
                        break;

		case DSPF_YUY2:
    			vo_draw_alpha_yuy2(w,h,src,srca,stride,((uint8_t *) dst) + pitch*y0 + frame_pixel_size*x0,pitch);
		break;

        	case DSPF_UYVY:
    			vo_draw_alpha_yuy2(w,h,src,srca,stride,((uint8_t *) dst) + pitch*y0 + frame_pixel_size*x0 + 1,pitch);
		break;

#if DIRECTFBVERSION > 908
        	case DSPF_I420:
		case DSPF_YV12:
    			vo_draw_alpha_yv12(w,h,src,srca,stride,((uint8_t *) dst) + pitch*y0 + frame_pixel_size*x0,pitch);
		break;
#endif
		}
#ifdef FLIPPING
	if (flipping && stretch) {
        DFBCHECK (primary->Unlock(primary));
	} else {
#endif
        DFBCHECK (frame->Unlock(frame));
#ifdef FLIPPING
	}
#endif
}

static uint32_t draw_frame(uint8_t *src[])
{
        void *dst;
        int pitch;
	int len;

//        printf("Drawframe\n");
#ifdef DIRECTRENDER
	if(framelocked) {
	    frame->Unlock(frame);
	    framelocked=0;
	};
#endif

        DFBCHECK (frame->Lock(frame,DSLF_WRITE,&dst,&pitch));

        switch (frame_format) {
                case DSPF_ARGB:
                case DSPF_RGB32:
		case DSPF_RGB24:
                case DSPF_RGB16:
                case DSPF_RGB15: switch (pixel_format) {
                                    case IMGFMT_YV12:
                                        yuv2rgb(dst,src[0],src[1],src[2],in_width,in_height,pitch,in_width,in_width/2);
                                        break;
                                /* how to handle this? need conversion from YUY2 to RGB*/
/*                                case IMGFMT_YUY2:
                                        yuv2rgb(dst,src[0],src[0]+1,src[0]+3,1,in_height*in_width/2,frame_pixel_size*2,4,4); //odd pixels
                                        yuv2rgb(dst+1,src[0]+2,src[0]+1,src[0]+3,1,in_height*in_width/2,frame_pixel_size*2,4,4); //even pixels
                                        break;*/
				// RGB - just copy
	                	    default:    if (source_pixel_size==frame_pixel_size) {
						    if (pitch==(in_width*frame_pixel_size)) {
						    memcpy(dst,src[0],in_width * in_height * source_pixel_size);
						    } else {
						    int i;
						    int sp=in_width*source_pixel_size;
						    int ll=min(sp,pitch);
						    for (i=0;i<in_height;i++) {
							memcpy(dst+i*pitch,src[0]+i*sp,ll);
						        };
						    };
						};
				};
				break;
                case DSPF_YUY2:
                        switch (pixel_format) {
                        	case IMGFMT_YV12:   yv12toyuy2(src[0],src[1],src[2],dst,in_width,in_height,in_width,in_width >>1,pitch);
		                        	    break;
	                        case IMGFMT_YUY2:   if (pitch==(in_width*2)) {
						     memcpy(dst,src[0],in_width * in_height * source_pixel_size);
						    } else {
                                            	    int i;
                                            	    for (i=0;i<in_height;i++) {
                                                        memcpy(dst+i*pitch,src[0]+i*in_width*2,in_width*2);
                                                         }
                                            	    }
		        	            	    break;
                                // hopefully there will be no RGB in this case otherwise convert - not implemented
	                };
                        break;

#if DIRECTFBVERSION > 908
                case DSPF_YV12:
                        switch (pixel_format) {
                        	case IMGFMT_YV12: {
						    int i;
						    int p=min(in_width,pitch);
                                            	    for (i=0;i<in_height;i++) {
                                                        memcpy(dst+i*pitch,src[0]+i*in_width,p);
                                                    }
						    dst += pitch*in_height;
						    p = p/2;
                                            	    for (i=0;i<in_height/2;i++) {
                                                        memcpy(dst+i*pitch/2,src[2]+i*in_width/2,p);
                                                    }
						    dst += pitch*in_height/4;
                                            	    for (i=0;i<in_height/2;i++) {
                                                        memcpy(dst+i*pitch/2,src[1]+i*in_width/2,p);
                                                    }
						  };
		                        	  break;
	                        case IMGFMT_YUY2: yuy2toyv12(src[0],dst,dst+pitch*in_height+pitch*in_height/4,dst+pitch*in_height,in_width,in_height,pitch,pitch/2,pitch/2);
		        	                  break;
                              // hopefully there will be no RGB in this case otherwise convert - not implemented
                        }
                        break;
                case DSPF_I420:
                        switch (pixel_format) {
                        	case IMGFMT_YV12: {
						    int i;
						    int p=min(in_width,pitch);
                                            	    for (i=0;i<in_height;i++) {
                                                        memcpy(dst+i*pitch,src[0]+i*in_width,p);
                                                    }
						    dst += pitch*in_height;
						    p = p/2;
                                            	    for (i=0;i<in_height/2;i++) {
                                                        memcpy(dst+i*pitch/2,src[1]+i*in_width/2,p);
                                                    }
						    dst += pitch*in_height/4;
                                            	    for (i=0;i<in_height/2;i++) {
                                                        memcpy(dst+i*pitch/2,src[2]+i*in_width/2,p);
                                                    }
						  };
		                        	  break;
	                        case IMGFMT_YUY2: yuy2toyv12(src[0],dst,dst+pitch*in_height,dst+pitch*in_height+pitch*in_height/4,in_width,in_height,pitch,pitch/2,pitch/2);
		        	                  break;
                              // hopefully there will be no RGB in this case otherwise convert - not implemented
                        }
                        break;
#endif
        }
        DFBCHECK (frame->Unlock(frame));
        return 0;
}

static uint32_t draw_slice(uint8_t *src[], int stride[], int w, int h, int x, int y)
{

        int err;
        void *dst;
	uint8_t *s;
        int pitch;
	int i;

#ifdef DIRECTRENDER
	if(framelocked) {
	    frame->Unlock(frame);
	    framelocked=0;
	};
#endif

        err = frame->Lock(frame,DSLF_WRITE,&dst,&pitch);
//        err = primary->Lock(primary,DSLF_WRITE,&dst,&pitch); // for direct rendering

//        printf("Drawslice w=%i h=%i x=%i y=%i pitch=%i\n",w,h,x,y,pitch);

	if (err) {
	    printf("DirectFB: Frame lock failed!");
	    return 1;
	};
        switch (frame_format) {
                case DSPF_ARGB:
                case DSPF_RGB32:
                case DSPF_RGB24:
                case DSPF_RGB16:
                case DSPF_RGB15:
                        switch (pixel_format) {
                                case IMGFMT_YV12:
                                        yuv2rgb(dst+ y * pitch + frame_pixel_size*x ,src[0],src[1],src[2],w,h,pitch,stride[0],stride[1]);
                                        break;
                                default:    if (source_pixel_size==frame_pixel_size) {
                                                        dst += x * frame_pixel_size;
				                        s = src[0];
				                        for (i=y;i<(y+h);i++) {
					                        memcpy(dst,s,w);
					                        dst += (pitch);
					                        s += stride[0];
					                        };
                                               }
				            break;

                                }
                        break;
                case DSPF_YUY2:
                	switch (pixel_format) {
	                        case IMGFMT_YV12:   yv12toyuy2(src[0],src[1],src[2],dst + pitch*y + frame_pixel_size*x ,w,h,stride[0],stride[1],pitch);
                 				break;
                                // hopefully there will be no RGB in this case otherwise convert - not implemented
                        	}
                         break;

#if DIRECTFBVERSION > 908
                case DSPF_YV12:
                        switch (pixel_format) {
                        	case IMGFMT_YV12: { 
						    void *d,*s;
						    int i;
						    d = dst + pitch*y + x;
						    s = src[0];
                                            	    for (i=0;i<h;i++) {
                                                        memcpy(d,s,w);
							d+=pitch;
							s+=stride[0];
                                                    }
						    d = dst + pitch*in_height + pitch*y/4 + x/2;
						    s = src[2];
                                            	    for (i=0;i<h/2;i++) {
                                                        memcpy(d,s,w/2);
							d+=pitch/2;
							s+=stride[2];
                                                    }
						    d = dst + pitch*in_height + pitch*in_height/4 + pitch*y/4 + x/2;
						    s = src[1];
                                            	    for (i=0;i<h/2;i++) {
                                                        memcpy(d,s,w/2);
							d+=pitch/2;
							s+=stride[1];
                                                    }
						  };
		                        	  break;
/*	                        case IMGFMT_YUY2: {
						    int i;
                                            	    for (i=y;i<(y+h);i++) {
							yuy2toyv12(src[0]+i*stride[0],dst+i*pitch+x*frame_pixel_size,dst+pitch*(in_height+i/2)+x*frame_pixel_size/2,dst+pitch*(in_height+in_height/4+i/2)+x*frame_pixel_size/2,w,h,pitch,pitch/2,pitch/2);
                                                    }
						  }
				 
		        	                  break;
*/                                // hopefully there will be no RGB in this case otherwise convert - not implemented
                        }
                        break;

                case DSPF_I420:
                        switch (pixel_format) {
                        	case IMGFMT_YV12: {
						    void *d,*s;
						    int i;
						    d = dst + pitch*y + x;
						    s = src[0];
                                            	    for (i=0;i<h;i++) {
                                                        memcpy(d,s,w);
							d+=pitch;
							s+=stride[0];
                                                    }
						    d = dst + pitch*in_height + pitch*y/4 + x/2;
						    s = src[1];
                                            	    for (i=0;i<h/2;i++) {
                                                        memcpy(d,s,w/2);
							d+=pitch/2;
							s+=stride[1];
                                                    }
						    d = dst + pitch*in_height + pitch*in_height/4 + pitch*y/4 + x/2;
						    s = src[2];
                                            	    for (i=0;i<h/2;i++) {
                                                        memcpy(d,s,w/2);
							d+=pitch/2;
							s+=stride[2];
                                                    }
		  				  };
		                        	  break;
/*	                        case IMGFMT_YUY2: {
						    int i;
                                            	    for (i=y;i<(y+h);i++) {
							yuy2toyv12(src[0]+i*stride[0],dst+i*pitch+x*frame_pixel_size,dst+pitch*(in_height+in_height/4+i/2)+x*frame_pixel_size/2,dst+pitch*(in_height+i/2)+x*frame_pixel_size/2,w,h,pitch,pitch/2,pitch/2);
                                                    }
						  }
				 
		        	                  break;
*/                                // hopefully there will be no RGB in this case otherwise convert - not implemented
                        }
                        break;
#endif
        };

        frame->Unlock(frame);
//        primary->Unlock(primary);
	 
	return 0;
}

extern void mplayer_put_key(int code);

#include "osdep/keycodes.h"

static void check_events(void)
{

	if (buffer) {

      DFBInputEvent event;
//if (verbose) printf ("DirectFB: Check events entered\n");
#if DIRECTFBVERSION > 909
if (buffer->GetEvent(buffer, DFB_EVENT (&event)) == DFB_OK) {
#else
if (buffer->GetEvent(buffer, &event) == DFB_OK) {
#endif
     if (event.type == DIET_KEYPRESS) { 
#if DIRECTFBVERSION > 910
    		switch (event.key_symbol) {
                                case DIKS_ESCAPE:
					mplayer_put_key('q');
				break;
				case DIKS_PAGE_UP: mplayer_put_key(KEY_PAGE_UP);break;
				case DIKS_PAGE_DOWN: mplayer_put_key(KEY_PAGE_DOWN);break;
                                case DIKS_CURSOR_UP: mplayer_put_key(KEY_UP);break;
                                case DIKS_CURSOR_DOWN: mplayer_put_key(KEY_DOWN);break;
                                case DIKS_CURSOR_LEFT: mplayer_put_key(KEY_LEFT);break;
                                case DIKS_CURSOR_RIGHT: mplayer_put_key(KEY_RIGHT);break;
				case DIKS_INSERT: mplayer_put_key(KEY_INSERT);break;
				case DIKS_DELETE: mplayer_put_key(KEY_DELETE);break;
				case DIKS_HOME: mplayer_put_key(KEY_HOME);break;
				case DIKS_END: mplayer_put_key(KEY_END);break;

                default:mplayer_put_key(event.key_symbol);
                };
#else  /* DirectFB < 0.9.11 */
    		switch (event.keycode) {
                                case DIKC_ESCAPE: mplayer_put_key('q');break;
                                case DIKC_KP_PLUS: mplayer_put_key('+');break;
                                case DIKC_KP_MINUS: mplayer_put_key('-');break;
				case DIKC_TAB: mplayer_put_key('\t');break;
				case DIKC_PAGEUP: mplayer_put_key(KEY_PAGE_UP);break;
				case DIKC_PAGEDOWN: mplayer_put_key(KEY_PAGE_DOWN);break;
                                case DIKC_UP: mplayer_put_key(KEY_UP);break;
                                case DIKC_DOWN: mplayer_put_key(KEY_DOWN);break;
                                case DIKC_LEFT: mplayer_put_key(KEY_LEFT);break;
                                case DIKC_RIGHT: mplayer_put_key(KEY_RIGHT);break;
                                case DIKC_ASTERISK:
				case DIKC_KP_MULT:mplayer_put_key('*');break;
                                case DIKC_KP_DIV: mplayer_put_key('/');break;
				case DIKC_INSERT: mplayer_put_key(KEY_INSERT);break;
				case DIKC_DELETE: mplayer_put_key(KEY_DELETE);break;
				case DIKC_HOME: mplayer_put_key(KEY_HOME);break;
				case DIKC_END: mplayer_put_key(KEY_END);break;

                default:mplayer_put_key(event.key_ascii);
                };
#endif
	};
    };
}
// empty buffer, because of repeating (keyboard repeat is faster than key handling
// and this causes problems during seek)
// temporary workabout should be solved in the future

if (buffer) buffer->Reset(buffer);
//if (verbose) printf ("DirectFB: Check events finished\n");

}

static void draw_osd(void)
{
// if flipping is active we will draw directly to primary
// we will also blit from frame to primary if necessary
#ifdef FLIPPING
        if (stretch && flipping) {
        	DFBRectangle rect;
        	rect.x=xoffset;
	        rect.y=yoffset;
	        rect.w=out_width;
	        rect.h=out_height;
#ifdef DIRECTRENDER
		if(framelocked) {
	            frame->Unlock(frame);
		    framelocked=0;
		};
#endif
		// let's clear blackborders
		primary->SetColor(primary,0,0,0,0);
		// top
		primary->FillRectangle(primary,0,0,screen_width,yoffset);
		// bottom
		primary->FillRectangle(primary,0,screen_height-yoffset,screen_width,yoffset);
		//left
//		primary->FillRectangle(primary,0,yoffset,xoffset,screen_height-2*yoffset);
		//right
//		primary->FillRectangle(primary,screen_width-xoffset-1,yoffset,xoffset,screen_height-2*yoffset);
		
                DFBCHECK (primary->StretchBlit(primary,frame,NULL,&rect));
		blitperformed=1;
    		vo_draw_text(screen_width, screen_height, draw_alpha);
        } else {
#endif	
	vo_draw_text(in_width, in_height, draw_alpha);
#ifdef FLIPPING
	}
#endif	
}

static void flip_page(void)
{
	DFBSurfaceBlittingFlags flags=DSBLIT_NOFX;

//	if (verbose) printf("DirectFB: Flip page entered");
	
	DFBCHECK (primary->SetBlittingFlags(primary,flags));

#ifdef DIRECTRENDER
	if(framelocked) {
	    frame->Unlock(frame);
	    framelocked=0;
	};
#endif
        if (stretch 
#ifdef FLIPPING
	&& (!blitperformed)
#endif
	) {
        	DFBRectangle rect;
        	rect.x=xoffset;
	        rect.y=yoffset;
	        rect.w=out_width;
	        rect.h=out_height;

                DFBCHECK (primary->StretchBlit(primary,frame,NULL,&rect));
#ifdef FLIPPING
		blitperformed=0;
#endif
                }
        else    {
#ifdef FLIPPING
	 if (!flipping) {
#endif
#if DIRECTFBVERSION > 908
		if (!memcpyBitBlt) {
#endif
            	    DFBCHECK (primary->Blit(primary,frame,NULL,xoffset,yoffset));
#if DIRECTFBVERSION > 908
		} else {
			
		    int err,err2;
		    void *dst,*src;
	            int pitch,pitch2;

//		    printf("MemcpyBlit");
		    
	            err = frame->Lock(frame,DSLF_READ,&src,&pitch);
	            err2 = primary->Lock(primary,DSLF_WRITE,&dst,&pitch2);

//		    printf("DirectFB: pitch=%i pitch2=%i\n",pitch,pitch2);
		

		    if (pitch==pitch2) {
			memcpy(dst,src,in_height * pitch * 1.5);
		    } else 
			    {
			int i;
			int p=min(pitch,pitch2);
			for (i=0;i<in_height;i++) {
			    memcpy (dst+i*pitch2,src+i*pitch,p);
			};
			dst+= in_height * pitch2;
			src+= in_height * pitch;
			p=p/2;
			for (i=0;i<in_height/2;i++) {
			    memcpy (dst+i*pitch2/2,src+i*pitch/2,p);
			};
			dst+= in_height * pitch2/4;
			src+= in_height * pitch/4;
			for (i=0;i<in_height/2;i++) {
			    memcpy (dst+i*pitch2/2,src+i*pitch/2,p);
			};
		    }	
	            frame->Unlock(frame);
	            primary->Unlock(primary);
		};
#endif		
#ifdef FLIPPING
                };
#endif		
        };
#ifdef FLIPPING
    if (flipping) { 
	if (videolayeractive && wait_vsync_after_flip) {
	    DFBCHECK (primary->Flip (primary, NULL, /*DSFLIP_WAITFORSYNC*/0));
	    /* workabout for videolayer
	    flip will take place on next vsync, but pointers are updated instanlty ->
	    -> decoding goes into "new" buffer which is still displayed ->
	    -> so wait for vsync to be safe (and have surfaces REALLY flipped)
	    */
	    dfb->WaitForSync(dfb); 
	} else {
    	    DFBCHECK (primary->Flip (primary, NULL, DSFLIP_WAITFORSYNC));
	};
    };
#endif    
}

static void uninit(void)
{
  if (verbose ) printf("DirectFB: uninit entered\n");
  /*
   * (Release)
   */
  if (verbose ) printf("DirectFB: Release buffer\n");
  if (buffer) buffer->Release (buffer);
  if (verbose ) printf("DirectFB: Release keyboard\n");
  if (keyboard) keyboard->Release (keyboard);
  if (frameallocated) {
    if (verbose ) printf("DirectFB: Release frame\n");
    frame->Release (frame);
    frameallocated=0;
  };
  
// we will not release dfb and layer because there could be a new film

  if (verbose ) printf("DirectFB: Release primary\n");
  primary->Release (primary);
//  switch off BES
  if (videolayer) videolayer->SetOpacity(videolayer,0);

#if DIRECTFBVERSION > 908
  if (verbose&&videolayer ) printf("DirectFB: Release videolayer\n");
  if (videolayer) videolayer->Release(videolayer);

  if (verbose ) printf("DirectFB: Release DirectFB library\n");
  dfb->Release (dfb);
#endif

  if (verbose ) printf("DirectFB: Uninit done.\n");
}

static uint32_t directfb_set_video_eq(char *data, int value) //data==name
{
	
	DFBColorAdjustment ca;
	float factor =  (float)0xffff / 200.0;
	DFBDisplayLayerCapabilities  caps;

if (videolayer) {	
	
#if DIRECTFBVERSION > 912
        DFBDisplayLayerDescription  desc;
        videolayer->GetDescription(videolayer,&desc);
        caps = desc.caps;
#else
	videolayer->GetCapabilities(videolayer,&caps);
#endif
	ca.flags=DCAF_NONE;
	
	if (! strcmp( data,"brightness" )) {
	    if (caps & DLCAPS_BRIGHTNESS) {
		ca.brightness = value * factor +0x8000;
	    ca.flags |= DCAF_BRIGHTNESS;
		if (verbose) printf("DirectFB: SetVEq Brightness 0x%X %i\n",ca.brightness,value);
	    } else return VO_FALSE;
	}

	if (! strcmp( data,"contrast" )) {
	    if ((caps & DLCAPS_CONTRAST)) {
	        ca.contrast = value * factor + 0x8000;
	    ca.flags |= DCAF_CONTRAST;
		if (verbose) printf("DirectFB: SetVEq Contrast 0x%X %i\n",ca.contrast,value);
	    } else return VO_FALSE;
	}

	if (! strcmp( data,"hue" )) {
    	    if ((caps & DLCAPS_HUE)) {
		ca.hue = value * factor + 0x8000;
	    ca.flags |= DCAF_HUE;
		if (verbose) printf("DirectFB: SetVEq Hue 0x%X %i\n",ca.hue,value);
	    } else return VO_FALSE;
	}

	if (! strcmp( data,"saturation" )) {
		if ((caps & DLCAPS_SATURATION)) {
	        ca.saturation = value * factor + 0x8000;
	    ca.flags |= DCAF_SATURATION;
		if (verbose) printf("DirectFB: SetVEq Saturation 0x%X %i\n",ca.saturation,value);
	    } else return VO_FALSE;
	}

	if (ca.flags != DCAF_NONE) {
	videolayer->SetColorAdjustment(videolayer,&ca);
	    return VO_TRUE;
	}
}	

    return VO_FALSE;

}

static uint32_t directfb_get_video_eq(char *data, int *value) // data==name
{
	
	DFBColorAdjustment ca;
	float factor = 200.0 / (float)0xffff;
	DFBDisplayLayerCapabilities  caps;
	
if (videolayer) {	
#if DIRECTFBVERSION > 912
        DFBDisplayLayerDescription  desc;
        videolayer->GetDescription(videolayer,&desc);
        caps = desc.caps;
#else
	videolayer->GetCapabilities(videolayer,&caps);
#endif

	videolayer->GetColorAdjustment(videolayer,&ca);
	
	if (! strcmp( data,"brightness" )) {
	    if (caps & DLCAPS_BRIGHTNESS) {
		*value = (int) ((ca.brightness-0x8000) * factor);
		if (verbose) printf("DirectFB: GetVEq Brightness 0x%X %i\n",ca.brightness,*value);
    		return VO_TRUE;
	    } else return VO_FALSE;
	}

	if (! strcmp( data,"contrast" )) {
	    if ((caps & DLCAPS_CONTRAST)) {
		*value = (int) ((ca.contrast-0x8000) * factor);
		if (verbose) printf("DirectFB: GetVEq Contrast 0x%X %i\n",ca.contrast,*value);
    		return VO_TRUE;
	    } else return VO_FALSE;
	}

	if (! strcmp( data,"hue" )) {
    	    if ((caps & DLCAPS_HUE)) {
		*value = (int) ((ca.hue-0x8000) * factor);
    		if (verbose) printf("DirectFB: GetVEq Hue 0x%X %i\n",ca.hue,*value);
    		return VO_TRUE;
	    } else return VO_FALSE;
	}

	if (! strcmp( data,"saturation" )) {
    	    if ((caps & DLCAPS_SATURATION)) {
		*value = (int) ((ca.saturation-0x8000) * factor);
    		if (verbose) printf("DirectFB: GetVEq Saturation 0x%X %i\n",ca.saturation,*value);
    		return VO_TRUE;
	    } else return VO_FALSE;
	}
}
    return VO_FALSE;
}

#ifdef DIRECTRENDER
static uint32_t get_image(mp_image_t *mpi){
        int err;
        void *dst;
        int pitch;

//    printf("DirectFB: get_image() called\n");

//    now we are always in system memory (in this version - mybe will change in future)
#ifdef FLIPPING
    if((mpi->flags&MP_IMGFLAG_READABLE) && invram) return VO_FALSE; // slow video ram
#endif
//    printf("width=%d vs. pitch=%d, flags=0x%X  \n",mpi->width,pitch,mpi->flags);
    if((mpi->width==pitch/frame_pixel_size) ||
       (mpi->flags&(MP_IMGFLAG_ACCEPT_STRIDE|MP_IMGFLAG_ACCEPT_WIDTH))){
       // we're lucky or codec accepts stride => ok, let's go!
       if(mpi->flags&MP_IMGFLAG_PLANAR){

#if DIRECTFBVERSION > 908
            err = frame->Lock(frame,DSLF_WRITE/*|DSLF_READ*/,&dst,&pitch);
//  	    err = primary->Lock(primary,DSLF_WRITE,&dst,&pitch); // for real direct rendering

	    if (err) {
		printf("DirectFB: Frame lock failed!");
		return VO_FALSE;
	    };
	    framelocked=1;

    	   //YV12 format
	   mpi->planes[0]=dst;
	   switch(frame_format) {
	    case DSPF_I420: mpi->planes[1]=dst + pitch*in_height;
			    mpi->planes[2]=mpi->planes[1] + pitch*in_height/4;
			    break;
	    case DSPF_YV12: mpi->planes[2]=dst + pitch*in_height;
			    mpi->planes[1]=mpi->planes[1] + pitch*in_height/4;
			    break;
			    
	   }
	   mpi->width=mpi->stride[0]=pitch;
	   mpi->stride[1]=mpi->stride[2]=pitch/2;
#else
	   return VO_FALSE;
#endif	   
       } else {
            err = frame->Lock(frame,DSLF_WRITE/*|DSLF_READ*/,&dst,&pitch);
//  	    err = primary->Lock(primary,DSLF_WRITE,&dst,&pitch); // for real direct rendering

	    if (err) {
		printf("DirectFB: Frame lock failed!");
		return VO_FALSE;
	    };
	    framelocked=1;
    	   //YUY2 and RGB formats
           mpi->planes[0]=dst;
	   mpi->width=pitch/frame_pixel_size;
	   mpi->stride[0]=pitch;
       }
       mpi->flags|=MP_IMGFLAG_DIRECT;
//       printf("DirectFB: get_image() SUCCESS -> Direct Rendering ENABLED\n");
       return VO_TRUE;
    }
    
    if(framelocked) {
	    frame->Unlock(frame);
	    framelocked=0;
    };
    return VO_FALSE;
}
#endif

static uint32_t control(uint32_t request, void *data, ...)
{
  switch (request) {
    case VOCTRL_SET_EQUALIZER:
      {
        va_list ap;
	int value;
    
        va_start(ap, data);
	value = va_arg(ap, int);
        va_end(ap);
    
	return(directfb_set_video_eq(data, value));
      }
    case VOCTRL_GET_EQUALIZER:
      {
	va_list ap;
        int *value;
    
        va_start(ap, data);
        value = va_arg(ap, int*);
        va_end(ap);
    
	return(directfb_get_video_eq(data, value));
      }
  case VOCTRL_QUERY_FORMAT:
    return query_format(*((uint32_t*)data));
#ifdef DIRECTRENDER
  case VOCTRL_GET_IMAGE:
//    printf("DirectFB: control(VOCTRL_GET_IMAGE) called\n");
    if (dr_enabled) return get_image(data);
#endif    
  }
  return VO_NOTIMPL;
}
