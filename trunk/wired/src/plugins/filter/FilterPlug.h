#ifndef __FILTERPLUG_H__
#define __FILTERPLUG_H__

#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"
#include "DownButton.h"
#include "StaticPosKnob.h"

#define PLUGIN_NAME		"Multi-Filter"

#define IMG_FL_BG		"plugins/filter/filter_bg.png"
#define IMG_FL_BMP		"plugins/filter/FilterPlug.bmp"
#define IMG_FL_FADER_BG		"plugins/filter/fader_bg.png"
#define IMG_FL_FADER_FG		"plugins/filter/fader_button.png"

#define IMG_FL_HP		"plugins/filter/filter_hp_dwn.png"
#define IMG_FL_BP		"plugins/filter/filter_bp_dwn.png"
#define IMG_FL_LP		"plugins/filter/filter_lp_dwn.png"
#define IMG_FL_NOTCH		"plugins/filter/filter_notch_dwn.png"
#define IMG_FL_NOTCHBAR		"plugins/filter/filter_notchbar_dwn.png"
#define IMG_FL_HP_UP		"plugins/filter/filter_hp_up.png"
#define IMG_FL_BP_UP		"plugins/filter/filter_bp_up.png"
#define IMG_FL_LP_UP		"plugins/filter/filter_lp_up.png"
#define IMG_FL_NOTCH_UP		"plugins/filter/filter_notch_up.png"
#define IMG_FL_NOTCHBAR_UP	"plugins/filter/filter_notchbar_up.png"
#define IMG_FL_KNOB_HP		"plugins/filter/filter_knob_hp.png"
#define IMG_FL_KNOB_BP		"plugins/filter/filter_knob_bp.png"
#define IMG_FL_KNOB_LP		"plugins/filter/filter_knob_lp.png"
#define IMG_FL_KNOB_NOTCH	"plugins/filter/filter_knob_notch.png"

#define IMG_LIQUID_ON		"plugins/filter/liquid-cristal_play.png"
#define IMG_LIQUID_OFF		"plugins/filter/liquid-cristal_stop.png"
#define IMG_BYPASS_ON		"plugins/filter/bypass_button_down.png"
#define IMG_BYPASS_OFF		"plugins/filter/bypass_button_up.png"

#define SIZE_CUTOFF		2205
#define SIZE_RES		100

#define FILTER_SIZE		5		// 3 inputs, 2 outpus

#define STR_FILTER _("Filter")
#define STR_CUTOFF _("Cutoff")
#define STR_RESONANCE _("Resonance")
#define STR_BYPASS _("Bypass")
#define STR_MIDI_BYPASS1 _("MIDIBypass1")
#define STR_MIDI_CUTOFF1 _("MIDICutoff1")
#define STR_MIDI_RESONANCE1 _("MIDIResonance1")
#define STR_MIDI_BYPASS2 _("MIDIBypass2")
#define STR_MIDI_CUTOFF2 _("MIDICutoff2")
#define STR_MIDI_RESONANCE2 _("MIDIResonance2")

static PlugInitInfo info;

class FilterPlugin: public Plugin
{
 public:
  FilterPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~FilterPlugin();

  void	 Init();
  void	 Process(float **input, float **output, long sample_length);
  void	 ProcessEvent(WiredEvent &event);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  void	 Load(int fd, long size);
  long   Save(int fd);
  
  void	 Load(WiredPluginData& Datas);
  void	 Save(WiredPluginData& Datas);
  
  void  SetSamplingRate(double rate)
    { 
      SamplePeriod = 1.f / rate;
    }
  
  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Filter"; }

  void Update();

  void SetFilter(int type, float cutoff, float resonance);
  void OnSelect(wxCommandEvent &e);  

  void OnLPSelect(wxCommandEvent &e);
  void OnBPSelect(wxCommandEvent &e);
  void OnHPSelect(wxCommandEvent &e);
  void OnNotchSelect(wxCommandEvent &e);
  void OnNotchBarSelect(wxCommandEvent &e);
  void OnBypass(wxCommandEvent &e);
  void OnButtonClick(wxCommandEvent &e); 
  void OnCutoff(wxScrollEvent &e);  
  void OnResonance(wxScrollEvent &e);  
  void OnPaint(wxPaintEvent &event);


  wxBitmap	*GetBitmap();

  float		Cutoff;
  float		Res;
  bool		Bypass;
  
 protected:
  wxMutex	Mutex;

  wxBitmap *bmp;   

  FaderCtrl *CutoffFader;
  FaderCtrl *ResFader;
  wxImage *img_fg;
  wxImage *img_bg;

  wxImage *hp_on;
  wxImage *hp_off;
  wxImage *bp_on;
  wxImage *bp_off;
  wxImage *lp_on;
  wxImage *lp_off;
  wxImage *notch_on;
  wxImage *notch_off;
  wxImage *notchbar_on;
  wxImage *notchbar_off;

  wxBitmap *TpBmp;

  wxImage *bypass_on;
  wxImage *bypass_off;
  wxImage *liquid_on;;
  wxImage *liquid_off;;

  StaticBitmap *Liquid;
  DownButton *BypassBtn;

  DownButton *HpBtn;
  DownButton *LpBtn;
  DownButton *BpBtn;
  DownButton *NotchBarBtn;
  DownButton *NotchBtn;

  StaticPosKnob  *FilterSelect;

  DECLARE_EVENT_TABLE()  

  float	History[2][FILTER_SIZE];
  float	Coefs[FILTER_SIZE];
  float	Reamp;
  float SamplePeriod;

  void	 ApplyFilter();

  void   SetCoeffs(double b0, double b1, double b2, double a0, double a1, double a2);

  void	 OnBypassController(wxMouseEvent &event);
  void	 OnCutoffController(wxMouseEvent &event);
  void	 OnResController(wxMouseEvent &event);

  int		MidiBypass[2];
  int		MidiCutoff[2];
  int		MidiRes[2];

  bool		UpdateBypass;
  bool		UpdateCutoff;
  bool		UpdateRes;

  void CheckExistingControllerData(int MidiData[3]);
};

enum
  {
    Filter_Cutoff = 1,
    Filter_Res,
    Filter_Select,
    Filter_Bypass,
    Filter_HP,
    Filter_BP,
    Filter_LP,
    Filter_Notch,
    Filter_NotchBar
  };

enum
  {
    filter_lp = 0,
    filter_bp,
    filter_hp,
    filter_notch,
    filter_peq,
    
    filter_count
  };

#endif
