/*
  This file has been modified for use in XBFileZilla.

  Taken from FileZilla Server 0.8.3 release

  changes:
  - made ShowStatus() virtual in order to allow subclassing for local notification interface
  - made OnServerMessage() virtual in order to allow subclassing for local notification interface
  

*/
// Server.h: Schnittstelle f�r die Klasse CServer.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__4896D8C6_EDB5_438E_98E6_08957DBCD1BC__INCLUDED_)
#define AFX_SERVER_H__4896D8C6_EDB5_438E_98E6_08957DBCD1BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CListenSocket;
class CServerThread;
class COptions;
class CAdminListenSocket;
class CAdminInterface;
class CAdminSocket;
class CFileLogger;

class CServer  
{
public:
	virtual void ShowStatus(LPCTSTR msg, int nType);
	virtual void ShowStatus(_int64 eventDate, LPCTSTR msg, int nType);
	BOOL ProcessCommand(CAdminSocket *pAdminSocket, int nID, unsigned char *pData, int nDataLength);
	virtual void OnClose();
	bool Create();
	CServer();
	virtual ~CServer();
	HWND GetHwnd();
protected:
	BOOL CreateAdminListenSocket();
	void OnTimer(UINT nIDEvent);
	BOOL m_bQuit;
	BOOL ToggleActive(int nServerState);
	int m_nServerState;
	CListenSocket *m_pListenSocket;
	CAdminInterface *m_pAdminInterface;
	CFileLogger *m_pFileLogger;
	
	std::list<CServerThread*> m_ThreadArray;
	COptions *m_pOptions;
	std::list<CAdminListenSocket*> m_AdminListenSocketList;

	std::map<int, t_connectiondata> m_UsersList;

	UINT m_nTimerID;
	
	_int64 m_nRecvCount;
	_int64 m_nSendCount;

	virtual LRESULT OnServerMessage(WPARAM wParam, LPARAM lParam);
private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND m_hWnd;
};

#endif // !defined(AFX_SERVER_H__4896D8C6_EDB5_438E_98E6_08957DBCD1BC__INCLUDED_)
