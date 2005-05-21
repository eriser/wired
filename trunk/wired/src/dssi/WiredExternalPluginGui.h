#ifndef __WIREDDSSI_H__
#define __WIREDDSSI_H__


#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include "WiredExternalPluginLADSPA.h"
#include "FaderCtrl.h"

class		WiredDSSIGui : public WiredLADSPAInstance
{
 public:
		WiredDSSIGui();
  		~WiredDSSIGui();
  void		DestroyView();
  wxWindow	*CreateView(wxWindow *rack, wxPoint &pos, wxSize &size);
  void		SetInfo(PlugInitInfo *info);

 protected:
  FaderCtrl	*Faders[];
  wxImage	*Background;
  wxBitmap	*TpBmp;
  wxImage	*img_bg;
  wxImage	*img_fg;
};


#endif	//__WIREDDSSI_H__
