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

#ifndef __STATICBITMAP_H__
#define __STATICBITMAP_H__

#include <wx/wx.h>

class StaticBitmap : public wxStaticBitmap
{
 public:
  StaticBitmap(wxWindow* parent, wxWindowID id, const wxBitmap& label, const wxPoint& pos, 
	       const wxSize& size = wxDefaultSize);
  ~StaticBitmap();

  virtual void OnMouseEvent(wxMouseEvent &event);

 protected:
  DECLARE_EVENT_TABLE()
};

#endif
