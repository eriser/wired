// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __WAVELOOP_H__
#define __WAVELOOP_H__

#include "WaveView.h"
#include "Slice.h"
#include "SliceGui.h"
#include "LoopCursor.h"

#define wxEVT_SLICE_SELECTED	31310031

using namespace std;

#include <list>

typedef struct 
{
  int	Start;
  int	End;
}	LoopPos;

class WaveLoop: public WaveView
{
 public:
  WaveLoop(wxMutex *mutex, LoopPos *loopinfo, wxWindow *parent, wxWindowID id, 
	   const wxPoint& pos, const wxSize& size);
  ~WaveLoop();

  void SetWave(WaveFile *w);
  void SetDrawing(bool draw);
  void SetSelect(bool select);
  void SetSlices(list<Slice *> *slices);
  void SetBarCoeff(double coef) { BarCoeff = coef; }
  void SetSamplingRate(int rate) { SamplingRate = rate; }
  void AddSlice(int x, int m_x);
  void CreateSlices(int beats, int mescount);

  int  LoopStart;
  int  LoopEnd;

 protected:
  void OnClick(wxMouseEvent &event);
  void OnSize(wxSizeEvent &event);
  void OnPaint(wxPaintEvent &event);
  void OnSliceBtnSelected(wxCommandEvent &event);
  void OnSliceMove(wxCommandEvent &event);
  void OnLoopMove(wxCommandEvent &event);

  double BarCoeff;
  int  NoteNumber;
  int SamplingRate;
  LoopPos *LoopInfo;

  bool Drawing;
  bool Select;
  list<Slice *> *Slices;

  wxMutex *Mutex;
  
  LoopCursor *LoopStartCursor;
  LoopCursor *LoopEndCursor;

  //DECLARE_EVENT_TABLE()
};

#endif
