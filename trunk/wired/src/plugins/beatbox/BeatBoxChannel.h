#ifndef __BEATBOXCHANNEL_H__
#define __BEATBOXCHANNEL_H__

#include <string>
#include <list>
#include <wx/wx.h>

//#include "BeatBox.h"
#include "WaveFile.h"
#include "DownButton.h"
#include "KnobCtrl.h"
#include "HintedKnob.h"
#include "CycleKnob.h"
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

class BeatBoxChannel;

class BeatNote
{
 public:
  BeatNote(double pos, unsigned int state, double bpos)
    { 
      State = state;
      Position = pos;
      BarPos = bpos;
      Start = End = Len = Pitch = Vel = Lev = 1.0f;
    }
  ~BeatNote() {}
  
  double	Position;
  double	BarPos;
  bool		Reversed;
  float		Start, End, Len;
  float		Vel, Pitch, Lev;
  unsigned int  State;
};

class BeatNoteToPlay
{
 public:
  BeatNoteToPlay(int notenum, float vel, unsigned long delta, 
		 BeatBoxChannel* c, float** b);
  BeatNoteToPlay(BeatNote* bn, unsigned int numchan, unsigned long delta, 
		 float** buf)
    {
      NumChan = numchan;
      NoteNum = 0;
      
      Pitch = bn->Pitch;
      Vel = bn->Vel;
      Lev = bn->Lev;
      Start = bn->Start;
      End = bn->End;
      Len = bn->Len;
      Reversed = bn->Reversed;
      Delta = delta;
      OffSet = 0;
      Buffer = buf;
    }
  BeatNoteToPlay(BeatBoxChannel* c, float** buffer);
  ~BeatNoteToPlay() 
    {
    }
  
  int		NoteNum;
  unsigned int	NumChan;
  long		OffSet;
  float		Start, End, Len;
  float		Vel, Pitch, Lev;
  bool		Reversed;
  float**	Buffer;
  unsigned long Delta;
  unsigned long SEnd;
};

class WiredBeatBox;

class BeatBoxChannel : public wxWindow
{
 public:
  BeatBoxChannel( wxWindow *parent, wxWindowID id, const wxPoint &pos,
		  const wxSize &size, unsigned int num, WiredBeatBox* drm31);
  //string datadir,  wxMutex* mutex );
  ~BeatBoxChannel();
 
  void		OnPaint(wxPaintEvent& event);
  void		OnSelectChannel(wxCommandEvent& event);
  void		OnLoadSound(wxCommandEvent& event);
  void		OnPlaySound(wxCommandEvent& event);
  void		ReverseSound(wxCommandEvent& event);
  void		OnMute(wxCommandEvent& event);
  void		OnSolo(wxCommandEvent& event);
  
  void		OnLevChange(wxScrollEvent& event);
  void		OnBalChange(wxScrollEvent& event);
  void		OnStartChange(wxScrollEvent& event);
  void		OnEndChange(wxScrollEvent& event);
  void		OnLenChange(wxScrollEvent& event);
  void		OnPitchChange(wxScrollEvent& event);
  void		OnVelChange(wxScrollEvent& event);
  void		OnPolyphonyChange(wxCommandEvent& e);

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
  unsigned int	NumVoices;

  DownButton*	PlayButton;
  bool		Muted;
  bool		Selected;
  bool		Reversed;
  float		Lev, Pitch, Vel;
  float		Start, End, Len;
  
  list<BeatNote*>*
    Rythms;
  /*
  list<BeatNote*>
		Rythm;
  */
  WaveFile*	Wave;
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
  HintedKnob*	KnobLev;
  HintedKnob*	KnobBal;
  HintedKnob*	KnobStart;
  HintedKnob*	KnobEnd;
  HintedKnob*	KnobLen;
  HintedKnob*	KnobPitch;
  HintedKnob*	KnobVel;
  CycleKnob*	PolyKnob;
DECLARE_EVENT_TABLE()
};

enum
  {
    BC_Select = 1,
    BC_OnLoadSound,
    BC_Velocity,
    BC_ReverseSound,
    BC_Mute,
    BC_Solo,
    BC_OnPlaySound,
    BC_Lev,
    BC_Bal,
    BC_Sta,
    BC_End,
    BC_Len,
    BC_Pit,
    BC_Vel,
    BC_Pol
  };
#endif//__BEATBOXCHANNEL_H__
