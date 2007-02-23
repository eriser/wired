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
#ifndef __STATICPOSKNOB_H__
#define __STATICPOSKNOB_H__

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "wx/dragimag.h"
#include <iostream>

using namespace std;

class StaticPosKnob : public wxWindow
{
 public:
  StaticPosKnob(wxWindow *parent, wxWindowID id, 
		int step, wxImage **img_bg, int mouse_step,
		int begin_value, int end_value, int init_val,
		const wxPoint &pos, const wxSize &size);
  ~StaticPosKnob();
  
  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnMotionEvent(wxMouseEvent &event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeftDown(wxMouseEvent &event);
  virtual void OnKeyDown(wxKeyEvent& event);
  void OnEnter(wxMouseEvent& event);
  
  int GetValue();
  void SetValue(int val);

 protected:
  bool		Clicked;
  int		Steps;
  int		MouseStep;
  int		BeginVal;
  int		EndVal;
  int		CurVal;
  
  int		ClickPos;
  int		Count;
  wxBitmap**	Bmps;


DECLARE_EVENT_TABLE()
};
#endif//__STATICPOSKNOB_H__
