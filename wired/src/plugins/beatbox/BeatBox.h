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
#include "StaticPosKnob.h"
#include "CycleKnob.h"
#include "midi.h"

using namespace std;

#define PLUGIN_NAME		wxT("DRUM-31")

#define BEATBOX_BG		wxT("plugins/beatbox/beatbox_bg.png")
#define BEATBOX_MINI_BG		wxT("plugins/beatbox/drm31.bmp")
#define BEATBTN_UNCLICKED	wxT("plugins/beatbox/beatbtn_unclicked.png")
#define BEATBTN_MEDIUM		wxT("plugins/beatbox/beatbtn_medium.png")
#define BEATBTN_VLOW		wxT("plugins/beatbox/beatbtn_vlow.png")
#define BEATBTN_LOW		wxT("plugins/beatbox/beatbtn_low.png")
#define BEATBTN_HIGH		wxT("plugins/beatbox/beatbtn_high.png")
#define BEATBTN_VHIGH		wxT("plugins/beatbox/beatbtn_vhigh.png")

#define PLAY_UP			wxT("plugins/beatbox/play_up.png")
#define PLAY_DO			wxT("plugins/beatbox/play_down.png")
#define EDIT_UP			wxT("plugins/beatbox/edit_up.png")
#define EDIT_DO			wxT("plugins/beatbox/edit_down.png")

#define POSON			wxT("plugins/beatbox/dot_pattern_on.png")
#define POSOFF			wxT("plugins/beatbox/dot_pattern_off.png")

#define LOADPATCH_UP		wxT("plugins/beatbox/load_up.png")
#define LOADPATCH_DO		wxT("plugins/beatbox/load_down.png")
#define SAVEPATCH_UP		wxT("plugins/beatbox/save_up.png")
#define SAVEPATCH_DO		wxT("plugins/beatbox/save_down.png")

#define SHOWOPT_UP		wxT("plugins/beatbox/optional_up.png")
#define SHOWOPT_DOWN		wxT("plugins/beatbox/optional_down.png")

#define KNOB			wxT("plugins/beatbox/knob_center_level.png")
#define DOT			wxT("plugins/beatbox/level_dot.png")

#define BANK1			wxT("plugins/beatbox/bank_knob_1.png")
#define BANK2			wxT("plugins/beatbox/bank_knob_2.png")
#define BANK3			wxT("plugins/beatbox/bank_knob_3.png")
#define BANK4			wxT("plugins/beatbox/bank_knob_4.png")
#define BANK5			wxT("plugins/beatbox/bank_knob_5.png")

#define STEPS_KNOB1		wxT("plugins/beatbox/steps_knob_3.png")
#define STEPS_KNOB2		wxT("plugins/beatbox/steps_knob_2.png")
#define STEPS_KNOB3		wxT("plugins/beatbox/steps_knob_1.png")

#define SELECTOR_UP		SHOWOPT_UP
#define SELECTOR_DO		SHOWOPT_DOWN

#define POS_UP			wxT("plugins/beatbox/pos_up.png")
#define POS_DO			wxT("plugins/beatbox/pos_down.png")

#define UP_1			wxT("plugins/beatbox/1_up.png")
#define DO_1			wxT("plugins/beatbox/1_down.png")
#define UP_2			wxT("plugins/beatbox/2_up.png")
#define DO_2			wxT("plugins/beatbox/2_down.png")
#define UP_3			wxT("plugins/beatbox/3_up.png")
#define DO_3			wxT("plugins/beatbox/3_down.png")
#define UP_4			wxT("plugins/beatbox/4_up.png")
#define DO_4			wxT("plugins/beatbox/4_down.png")
#define UP_5			wxT("plugins/beatbox/5_up.png")
#define DO_5			wxT("plugins/beatbox/5_down.png")
#define UP_6			wxT("plugins/beatbox/6_up.png")
#define DO_6			wxT("plugins/beatbox/6_down.png")
#define UP_7			wxT("plugins/beatbox/7_up.png")
#define DO_7			wxT("plugins/beatbox/7_down.png")
#define UP_8			wxT("plugins/beatbox/8_up.png")
#define DO_8			wxT("plugins/beatbox/8_down.png")

#define NUM_PATTERNS		8
#define NUM_BANKS		5
#define ID_VIEW_ACT		313131

#define NB_CHAN 11

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)
#define undenormalise(sample) \
  if(((*(unsigned int*)&sample)&0x7f800000)==0) sample=0.0f

#define DELETE_RYTHMS(R) {						\
                          for (unsigned char bank = 0; bank < 5; bank++)\
			    {						\
			      for (unsigned char ps = 0; ps < 8; ps++)	\
			        {					\
			         for (list<BeatNote*>::iterator		\
				      bn = R[bank][ps].begin();		\
				      bn != R[bank][ps].end();)		\
				   {					\
				     delete *bn;			\
				     bn = R[bank][ps].erase(bn);	\
				   }					\
			        }					\
			    }						\
			 }						


#define CLIP(x)	{							\
			if (x < 0.f)					\
			  { x = 0.f; }					\
			else if (x > 1.f)				\
			  { x = 1.f }					\
		}


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
  
  wxString	GetHelpString() { return _("No help provided for this plugin"); }
  wxString	DefaultName() { return wxT("WiredBeatBox"); }
  
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
    

  void		LockLoading();
  void		UnlockLoading();
  
  wxMutex*	GetMutexPtr() { return &PatternMutex; }
  wxBitmap*	GetBitmap() { return MiniBmp; }
  
  void		SetVoices();
  void		AddBeatNote(BeatBoxChannel* c, double rel_pos,
			    unsigned int state);
  // View interface
    
  int		GetSig(void);
  int		GetSteps(void);
  void		AddBeatNote(BeatNote* note, BeatBoxChannel* c, 
			    unsigned int bank, unsigned int track);
  void		RemBeatNote(BeatNote* note, BeatBoxChannel* c, 
			    unsigned int bank, unsigned int track);

  void		SetHelpMode(bool on) { HelpMode = true; }
  bool		HelpMode;
  
  BeatBoxChannel**	Channels;
  BeatBoxChannel*	SelectedChannel;
  unsigned int		EditedPattern;
  unsigned int		EditedBank;
  
  void			OnVolumeController(wxMouseEvent& event);
  void			OnStepsController(wxMouseEvent& event);
  
  void			ProcessMidiControls(int data[3]);
  void			CheckExistingControllerData(int data[3]);
  
  int			BanksMidiNotes[NUM_BANKS];
  int			PatternsMidiNotes[NUM_PATTERNS];
  int			ChanMidiNotes[NB_CHAN];
  int			MidiVolume[3];
  int			MidiSteps[3];
  
 protected:
  bool			OnLoading;
  wxMutex		PatternMutex;
  wxMutex		MidiMutex;
  BeatBoxView*		View;
  void			OnViewAction(wxCommandEvent& event);
  int			VoicesCount[NB_CHAN];
  int			Voices;
  //vars mutexed
  float			MLevel;
  list<BeatNoteToPlay*>	NotesToPlay;
  Polyphony*		Pool;
  //bool			PlayNext;
  bool			Playing;
  bool			SeqPlaying;
  bool			AutoPlay;

  KnobCtrl*		MVol;
  void			ReCalcStepsSigCoef(void);
  void			UpdateSteps(unsigned int bank, 
				    unsigned int track);
  void			UpdateStepsDeps(unsigned int steps);
  void			RefreshPosLeds(double pos);
  void			UpdateNotesPositions(unsigned int bank,
					     unsigned int track);
  void			SetNoteAttr(BeatNoteToPlay* note, BeatBoxChannel* c);
  void			SetChanAttrToNote(BeatNoteToPlay* note, 
					  BeatBoxChannel* c);
  void			SetMidiNoteAttr(BeatNoteToPlay* note, 
					BeatBoxChannel* c);
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
  int			SigIndex[NUM_BANKS][NUM_PATTERNS];
  
  DownButton*		OptViewBtn;
  
  unsigned int		SelectedPattern;
  unsigned int		NewSelectedPattern;
  unsigned int		SelectedBank;
  unsigned int		NewSelectedBank;
  //unsigned int		EditedPattern;
  //unsigned int		EditedBank;
  
  bool			OnEdit;
  
  unsigned int		PosIndex;
  
  IdButton**		PatternSelectors;
  IdButton**		SignatureButtons;
  IdButton**		PositionButtons;
  
  StaticBitmap**	PositionLeds;
  bool			LedsOn[16];
  BeatButton**		Beat;
  DownButton*		LoadKit;
  
    
  wxBitmap*		PositionOn;
  wxBitmap*		PositionOff;
  wxImage**		Imgs;
  wxBitmap**		Bitmaps;
  wxBitmap*		bmp;
  wxBitmap*		BgBmp;
  wxBitmap*		MiniBmp;
  
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
  void			OnRightDown(wxMouseEvent& event);
  void			OnCopyPattern(wxCommandEvent& event);
  void			OnPastePattern(wxCommandEvent& event);
    
  wxMenu*		PopMenu;
  wxMenu*		BankMenu;
  wxMenu*		PatternMenu;
  list<BeatNote*>	TmpPattern[NB_CHAN];
  list<BeatNote*>	TmpPatternToErase[NB_CHAN];
 
  /* graphical updates relatives */
  void			Update();
  bool			AskUpdateSteps;
  bool			AskUpdatePattern;
  bool			AskUpdateBank;
  bool			AskUpdateLevel;
  
  
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
    BB_OnBankChange,
    BB_PopMenu,
    BB_PatternCopy,
    BB_PatternPaste
  };

#endif//__BEATBOX_H__
