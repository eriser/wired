#ifndef __MIDICONTROLLER_H__
#define __MIDICONTROLLER_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include "midi.h"

#define ID_OK     100
#define ID_CANCEL 101

class MidiController : public wxDialog
{
 public:
  MidiController(wxWindow *parent);
  ~MidiController();

  void OnOkBtnClick(wxCommandEvent &event);
  void OnCancelBtnClick(wxCommandEvent &event);
  
  void ProcessMidi(int midi_msg[3]);
 
  int Channel; 
  int Controller;
  int Value;
  
  bool Note;
  
 private:
  wxStaticBox *sb;
  wxButton    *OkBtn;
  wxButton    *CancelBtn;  
  
  wxSpinCtrl  *ChannelCtrl;
  wxSpinCtrl  *ControllerCtrl;
  wxSpinCtrl  *ValueCtrl;
  wxStaticText *ChannelText;
  wxStaticText *ControllerText;
  wxStaticText *ValueText;
 
  DECLARE_EVENT_TABLE()
};

extern MidiController *Controller;

#endif
