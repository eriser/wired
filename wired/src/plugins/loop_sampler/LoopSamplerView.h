// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __LOOPSAMPLERVIEW_H__
#define __LOOPSAMPLERVIEW_H__

#include <wx/wx.h>
#include "RulerLoop.h"
#include "DownButton.h"
#include "KnobCtrl.h"
#include "Slice.h"
#include "WaveLoop.h"

using namespace std;

#include <list>

#define IMG_LSV_HAND_UP		"plugins/loopsampler/hand_up.png"
#define IMG_LSV_HAND_DOWN	"plugins/loopsampler/hand_down.png"
#define IMG_LSV_PEN_UP		"plugins/loopsampler/pen_up.png"
#define IMG_LSV_PEN_DOWN	"plugins/loopsampler/pen_down.png"
#define IMG_LSV_INV_UP		"plugins/loopsampler/inverse_up.png"
#define IMG_LSV_INV_DOWN	"plugins/loopsampler/inverse_down.png"
#define IMG_LSV_CREATE_UP	"plugins/loopsampler/create_up.png"
#define IMG_LSV_CREATE_DOWN	"plugins/loopsampler/create_down.png"
#define IMG_LSV_FADER_BG	"plugins/loopsampler/fader_bg.png"
#define IMG_LSV_FADER_FG	"plugins/loopsampler/fader_fg.png"
#define IMG_LSV_KNOB_BG		"plugins/loopsampler/knob_bg.png"
#define IMG_LSV_KNOB_FG		"plugins/loopsampler/knob_fg.png"

class LoopSamplerView : public wxPanel
{
 public:
  LoopSamplerView(wxMutex *mutex, wxWindow *parent, const wxPoint &pos, 
		  const wxSize &size, string datadir, LoopPos *loopinfo);
  ~LoopSamplerView();

  void SetWaveFile(WaveFile *w);
  void SetBeats(int bars, int beats);
  void SetSlices(list<Slice *> *slices);
  void SetBarCoeff(double coeff) { if (Wave) Wave->SetBarCoeff(coeff); }
  void SetSamplingRate(int rate) { if (Wave) Wave->SetSamplingRate(rate); }

 protected:
  wxMutex *Mutex;

  list<Slice *> *Slices;
  int Beats;
  int Bars;

  string DataDir;
  LoopPos *LoopInfo;

  wxPanel *Toolbar;
  WaveLoop *Wave;
  RulerLoop *Rule;

  DownButton *DragBtn;
  DownButton *PenBtn;
  DownButton *InvertBtn;
  DownButton *AutoBtn;

  KnobCtrl *NoteKnob;
  KnobCtrl *PitchKnob;
  KnobCtrl *VolKnob;
  KnobCtrl *AffectKnob;

  wxStaticText *NoteLabel;
  wxStaticText *PitchLabel;
  wxStaticText *VolLabel;
  wxStaticText *AffectLabel;

  wxImage *hand_up;
  wxImage *hand_down;
  wxImage *pen_up;
  wxImage *pen_down;
  wxImage *inverse_up;
  wxImage *inverse_down;
  wxImage *create_up;
  wxImage *create_down;

  wxImage *fader_bg;
  wxImage *fader_fg;
  wxImage *knob_bg;
  wxImage *knob_fg;

  void OnDragClick(wxCommandEvent &event);
  void OnPenClick(wxCommandEvent &event);
  void OnInvertClick(wxCommandEvent &event);
  void OnAutoClick(wxCommandEvent &event);
  void OnSliceSelected(wxCommandEvent &event);
  void OnNote(wxScrollEvent &event);
  void OnPitch(wxScrollEvent &event);
  void OnVolume(wxScrollEvent &event);
  void OnAffectMidi(wxScrollEvent &event);

  DECLARE_EVENT_TABLE()
};

enum
  {
    LoopSamplerView_Drag = 1,
    LoopSamplerView_Pen,
    LoopSamplerView_Invert,
    LoopSamplerView_Auto,
    LoopSamplerView_Note,
    LoopSamplerView_Pitch,
    LoopSamplerView_Vol,
    LoopSamplerView_Affect,
    LoopSamplerView_Wave
  };
#endif
