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
#include "DllLibCurl.h"
#include "TimeUtils.h"

#include <assert.h>

using namespace XCURL;

/* okey this is damn ugly. our dll loader doesn't allow for postload, preunload functions */
static long g_curlReferences = 0;
static unsigned int g_curlTimeout = 0;
DllLibCurlGlobal g_curlInterface;

bool DllLibCurlGlobal::Load()
{
  CSingleLock lock(m_critSection);
  if(g_curlReferences > 0)
  {
    g_curlReferences++;
    return true;
  }

  /* we handle this ourself */
  DllDynamic::EnableDelayedUnload(false);
  if (!DllDynamic::Load())
    return false;

  if (global_init(CURL_GLOBAL_ALL))
  {
    DllDynamic::Unload();
    CLog::Log(LOGERROR, "Error initializing libcurl");
    return false;
  }

  /* check idle will clean up the last one */
  g_curlReferences = 2;

  return true;
}

void DllLibCurlGlobal::Unload()
{
  CSingleLock lock(m_critSection);
  if (--g_curlReferences == 0)
  {
    if (!IsLoaded())
      return;

    // close libcurl
    global_cleanup();

    DllDynamic::Unload();
  }

  /* CheckIdle will clear this one up */
  if(g_curlReferences == 1)
    g_curlTimeout = CTimeUtils::GetTimeMS();
}

void DllLibCurlGlobal::CheckIdle()
{
  /* avoid locking section here, to avoid stalling gfx thread on loads*/
  if(g_curlReferences == 0)
    return;

  CSingleLock lock(m_critSection);
#ifdef _XBOX  
  /* 10 seconds idle time before closing handle. Set this low on Xbox to save memory */
  const DWORD idletime = 10000;
#else
  /* 30 seconds idle time before closing handle */
  const DWORD idletime = 30000;
#endif
  VEC_CURLSESSIONS::iterator it = m_sessions.begin();
  while(it != m_sessions.end())
  {
    if( !it->m_busy && it->m_idletimestamp + idletime < GetTickCount())
    {
      CLog::Log(LOGINFO, "%s - Closing session to %s://%s (easy=%p, multi=%p)\n", __FUNCTION__, it->m_protocol.c_str(), it->m_hostname.c_str(), (void*)it->m_easy, (void*)it->m_multi);

      // It's important to clean up multi *before* cleaning up easy, because the multi cleanup
      // code accesses stuff in the easy's structure.
      if(it->m_multi)
        multi_cleanup(it->m_multi);
      if(it->m_easy)
        easy_cleanup(it->m_easy);

      Unload();

      it = m_sessions.erase(it);
      continue;
    }
    it++;
  }

  /* check if we should unload the dll */
  if(g_curlReferences == 1 && CTimeUtils::GetTimeMS() > g_curlTimeout + idletime)
    Unload();
}

void DllLibCurlGlobal::easy_aquire(const char *protocol, const char *hostname, CURL_HANDLE** easy_handle, CURLM** multi_handle)
{
  assert(easy_handle != NULL);

  CSingleLock lock(m_critSection);

  VEC_CURLSESSIONS::iterator it;
  for(it = m_sessions.begin(); it != m_sessions.end(); it++)
  {
    if( !it->m_busy )
    {
      /* allow reuse of requester is trying to connect to same host */
      /* curl will take care of any differences in username/password */
      if( it->m_protocol.compare(protocol) == 0 && it->m_hostname.compare(hostname) == 0)
      {
        it->m_busy = true;
        if(easy_handle)
        {
          if(!it->m_easy)
            it->m_easy = easy_init();

          *easy_handle = it->m_easy;
        }

        if(multi_handle)
        {
          if(!it->m_multi)
            it->m_multi = multi_init();

          *multi_handle = it->m_multi;
        }

        return;
      }
    }
  }

  SSession session = {};
  session.m_busy = true;
  session.m_protocol = protocol;
  session.m_hostname = hostname;

  /* count up global interface counter */
  Load();

  if(easy_handle)
  {
    session.m_easy = easy_init();
    *easy_handle = session.m_easy;
  }

  if(multi_handle)
  {
    session.m_multi = multi_init();
    *multi_handle = session.m_multi;
  }

  m_sessions.push_back(session);


  CLog::Log(LOGINFO, "%s - Created session to %s://%s\n", __FUNCTION__, protocol, hostname);

  return;

}

void DllLibCurlGlobal::easy_release(CURL_HANDLE** easy_handle, CURLM** multi_handle)
{
  CSingleLock lock(m_critSection);

  CURL_HANDLE* easy = NULL;
  CURLM*       multi = NULL;

  if(easy_handle)
  {
    easy = *easy_handle;
    *easy_handle = NULL;
  }

  if(multi_handle)
  {
    multi = *multi_handle;
    *multi_handle = NULL;
  }

  VEC_CURLSESSIONS::iterator it;
  for(it = m_sessions.begin(); it != m_sessions.end(); it++)
  {
    if( it->m_easy == easy && (multi == NULL || it->m_multi == multi) )
    {
      /* reset session so next caller doesn't reuse options, only connections */
      /* will reset verbose too so it won't print that it closed connections on cleanup*/
      easy_reset(easy);
      it->m_busy = false;
      it->m_idletimestamp = GetTickCount();
      return;
    }
  }
}

CURL_HANDLE* DllLibCurlGlobal::easy_duphandle(CURL_HANDLE* easy_handle)
{
  CSingleLock lock(m_critSection);

  VEC_CURLSESSIONS::iterator it;
  for(it = m_sessions.begin(); it != m_sessions.end(); it++)
  {
    if( it->m_easy == easy_handle )
    {
      SSession session = *it;
      session.m_easy = DllLibCurl::easy_duphandle(easy_handle);
      Load();
      m_sessions.push_back(session);
      return session.m_easy;
    }
  }
  return DllLibCurl::easy_duphandle(easy_handle);
}

void DllLibCurlGlobal::easy_duplicate(CURL_HANDLE* easy, CURLM* multi, CURL_HANDLE** easy_out, CURLM** multi_out)
{
  CSingleLock lock(m_critSection);

  if(easy_out && easy)
    *easy_out = DllLibCurl::easy_duphandle(easy);

  if(multi_out && multi)
    *multi_out = DllLibCurl::multi_init();

  VEC_CURLSESSIONS::iterator it;
  for(it = m_sessions.begin(); it != m_sessions.end(); it++)
  {
    if( it->m_easy == easy )
    {
      SSession session = *it;
      if(easy_out && easy)
        session.m_easy = *easy_out;
      else
        session.m_easy = NULL;

      if(multi_out && multi)
        session.m_multi = *multi_out;
      else
        session.m_multi = NULL;

      Load();
      m_sessions.push_back(session);
      return;
    }
  }
  return;
}
