// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __HELPPANEL_H__
#define __HELPPANEL_H__

#include <wx/wx.h>

class				HelpCtrl;

class				HelpPanel : public wxPanel
{
 public:
  HelpPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size);
  ~HelpPanel();
  void				SetText(wxString &s);

   HelpCtrl			*Help;
 protected:
  void				OnSize(wxSizeEvent &event);

  DECLARE_EVENT_TABLE()
};

extern HelpPanel		*HelpWin;

#endif
