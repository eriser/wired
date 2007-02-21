// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include <wx/wxprec.h>
#include <wx/progdlg.h>
#ifndef WX_PRECOMP
# include <wx/wx.h>
#endif

#include "Plugin.h"
#include "midi.h"
#include "FaderCtrl.h"
#include "KnobCtrl.h"
#include "Osc.h"
#include "Adsr.h"
#include "Lfo.h"

#include <iostream>

#ifndef SYNTHPLUG_H_
# define SYNTHPLUG_H_

#define PLUGIN_NAME	wxT("Yeah-Yeah Synth")

static PlugInitInfo info;

class SynthPlugin: public Plugin
{
 public:
  SynthPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~SynthPlugin();

  void	Stop();

  void   Process(float **input, float **output, long sample_length);
  void   ProcessEvent(WiredEvent &event);

  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  void	 SetSamplingRate(double rate);
 
  float	GetFreq() { return Freq; }

  bool	 IsInstrument();

  bool	 IsAudio();
  bool	IsMidi();

  wxBitmap *GetBitmap();

protected:
  bool	NoteIsOn;
  float	Freq;

  double SampleRate;

  wxImage *fader_bg;
  wxImage *fader_fg;
  wxImage *knob_bg;
  wxImage *knob_fg;

  // osc 1
  Oscillator *Osc1;
  KnobCtrl *Osc1Phase;
  KnobCtrl *Osc1Transpose;
  KnobCtrl *Osc1Fine;
  FaderCtrl *Osc1Volume;

  // osc 2
  Oscillator *Osc2;
  KnobCtrl *Osc2Phase;
  KnobCtrl *Osc2Transpose;
  KnobCtrl *Osc2Fine;
  FaderCtrl *Osc2Volume;

  // adsr 1
  Adsr		*Adsr1;
  KnobCtrl  *Adsr1Attack;
  KnobCtrl  *Adsr1Decay;
  KnobCtrl  *Adsr1Sustain;
  KnobCtrl  *Adsr1Release;
  KnobCtrl  *Adsr1Level;

  // adsr 2
  Adsr		*Adsr2;
  KnobCtrl  *Adsr2Attack;
  KnobCtrl  *Adsr2Decay;
  KnobCtrl  *Adsr2Sustain;
  KnobCtrl  *Adsr2Release;
  KnobCtrl  *Adsr2Level;

  // lfo 1
  Lfo		*Lfo1;
  KnobCtrl  *Lfo1Rate;
  KnobCtrl  *Lfo1Amount;
  KnobCtrl  *Lfo1Delay;

  // lfo 2
  Lfo		*Lfo2;
  KnobCtrl  *Lfo2Rate;
  KnobCtrl  *Lfo2Amount;
  KnobCtrl  *Lfo2Delay;

  wxImage *png_tmp;  
  wxBitmap *BgBmp;  

  bool		first;

  wxMutex	Mutex;

  void OnButtonClick(wxCommandEvent &e);  
  void OnPaint(wxPaintEvent &event);
  void OnOsc1Phase(wxScrollEvent &event);
  void OnOsc1Transpose(wxScrollEvent &event);
  void OnOsc1Fine(wxScrollEvent &event);
  void OnOsc2Phase(wxScrollEvent &event);
  void OnOsc2Transpose(wxScrollEvent &event);
  void OnOsc2Fine(wxScrollEvent &event);
  void OnAdsr1Attack(wxScrollEvent &event);
  void OnAdsr1Decay(wxScrollEvent &event);
  void OnAdsr1Sustain(wxScrollEvent &event);
  void OnAdsr1Release(wxScrollEvent &event);
  void OnAdsr2Attack(wxScrollEvent &event);
  void OnAdsr2Decay(wxScrollEvent &event);
  void OnAdsr2Sustain(wxScrollEvent &event);
  void OnAdsr2Release(wxScrollEvent &event);
  void OnLfo1Rate(wxScrollEvent &event);
  void OnLfo1Amount(wxScrollEvent &event);
  void OnLfo1Delay(wxScrollEvent &event);
  void OnLfo2Rate(wxScrollEvent &event);
  void OnLfo2Amount(wxScrollEvent &event);
  void OnLfo2Delay(wxScrollEvent &event);

  DECLARE_EVENT_TABLE()
    };

    enum
      {
    Synth_Osc1_Phase = 1,
    Synth_Osc1_Transpose,
    Synth_Osc1_Fine,
    Synth_Osc2_Phase,
    Synth_Osc2_Transpose,
    Synth_Osc2_Fine,
    Synth_Adsr1_Attack,
    Synth_Adsr1_Decay,
    Synth_Adsr1_Sustain,
    Synth_Adsr1_Release,
    Synth_Adsr2_Attack,
    Synth_Adsr2_Decay,
    Synth_Adsr2_Sustain,
    Synth_Adsr2_Release,
    Synth_Lfo1_Rate,
    Synth_Lfo1_Amount,
    Synth_Lfo1_Delay,
    Synth_Lfo2_Rate, 
    Synth_Lfo2_Amount,
    Synth_Lfo2_Delay
  };

#endif /* SYNTHPLUG_H_ */
