// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MIDIPATTERN_H__
#define __MIDIPATTERN_H__

using namespace std;

#include <list>
#include "Pattern.h"
#include "midi.h"

class MidiPattern: public Pattern
{
 public:
  MidiPattern(double pos, double endpos, long trackindex);
  MidiPattern(double pos, MidiTrack *t, long trackindex);
  ~MidiPattern();
  
  void			AddEvent(MidiEvent *event);
  void			AddEvent(MidiFileEvent *event);
  void			Update();
  void			SetSelected(bool sel);
  unsigned short	GetPPQN() { return ppqn; }
  void			SetPPQN(unsigned short p) { ppqn = p; }

  Pattern		*CreateCopy(double pos);
  void			DrawMidi();
  
  vector<MidiEvent *>	Events;
  list<MidiEvent *>	temp;
  list<MidiEvent *>	RecordingEvents;  // Evenements en train d'etre record (attente d'un NOTE OFF)

 protected:
  void			Init();
  void			OnClick(wxMouseEvent &e);
  void			OnDoubleClick(wxMouseEvent &e);
  void			OnRightClick(wxMouseEvent &e);
  void			OnMotion(wxMouseEvent &e);
  void			OnDeleteClick(wxCommandEvent &e);
  void			OnMoveToCursorClick(wxCommandEvent &e);
  void			OnPaint(wxPaintEvent &e);
  
  wxColour		BorderColour;
  wxBitmap		*Bmp;
  wxMemoryDC		memDC;

  unsigned short	ppqn;
  //  long			OldMousePosX;
  DECLARE_EVENT_TABLE()
};

/*
enum
  {
    Midi_MoveToCursor = 2222,
    Midi_Cut,
    Midi_Copy,
    Midi_Paste,
    Midi_Delete
  };
*/
#endif
