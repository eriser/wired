#ifndef __BEATBOX_H__
#define __BEATBOX_H__

#include <iostream>
#include <list>
#include <wx/progdlg.h>

#include "Plugin.h"
#include "Polyphony.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "FaderCtrl.h"
#include "BeatButton.h"
#include "WaveFile.h"
//#include "FileLoader.h"
#include "BeatBoxChannel.h"
#include "IdButton.h"
#include "KnobCtrl.h"
#include "BeatBoxView.h"
//#include "Settings.h"
#include "HintedKnob.h"
#include "StaticPosKnob.h"
#include "CycleKnob.h"

using namespace std;

#define PLUGIN_NAME		"DRM-31"

#define NUM_PATTERNS		8
#define NUM_BANKS		5
#define ID_VIEW_ACT		313131

#define BEATBOX_BG		"plugins/beatbox/beatbox_bg.png"
#define BEATBTN_UNCLICKED	"plugins/beatbox/beatbtn_unclicked.png"
#define BEATBTN_MEDIUM		"plugins/beatbox/beatbtn_medium.png"
#define BEATBTN_VLOW		"plugins/beatbox/beatbtn_vlow.png"
#define BEATBTN_LOW		"plugins/beatbox/beatbtn_low.png"
#define BEATBTN_HIGH		"plugins/beatbox/beatbtn_high.png"
#define BEATBTN_VHIGH		"plugins/beatbox/beatbtn_vhigh.png"

#define PLAY_UP			"plugins/beatbox/play_up.png"
#define PLAY_DO			"plugins/beatbox/play_down.png"
#define EDIT_UP			"plugins/beatbox/edit_up.png"
#define EDIT_DO			"plugins/beatbox/edit_down.png"

#define POSON			"plugins/beatbox/dot_pattern_on.png"
#define POSOFF			"plugins/beatbox/dot_pattern_off.png"

#define LOADPATCH_UP		"plugins/beatbox/load_up.png"
#define LOADPATCH_DO		"plugins/beatbox/load_down.png"
#define SAVEPATCH_UP		"plugins/beatbox/save_up.png"
#define SAVEPATCH_DO		"plugins/beatbox/save_down.png"

#define SHOWOPT_UP		"plugins/beatbox/optional_up.png"
#define SHOWOPT_DOWN		"plugins/beatbox/optional_down.png"

#define KNOB			"plugins/beatbox/knob_center_level.png"
#define DOT			"plugins/beatbox/level_dot.png"

#define BANK1			"plugins/beatbox/bank_knob_1.png"
#define BANK2			"plugins/beatbox/bank_knob_2.png"
#define BANK3			"plugins/beatbox/bank_knob_3.png"
#define BANK4			"plugins/beatbox/bank_knob_4.png"
#define BANK5			"plugins/beatbox/bank_knob_5.png"

#define STEPS_KNOB1		"plugins/beatbox/steps_knob_3.png"
#define STEPS_KNOB2		"plugins/beatbox/steps_knob_2.png"
#define STEPS_KNOB3		"plugins/beatbox/steps_knob_1.png"

#define SELECTOR_UP		SHOWOPT_UP
#define SELECTOR_DO		SHOWOPT_DOWN

#define POS_UP			"plugins/beatbox/pos_up.png"
#define POS_DO			"plugins/beatbox/pos_down.png"

#define UP_1			"plugins/beatbox/1_up.png"
#define DO_1			"plugins/beatbox/1_down.png"
#define UP_2			"plugins/beatbox/2_up.png"
#define DO_2			"plugins/beatbox/2_down.png"
#define UP_3			"plugins/beatbox/3_up.png"
#define DO_3			"plugins/beatbox/3_down.png"
#define UP_4			"plugins/beatbox/4_up.png"
#define DO_4			"plugins/beatbox/4_down.png"
#define UP_5			"plugins/beatbox/5_up.png"
#define DO_5			"plugins/beatbox/5_down.png"
#define UP_6			"plugins/beatbox/6_up.png"
#define DO_6			"plugins/beatbox/6_down.png"
#define UP_7			"plugins/beatbox/7_up.png"
#define DO_7			"plugins/beatbox/7_down.png"
#define UP_8			"plugins/beatbox/8_up.png"
#define DO_8			"plugins/beatbox/8_down.png"


class WiredBeatBox : public Plugin
{
 public:
  WiredBeatBox(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~WiredBeatBox();
  
  void		Play();
  void		Stop();
  
  void		Load(int fd, long size);
  long		Save(int fd);
  
  void		SetBufferSize(long size);
  void		SetSamplingRate(long rate) { SamplingRate = rate; }
  void		Process(float **input, float **output, long sample_length);
  void		ProcessEvent(WiredEvent &event);
  
  bool		HasView() { return true; }
  wxWindow*	CreateView(wxWindow *zone, wxPoint &pos, wxSize &size);
  void		DestroyView();
  
  bool		IsAudio()	{ return false; }
  bool		IsMidi()	{ return true; }
  
  string	GetHelpString() { return "No help provided for this plugin"; }
  string	DefaultName() { return "WiredBeatBox"; }
  
  void		OnPaint(wxPaintEvent& event);
  void		OnMasterChange(wxScrollEvent& event);
  //void	OnKeyEvent(wxKeyEvent &event);
  //void	OnMouseWheelEvent(wxMouseEvent &event);
  void		ShowOpt(wxCommandEvent &event);
  void		OnPatternClick(wxCommandEvent &event);
  void		OnPatternMotion(wxCommandEvent &event);
  void		OnLoadKit(wxCommandEvent &event) {}
  void		OnToggleChannel(wxCommandEvent &event);
  void		OnPlay(wxCommandEvent& event);
  void		OnSigChoice(wxCommandEvent& event);
  void		OnPositionChoice(wxCommandEvent& event);
  void		OnLoadPatch(wxCommandEvent& event);
  void		OnSavePatch(wxCommandEvent& event);
  void		OnPatternSelectors(wxCommandEvent& event);
  void		OnEditButton(wxCommandEvent& event);
  void		OnStepsChange(wxCommandEvent& event);
  void		OnBankChange(wxCommandEvent& event);
  
  wxMutex*	GetMutexPtr() { return &PatternMutex; }
  wxBitmap*	GetBitmap() { return bmp; }
  void		AddBeatNote(BeatBoxChannel* c, double rel_pos,
			    unsigned int state);
  // View interface
    
  int		GetSig(void);
  
  void		SetHelpMode(bool on) { HelpMode = true; }
  bool		HelpMode;
  
  BeatBoxChannel**	Channels;
  BeatBoxChannel*	SelectedChannel;
  
 protected:
  wxMutex		PatternMutex;
  BeatBoxView*		View;
  void		OnViewAction(wxCommandEvent& event);
  
  //vars mutexed
  float			MLevel;
  list<BeatNoteToPlay*>	NotesToPlay;
  Polyphony*		Pool;
  bool			Playing;
  bool			SeqPlaying;
  bool			AutoPlay;
  HintedKnob*		MVol;
  //KnobCtrl*		MVol;
  void			ReCalcStepsSigCoef(void);
  void			UpdateSteps(unsigned int bank, 
				    unsigned int track);
  void			RefreshPosLeds(double pos);
  void			UpdateNotesPositions(unsigned int bank,
					     unsigned int track);
  void			SetNoteAttr(BeatNoteToPlay* note, BeatBoxChannel* c);
  void			GetNotesFromChannel(BeatBoxChannel* channel,
					    double bar_pos,
					    double bar_end,
					    double new_bar_pos,
					    double new_bar_end,
					    bool* isend);
  void			SetPatternList();
  
  DownButton*		PlayButton;
  DownButton*		EditButton;
  
  wxStaticText**	BeatLabels;
  wxStaticText*		StepsLabel;
  
  StaticPosKnob*	BankKnob;    
  CycleKnob*		StepsKnob;
  
  float			Signatures[5];
  char			SigDen[5];
  
  long			SamplingRate;
  long			BufferSize;
  
  int			Steps[NUM_BANKS][NUM_PATTERNS];
  double		StepsSigCoef[NUM_BANKS][NUM_PATTERNS];
  int			SignatureDen[NUM_BANKS][NUM_PATTERNS];
  double		Signature[NUM_BANKS][NUM_PATTERNS];
  unsigned long		SamplesPerBar[NUM_BANKS][NUM_PATTERNS];
  double		OldSamplesPerBar;
  double		BarsPerSample[NUM_BANKS][NUM_PATTERNS];
  double		OldBarsPerSample;
  unsigned int		SigIndex[NUM_BANKS][NUM_PATTERNS];
  
  DownButton*		OptViewBtn;
  
  unsigned int		SelectedPattern;
  unsigned int		NewSelectedPattern;
  unsigned int		EditedPattern;
  unsigned int		SelectedBank;
  unsigned int		NewSelectedBank;
  unsigned int		EditedBank;
  
  bool			OnEdit;
  
  unsigned int		PosIndex;
  
  IdButton**		PatternSelectors;
  IdButton**		SignatureButtons;
  IdButton**		PositionButtons;
  
  wxStaticBitmap**	PositionLeds;
  BeatButton**		Beat;
  DownButton*		LoadKit;
  
  int			ChanMidiNotes[12];
  
  wxBitmap*		PositionOn;
  wxBitmap*		PositionOff;
  wxImage**		Imgs;
  wxBitmap**		Bitmaps;
  wxBitmap*		bmp;
  wxBitmap*		BgBmp;
  
  void			OnHelp(wxMouseEvent& event);
  void			OnPlayHelp(wxMouseEvent& event);
  void			OnPatternHelp(wxMouseEvent& event);
  void			OnMasterLevHelp(wxMouseEvent& event);
  void			OnChannelHelp(wxMouseEvent& event);
  void			OnPositionHelp(wxMouseEvent& event);
  void			OnSignatureHelp(wxMouseEvent& event);
  void			OnStepsHelp(wxMouseEvent& event);
  void			OnBankHelp(wxMouseEvent& event);
  void			OnEditHelp(wxMouseEvent& event);
  void			OnPatternsSelectionHelp(wxMouseEvent& event);
  void			OnSaveLoadHelp(wxMouseEvent& event);


DECLARE_EVENT_TABLE()
};

enum
  {
    BB_ShowOpt = 10000,
    BB_OnMasterChange,
    BB_OnSigChoice,
    BB_OnPosChoice,
    BB_PatternClick,
    BB_OnPlayClick,
    BB_OnEditClick,
    BB_Channel,
    BB_OnPatternSelectors,
    BB_OnLoadPatch,
    BB_OnSavePatch,
    BB_OnStepsChange,
    BB_OnBankChange
  };

#endif//__BEATBOX_H__
