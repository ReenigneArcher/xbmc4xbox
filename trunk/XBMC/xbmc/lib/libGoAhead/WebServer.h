/* WebServer.h: interface for the CWebServer class.
 * A darivation of:  main.c -- Main program for the GoAhead WebServer
 *
 * main.c -- Main program for the GoAhead WebServer
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

#pragma comment(linker, "/merge:WEB_TEXT=LIBHTTP")
#pragma comment(linker, "/merge:WEB_DATA=LIBHTTP")
#pragma comment(linker, "/merge:WEB_BSS=LIBHTTP")
#pragma comment(linker, "/merge:WEB_RD=LIBHTTP")
#pragma comment(linker, "/section:LIBHTTP,RWE")

#pragma once

#include "includes.h"

#ifdef __cplusplus
extern "C" {
#endif

int		aspTest(int eid, webs_t wp, int argc, char_t **argv);
void	formTest(webs_t wp, char_t *path, char_t *query);
int		XbmcWebsAspCommand(int eid, webs_t wp, int argc, char_t **argv);
void	XbmcWebsForm(webs_t wp, char_t *path, char_t *query);

// wrapers for XBMCConfiguration
int XbmcWebsAspConfigBookmarkSize(int eid, webs_t wp, int argc, char_t **argv);
int XbmcWebsAspConfigGetBookmark( int eid, webs_t wp, int argc, char_t **argv);
int XbmcWebsAspConfigAddBookmark( int eid, webs_t wp, int argc, char_t **argv);
int XbmcWebsAspConfigSaveBookmark( int eid, webs_t wp, int argc, char_t **argv);
int XbmcWebsAspConfigRemoveBookmark( int eid, webs_t wp, int argc, char_t **argv);
int XbmcWebsAspConfigSaveConfiguration( int eid, webs_t wp, int argc, char_t **argv);
int XbmcWebsAspConfigGetOption( int eid, webs_t wp, int argc, char_t **argv);
int XbmcWebsAspConfigSetOption( int eid, webs_t wp, int argc, char_t **argv);

#if defined(__cplusplus)
}
#endif 

#include <xtl.h>
#include "..\..\utils\Thread.h"

class CWebServer : public CThread
{
public:

	CWebServer();
	virtual ~CWebServer();
	virtual void		OnStartup();
	virtual void		OnExit();
	virtual void		Process();
	bool						Start(const char* szLocalAddress, int port = 80, const char* web = "Q:\\web");
	void						Stop();

	DWORD						SuspendThread();
	DWORD						ResumeThread();

	const char_t*		GetPassword();
	void						SetPassword(char_t* Password);
	int							initWebs();

private:
	
	const char_t*		m_szLocalAddress;		/* local ip address */
	char						m_szRootWeb[1024];	/* local directory */
	const char_t*		m_password;					/* Security password */
	int							m_port;							/* Server port */
	int							m_sockServiceTime;	/* in milliseconds */
	bool						m_bFinished;				/* Finished flag */
	HANDLE					m_hEvent;
};
