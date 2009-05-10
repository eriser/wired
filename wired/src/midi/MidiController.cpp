// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MidiController.h"

MidiController *Controller = 0x0;

BEGIN_EVENT_TABLE(MidiController, wxDialog)
  EVT_BUTTON(wxID_OK, MidiController::OnOkBtnClick)
  EVT_BUTTON(wxID_CANCEL, MidiController::OnCancelBtnClick)
END_EVENT_TABLE()

#define MIDIWIDTH 206

MidiController::MidiController(wxWindow *parent) :
  wxDialog(parent, -1, _("Assign Midi Controller"))
{
  Centre();

  // sizer default flag:
  wxSizerFlags	flagsExpand;
  flagsExpand.Expand().Border(wxALL, 3);

  // create a grid with 2 columns and 5 pixels of space between cells
  wxGridSizer* valuesSizer = new wxGridSizer(2, 3, 3);

  ChannelCtrl = new wxSpinCtrl(this, -1, wxT("1"));
  ControllerCtrl = new wxSpinCtrl(this, -1, wxT("0"));
  ValueCtrl = new wxSpinCtrl(this, -1, wxT("0"));

  valuesSizer->Add(new wxStaticText(this, -1, _("Channel:")), flagsExpand);
  valuesSizer->Add(ChannelCtrl, flagsExpand);

  valuesSizer->Add(new wxStaticText(this, -1, _("Control:")), flagsExpand);
  valuesSizer->Add(ControllerCtrl, flagsExpand);

  valuesSizer->Add(new wxStaticText(this, -1, _("Value:")), flagsExpand);
  valuesSizer->Add(ValueCtrl, flagsExpand);

  // create the main static box
  wxStaticBoxSizer* sbSizer = new wxStaticBoxSizer(new wxStaticBox(this,
								   -1,
								   _("Assign to controller number :")),
						   wxVERTICAL);
  sbSizer->Add(valuesSizer, flagsExpand);

  // create the button
  wxStdDialogButtonSizer* buttonSizer = this->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
  buttonSizer->Realize(); // This cmd is warned by wxWidgets debug mode. I think it's a wx bug!

  // .. and attach static, and buttons to the main sizer
  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  mainSizer->Add(sbSizer, flagsExpand);
  mainSizer->Add(buttonSizer, flagsExpand);

  // attach the sizer to the dialog
  SetSizer( mainSizer );
  mainSizer->SetSizeHints( this );

  // default type of midi event
  Type = 0;
}

MidiController::~MidiController()
{
  // we don't need to delete anything!
  // main sizer do his job
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
  Note = false;
  Type = M_CONTROL;
  Controller = ControllerCtrl->GetValue();
  EndModal(1);
}

void MidiController::OnCancelBtnClick(wxCommandEvent &event)
{
  Type = -1;
  EndModal(0);
}

