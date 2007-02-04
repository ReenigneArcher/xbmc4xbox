// HTTP.h: interface for the CHTTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTP_H__A368CB6F_3D08_4966_9F9F_961A59CB4EC7__INCLUDED_)
#define AFX_HTTP_H__A368CB6F_3D08_4966_9F9F_961A59CB4EC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../AutoPtrHandle.h"
using namespace AUTOPTR;

class CHTTP
{
public:
  CHTTP();
  CHTTP(const string& strProxyServer, int iProxyPort);
  virtual ~CHTTP();

  bool Post(const string& strURL, const string& strPostData, string& strHTML);
  void SetCookie(const string& strCookie);
  bool Get(string& strURL, string& strHTML);
  bool Head(string& strURL);
  bool Download(const string &strURL, const string &strFileName, LPDWORD pdwSize = NULL);
  bool GetHeader(CStdString strName, CStdString& strValue) const;
  void Cancel();
  void SetUserAgent(string strUserAgent);

  string m_redirectedURL;
  bool IsInternet(bool checkDNS = true);
  static bool BreakURL(const string& strURL, string& strHostName, string &strUsername, string &strPassword, int& iPort, string& Page);
  int Open(const string& strURL, const char* verb, const char* pData);
  void Close();

protected:
  bool Send(char* pBuffer, int iLen);
  bool Connect();
  bool Recv(int iLen);
  bool ReadData(string& strData);
  void ParseHeaders();

private:
  void ParseHeader(string::size_type start, string::size_type colon, string::size_type end);

  CAutoPtrSocket m_socket;
  WSAEVENT hEvent;

  string m_strProxyServer;
  string m_strHostName;
  string m_strCookie;
  string m_strHeaders;
  string m_strUsername;
  string m_strPassword;
  string m_strUserAgent;
  map<CStdString, CStdString> m_mapHeaders;

  bool m_bProxyEnabled;
  int m_iProxyPort;
  int m_iPort;

  char* m_RecvBuffer;
  int m_RecvBytes;
};

#endif // !defined(AFX_HTTP_H__A368CB6F_3D08_4966_9F9F_961A59CB4EC7__INCLUDED_)

//------------------------------------------------------------------------------------------------------------------
//Thread Detection if we are online or not! Very Simple and Dirty!
#include "HTTP.h"
#include "../util.h"
class CHTTPD : public CThread
  {
  public:
      CHTTPD();
      ~CHTTPD();
      bool bIsConnected;
      
      virtual bool Start();
      virtual void Stop();
      virtual bool IsRunning();
      virtual void OnExit();
      virtual void OnStartup();
      virtual void Process();
  };
extern CHTTPD g_httpd;
