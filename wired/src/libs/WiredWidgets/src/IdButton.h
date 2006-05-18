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
#ifndef __IDBUTTON_H__
#define __IDBUTTON_H__

#include <iostream>
#include <wx/wx.h>

#define UNCLICKED	0
#define CLICKED		1


class IdButton : public wxWindow
{
 public:
  IdButton(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
	   const wxSize& size, wxImage* up, wxImage* down,
	   unsigned int num_id);
  ~IdButton();
  
  void OnPaint(wxPaintEvent &event);
  void OnMouseEvent(wxMouseEvent &event);
  void OnEnter(wxMouseEvent &event);
  
  void SetOn(void);
  void SetOff(void);
  unsigned int	Num;
  
 protected:
  unsigned int	State;
  wxBitmap**	Bitmaps;

DECLARE_EVENT_TABLE()

};

#endif//__IDBUTTON_H__
