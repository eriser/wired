// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include "WiredExternalPluginGui.h"
#include <string>
#include <iostream>
#include <sstream>

BEGIN_EVENT_TABLE(WiredDSSIGui, wxWindow)
  //EVT_BUTTON(Delay_Bypass, DelayPlugin::OnBypass)
  //EVT_COMMAND_SCROLL(Delay_Time, DelayPlugin::OnDelayTime)
  //EVT_COMMAND_SCROLL(Delay_Feedback, DelayPlugin::OnFeedback)
  //EVT_COMMAND_SCROLL(Delay_DryWet, DelayPlugin::OnDryWet)
  EVT_PAINT(WiredDSSIGui::OnPaint)
END_EVENT_TABLE()

WiredDSSIGui::WiredDSSIGui(PlugStartInfo &startinfo) : WiredLADSPAInstance(startinfo)
{
  // wxPoint	pt(0, 30);
//   wxSize	sz(GetSize().GetWidth(), GetSize().GetHeight() - 30);

  //  cout << "CreateView Called" << endl;
  
  //cout << "******** CreateView processed" << endl;
}

WiredDSSIGui::~WiredDSSIGui()
{
  
}

void		WiredDSSIGui::SetInfo(PlugInitInfo *info)
{
  InitInfo = info;
  this->WiredLADSPAInstance::SetInfo(info);
}

void		WiredDSSIGui::SetInfo(PlugStartInfo *info)
{
  StartInfo = info;
}

wxWindow	*WiredDSSIGui::CreateView(wxWindow *rack, wxPoint &pos, wxSize &size)
{
  int		i;
  map<unsigned long, t_gui_control>::iterator iter;

  // [creation du rack]
  
  wxImage *tr_bg = new wxImage(string(GetDataDir() + string("dssi/dssi_wide_bg.png")).c_str(), wxBITMAP_TYPE_PNG);
  SetSize(-1, -1, 400, -1);
  //size = wxSize(200, 100);
  TpBmp = new wxBitmap(tr_bg);
  
#define IMG_DL_SINGLE_BG	"dssi/dssi_single_bg.png"
#define IMG_DL_MIDDLE_BG	"dssi/dssi_middle_bg.png"
#define IMG_DL_END_BG		"dssi/dssi_end_bg.png"
#define IMG_DL_BEGIN_BG		"dssi/dssi_begin_bg.png"

  img_bg = NULL;
  img_fg = NULL;
  img_bg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_BG)).c_str(), wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_FG)).c_str(), wxBITMAP_TYPE_PNG);

  GetDataDir();

  i = 0;
  Faders = (FaderCtrl**) new void*[_GuiControls.size()];
  for (iter = _GuiControls.begin(); iter != _GuiControls.end(); iter++)
    {
      wxSize(img_bg->GetWidth(), img_bg->GetHeight());
      Faders[i] = new FaderCtrl(this, i + 1, img_bg, img_fg, 0, 100, 50,
				wxPoint(i * 37 + 73, 11) , wxSize(img_bg->GetWidth(), img_bg->GetHeight()) );
      //cout << "size : " << img_bg->GetWidth() << img_bg->GetHeight() << endl;
      i++;
    }
  SetBackgroundColour(wxColour(237, 237, 237));
  return (NULL);
}

void		WiredDSSIGui::DestroyView()
{
  
}

bool		WiredDSSIGui::Load()
{
  wxPoint	pt(0, 30);
  wxSize	sz(GetSize().GetWidth(), GetSize().GetHeight() - 30);

  CreateView(this, pt, sz);
  return true;
}

// [implementation evenements]

void WiredDSSIGui::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*TpBmp);
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    

      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }
  Plugin::OnPaintEvent(event);
}
