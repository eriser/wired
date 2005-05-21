// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include "WiredExternalPluginGui.h"
//#include "WiredDSSIGui.h"
#include <string>
#include <iostream>
#include <sstream>

WiredDSSIGui::WiredDSSIGui()
{
  
}

WiredDSSIGui::~WiredDSSIGui()
{
  
}

void		WiredDSSIGui::SetInfo(PlugInitInfo *info)
{
  wxPoint	pt(0, 30);
  wxSize	sz(GetSize().GetWidth(), GetSize().GetHeight() - 30);

  this->WiredLADSPAInstance::SetInfo(info);
  cout << "CreateView Called" << endl;
  CreateView(this, pt, sz);
  cout << "******** CreateView processed" << endl;
}

wxWindow	*WiredDSSIGui::CreateView(wxWindow *rack, wxPoint &pos, wxSize &size)
{
  int		i;
  map<unsigned long, t_gui_control>::iterator iter;

  // [creation du rack]
  
  cout << "PWET" << endl;
  //cout << "data dir" << string(GetDataDir()) << endl;
  wxImage *tr_bg = new wxImage("/usr/local/share/wired/data/plugins/delay/delay_bg.png", wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  
  img_bg = NULL;
  img_fg = NULL;
  img_bg = new wxImage("/usr/local/share/wired/data/plugins/delay/fader_fg.png", wxBITMAP_TYPE_PNG );
  img_fg = new wxImage("/usr/local/share/wired/data/plugins/delay/fader_button.png", wxBITMAP_TYPE_PNG );

  i = 0;
  cout << "boucle for" << endl;
  for (iter = _GuiControls.begin(); iter != _GuiControls.end(); iter++)
    {

      iter->second;
      //cout << "value : " << iter->second.Data << endl;
      Faders[i] = new FaderCtrl(this, i, img_bg, img_fg, 0, 100, 50,
			       wxPoint(i * 40, 11), wxSize(img_bg->GetWidth(), img_bg->GetHeight()));
      i++;
    }
  cout << "number of controls : " << i << endl;
  return (NULL);
}

void		WiredDSSIGui::DestroyView()
{
  
}

// [implementation evenements]
