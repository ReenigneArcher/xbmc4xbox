/*
    libgha.c - Library for direct hardware access
    Copyrights:
    1996/10/27	- Robin Cutshaw (robin@xfree86.org)
		  XFree86 3.3.3 implementation
    1999	- ?yvind Aabling.
    		  Modified for GATOS/win/gfxdump.
		  
    2002	- library implementation by Nick Kurshev
		- dhahelper and some changes by Alex Beregszaszi
    
    supported O/S's:	SVR4, UnixWare, SCO, Solaris,
			FreeBSD, NetBSD, 386BSD, BSDI BSD/386,
			Linux, Mach/386, ISC
			DOS (WATCOM 9.5 compiler), Win9x (with mapdev.vxd)
    Licence: GPL
    Original location: www.linuxvideo.org/gatos
*/

#include "config.h"

#include "libdha.h"
#include "AsmMacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef ARCH_ALPHA
#include <sys/io.h>
#endif
#include <unistd.h>

#if defined(WIN32)
#include "sysdep/libdha_win32.c"
#elif defined (__EMX__)
#include "sysdep/libdha_os2.c"
#else

#if defined(SVR4) || defined(SCO325)
#  if !(defined(sun) && defined (i386) && defined (SVR4))
#    define DEV_MEM "/dev/pmem"
#  elif defined(PowerMAX_OS)
#    define DEV_MEM "/dev/iomem"
#  endif
#  ifdef SCO325
#   undef DEV_MEM
#   define DEV_MEM "/dev/mem"
#  endif
# endif /* SVR4 */

/* Generic version */
#include <sys/mman.h>

#ifndef DEV_MEM
#define DEV_MEM "/dev/mem"
#endif

#ifdef CONFIG_DHAHELPER
#include "kernelhelper/dhahelper.h"
#endif

#ifdef CONFIG_SVGAHELPER
#include <svgalib_helper.h>
#endif

static int mem_fd = -1;

void *map_phys_mem(unsigned long base, unsigned long size)
{    
#ifdef ARCH_ALPHA
/* TODO: move it into sysdep */
  base += bus_base();
#endif

#ifdef CONFIG_SVGAHELPER
  if ( (mem_fd = open(DEV_SVGA,O_RDWR)) == -1) {
      perror("libdha: SVGAlib kernelhelper failed");
#ifdef CONFIG_DHAHELPER
      goto dha_helper_way;
#else
      goto dev_mem_way;
#endif
  }
  else
      goto mmap;
#endif

#ifdef CONFIG_DHAHELPER
#ifdef CONFIG_SVGAHELPER
dha_helper_way:
#endif
  if ( (mem_fd = open("/dev/dhahelper",O_RDWR)) < 0)
  {
      perror("libdha: DHA kernelhelper failed");
      goto dev_mem_way;
  }
  else
  {
    dhahelper_memory_t mem_req;
    
    mem_req.operation = MEMORY_OP_MAP;
    mem_req.start = base;
    mem_req.offset = 0;
    mem_req.size = size;
    
    if (ioctl(mem_fd, DHAHELPER_MEMORY, &mem_req) < 0)
    {
	perror("libdha: DHA kernelhelper failed");
	close(mem_fd);
	goto dev_mem_way;
    }
    else
	goto mmap;
  }
#endif

dev_mem_way:
  if ( (mem_fd = open(DEV_MEM,O_RDWR)) == -1)
  {
    perror("libdha: opening /dev/mem failed");
    return MAP_FAILED;
  }

mmap:
  return mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,mem_fd,base);
}

void unmap_phys_mem(void *ptr, unsigned long size)
{
  int res = munmap(ptr,size);

  if (res == (int)MAP_FAILED)
  {
      perror("libdha: unmapping memory failed");
      return;
  }
  
  close(mem_fd);
  mem_fd = -1;
  
  return;
}

#endif /* Generic mmap (not win32, nor os2) */

unsigned char INPORT8(unsigned idx)
{
  return inb(idx);
}

unsigned short INPORT16(unsigned idx)
{
  return inw(idx);
}

unsigned INPORT32(unsigned idx)
{
  return inl(idx);
}

void OUTPORT8(unsigned idx,unsigned char val)
{
  outb(idx,val);
}

void OUTPORT16(unsigned idx,unsigned short val)
{
  outw(idx,val);
}

void OUTPORT32(unsigned idx,unsigned val)
{
  outl(idx,val);
}
