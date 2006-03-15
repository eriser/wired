#ifndef __WIREDDSSI_H__
#define __WIREDDSSI_H__


#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include "Plugin.h"
#include "WiredExternalPluginLADSPA.h"
#include "FaderCtrl.h"
#include "KnobCtrl.h"
#include "DownButton.h"

#define IMG_DL_SINGLE_BG	wxT("dssi/dssi_single_bg.png")
#define IMG_DL_MID_BG		wxT("dssi/dssi_middle_bg.png")
#define IMG_DL_END_BG		wxT("dssi/dssi_end_bg.png")
#define IMG_DL_WIDE_BG		wxT("dssi/dssi_wide_bg.png")
#define IMG_DL_VWIDE_BG		wxT("dssi/dssi_very_wide_bg.png")
#define IMG_DL_BEG_BG		wxT("dssi/dssi_begin_bg.png")
#define IMG_DL_BMP		wxT("dssi/DelayPlug.bmp")
#define IMG_DL_FADER_BG		wxT("dssi/fader_bg.png")
#define IMG_DL_FADER_FG		wxT("dssi/fader_button.png")
#define IMG_DL_KNOB_FG		wxT("dssi/knob_fg.png")
#define IMG_DL_KNOB_BG		wxT("dssi/knob_bg.png")
#define IMG_LIQUID_ON		wxT("dssi/liquid-cristal_play.png")
#define IMG_LIQUID_OFF		wxT("dssi/liquid-cristal_stop.png")
#define IMG_BYPASS_ON		wxT("dssi/bypass_button_down.png")
#define IMG_BYPASS_OFF		wxT("dssi/bypass_button_up.png")

class		WiredDSSIGui : public WiredLADSPAInstance
{
 public:
  WiredDSSIGui(PlugStartInfo &startinfo);
  WiredDSSIGui(const WiredDSSIGui& copy) {*this = copy;}
  WiredDSSIGui operator=(const WiredDSSIGui& right);
  ~WiredDSSIGui();
  
  void		DestroyView();
  wxWindow	*CreateView(wxWindow *rack, wxPoint &pos, wxSize &size);
  void		SetInfo(PlugInitInfo *info);
  void		SetInfo(PlugStartInfo *info);
  bool		Load();
  void		OnPaint(wxPaintEvent &event);
  void		OnFaderMove(wxScrollEvent &WXUNUSED(e));
  void		OnBypass(wxCommandEvent &e);
  void		OnBypassController(wxMouseEvent &event);
  
 protected:
  bool		Bypass;
  wxWindow	**Faders;
  wxImage	*Background;
  wxBitmap	*TpBmp;
  wxImage	*img_fader_bg;
  wxImage	*img_fader_fg;
  wxImage	*img_knob_bg;
  wxImage	*img_knob_fg;
  wxImage	*tr_bg;
  wxImage	*liquid_off;
  wxImage	*liquid_on;
  wxImage	*bypass_on;
  wxImage	*bypass_off;
  StaticBitmap	*Liquid;
  DownButton	*BypassBtn;
  PlugStartInfo *StartInfo;
  PlugInitInfo	*InitInfo;

  map <int, unsigned long> FaderIndex;

  DECLARE_EVENT_TABLE();
};



#endif	//__WIREDDSSI_H__
