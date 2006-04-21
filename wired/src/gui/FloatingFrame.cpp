// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "FloatingFrame.h"

FloatingFrame::FloatingFrame(wxWindow *parent, wxWindowID id, const wxString &title,
			     const wxPoint &pos, const wxSize &size,
			     wxWindow *frame_old_parent,
			     wxMenuItem *menu, int event_no)
  : wxFrame(parent, id, title, pos, size)
{
  OldParent = frame_old_parent;
  MenuItem = menu;
  EventMenu = event_no;
}

FloatingFrame::~FloatingFrame()
{
}

void		FloatingFrame::OnClose(wxCloseEvent &event)
{
  wxCommandEvent	newevent(wxEVT_COMMAND_MENU_SELECTED, EventMenu);

  MenuItem->Check(false);
  OldParent->GetEventHandler()->ProcessEvent(newevent);
}

BEGIN_EVENT_TABLE(FloatingFrame, wxFrame)
  EVT_CLOSE(FloatingFrame::OnClose)
END_EVENT_TABLE()
