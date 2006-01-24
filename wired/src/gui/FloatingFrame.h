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

class			FloatingFrame : public wxFrame
{
 public:
  FloatingFrame();
  FloatingFrame(wxWindow *, wxWindowID, const wxString &, const wxPoint &,
		const wxSize &size, wxWindow *, wxWindow *, wxMenuItem *);
  ~FloatingFrame();
  
  void			OnClose(wxCloseEvent &event);
  
 private:
  wxWindow		*Child;
  wxWindow		*OldParent;
  wxMenuItem		*MenuItem;

 protected:
  DECLARE_EVENT_TABLE();
};

#endif
