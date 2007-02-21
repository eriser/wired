// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "BeatDialog.h"

BEGIN_EVENT_TABLE(BeatDialog, wxDialog)
  EVT_BUTTON(ID_OK, BeatDialog::OnOkBtnClick)
  EVT_BUTTON(ID_CANCEL, BeatDialog::OnCancelBtnClick)
END_EVENT_TABLE()

BeatDialog::BeatDialog(wxWindow *parent) : 
  wxDialog(parent, -1, _("Set loop's number of beat per bar"), wxDefaultPosition, 
	   wxSize(206, 114))
{
  Centre();

  OkBtn = new wxButton(this, ID_OK, _("OK"), wxPoint(20, 78));
  CancelBtn = new wxButton(this, ID_CANCEL, _("Cancel"), wxPoint(110, 78));
  
  BeatText = new wxStaticText(this, -1, _("Number of beat per bar:"), wxPoint(30, 10));
  BeatCtrl = new wxSpinCtrl(this, -1, wxT("4"), wxPoint(60, 40));
}

BeatDialog::~BeatDialog()
{

}

void BeatDialog::OnOkBtnClick(wxCommandEvent &event)
{
  EndModal(wxID_OK);
}

void BeatDialog::OnCancelBtnClick(wxCommandEvent &event)
{
  EndModal(wxID_CANCEL);
}

