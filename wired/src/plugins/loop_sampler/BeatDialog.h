// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __BEATDIALOG_H__
#define __BEATDIALOG_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

#define ID_OK     100
#define ID_CANCEL 101

class BeatDialog : public wxDialog
{
 public:
  BeatDialog(wxWindow *parent);
  ~BeatDialog();

  void OnOkBtnClick(wxCommandEvent &event);
  void OnCancelBtnClick(wxCommandEvent &event);

  wxSpinCtrl  *BeatCtrl;
  
 private:
  wxButton    *OkBtn;
  wxButton    *CancelBtn;  
  
  wxStaticText *BeatText;
 
  DECLARE_EVENT_TABLE()
};

#endif
