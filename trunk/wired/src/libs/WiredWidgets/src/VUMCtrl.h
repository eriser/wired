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

#ifndef __VUMCTRL_H__
#define __VUMCTRL_H__

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <iostream>

using namespace std;

class VUMCtrl : public wxWindow
{
 public:
  VUMCtrl(wxWindow *parent, wxWindowID id, int max_value, wxImage *green, wxImage *orange, wxImage *red, 
	  const wxPoint &pos, const wxSize &size = wxDefaultSize, long style = wxSUNKEN_BORDER);

  ~VUMCtrl();
  void SetValue(int val);
  int GetValue();
 protected:
  int Max_Value;
  int height;
  int width;
  float coeff;
  int zero_height;
  int green_height;
  int yellow_height;
  int orange_height;
  int red_height;
  wxCoord w, h;
  int value;
  int tmp;
  wxImage *img_g;
  wxImage *img_o;
  wxImage *img_r;
  virtual void OnPaint(wxPaintEvent &event);
  //  void SetValue(int val);
  
  
  DECLARE_EVENT_TABLE()
};



#endif
