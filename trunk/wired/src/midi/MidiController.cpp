#include "MidiController.h"

MidiController *Controller = 0x0;

BEGIN_EVENT_TABLE(MidiController, wxDialog)
  EVT_BUTTON(ID_OK, MidiController::OnOkBtnClick)
  EVT_BUTTON(ID_CANCEL, MidiController::OnCancelBtnClick)
END_EVENT_TABLE()

#define MIDIWIDTH 206

MidiController::MidiController(wxWindow *parent) : 
  wxDialog(parent, -1, "Assign Midi Controller", wxDefaultPosition, wxSize(206, MIDIWIDTH))
{
  Centre();
  sb = new wxStaticBox(this, -1, "Assign to controller number :", wxPoint(6, 4), wxSize(192, 160));
  OkBtn = new wxButton(this, ID_OK, "OK", wxPoint(20, 170));
  CancelBtn = new wxButton(this, ID_CANCEL, "Cancel", wxPoint(110, 170));
  
  ChannelText = new wxStaticText(this, -1, "Channel:", wxPoint(52, 26));
  ChannelCtrl = new wxSpinCtrl(this, -1, "1", wxPoint(52, 44));

  ControllerText = new wxStaticText(this, -1, "Control:", wxPoint(52, 70));
  ControllerCtrl = new wxSpinCtrl(this, -1, "0", wxPoint(52, 88));
  
  ValueText = new wxStaticText(this, -1, "Value:", wxPoint(52, 114));
  ValueCtrl = new wxSpinCtrl(this, -1, "0", wxPoint(52, 130));

  Channel = 0;
}

MidiController::~MidiController()
{
  delete sb;
  delete OkBtn;
  delete CancelBtn;
  delete ChannelCtrl;
  delete ControllerCtrl;
  delete ValueCtrl;
  delete ChannelText;
  delete ControllerText;
  delete ValueText;
}

void MidiController::ProcessMidi(int midi_msg[3])
{
  Note = false;
  if ((STATUS(midi_msg[0]) == M_NOTEON1) ||  (STATUS(midi_msg[0]) == M_NOTEON2))
    {
      Note = true;
      ChannelCtrl->SetValue(CHANNEL(midi_msg[0]));
    }
  else if (STATUS(midi_msg[0]) != M_CONTROL)
    ChannelCtrl->SetValue(CHANNEL(midi_msg[0]));
  ChannelCtrl->SetValue(CHANNEL(midi_msg[0]));
  ControllerCtrl->SetValue(midi_msg[1]);   
  ValueCtrl->SetValue(midi_msg[2]);
  Channel = CHANNEL(midi_msg[0]);
  Controller = midi_msg[1];
  Value = midi_msg[2];
}

void MidiController::OnOkBtnClick(wxCommandEvent &event)
{
  EndModal(1);
}

void MidiController::OnCancelBtnClick(wxCommandEvent &event)
{
  Channel = -1;
  EndModal(0);
}

