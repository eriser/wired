#ifndef __REVERBPLUG_H__
#define __REVERBPLUG_H__

#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "KnobCtrl.h"

#include "revmodel.hpp"

#define PLUGIN_NAME	"Reverb"

#define IMG_RV_BG	"plugins/reverb/reverb.png"
#define IMG_RV_BMP	"plugins/reverb/ReverbPlug.bmp"
#define IMG_RV_KNOB_BG	"plugins/reverb/knob_bg.png"
#define IMG_RV_KNOB_FG	"plugins/reverb/knob_fg.png"

class ReverbPlugin: public Plugin
{
 public:
  ReverbPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~ReverbPlugin();

  void	 Init();

  void	 Process(float **input, float **output, long sample_length);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  
  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Reverb"; }

  void OnRoomSize(wxScrollEvent &e);  
  void OnDamp(wxScrollEvent &e);  
  void OnDryWet(wxScrollEvent &e);  

  void OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();
  float		Wide;
  
 protected:
  revmodel	model;

  wxBitmap *bmp;   

  KnobCtrl *RoomSizeKnob;
  KnobCtrl *DampKnob;
  KnobCtrl *DryWetKnob;

  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;

  DECLARE_EVENT_TABLE()  

};

enum
  {
    Reverb_RoomSize = 1,
    Reverb_Damp,
    Reverb_DryWet
  };

#endif
