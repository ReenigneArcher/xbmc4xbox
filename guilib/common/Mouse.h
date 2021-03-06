#ifndef MOUSE_H
#define MOUSE_H

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

#include "../Geometry.h"

#define MOUSE_MINIMUM_MOVEMENT 2
#define MOUSE_DOUBLE_CLICK_LENGTH 500L
#define MOUSE_ACTIVE_LENGTH   5000L

enum MOUSE_STATE { MOUSE_STATE_NORMAL = 1, MOUSE_STATE_FOCUS, MOUSE_STATE_DRAG, MOUSE_STATE_CLICK };
enum MOUSE_BUTTON { MOUSE_LEFT_BUTTON = 0, MOUSE_RIGHT_BUTTON, MOUSE_MIDDLE_BUTTON, MOUSE_EXTRA_BUTTON1, MOUSE_EXTRA_BUTTON2 };

struct MouseState
{
  int x;            // x location
  int y;            // y location
  char dx;          // change in x
  char dy;          // change in y
  char dz;          // change in z (wheel)
  bool button[5];   // true if a button is down
  bool active;      // true if the mouse is active
};

class IMouseDevice
{
public:
  virtual ~IMouseDevice() {}
  virtual void Initialize(void *appData = NULL)=0;
  virtual void Acquire()=0;
  virtual bool Update(MouseState &state)=0;
  virtual void ShowPointer(bool show)=0;
};

class CMouse
{
public:

  CMouse();
  virtual ~CMouse();

  void Initialize(void *appData = NULL);
  void Update();
  void Acquire();
  void SetResolution(int maxX, int maxY, float speedX, float speedY);
  bool IsActive() const;
  bool IsEnabled() const;
  bool HasMoved() const;
  void SetInactive();
  void SetExclusiveAccess(DWORD dwControlID, DWORD dwWindowID, const CPoint &point);
  void EndExclusiveAccess(DWORD dwControlID, DWORD dwWindowID);
  DWORD GetExclusiveWindowID() const { return m_exclusiveWindowID; };
  DWORD GetExclusiveControlID() const { return m_exclusiveControlID; };
  const CPoint &GetExclusiveOffset() const { return m_exclusiveOffset; };
  void SetState(DWORD state) { m_pointerState = state; };
  void SetEnabled(bool enabled) { m_mouseEnabled = enabled; };
  DWORD GetState() const { return m_pointerState; };
  CPoint GetLocation() const;
  void SetLocation(const CPoint &point, bool activate=false);
  CPoint GetLastMove() const;
  char GetWheel() const;

private:
  // exclusive access to mouse from a control
  DWORD m_exclusiveWindowID;
  DWORD m_exclusiveControlID;
  CPoint m_exclusiveOffset;
  
  // state of the mouse
  DWORD m_pointerState;
  MouseState m_mouseState;
  bool m_mouseEnabled;
  bool m_lastDown[5];

  // mouse device
  IMouseDevice *m_mouseDevice;

  // mouse limits and speed
  int m_maxX;
  int m_maxY;
  float m_speedX;
  float m_speedY;

  // active/click timers
  DWORD m_lastActiveTime;
  DWORD m_lastClickTime[5];

public:
  // public access variables to button clicks etc.
  bool bClick[5];
  bool bDoubleClick[5];
  bool bHold[5];
};

extern CMouse g_Mouse;

#endif



