// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#include <wx/wxprec.h>
#include <wx/event.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include "WiredExternalPluginGui.h"
#include "ladspa.h"
#include <string>
#include <iostream>
#include <sstream>

static PlugInitInfo info;

BEGIN_EVENT_TABLE(WiredDSSIGui, wxWindow)
  EVT_BUTTON(4242, WiredDSSIGui::OnBypass)
  //EVT_COMMAND_SCROLL(Delay_Time, DelayPlugin::OnDelayTime)
  EVT_COMMAND_SCROLL(1, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(2, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(3, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(4, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(5, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(6, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(7, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(8, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(9, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(10, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(11, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(12, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(13, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(14, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(15, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(16, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(17, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(18, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(19, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(20, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(21, WiredDSSIGui::OnFaderMove)
  EVT_COMMAND_SCROLL(22, WiredDSSIGui::OnFaderMove)
  EVT_PAINT(WiredDSSIGui::OnPaint)
END_EVENT_TABLE()

  WiredDSSIGui::WiredDSSIGui(PlugStartInfo &startinfo) : WiredLADSPAInstance(startinfo), Bypass(false)
{
  // wxPoint	pt(0, 30);
//   wxSize	sz(GetSize().GetWidth(), GetSize().GetHeight() - 30);

  //  cout << "CreateView Called" << endl;
  
  //cout << "******** CreateView processed" << endl;
}

WiredDSSIGui::~WiredDSSIGui()
{
  delete[] Faders;
}

WiredDSSIGui WiredDSSIGui::operator=(const WiredDSSIGui& right)
{
	if (this != &right)
	{
		(WiredLADSPAInstance)*this = (WiredLADSPAInstance) right;
		Bypass = right.Bypass;
		Faders = right.Faders;
		Background= right.Background;
		TpBmp = right.TpBmp;
		img_fader_bg = right.img_fader_bg;
		img_fader_fg = right.img_fader_fg;
		img_knob_bg = right.img_knob_bg;
		img_knob_fg = right.img_knob_fg;
		tr_bg = right.tr_bg;
		liquid_off = right.liquid_off;
		liquid_on = right.liquid_on;
		bypass_on = right.bypass_on;
		bypass_off = right.bypass_off;
		Liquid = right.Liquid;
		BypassBtn = right.BypassBtn;
		StartInfo = right.StartInfo;
		InitInfo = right.InitInfo;
	}
	return *this;
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
  wxImage *tr_bg = NULL;
  int		width;
  int		interspace;
  unsigned int NbCtrls = _GuiControls.size();
  
  if (NbCtrls == 0)
    NbCtrls = 1;
  width = (NbCtrls / 5 + (NbCtrls % 5) / 4 + 1) * 200;
  interspace = (width - 73 - 21) / NbCtrls;
  rack->SetSize(wxSize(width, 100));

  // gruik  
  if (_GuiControls.size() <= 3)
    tr_bg = 
      new wxImage(string(GetDataDir() + string(IMG_DL_SINGLE_BG)).c_str(), 
		  wxBITMAP_TYPE_PNG);
  else if (_GuiControls.size() < 9)
    tr_bg = new wxImage(string(GetDataDir() + string(IMG_DL_WIDE_BG)).c_str(),
			wxBITMAP_TYPE_PNG);
  else
    tr_bg = new wxImage(string(GetDataDir() + string(IMG_DL_VWIDE_BG)).c_str(),
			wxBITMAP_TYPE_PNG); 
  TpBmp = new wxBitmap(tr_bg);

  img_fader_bg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_FADER_BG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  img_fader_fg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_FADER_FG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  img_knob_bg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_KNOB_FG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  img_knob_fg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_KNOB_BG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  Faders = (wxWindow**) new void*[_GuiControls.size()];

  for (i = 0, iter = _GuiControls.begin(); iter != _GuiControls.end(); 
       iter++, i++)
    {
      //wxSize(img_fader_bg->GetWidth(), img_fader_bg->GetHeight());
      if (LADSPA_IS_HINT_LOGARITHMIC(iter->second.Descriptor.RangeHint.HintDescriptor))
	{
	  //cout << "LOG" << endl;
	  Faders[i] = new KnobCtrl(this, i + 1, img_knob_bg, img_knob_fg, 
				   0, 100, 50, 10,
				   wxPoint(73 + i * interspace + interspace / 2 - 5, 11),
				   wxSize(img_knob_bg->GetWidth(), img_knob_bg->GetHeight()), this, GetPosition() + 
				   wxPoint(73 + i * interspace + interspace / 2 - 25, 35));
	  //cout << "knob instancie" << endl;
	}
      else
	{
	  Faders[i] = new FaderCtrl(this, i + 1, 
				    img_fader_bg, img_fader_fg, 
				    iter->second.Data.LowerBound, 
				    iter->second.Data.UpperBound, 
				    *(iter->second.Data.Data),
				    wxPoint(73 + i * interspace + interspace / 2 - 5, 11),
				    wxSize(img_fader_bg->GetWidth(), 
					   img_fader_bg->GetHeight()), this,
				    GetPosition() + 
				    wxPoint(73 + i * interspace + interspace / 2 - 25, 35));
	  Connect(i + 1, wxEVT_RIGHT_DOWN, /*(wxObjectEventFunction)(wxEventFunction) */
		  wxScrollEventHandler(WiredDSSIGui::OnFaderMove));
	  FaderIndex[i + 1] = iter->first;
	  cout << "** " << iter->second.Data.LowerBound << "<" << *(iter->second.Data.Data) << "<" 
	     << iter->second.Data.UpperBound << endl;
	  //((HintedFader*)Faders[i])->SetValue((int)(*(iter->second.Data.Data) / (iter->second.Data.UpperBound - iter->second.Data.LowerBound) * 100));
	  ((FaderCtrl*)Faders[i])->SetValue((int)(*(iter->second.Data.Data)));
	  // gruik bis
	  if (i == 13)
	    break;
	}
    }
  
  // bypass
  
  liquid_off = new wxImage(string(GetDataDir() + string(IMG_LIQUID_OFF)).c_str(), wxBITMAP_TYPE_PNG);
  liquid_on = new wxImage(string(GetDataDir() + string(IMG_LIQUID_ON)).c_str(), wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));
  bypass_on = new wxImage(string(GetDataDir() + string(IMG_BYPASS_ON)).c_str(), wxBITMAP_TYPE_PNG);
  bypass_off = new wxImage(string(GetDataDir() + string(IMG_BYPASS_OFF)).c_str(), wxBITMAP_TYPE_PNG);
  BypassBtn = new DownButton(this, 4242, wxPoint(21, 58), wxSize(bypass_on->GetWidth(), bypass_on->GetHeight()),
			     bypass_off, bypass_on);
  Connect(4242, wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredDSSIGui::OnBypassController);
  
  SetBackgroundColour(wxColour(237, 237, 237));
  return (NULL);
}

void		WiredDSSIGui::OnBypass(wxCommandEvent &e) 
{
  Bypass = BypassBtn->GetOn();
  Liquid->SetBitmap(wxBitmap((Bypass) ? liquid_off : liquid_on));
  this->WiredLADSPAInstance::Bypass();
}

void		WiredDSSIGui::OnBypassController(wxMouseEvent &e)
{
  //int *midi_data;

  //midi_data = new int[3];
  //_Bypass();
}

void		WiredDSSIGui::OnFaderMove(wxScrollEvent &e)
{
  int		id = e.GetId();

  if (FaderIndex.find(id) != FaderIndex.end())
    if (_GuiControls.find(FaderIndex[id]) != _GuiControls.end())
      {
	float min = _GuiControls[FaderIndex[id]].Data.LowerBound;
	float max = _GuiControls[FaderIndex[id]].Data.UpperBound;
	float range = max - min;

	//cout << "* " << (*(_GuiControls[FaderIndex[id]].Data.Data)) * 100 << endl;
	*_GuiControls[FaderIndex[id]].Data.Data = ((FaderCtrl*)Faders[id - 1])->GetValue() / 100.f * range + min;	  
	//cout << (float)_GuiControls[FaderIndex[id]].Data.LowerBound << " < " <<  
	//Faders[id - 1]->GetValue() << " < " << (float)_GuiControls[FaderIndex[id]].Data.UpperBound << endl;
	//cout << "* " << (*(_GuiControls[FaderIndex[id]].Data.Data)) * 100 << endl;
      }
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

void		WiredDSSIGui::OnPaint(wxPaintEvent &event)
{
  int		i;
  int		width;
  int		interspace;
  map<unsigned long, t_gui_control>::iterator iter;
  wxMemoryDC	memDC;
  wxPaintDC	dc(this);
  
  wxFont lblfont(5, wxDECORATIVE, wxFONTFLAG_BOLD, wxBOLD, false, "");
  dc.SetFont(lblfont);  
  memDC.SelectObject(*TpBmp);
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), 
	      &memDC, upd.GetX(), upd.GetY(), wxCOPY, FALSE);      
      upd++;
    }
  width = (_GuiControls.size() / 5 + (_GuiControls.size() % 5) / 4 + 1) * 200;
  
  interspace = (width - 73 - 21) / _GuiControls.size();
  for (i = 0, iter = _GuiControls.begin(); iter != _GuiControls.end(); iter++, i++)
    dc.DrawRotatedText(iter->second.Descriptor.Name, 73 + i * interspace + interspace / 2 - 13 , 80, 90);
  Plugin::OnPaintEvent(event);
}
