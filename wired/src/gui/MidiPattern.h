// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDIPATTERN_H__
#define __MIDIPATTERN_H__

#include <vector>
#include <list>
#include "Pattern.h"
#include "WiredDocument.h"

class				MidiTrack;
class				MidiEvent;
class				MidiFileEvent;

class				MidiPattern : public Pattern
{
 private:
  WiredDocument*		_documentParent;

  // only set when we add a pattern from MidiTrack class
  wxString			_filename;
  unsigned short		_noTrack;

 public:
  MidiPattern(WiredDocument *parent, double pos, double endpos, long trackindex);
  MidiPattern(WiredDocument *parent, double pos, MidiTrack *t, long trackindex);
  ~MidiPattern();

 private:
  void				SetMidiTrack(MidiTrack* midiTrack);

 public:
  MidiTrack*			GetMidiTrack();
  void				AddEvent(MidiEvent *event);
  void				AddEvent(MidiFileEvent *event);
  void				Update();
  void				SetSelected(bool sel);
  unsigned short		GetPPQN() { return (ppqn); }
  void				SetPPQN(unsigned short p) { ppqn = p; }
  void				SetDrawColour(wxColour c);
  Pattern			*CreateCopy(double pos);
  void				DrawMidi();
  void				Split(double pos);

  // WiredDocument implementation
  void				Save();
  void				Load(SaveElementArray data);

  std::vector<MidiEvent *>	Events;
  std::list<MidiEvent *>	temp;
  std::list<MidiEvent *>	RecordingEvents;  // Events being record (waits for NOTE OFF)

 protected:
  void				Init(WiredDocument* parent);
  void				OnClick(wxMouseEvent &e);
  void				OnLeftUp(wxMouseEvent &e);
  void				OnDoubleClick(wxMouseEvent &e);
  void				OnRightClick(wxMouseEvent &e);
  void				OnMotion(wxMouseEvent &e);
  void				OnDeleteClick(wxCommandEvent &e);
  void				OnMoveToCursorClick(wxCommandEvent &e);
  void				OnPaint(wxPaintEvent &e);
  void				OnHelp(wxMouseEvent &event);

  std::vector<MidiEvent *>	*GetEvents() { return (&Events); };
  wxBitmap			*Bmp;
  wxMemoryDC			memDC;
  unsigned short		ppqn;

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
