// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __FLOATINGFRAME_H
#define __FLOATINGFRAME_H

#include <wx/wxprec.h>
#include <wx/frame.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

using namespace		std;

// A FloatingFrame is linked with a menuitem

class			FloatingFrame : public wxFrame
{
 public:
  FloatingFrame();
  FloatingFrame(wxWindow *parent, wxWindowID, const wxString &title,
		const wxPoint &pos, const wxSize &size,
		wxWindow *oldparent, wxMenuItem *menuitem,
		int EventMenu);
  ~FloatingFrame();
  
  void			OnClose(wxCloseEvent &event);
  
 private:
  wxWindow		*OldParent;
  wxMenuItem		*MenuItem;
  int			EventMenu;

 protected:
  DECLARE_EVENT_TABLE();
};

#endif
