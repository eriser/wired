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

#define PLUGIN_NAME	wxT("Reverb")

#define IMG_RV_BG	wxT("plugins/reverb/reverb_bg.png")
#define IMG_RV_BMP	wxT("plugins/reverb/ReverbPlug.bmp")
#define IMG_RV_FADER_BG	wxT("plugins/reverb/fader_bg.png")
#define IMG_RV_FADER_FG	wxT("plugins/reverb/fader_button.png")
#define IMG_RV_KNOB_BG	wxT("plugins/reverb/knob_bg.png")
#define IMG_RV_KNOB_FG	wxT("plugins/reverb/knob_fg.png")
#define IMG_LIQUID_ON	wxT("plugins/reverb/liquid-cristal_play.png")
#define IMG_LIQUID_OFF	wxT("plugins/reverb/liquid-cristal_stop.png")
#define IMG_BYPASS_ON	wxT("plugins/reverb/bypass_button_down.png")
#define IMG_BYPASS_OFF	wxT("plugins/reverb/bypass_button_up.png")
#define EFFECT_MIX	100.f

#define IMG_RV_PRC		wxT("plugins/reverb/reverb_prc_dwn.png")
#define IMG_RV_N		wxT("plugins/reverb/reverb_n_dwn.png")
#define IMG_RV_JC		wxT("plugins/reverb/reverb_jc_dwn.png")

#define IMG_RV_PRC_UP		wxT("plugins/reverb/reverb_prc_up.png")
#define IMG_RV_N_UP		wxT("plugins/reverb/reverb_n_up.png")
#define IMG_RV_JC_UP		wxT("plugins/reverb/reverb_jc_up.png")

#define IMG_RV_KNOB_PRC		wxT("plugins/reverb/reverb_knob_prc.png")
#define IMG_RV_KNOB_N		wxT("plugins/reverb/reverb_knob_n.png")
#define IMG_RV_KNOB_JC		wxT("plugins/reverb/reverb_knob_jc.png")


#define STR_REVERB_SELECTED	_("ReverbSelected")
#define STR_MIX_LEVEL	_("MixLevel")
#define STR_DECAY _("Decay")

typedef struct	s_param
{
  int		sel_rev;
  float		Mix;
  float		Decay;
}		t_param;

class		ReverbPlugin: public Plugin
{
 public:
  ReverbPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~ReverbPlugin();

  void		Init();

  void		Process(float **input, float **output, long sample_length);
  void		CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  void		Load(int fd, long size);
  long		Save(int fd);

  void		Load(WiredPluginData& Datas);
  void		Save(WiredPluginData& Datas);

  bool		IsAudio();
  bool		IsMidi();

  wxString	DefaultName() { return wxT("Reverb"); }

  void		OnSelect(wxCommandEvent &e);
  void		OnASelect(wxCommandEvent &e);
  void		OnBSelect(wxCommandEvent &e);
  void		OnCSelect(wxCommandEvent &e);
  void		OnDecay(wxScrollEvent &e);
  void		OnMix(wxScrollEvent &e);
  void		OnBypass(wxCommandEvent &e);
  void		OnBypassController(wxMouseEvent &event);

  void		OnPaint(wxPaintEvent &event);

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
