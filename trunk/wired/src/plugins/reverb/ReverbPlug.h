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

#include "NRev.h"
#include "JCRev.h"
#include "PRCRev.h"

#define PLUGIN_NAME	"Reverb"

#define IMG_RV_BG	"plugins/reverb/reverb.png"
#define IMG_RV_BMP	"plugins/reverb/ReverbPlug.bmp"
#define IMG_RV_FADER_BG	"plugins/filter/fader_bg.png"
#define IMG_RV_FADER_FG	"plugins/filter/fader_fg.png"
#define IMG_RV_KNOB_BG	"plugins/reverb/knob_bg.png"
#define IMG_RV_KNOB_FG	"plugins/reverb/knob_fg.png"
#define IMG_LIQUID_ON	"plugins/delay/liquid-cristal_play.png"
#define IMG_LIQUID_OFF	"plugins/delay/liquid-cristal_stop.png"
#define IMG_BYPASS_ON	"plugins/delay/bypass_button_down.png"
#define IMG_BYPASS_OFF	"plugins/delay/bypass_button_up.png"
#define EFFECT_MIX	100.f

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

  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Reverb"; }

  void OnSelrev(wxScrollEvent &e);
  void OnDecay(wxScrollEvent &e);
  void OnMix(wxScrollEvent &e);  

  void OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();
  float		Wide;
  
 protected:
  PRCRev    PRCreverb_stk;
  JCRev     JCreverb_stk;
  NRev	    Nreverb_stk;
  int       rev_sel;
  
  t_param param;

  wxBitmap *bmp;   

  FaderCtrl *SelrevKnob;
  FaderCtrl *DecayKnob;
  FaderCtrl *MixKnob;
 
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;

  wxImage *bypass_on;
  wxImage *bypass_off;
  wxImage *liquid_on;
  wxImage *liquid_off;

  StaticBitmap *Liquid;
  DownButton *BypassBtn;

  DECLARE_EVENT_TABLE()  

};

enum
  {
    Reverb_Selrev = 1,
    Reverb_Decay,
    Reverb_Mix
  };

#endif
