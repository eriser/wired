// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

/*
** Copyright (C) 2004-2006 by Wired Team
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version 2.1
** as published by the Free Software Foundation.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __HOLDBUTTON_H__
#define __HOLDBUTTON_H__

#include <iostream>

#include <wx/wx.h>
#include <wx/timer.h>

#define TIME_INTERVAL1	40
#define TIME_INTERVAL2	3
#define TIME_CHANGE	750

class HoldButton : public wxWindow
{
 public:
  HoldButton(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	     const wxSize &size, wxImage *up, wxImage *down);

  virtual ~HoldButton();

  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnLeftDown(wxMouseEvent &event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeaveWindow(wxMouseEvent &event);
  virtual void OnTimer(wxTimerEvent &event);


 protected:
  wxBitmap *Up;
  wxBitmap *Down;
  bool	    isdown;
  wxTimer   *Timer;
  int	    CountTime;
  bool	    Interval2;

  DECLARE_EVENT_TABLE()
  
};

#endif

