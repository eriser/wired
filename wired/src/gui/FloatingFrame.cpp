// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "FloatingFrame.h"

FloatingFrame::FloatingFrame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos,
			const wxSize &size, wxWindow *frame_child, wxWindow *frame_old_parent, wxMenuItem *menu)
  : wxFrame(parent, id, title, pos, size)
{
  Child = frame_child;
  OldParent = frame_old_parent;
  MenuItem = menu;
}

// FloatingFrame::FloatingFrame() : wxFrame()
// {
//   ;
// }

FloatingFrame::~FloatingFrame()
{
  ;
}

void		FloatingFrame::OnClose(wxCloseEvent &event)
{
  Child->Reparent(OldParent);
  delete this;
}

BEGIN_EVENT_TABLE(FloatingFrame, wxFrame)
  EVT_CLOSE(FloatingFrame::OnClose)
END_EVENT_TABLE()
