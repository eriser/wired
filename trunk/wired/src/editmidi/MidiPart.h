#ifndef __MIDIPART_H__
#define __MIDIPART_H__

#include <wx/wx.h>
#include <iostream>
#include <vector>
#include "MidiPattern.h"
#include "midi.h"
#include "MidiFile.h"
#include "Note.h"
#include "Clavier.h"
#include "EditMidi.h"
#include "EditNote.h"
#include "Sequencer.h"

enum
  {
    ID_TOOL_MOVE_MIDIPART = 1342,
    ID_TOOL_EDIT_MIDIPART,
    ID_TOOL_DEL_MIDIPART
  };

#define NB_ROWS				(NB_WHITEKEY * 12 / 7)	
#define ROW_HEIGHT			BLACKKEY_HEIGHT
#define ROW_WIDTH			BLACKKEY_WIDTH
#define MAXNPM				64

using namespace std;
using std::vector;

class MidiPart: public wxControl
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

  DECLARE_EVENT_TABLE()
};

wxRect					*CalcIntersection(wxRect &, wxRect &);

extern Sequencer			*Seq;

#endif
