// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __LEDBUTTON_H__
#define __LEDBUTTON_H__

#include <wx/wx.h>

class LedButton: public wxWindow
{
 public:
  LedButton(wxWindow *parent, long time_int, wxPoint &p, wxSize &s);
  ~LedButton();

  void SetOn();

 protected:
  wxTimer *time;
};

#endif
