#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USE_THEORA

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#include "codec-cfg.h"
//#include "mp_image.h"

#include "img_format.h"

#include "stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "dec_video.h"

#include "vd.h"
#include "vf.h"

//#include "vd_internal.h"

extern vd_functions_t mpcodecs_vd_null;
extern vd_functions_t mpcodecs_vd_ffmpeg;
extern vd_functions_t mpcodecs_vd_theora;
extern vd_functions_t mpcodecs_vd_dshow;
extern vd_functions_t mpcodecs_vd_dmo;
extern vd_functions_t mpcodecs_vd_vfw;
extern vd_functions_t mpcodecs_vd_vfwex;
extern vd_functions_t mpcodecs_vd_odivx;
extern vd_functions_t mpcodecs_vd_divx4;
extern vd_functions_t mpcodecs_vd_raw;
extern vd_functions_t mpcodecs_vd_hmblck;
extern vd_functions_t mpcodecs_vd_xanim;
extern vd_functions_t mpcodecs_vd_mpng;
extern vd_functions_t mpcodecs_vd_ijpg;
extern vd_functions_t mpcodecs_vd_mtga;
extern vd_functions_t mpcodecs_vd_sgi;
extern vd_functions_t mpcodecs_vd_libmpeg2;
extern vd_functions_t mpcodecs_vd_mpegpes;
extern vd_functions_t mpcodecs_vd_zrmjpeg;
extern vd_functions_t mpcodecs_vd_realvid;
extern vd_functions_t mpcodecs_vd_xvid;
extern vd_functions_t mpcodecs_vd_libdv;
extern vd_functions_t mpcodecs_vd_lzo;
extern vd_functions_t mpcodecs_vd_qtvideo;

vd_functions_t* mpcodecs_vd_drivers[] = {
        &mpcodecs_vd_null,
#ifdef USE_LIBAVCODEC
        &mpcodecs_vd_ffmpeg,
#endif
#ifdef HAVE_OGGTHEORA
	&mpcodecs_vd_theora,
#endif
#ifdef USE_WIN32DLL
#ifdef USE_DIRECTSHOW
        &mpcodecs_vd_dshow,
        &mpcodecs_vd_dmo,
#endif
        &mpcodecs_vd_vfw,
        &mpcodecs_vd_vfwex,
#endif
#ifdef USE_DIVX
        &mpcodecs_vd_odivx,
#ifdef NEW_DECORE
        &mpcodecs_vd_divx4,
#endif
#endif
        &mpcodecs_vd_lzo,
        &mpcodecs_vd_raw,
        &mpcodecs_vd_hmblck,
#ifdef USE_XANIM
        &mpcodecs_vd_xanim,
#endif
#ifdef HAVE_PNG
        &mpcodecs_vd_mpng,
#endif
#ifdef HAVE_JPEG
	&mpcodecs_vd_ijpg,
#endif
	&mpcodecs_vd_mtga,
	&mpcodecs_vd_sgi,
#ifdef USE_LIBMPEG2
        &mpcodecs_vd_libmpeg2,
#endif
        &mpcodecs_vd_mpegpes,
#ifdef HAVE_ZR
        &mpcodecs_vd_zrmjpeg,
#endif
#ifdef USE_REALCODECS
	&mpcodecs_vd_realvid,
#endif
#if defined(HAVE_XVID3) || defined(HAVE_XVID4)
	&mpcodecs_vd_xvid,
#endif
#ifdef HAVE_LIBDV095
	&mpcodecs_vd_libdv,
#endif
#if defined(USE_QTX_CODECS) || defined(MACOSX)
	&mpcodecs_vd_qtvideo,
#endif
	NULL
};

#include "libvo/video_out.h"

// libvo opts:
int fullscreen=0;
int vidmode=0;
int softzoom=0;
int flip=-1;
int opt_screen_size_x=0;
int opt_screen_size_y=0;
float screen_size_xy=0;
float movie_aspect=-1.0;
int vo_flags=0;
int vd_use_slices=1;

/** global variables for gamma, brightness, contrast, saturation and hue 
    modified by mplayer.c and Gui/mplayer/gtk/eq.c:
    ranges -100 - 100
    1000 if the vo default should be used
*/   
int vo_gamma_gamma = 1000;
int vo_gamma_brightness = 1000;
int vo_gamma_contrast = 1000;
int vo_gamma_saturation = 1000;
int vo_gamma_hue = 1000;

extern vd_functions_t* mpvdec; // FIXME!
extern int divx_quality;

int mpcodecs_config_vo(sh_video_t *sh, int w, int h, unsigned int preferred_outfmt){
    int i,j;
    unsigned int out_fmt=0;
    int screen_size_x=0;//SCREEN_SIZE_X;
    int screen_size_y=0;//SCREEN_SIZE_Y;
//    vo_functions_t* video_out=sh->video_out;
    vf_instance_t* vf=sh->vfilter,*sc=NULL;
    int palette=0;

    if(!sh->disp_w || !sh->disp_h)
        mp_msg(MSGT_DECVIDEO,MSGL_WARN, MSGTR_CodecDidNotSet);
    /* XXX: HACK, if sh->disp_* aren't set,
     * but we have w and h, set them :: atmos */
    if(!sh->disp_w && w)
        sh->disp_w=w;
    if(!sh->disp_h && h)
        sh->disp_h=h;

    if(!sh->disp_w || !sh->disp_h)
	return 0;

    mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_VoConfigRequest,w,h,vo_format_name(preferred_outfmt));

//    if(!vf) return 1; // temp hack
    
    if(get_video_quality_max(sh)<=0 && divx_quality){
	// user wants postprocess but no pp filter yet:
	sh->vfilter=vf=vf_open_filter(vf,"pp",NULL);
    }

    // check if libvo and codec has common outfmt (no conversion):
csp_again:

    if(verbose>0){
	vf_instance_t* f=vf;
	mp_msg(MSGT_DECVIDEO,MSGL_V,"Trying filter chain:");
	for(f = vf ; f ; f = f->next)
	    mp_msg(MSGT_DECVIDEO,MSGL_V," %s",f->info->name);
	mp_msg(MSGT_DECVIDEO,MSGL_V,"\n");
    }

    j=-1;
    for(i=0;i<CODECS_MAX_OUTFMT;i++){
	int flags;
	out_fmt=sh->codec->outfmt[i];
	if(out_fmt==(unsigned int)0xFFFFFFFF) continue;
	flags=vf->query_format(vf,out_fmt);
	mp_msg(MSGT_CPLAYER,MSGL_DBG2,"vo_debug: query(%s) returned 0x%X (i=%d) \n",vo_format_name(out_fmt),flags,i);
	if((flags&2) || (flags && j<0)){
	    // check (query) if codec really support this outfmt...
	    sh->outfmtidx=j; // pass index to the control() function this way
	    if(mpvdec->control(sh,VDCTRL_QUERY_FORMAT,&out_fmt)==CONTROL_FALSE){
		mp_msg(MSGT_CPLAYER,MSGL_DBG2,"vo_debug: codec query_format(%s) returned FALSE\n",vo_format_name(out_fmt));
		continue;
	    }
	    j=i; vo_flags=flags; if(flags&2) break;
	} else
	if(!palette && !(flags&3) && (out_fmt==IMGFMT_RGB8||out_fmt==IMGFMT_BGR8)){
	    sh->outfmtidx=j; // pass index to the control() function this way
	    if(mpvdec->control(sh,VDCTRL_QUERY_FORMAT,&out_fmt)!=CONTROL_FALSE)
		palette=1;
	}
    }
    if(j<0){
	// TODO: no match - we should use conversion...
	if(strcmp(vf->info->name,"scale") && palette!=-1){
	    mp_msg(MSGT_DECVIDEO,MSGL_INFO,MSGTR_CouldNotFindColorspace);
	    sc=vf=vf_open_filter(vf,"scale",NULL);
	    goto csp_again;
	} else
	if(palette==1){
	    mp_msg(MSGT_DECVIDEO,MSGL_V,"vd: Trying -vf palette...\n");
	    palette=-1;
	    vf=vf_open_filter(vf,"palette",NULL);
	    goto csp_again;
	} else 
	{ // sws failed, if the last filter (vf_vo) support MPEGPES try to append vf_lavc
	     vf_instance_t* vo, *vp = NULL, *ve;
	     // Remove the scale filter if we added it ourself
	     if(vf == sc) {
	       ve = vf;
	       vf = vf->next;
	       vf_uninit_filter(ve);
	     }
	     // Find the last filter (vf_vo)
	     for(vo = vf ; vo->next ; vo = vo->next)
	       vp = vo;
	     if(vo->query_format(vo,IMGFMT_MPEGPES) && (!vp || (vp && strcmp(vp->info->name,"lavc")))) {
	       ve = vf_open_filter(vo,"lavc",NULL);
	       if(vp) vp->next = ve;
		 else vf = ve;
	       goto csp_again;
	     }
	}
	mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_VOincompCodec);
	sh->vf_inited=-1;
	return 0;	// failed
    }
    out_fmt=sh->codec->outfmt[j];
    mp_msg(MSGT_CPLAYER,MSGL_INFO,"VDec: using %s as output csp (no %d)\n",vo_format_name(out_fmt),j);
    sh->outfmtidx=j;
    sh->vfilter=vf;

    // autodetect flipping
    if(flip==-1){
	flip=0;
	if(sh->codec->outflags[j]&CODECS_FLAG_FLIP)
	    if(!(sh->codec->outflags[j]&CODECS_FLAG_NOFLIP))
		flip=1;
    }
    if(vo_flags&VFCAP_FLIPPED) flip^=1;
    if(flip && !(vo_flags&VFCAP_FLIP)){
	// we need to flip, but no flipping filter avail.
	vf_add_before_vo(&vf, "flip", NULL);
	sh->vfilter = vf;
    }

    // time to do aspect ratio corrections...

  if(movie_aspect>-1.0) sh->aspect = movie_aspect; // cmdline overrides autodetect
//  if(!sh->aspect) sh->aspect=1.0;

  if(opt_screen_size_x||opt_screen_size_y){
    screen_size_x = opt_screen_size_x;
    screen_size_y = opt_screen_size_y;
    if(!vidmode){
     if(!screen_size_x) screen_size_x=SCREEN_SIZE_X;
     if(!screen_size_y) screen_size_y=SCREEN_SIZE_Y;
     if(screen_size_x<=8) screen_size_x*=sh->disp_w;
     if(screen_size_y<=8) screen_size_y*=sh->disp_h;
    }
  } else {
    // check source format aspect, calculate prescale ::atmos
    screen_size_x=sh->disp_w;
    screen_size_y=sh->disp_h;
    if(screen_size_xy>=0.001){
     if(screen_size_xy<=8){
       // -xy means x+y scale
       screen_size_x*=screen_size_xy;
       screen_size_y*=screen_size_xy;
     } else {
       // -xy means forced width while keeping correct aspect
       screen_size_x=screen_size_xy;
       screen_size_y=screen_size_xy*sh->disp_h/sh->disp_w;
     }
    }
    if(sh->aspect>0.01){
      int w;
      mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_MovieAspectIsSet,sh->aspect);
      w=(int)((float)screen_size_y*sh->aspect); w+=w%2; // round
      // we don't like horizontal downscale || user forced width:
      if(w<screen_size_x || screen_size_xy>8){
        screen_size_y=(int)((float)screen_size_x*(1.0/sh->aspect));
        screen_size_y+=screen_size_y%2; // round
      } else screen_size_x=w; // keep new width
    } else {
      mp_msg(MSGT_CPLAYER,MSGL_INFO,MSGTR_MovieAspectUndefined);
    }
  }

    // Time to config libvo!
    mp_msg(MSGT_CPLAYER,MSGL_V,"VO Config (%dx%d->%dx%d,flags=%d,'%s',0x%X)\n",
                      sh->disp_w,sh->disp_h,
                      screen_size_x,screen_size_y,
                      fullscreen|(vidmode<<1)|(softzoom<<2)|(flip<<3),
                      "MPlayer",out_fmt);

    vf->w = sh->disp_w;
    vf->h = sh->disp_h;
    if(vf->config(vf,sh->disp_w,sh->disp_h,
                      screen_size_x,screen_size_y,
                      fullscreen|(vidmode<<1)|(softzoom<<2)|(flip<<3),
                      out_fmt)==0){
//                      "MPlayer",out_fmt)){
	mp_msg(MSGT_CPLAYER,MSGL_WARN,MSGTR_CannotInitVO);
	sh->vf_inited=-1;
	return 0;
    }

    sh->vf_inited=1;

    if (vo_gamma_gamma != 1000)
        set_video_colors(sh, "gamma", vo_gamma_gamma);
    if (vo_gamma_brightness != 1000)
        set_video_colors(sh, "brightness", vo_gamma_brightness);
    if (vo_gamma_contrast != 1000)
        set_video_colors(sh, "contrast", vo_gamma_contrast);
    if (vo_gamma_saturation != 1000)
        set_video_colors(sh, "saturation", vo_gamma_saturation);
    if (vo_gamma_hue != 1000)
        set_video_colors(sh, "hue", vo_gamma_hue);

    return 1;
}

// mp_imgtype: buffering type, see mp_image.h
// mp_imgflag: buffer requirements (read/write, preserve, stride limits), see mp_image.h
// returns NULL or allocated mp_image_t*
// Note: buffer allocation may be moved to mpcodecs_config_vo() later...
mp_image_t* mpcodecs_get_image(sh_video_t *sh, int mp_imgtype, int mp_imgflag, int w, int h){
  mp_image_t* mpi=vf_get_image(sh->vfilter,sh->codec->outfmt[sh->outfmtidx],mp_imgtype,mp_imgflag,w,h);
  mpi->x=mpi->y=0;
  return mpi;
}

void mpcodecs_draw_slice(sh_video_t *sh, unsigned char** src, int* stride, int w,int h, int x, int y) {
  struct vf_instance_s* vf = sh->vfilter;

  if(vf->draw_slice)
    vf->draw_slice(vf,src,stride,w,h,x,y);
}
