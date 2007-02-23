// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include "StaticLabel.h"

BEGIN_EVENT_TABLE(StaticLabel, wxStaticText)
  EVT_LEFT_DOWN(StaticLabel::OnMouseEvent) 
END_EVENT_TABLE()

StaticLabel::StaticLabel(wxWindow *parent, wxWindowID id, const wxString &text,
			 const wxPoint &pos, const wxSize &size)
  : wxStaticText(parent, id, text, pos, size)
{
  
}
  
StaticLabel::~StaticLabel()
{

}

void StaticLabel::OnMouseEvent(wxMouseEvent &event)
{
  wxCommandEvent _event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  wxPostEvent(GetParent(), _event);  
}
