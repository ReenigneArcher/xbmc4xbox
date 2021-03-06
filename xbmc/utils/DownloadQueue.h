#pragma once

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

#include "Thread.h"
#include <queue>

struct TICKET
{
  TICKET(WORD aQueueId, DWORD aItemId)
  {
    wQueueId = aQueueId;
    dwItemId = aItemId;
  };

  WORD wQueueId;
  DWORD dwItemId;
};

class IDownloadQueueObserver
{
public:
  enum Result {Succeeded, Failed};
  virtual void OnContentComplete(TICKET aTicket, CStdString& aContentString, Result aResult){};
  virtual void OnFileComplete(TICKET aTicket, CStdString& aFilePath, INT aByteRxCount, Result aResult){};
};


class CDownloadQueue: CThread
{
public:
  CDownloadQueue();
  virtual ~CDownloadQueue(void);

  TICKET RequestContent(CStdString& aUrl, IDownloadQueueObserver* aObserver);
  TICKET RequestFile(CStdString& aUrl, IDownloadQueueObserver* aObserver);
  TICKET RequestFile(CStdString& aUrl, CStdString& aFilePath, IDownloadQueueObserver* aObserver);
  void CancelRequests(IDownloadQueueObserver* aObserver);

  VOID Flush();
  INT Size();

protected:

  void OnStartup();
  void Process();

  struct Command
  {
    TICKET ticket;
    CStdString location;
    CStdString content;
    IDownloadQueueObserver* observer;
  };

  typedef std::queue<Command> COMMANDQUEUE;
  COMMANDQUEUE m_queue;
  CRITICAL_SECTION m_critical;

  WORD m_wQueueId;
  DWORD m_dwNextItemId;

  static WORD m_wNextQueueId;
};
