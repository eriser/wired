// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __WAVELOOP_H__
#define __WAVELOOP_H__

#include <wx/statline.h>
#include "WaveView.h"
#include "Slice.h"

#define wxEVT_SLICE_SELECTED	31310031

using namespace std;

#include <list>

class WaveLoop: public WaveView
{
 public:
  WaveLoop(wxMutex *mutex, wxWindow *parent, wxWindowID id, 
	   const wxPoint& pos, const wxSize& size);
  ~WaveLoop();

  void SetDrawing(bool draw);
  void SetSelect(bool select);
  void SetSlices(list<Slice *> *slices);
  void SetBarCoeff(double coef) { BarCoeff = coef; }
  void SetSamplingRate(int rate) { SamplingRate = rate; }
  void AddSlice(int x, int m_x);
  void CreateSlices(int beats, int mescount);

 protected:
  void OnClick(wxMouseEvent &event);
  void OnSize(wxSizeEvent &event);
  void OnPaint(wxPaintEvent &event);

  double BarCoeff;
  int  NoteNumber;
  int SamplingRate;

  bool Drawing;
  bool Select;
  list<Slice *> *Slices;

  wxMutex *Mutex;

  //DECLARE_EVENT_TABLE()
};

#endif
