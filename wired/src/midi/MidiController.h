// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDICONTROLLER_H__
#define __MIDICONTROLLER_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include "midi.h"

class MidiController : public wxDialog
{
 public:
  MidiController(wxWindow *parent);
  ~MidiController();

  void OnOkBtnClick(wxCommandEvent &event);
  void OnCancelBtnClick(wxCommandEvent &event);

  void ProcessMidi(int midi_msg[3]);

  int Type;
  int Controller;
  int Value;

  bool Note;

 private:
  wxSpinCtrl  *ChannelCtrl;
  wxSpinCtrl  *ControllerCtrl;
  wxSpinCtrl  *ValueCtrl;

  DECLARE_EVENT_TABLE()
};

extern MidiController *Controller;

#endif
