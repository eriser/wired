// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MidiController.h"

MidiController *Controller = 0x0;

BEGIN_EVENT_TABLE(MidiController, wxDialog)
  EVT_BUTTON(ID_OK, MidiController::OnOkBtnClick)
  EVT_BUTTON(ID_CANCEL, MidiController::OnCancelBtnClick)
END_EVENT_TABLE()

#define MIDIWIDTH 206

MidiController::MidiController(wxWindow *parent) : 
  wxDialog(parent, -1, _("Assign Midi Controller"), wxDefaultPosition, wxSize(206, MIDIWIDTH))
{
  Centre();
  sb = new wxStaticBox(this, -1, _("Assign to controller number :"), wxPoint(6, 4), wxSize(192, 160));
  OkBtn = new wxButton(this, ID_OK, _("OK"), wxPoint(20, 170));
  CancelBtn = new wxButton(this, ID_CANCEL, _("Cancel"), wxPoint(110, 170));
  
  ChannelText = new wxStaticText(this, -1, _("Channel:"), wxPoint(52, 26));
  ChannelCtrl = new wxSpinCtrl(this, -1, wxT("1"), wxPoint(52, 44));

  ControllerText = new wxStaticText(this, -1, _("Control:"), wxPoint(52, 70));
  ControllerCtrl = new wxSpinCtrl(this, -1, wxT("0"), wxPoint(52, 88));
  
  ValueText = new wxStaticText(this, -1, _("Value:"), wxPoint(52, 114));
  ValueCtrl = new wxSpinCtrl(this, -1, wxT("0"), wxPoint(52, 130));

  Type = 0;
}

MidiController::~MidiController()
{
	if(sb)
	  delete sb;
	if (OkBtn)
	  delete OkBtn;
	if (CancelBtn)
     delete CancelBtn;
	if (ChannelCtrl)
	 delete ChannelCtrl;
	if(ControllerCtrl)
	 delete ControllerCtrl;
	if (ValueCtrl)
	 delete ValueCtrl;
	if (ChannelText)
	 delete ChannelText;
	if (ControllerText)
	 delete ControllerText;
	if (ValueText)
	 delete ValueText;
}

void MidiController::ProcessMidi(int midi_msg[3])
{
  Note = false;
  if ((STATUS(midi_msg[0]) == M_NOTEON1) ||  (STATUS(midi_msg[0]) == M_NOTEON2))    
    Note = true;
  ChannelCtrl->SetValue(CHANNEL(midi_msg[0]) + 1);
  ControllerCtrl->SetValue(midi_msg[1]);   
  ValueCtrl->SetValue(midi_msg[2]);
  Type = midi_msg[0];
  Controller = midi_msg[1];
  Value = midi_msg[2];
}

void MidiController::OnOkBtnClick(wxCommandEvent &event)
{
  EndModal(1);
}

void MidiController::OnCancelBtnClick(wxCommandEvent &event)
{
  Type = -1;
  EndModal(0);
}

