/*
** Copyright (C) 2004 by Wired Team
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
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

#ifndef __FADERCTRL_H__
#define __FADERCTRL_H__

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
//#include <wx/dragimg.h>
#include "wx/dragimag.h"
#include <iostream>
#include "StaticBitmap.h"
#include <wx/cshelp.h>
#include "Hint.h"

using namespace std;

class FaderCtrl : public wxWindow
{
 public:
  FaderCtrl(wxWindow *parent, wxWindowID id,  wxImage *img_bg, wxImage *img_fg, long begin_value, 
	    long end_value, long val,
	    const wxPoint &pos, const wxSize &size = wxDefaultSize);
  FaderCtrl(wxWindow *parent, wxWindowID id,
	    wxImage *img_bg, wxImage  *img_fg,
	    long begin_value, long end_value, long val,
	    const wxPoint &pos, const wxSize &size,
	    wxWindow* hintparent, const wxPoint &hintpos);
  ~FaderCtrl();
  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnMouseEvent(wxMouseEvent &event);
  virtual void OnKeyUp(wxKeyEvent& event);
  virtual void OnKeyDown(wxKeyEvent& event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeftDown(wxMouseEvent& event);
  virtual void OnEnterWindow(wxMouseEvent &event);
  virtual void OnLeaveWindow(wxMouseEvent &event);

  int GetValue();
  void SetValue(int val);

 protected:
  Hint* Label;
  long Value;
  long BeginValue;
  long EndValue;
  double coeff;
  wxBitmap *bg;
  wxBitmap *tmp_fg;
  wxStaticBitmap *fg;
  
DECLARE_EVENT_TABLE()
};

#endif
