// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDIPART_H__
#define __MIDIPART_H__

#include <wx/wx.h>
#include <iostream>
#include <vector>

using namespace				std;

#define NB_ROWS				(NB_WHITEKEY * 12 / 7)	
#define ROW_HEIGHT			BLACKKEY_HEIGHT
#define ROW_WIDTH			BLACKKEY_WIDTH
#define MAXNPM				(64)

enum
  {
    ID_TOOL_MOVE_MIDIPART = 1342,
    ID_TOOL_EDIT_MIDIPART,
    ID_TOOL_DEL_MIDIPART
  };

class					Note;
class					EditNote;
class					Sequencer;
class					MidiPattern;

class					MidiPart: public wxControl
{
 public:
  MidiPart(wxWindow *parent, wxWindowID id, const wxPoint& pos,
	const wxSize& size, long style, class EditMidi *em);
  void					ChangeMesureCount(int NbMesures);
  void					OnPaint(wxPaintEvent &e);
  void					OnClick(wxMouseEvent &e);
  void					OnReleaseClick(wxMouseEvent &e);
  void					OnMouseMove(wxMouseEvent &e);
  void					SetZoomX(double pZoomX);
  void					SetZoomY(double pZoomY);
  void					SetNPM(int pNPM);
  void					SetMidiPattern(MidiPattern *p);
  int					GetNPM();
  void					SetTool(int numtool);
  EditMidi				*em;

 private:
  int					NbMesures;
  int					NbRows;
  double				ZoomX;
  double				ZoomY;
  int					NPM; 
  vector<bool>				BlackKeys;
  vector<Note *>			Notes;
  EditNote				*selected;
  int					tool;
  Note					*selected2;
  MidiPattern				*pattern;
  int					lastOne;

  DECLARE_EVENT_TABLE()
};

wxRect					*CalcIntersection(wxRect &, wxRect &);

extern Sequencer			*Seq;

#endif
