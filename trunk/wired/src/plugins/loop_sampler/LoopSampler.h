// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __LOOPSAMPLER_H__
#define __LOOPSAMPLER_H__

#include "Plugin.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "FaderCtrl.h"
#include "LoopSamplerView.h"
#include "WaveFile.h"
#include "Slice.h"
#include "Polyphony.h"
#include "KnobCtrl.h"
#include "CycleKnob.h"

using namespace std;

#include <list>

#define PLUGIN_NAME "Loop Sampler"

#define IMG_LS_BMP	"plugins/loopsampler/LoopSampler.bmp"
#define IMG_LS_PLAY_UP  "plugins/loopsampler/loop_play_up.png"
#define IMG_LS_PLAY_DOWN "plugins/loopsampler/loop_play_down.png"
#define IMG_LS_OPEN_UP  "plugins/loopsampler/loop_open_up.png"
#define IMG_LS_OPEN_DOWN "plugins/loopsampler/loop_open_down.png"
#define IMG_LS_SAVE_UP  "plugins/loopsampler/loop_save_up.png"
#define IMG_LS_SAVE_DOWN "plugins/loopsampler/loop_save_down.png"
#define IMG_LS_SEQ_UP   "plugins/loopsampler/loop_toseq_up.png"
#define IMG_LS_SEQ_DOWN "plugins/loopsampler/loop_toseq_down.png"
#define IMG_LS_OPT_UP   "plugins/loopsampler/loop_opt_up.png"
#define IMG_LS_OPT_DOWN "plugins/loopsampler/loop_opt_down.png"
#define IMG_LS_UPUP_IMG "plugins/loopsampler/loop_up_button_up.png"
#define IMG_LS_UPDO_IMG    "plugins/loopsampler/loop_up_button_down.png"
#define IMG_LS_DOWNUP_IMG  "plugins/loopsampler/loop_down_button_up.png"
#define IMG_LS_DOWNDO_IMG  "plugins/loopsampler/loop_down_button_down.png"
#define IMG_LS_LED_OFF_IMG  "plugins/loopsampler/loop_midi_light_off.png"
#define IMG_LS_LED_ON_IMG  "plugins/loopsampler/loop_midi_light_on.png"
#define IMG_LS_BTN_ON_IMG  "plugins/loopsampler/loop_button_down.png"
#define IMG_LS_BTN_DOWN_IMG  "plugins/loopsampler/loop_button_up.png"
#define IMG_LS_BG	"plugins/loopsampler/loopsampler_bg.png"
#define IMG_LS_FADER_BG  "plugins/loopsampler/loop_fader_bg.png"
#define IMG_LS_FADER_FG  "plugins/loopsampler/loop_fader_button.png"
#define IMG_LS_FADER_VOL_BG  "plugins/loopsampler/loop_fader_vol_bg.png"
#define IMG_LS_FADER_VOL_FG  "plugins/loopsampler/loop_fader_vol_button.png"
#define IMG_LS_KNOB_BG  "plugins/loopsampler/loop_knob_1.png"
#define IMG_LS_KNOB_FG  "plugins/loopsampler/loop_knob_dot.png"

#define IMG_LS_KNOB_0   "plugins/loopsampler/loop_knob_octave_0.png"
#define IMG_LS_KNOB_1   "plugins/loopsampler/loop_knob_octave_1.png"
#define IMG_LS_KNOB_2   "plugins/loopsampler/loop_knob_octave_2.png"
#define IMG_LS_KNOB_3   "plugins/loopsampler/loop_knob_octave_3.png"
#define IMG_LS_KNOB_4   "plugins/loopsampler/loop_knob_octave_4.png"
#define IMG_LS_KNOB_A   "plugins/loopsampler/loop_knob_octave_a.png"
#define IMG_LS_KNOB_B   "plugins/loopsampler/loop_knob_octave_b.png"
#define IMG_LS_KNOB_C   "plugins/loopsampler/loop_knob_octave_c.png"
#define IMG_LS_KNOB_D   "plugins/loopsampler/loop_knob_octave_d.png"

class LoopNote
{
 public:
  LoopNote(int note, float vol, Slice *sl, long delta, float **f, long length = 0, 
	   bool play_mode = false) 
    : Note(note), Volume(vol), SliceNote(sl), Delta(delta), Position(sl->Position), 
    BeginPosition(sl->Position), Buffer(f), Length(length), PlayMode(play_mode), 
    Attack(0.f), End(false), Start(true)
    {}
  ~LoopNote() {}

  int	Note;
  float	Volume;
  Slice *SliceNote;
  long  Delta;
  long  Position;
  long  BeginPosition;
  float **Buffer;
  long  Length;
  bool  PlayMode;
  float Attack;
  bool  End;
  bool  Start;
};

class LoopSampler: public Plugin
{
 public:
  LoopSampler(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~LoopSampler();

  void   Play(); //locks mutex
  void   Stop(); //locks mutex

  void	 Load(int fd, long size); //locks mutex
  long	 Save(int fd);

  void	 SetBufferSize(long size); //locks mutex
  void	 SetSamplingRate(double rate); //locks mutex
  void   SetBPM(float bmp); //locks mutex
  void	 SetSignature(int num, int den); //locks mutex

  std::string DefaultName() { return "LoopSampler"; }

  void   Process(float **input, float **output, long sample_length); //locks mutex
  void   ProcessEvent(WiredEvent &event); //locks mutex

  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  bool	 HasView() { return true; }
  wxWindow *CreateView(wxWindow *zone, wxPoint &pos, wxSize &size);
  void	 DestroyView();

  bool	 IsInstrument();

  bool	 IsAudio();
  bool	 IsMidi();

  void   Update();

  void	 SetWaveFile(WaveFile *w); //locks mutex
  void	 SetBarCoeff();
  
  wxBitmap *GetBitmap();

  void   SetHelpMode(bool on) { HelpMode = true; }

protected:
  /* Audio/Midi members */
  list<LoopNote *>	Notes;
  list<Slice *>		Slices;
  Polyphony		Workshop;

  LoopPos		LoopInfo;

  float AttackMs;  // millisecond
  long  AttackLen; // samples
  float AttackCoef; // coef volume

  float ReleaseCoef;
  float Envelope;

  double BarCount;
  int	 BeatCount;
  float	 Volume;
  float	 Attack;
  float	 Decay;
  float	 Sustain;
  float  Release;
  float	 Octave;
  float	 Pitch;
  bool	 Invert;
  bool	 Tempo;

  int	 MidiVolume[2];
  int	 MidiAttack[2];
  int	 MidiDecay[2];
  int	 MidiSustain[2];
  int	 MidiRelease[2];
  int	 MidiOctave[2];
  int	 MidiPitch[2];
  int	 MidiTempo[2];
  int	 MidiInvert[2];
  int	 MidiSliceNote[2];
  int	 MidiSlicePitch[2];
  int	 MidiSliceVol[2];
  int	 MidiSliceAffect[2];
  int	 MidiSliceInvert[2];

  bool   UpdateMidiLed;
  bool	 UpdateVolume;
  bool	 UpdateAttack;
  bool	 UpdateDecay;
  bool	 UpdateSustain;
  bool	 UpdateRelease;
  bool	 UpdateOctave;
  bool	 UpdatePitch;
  bool	 UpdateTempo;
  bool	 UpdateInvert;
  bool	 UpdateSliceNote;
  bool	 UpdateSlicePitch;
  bool	 UpdateSliceVol;
  bool	 UpdateSliceAffect;
  bool	 UpdateSliceInvert;
  
  bool	 MidiLed;

  unsigned int PolyphonyCount;

  bool	 Playing;
  bool	 SeqPlaying;
  bool	 AutoPlaying;
  double SamplingRate;

  bool   HelpMode;

  int  curL; // current position for sample left
  int  curR; // current position for sample right
  int  retTouchL;  // returned size of soundtouch buffer (left)
  int  retTouchR;  // returned size of soundtouch buffer (right)
  float **read_buf; // wave file read buffer

  /* Graphic controls */
  LoopSamplerView *View;
  WaveFile *Wave;

  wxBitmap *bmp;  
  wxImage  *ls_bg;
  wxImage *play_up;
  wxImage *play_down;
  wxImage *open_up;
  wxImage *open_down;
  wxImage *save_up;
  wxImage *save_down;
  wxImage *seq_up;
  wxImage *seq_down;
  wxImage *opt_up;
  wxImage *opt_down;
  wxImage *up_up;
  wxImage *up_down;
  wxImage *down_up;
  wxImage *down_down;
  wxImage *fader_bg;
  wxImage *fader_fg;
  wxImage *fader_vol_bg;
  wxImage *fader_vol_fg;
  wxImage *knob_bg;
  wxImage *knob_fg;
  wxImage *btn_up;
  wxImage *btn_down;

  DownButton *ShowOptBtn;

  DownButton *PlayBtn;
  DownButton *ToSeqTrackBtn;
  DownButton *SaveBtn;
  DownButton *OpenBtn;

  HoldButton *MesUpBtn;
  HoldButton *MesDownBtn;
  HoldButton *PolyUpBtn;
  HoldButton *PolyDownBtn;

  wxStaticText *MesCountLabel;
  wxStaticText *PolyCountLabel;
  
  wxStaticBitmap *MidiInBmp;

  FaderCtrl *VolumeFader;

  FaderCtrl *AttackFader;
  FaderCtrl *DecayFader;
  FaderCtrl *SustainFader;
  FaderCtrl *ReleaseFader;

  CycleKnob  *OctaveKnob;
  KnobCtrl  *PitchKnob;

  DownButton *InvertBtn;
  DownButton *TempoBtn;
  
  wxBitmap   *LedOff;
  wxBitmap   *LedOn;
  wxBitmap   *BgBmp;

  wxMutex    Mutex;

  void OnOpenFile(wxCommandEvent &event);
  void OnSaveFile(wxCommandEvent &event);
  void OnPlay(wxCommandEvent &event); //locks mutex
  void OnToSeqTrack(wxCommandEvent &event);
  void OnShowView(wxCommandEvent &event);
  void OnMesUp(wxCommandEvent &event); //locks mutex
  void OnMesDown(wxCommandEvent &event); //locks mutex
  void OnPolyUp(wxCommandEvent &event); //locks mutex
  void OnPolyDown(wxCommandEvent &event); //locks mutex
  void OnTempo(wxCommandEvent &event); // locks mutex
  void OnInvert(wxCommandEvent &event); //locks mutex
  void OnOctave(wxCommandEvent &event); //locks mutex
  void OnPitch(wxScrollEvent &event); //locks mutex
  void OnVolume(wxScrollEvent &event); //locks mutex
  void OnAttack(wxScrollEvent &event); //locks mutex
  void OnPaint(wxPaintEvent &event);

  void OnHelp(wxMouseEvent &event);
  void OnOctaveHelp(wxMouseEvent &event);
  void OnPitchHelp(wxMouseEvent &event);
  void OnAutoStretchHelp(wxMouseEvent &event);
  void OnInvertHelp(wxMouseEvent &event);
  void OnPlayHelp(wxMouseEvent &event);
  void OnOpenHelp(wxMouseEvent &event);
  void OnSaveHelp(wxMouseEvent &event);
  void OnSendSeqHelp(wxMouseEvent &event);
  void OnOptViewHelp(wxMouseEvent &event);

  void OnVolumeController(wxMouseEvent &event);
  void OnAttackController(wxMouseEvent &event);
  void OnDecayController(wxMouseEvent &event);
  void OnSustainController(wxMouseEvent &event);
  void OnReleaseController(wxMouseEvent &event);
  void OnOctaveController(wxMouseEvent &event);
  void OnPitchController(wxMouseEvent &event);
  void OnTempoController(wxMouseEvent &event);
  void OnInvertController(wxMouseEvent &event);

  void SetTempo();
  void ProcessMidiControls(int MidiData[3]);
  void CheckExistingControllerData(int MidiData[3]);

  DECLARE_EVENT_TABLE()
};

enum
  {
    LoopSampler_MesUp = 1,
    LoopSampler_MesDown,
    LoopSampler_PolyUp,
    LoopSampler_PolyDown,
    LoopSampler_ShowOpt,
    LoopSampler_Play,
    LoopSampler_ToSeqTrack,
    LoopSampler_Save,
    LoopSampler_Open,
    LoopSampler_Invert,
    LoopSampler_Tempo,
    LoopSampler_Octave,
    LoopSampler_Pitch,
    LoopSampler_Volume,
    LoopSampler_Attack,
    LoopSampler_Decay,
    LoopSampler_Sustain,
    LoopSampler_Release
  };

#endif
