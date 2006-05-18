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
#ifndef __HINTEDFADER_H__
#define __HINTEDFADER_H__

#include <wx/wx.h>
#include "Hint.h"
#include "FaderCtrl.h"
#include <wx/tooltip.h>
#include <wx/tipwin.h>

class HintedFader : public FaderCtrl
{
 public:
  HintedFader(wxWindow *parent, wxWindowID id, wxWindow* hintparent,
	      wxImage *img_bg, wxImage  *img_fg,
	      long begin_value, long end_value, long val,
	      const wxPoint &pos, const wxSize &size,
	      const wxPoint &hintpos);
  virtual ~HintedFader();
  
  virtual void OnMouseEvent(wxMouseEvent &event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeftDown(wxMouseEvent& event);
  virtual void OnKeyDown(wxKeyEvent& event);
  virtual void OnKeyUp(wxKeyEvent& event);
  virtual void OnEnterWindow(wxMouseEvent &event);
  virtual void OnLeave(wxMouseEvent &event);

 protected:
  Hint* Label;
  wxWindow  *HintParent;
DECLARE_EVENT_TABLE()
};

#endif//__HINTEDFADER_H__
