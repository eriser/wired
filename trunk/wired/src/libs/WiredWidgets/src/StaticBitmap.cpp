// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#include "StaticBitmap.h"

BEGIN_EVENT_TABLE(StaticBitmap, wxStaticText)
  EVT_MOUSE_EVENTS(StaticBitmap::OnMouseEvent) 
END_EVENT_TABLE()

StaticBitmap::StaticBitmap(wxWindow* parent, wxWindowID id, const wxBitmap& label, 
			   const wxPoint& pos, const wxSize& size)
  : wxStaticBitmap(parent, id, label, pos, size)
{
  
}
  
StaticBitmap::~StaticBitmap()
{

}

void StaticBitmap::OnMouseEvent(wxMouseEvent &event)
{
  //GetParent()->ProcessEvent(event);
}
