#ifndef __BEATBOXCHANNEL_H__
#define __BEATBOXCHANNEL_H__

#include <string>
#include <list>
#include <wx/wx.h>

//#include "BeatBox.h"
#include "BeatNote.h"
#include "WaveFile.h"
#include "DownButton.h"
#include "KnobCtrl.h"
#include "CycleKnob.h"
#include "Polyphony.h"

//#include "Plugin.h"

#define ACT_SELECT	0
#define ACT_SOLO	1
#define ACT_PLAY	2
#define ACT_SETWAVE	3

#define CHANNEL_BG	"plugins/beatbox/channel_bg.png"
#define SELECT_UP	"plugins/beatbox/selectchannel_up.png"
#define SELECT_DO	"plugins/beatbox/selectchannel_down.png"
#define CH_PLAY_UP	"plugins/beatbox/channel_play_up.png"
#define CH_PLAY_DO	"plugins/beatbox/channel_play_down.png"
#define CH_LOAD_UP	"plugins/beatbox/channel_load_up.png"
#define CH_LOAD_DO	"plugins/beatbox/channel_load_down.png"
#define REV_UP		"plugins/beatbox/channel_r_up.png"
#define REV_DO		"plugins/beatbox/channel_r_down.png"
#define MUTE_UP		"plugins/beatbox/channel_m_up.png"
#define MUTE_DO		"plugins/beatbox/channel_m_down.png"
#define CH_DOT		"plugins/beatbox/level_dot.png"
#define CH_KNOB		"plugins/beatbox/channel_knob.png"
#define CH_KNOB_CENTER	"plugins/beatbox/channel_knob-center.png"
#define SOLO_UP		"plugins/beatbox/channel_s_up.png"
#define SOLO_DO		"plugins/beatbox/channel_s_down.png"
#define CH_KNOB_LEV_CENTER	"plugins/beatbox/channel_knob_level-center.png"
#define CH_KNOB_PAN_CENTER	"plugins/beatbox/channel_knob_pan-center.png"
#define CH_LEV_DOT	CH_DOT//"plugins/beatbox/channel_dot_lev.png"

#define CH_POLY1	"plugins/beatbox/channel_knob_poly3.png"
#define CH_POLY2	"plugins/beatbox/channel_knob_poly2.png"
#define CH_POLY3	"plugins/beatbox/channel_knob_poly1.png"

class WiredBeatBox;

class BeatBoxChannel : public wxWindow
{
 public:
  BeatBoxChannel( wxWindow *parent, wxWindowID id, const wxPoint &pos,
		  const wxSize &size, unsigned int num, WiredBeatBox* drm31);
  //string datadir,  wxMutex* mutex );
  ~BeatBoxChannel();
  
  void		OnPaint(wxPaintEvent& event);
  void		OnMouseEvent(wxMouseEvent& event);
  void		OnEnterWindow(wxMouseEvent& event);
  void		OnLeave(wxMouseEvent& event);
  
  void		OnSelectHelp(wxMouseEvent& event);
  void		OnLoadHelp(wxMouseEvent& event);
  void		OnReverseHelp(wxMouseEvent& event);
  void		OnMuteHelp(wxMouseEvent& event);
  void		OnSoloHelp(wxMouseEvent& event);
  void		OnPlayHelp(wxMouseEvent& event);
  void		OnLevHelp(wxMouseEvent& event);
  void		OnPanHelp(wxMouseEvent& event);
  void		OnStaHelp(wxMouseEvent& event);
  void		OnEndHelp(wxMouseEvent& event);
  void		OnPitHelp(wxMouseEvent& event);
  void		OnVelHelp(wxMouseEvent& event);
  void		OnPolHelp(wxMouseEvent& event);
  
  void		OnSelectChannel(wxCommandEvent& event);
  void		OnLoadSound(wxCommandEvent& event);
  void		OnPlaySound(wxCommandEvent& event);
  void		ReverseSound(wxCommandEvent& event);
  void		OnMute(wxCommandEvent& event);
  void		OnSolo(wxCommandEvent& event);
  
  void		OnLevChange(wxScrollEvent& event);
  void		OnPanChange(wxScrollEvent& event);
  void		OnStartChange(wxScrollEvent& event);
  void		OnEndChange(wxScrollEvent& event);
  void		OnPitchChange(wxScrollEvent& event);
  void		OnVelChange(wxScrollEvent& event);
  void		OnPolyphonyChange(wxCommandEvent& e);
  
  void		SetPlay();
  void		SetSolo();
  void		SetMute();
  void		SetUnMute();
  
  void		SetLev(int lev);
  void		SetPan(int pan);
  void		SetStart(int start);
  void		SetEnd(int end);
  void		SetPitch(int pitch);
  void		SetVel(int vel);
  void		SetPolyphony(int voices);
  
  bool		IsSolo;
  
  void		Reset(void);
  
  void		Mute(void);
  void		UnMute(void);
  void		Solo(void);
  void		UnSolo(void);
  void		SetWaveFile(WaveFile* wave);
  void		Select(void);
  void		DeSelect(void);
  
  unsigned int	Id;
  unsigned int  Action;
  unsigned int* Data;
  
  wxStaticText* VoicesLabel;
  unsigned int	Voices;

  DownButton*	PlayButton;
  bool		Muted;
  bool		Selected;
  bool		Reversed;
  
  float		Params[NB_PARAMS];
    
  list<BeatNote*>**
    Rythms;
  
  WaveFile*	Wave;
  //Polyphony	*Voices;
  
  // MIDI Controls
  int		MidiVolume[3];
  int		MidiPan[3];
  int		MidiStart[3];
  int		MidiEnd[3];
  int		MidiPoly[3];
  int		MidiPitch[3];
  int		MidiVel[3];
  
  void OnLevController(wxMouseEvent& event);
  void OnPanController(wxMouseEvent& event);
  void OnStartController(wxMouseEvent& event);
  void OnEndController(wxMouseEvent& event);
  void OnPitchController(wxMouseEvent& event);
  void OnVelController(wxMouseEvent& event);
  void OnPolyController(wxMouseEvent& event);
  
  bool AskUpdateChannel;
  
  bool AskUpdatePlay;
  bool AskUpdateMute;
  bool AskUpdateSolo;
  bool AskUpdateLevel;
  bool AskUpdatePan;
  bool AskUpdateVel;
  bool AskUpdatePitch;
  bool AskUpdateStart;
  bool AskUpdateEnd;
  
  void Update();
 protected:
  WiredBeatBox*	DRM31;
  string        DataDir;
  wxStaticText* WaveLabel;
  wxBitmap*	BgBmp;
  wxMutex*	PatternMutex;
  DownButton*	MuteButton;
  DownButton*	SoloButton;
  DownButton*	OpenFile;
  DownButton*	SelectionButton;
  KnobCtrl*	KnobLev;
  KnobCtrl*	KnobPan;
  KnobCtrl*	KnobStart;
  KnobCtrl*	KnobEnd;
  KnobCtrl*	KnobPitch;
  KnobCtrl*	KnobVel;
  CycleKnob*	PolyKnob;
DECLARE_EVENT_TABLE()
};

enum
  {
    BC_Select = 1,
    BC_LoadSound,
    BC_ReverseSound,
    BC_Mute,
    BC_Solo,
    BC_PlaySound,
    BC_Lev,
    BC_Pan,
    BC_Sta,
    BC_End,
    BC_Pit,
    BC_Vel,
    BC_Pol
  };
#endif//__BEATBOXCHANNEL_H__
