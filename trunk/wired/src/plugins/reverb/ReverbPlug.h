// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#ifndef __REVERBPLUG_H__
#define __REVERBPLUG_H__

#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "KnobCtrl.h"
#include "FaderCtrl.h"
#include "DownButton.h"
#include "StaticPosKnob.h"

#include "NRev.h"
#include "JCRev.h"
#include "PRCRev.h"

#define PLUGIN_NAME	"Reverb"

#define IMG_RV_BG	"plugins/reverb/reverb_bg.png"
#define IMG_RV_BMP	"plugins/reverb/ReverbPlug.bmp"
#define IMG_RV_FADER_BG	"plugins/reverb/fader_bg.png"
#define IMG_RV_FADER_FG	"plugins/reverb/fader_button.png"
#define IMG_RV_KNOB_BG	"plugins/reverb/knob_bg.png"
#define IMG_RV_KNOB_FG	"plugins/reverb/knob_fg.png"
#define IMG_LIQUID_ON	"plugins/reverb/liquid-cristal_play.png"
#define IMG_LIQUID_OFF	"plugins/reverb/liquid-cristal_stop.png"
#define IMG_BYPASS_ON	"plugins/reverb/bypass_button_down.png"
#define IMG_BYPASS_OFF	"plugins/reverb/bypass_button_up.png"
#define EFFECT_MIX	100.f

#define IMG_FL_HP		"plugins/filter/filter_hp_dwn.png"
#define IMG_FL_BP		"plugins/filter/filter_bp_dwn.png"
#define IMG_FL_LP		"plugins/filter/filter_lp_dwn.png"
//#define IMG_FL_NOTCH		"plugins/filter/filter_notch_dwn.png"
//#define IMG_FL_NOTCHBAR		"plugins/filter/filter_notchbar_dwn.png"
#define IMG_FL_HP_UP		"plugins/filter/filter_hp_up.png"
#define IMG_FL_BP_UP		"plugins/filter/filter_bp_up.png"
#define IMG_FL_LP_UP		"plugins/filter/filter_lp_up.png"
//#define IMG_FL_NOTCH_UP		"plugins/filter/filter_notch_up.png"
#define IMG_FL_NOTCHBAR_UP	"plugins/filter/filter_notchbar_up.png"
#define IMG_FL_KNOB_HP		"plugins/filter/filter_knob_hp.png"
#define IMG_FL_KNOB_BP		"plugins/filter/filter_knob_bp.png"
#define IMG_FL_KNOB_LP		"plugins/filter/filter_knob_lp.png"
//#define IMG_FL_KNOB_NOTCH	"plugins/filter/filter_knob_notch.png"

#define STR_REVERB_SELECTED "ReverbSelected"
#define STR_MIX_LEVEL "MixLevel"
#define STR_DECAY "Decay"

typedef struct s_param
{
  int		sel_rev;
  float		Mix;
  float		Decay;
}		t_param;

class ReverbPlugin: public Plugin
{
 public:
  ReverbPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~ReverbPlugin();

  void	 Init();

  void	 Process(float **input, float **output, long sample_length);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  void   Load(int fd, long size);
  long   Save(int fd);

  void	 Load(WiredPluginData& Datas);
  void	 Save(WiredPluginData& Datas);

  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Reverb"; }

  void OnSelect(wxCommandEvent &e);
  void OnASelect(wxCommandEvent &e);
  void OnBSelect(wxCommandEvent &e);
  void OnCSelect(wxCommandEvent &e);
  void OnDecay(wxScrollEvent &e);
  void OnMix(wxScrollEvent &e);
  void OnBypass(wxCommandEvent &e);
  void OnBypassController(wxMouseEvent &event);

  void OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();
  float		Wide;
  
 protected:
  bool		Bypass;

  int		MidiBypass[2];

  PRCRev	PRCreverb_stk;
  JCRev		JCreverb_stk;
  NRev		Nreverb_stk;
  int		rev_sel;
  
  t_param	param;

  wxBitmap	*bmp;   

  // reverb type selection stuff
  StaticPosKnob	*SelrevKnob;
  wxImage	*a_rev_on;
  wxImage	*a_rev_off;
  wxImage	*b_rev_on;
  wxImage	*b_rev_off;
  wxImage	*c_rev_on;
  wxImage	*c_rev_off;
  DownButton	*AReverbBtn;
  DownButton	*BReverbBtn;
  DownButton	*CReverbBtn;


  FaderCtrl	*DecayKnob;
  FaderCtrl	*MixKnob;
 
  wxImage	*img_fg;
  wxImage	*img_bg;
  wxBitmap	*TpBmp;

  wxImage	*bypass_on;
  wxImage	*bypass_off;
  wxImage	*liquid_on;
  wxImage	*liquid_off;

  StaticBitmap	*Liquid;
  DownButton	*BypassBtn;

  wxMutex	ReverbMutex;

  void CheckExistingControllerData(int MidiData[3]);
  
  DECLARE_EVENT_TABLE()  
};

enum
  {
    Reverb_Bypass = 1,
    Reverb_Select,
    Reverb_A,
    Reverb_B,
    Reverb_C,
    Reverb_Decay,
    Reverb_Mix
  };

#endif
