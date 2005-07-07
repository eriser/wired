#ifndef __WIREDDSSI_H__
#define __WIREDDSSI_H__


#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include "Plugin.h"
#include "WiredExternalPluginLADSPA.h"
#include "FaderCtrl.h"
#include "DownButton.h"

#define IMG_DL_SINGLE_BG	"dssi/dssi_single_bg.png"
#define IMG_DL_MIDDLE_BG	"dssi/dssi_middle_bg.png"
#define IMG_DL_END_BG		"dssi/dssi_end_bg.png"
#define IMG_DL_WIDE_BG		"dssi/dssi_wide_bg.png"
#define IMG_DL_VWIDE_BG		"dssi/dssi_very_wide_bg.png"
#define IMG_DL_BEGIN_BG		"dssi/dssi_begin_bg.png"
#define IMG_DL_BMP		"dssi/DelayPlug.bmp"
#define IMG_DL_FADER_BG		"dssi/fader_bg.png"
#define IMG_DL_FADER_FG		"dssi/fader_button.png"
#define IMG_LIQUID_ON		"dssi/liquid-cristal_play.png"
#define IMG_LIQUID_OFF		"dssi/liquid-cristal_stop.png"
#define IMG_BYPASS_ON		"dssi/bypass_button_down.png"
#define IMG_BYPASS_OFF		"dssi/bypass_button_up.png"

class				WiredDSSIGui : public WiredLADSPAInstance
{
 public:
		WiredDSSIGui(PlugStartInfo &startinfo);
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
  FaderCtrl	**Faders;
  wxImage	*Background;
  wxBitmap	*TpBmp;
  wxImage	*img_bg;
  wxImage	*img_fg;
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
