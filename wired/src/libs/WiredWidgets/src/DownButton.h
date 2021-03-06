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

#ifndef __DOWNBUTTON_H__
#define __DOWNBUTTON_H__

#include <iostream>

#include <wx/wx.h>

#define B_UP 0
#define B_DOWN 1

class DownButton : public wxWindow
{
 public:
  DownButton(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	     const wxSize &size, wxImage *up, wxImage *down, bool simple = false);

  virtual ~DownButton();

  virtual void SetOn();
  virtual void SetOff();
  virtual bool GetOn();
  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnMouseEvent(wxMouseEvent &event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeave(wxMouseEvent &event);
  virtual void OnEnterWindow(wxMouseEvent &event);

 protected:
  wxBitmap *Up;
  wxBitmap *Down;
  int	   state;
  bool	   isdown;
  bool	   Simple;

  wxBitmap *GetCurrentBitmap();

DECLARE_EVENT_TABLE()
  
};

#endif

