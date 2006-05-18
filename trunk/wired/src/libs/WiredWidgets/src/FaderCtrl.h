// Copyright (C) 2004-2006 by Wired Team
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
#include <wx/thread.h>
#include "Hint.h"

using namespace std;

class		FaderCtrl : public wxWindow
{
 public:
  FaderCtrl(wxWindow *parent, wxWindowID id, 
	    wxImage *img_bg, wxImage *img_fg, 
	    float begin_value, float end_value, float *val, bool is_int, 
	    const wxPoint &pos, const wxSize &size = wxDefaultSize);
  FaderCtrl(wxWindow *parent, wxWindowID id,
	    wxImage *img_bg, wxImage  *img_fg,
	    float begin_value, float end_value, float *val, bool is_int,
	    const wxPoint &pos, const wxSize &size,
	    wxWindow* hintparent, const wxPoint &hintpos);
  ~FaderCtrl();
  virtual void		OnPaint(wxPaintEvent &event);
  virtual void		OnMouseEvent(wxMouseEvent &event);
  virtual void		OnKeyUp(wxKeyEvent& event);
  virtual void		OnKeyDown(wxKeyEvent& event);
  virtual void		OnLeftUp(wxMouseEvent &event);
  virtual void		OnLeftDown(wxMouseEvent& event);
  virtual void		OnEnterWindow(wxMouseEvent &event);
  virtual void		OnLeaveWindow(wxMouseEvent &event);

  float			GetValue();
  void			SetValue(float *val);
  void			SetValue(float val);
/*   void			SetValue(int val); */

 protected:
  Hint*			Label;
  float			*Value;
  float			BeginValue;
  float			EndValue;
  float			Coeff;
  float			Ord;
  bool			IsInteger;
  wxBitmap		*bg;
  wxBitmap		*tmp_fg;
  wxStaticBitmap	*fg;
  
DECLARE_EVENT_TABLE()
};

#endif

//extern wxMutex		*FaderCtrlMutex;
