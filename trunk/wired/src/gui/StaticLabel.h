// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __STATICLABEL_H__
#define __STATICLABEL_H__

#include <wx/wx.h>

class StaticLabel : public wxStaticText
{
 public:
  StaticLabel(wxWindow *parent, wxWindowID id, const wxString &text, 
	      const wxPoint &pos, const wxSize &size = wxDefaultSize);
  
  ~StaticLabel();

  virtual void OnMouseEvent(wxMouseEvent &event);

 protected:
  DECLARE_EVENT_TABLE()
};

#endif
