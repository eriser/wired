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
      FaderIndex = right.FaderIndex;
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
    {
      tr_bg = 
	new wxImage(string(GetDataDir() + string(IMG_DL_SINGLE_BG)).c_str(), 
		    wxBITMAP_TYPE_PNG);
      TpBmp = new wxBitmap(tr_bg);
      delete tr_bg;
    }
  else
    {
      wxImage		*beg = new wxImage(string(GetDataDir() + string(IMG_DL_BEG_BG)).c_str());
      wxImage		*mid = new wxImage(string(GetDataDir() + string(IMG_DL_MID_BG)).c_str());
      wxImage		*end = new wxImage(string(GetDataDir() + string(IMG_DL_END_BG)).c_str());
      wxMemoryDC	dc;

      TpBmp = new wxBitmap(width, 100);
      dc.SelectObject(*TpBmp);

      wxBitmap *tmp1 = new wxBitmap(beg);
      dc.DrawBitmap(*tmp1, wxPoint(0, 0), false);
      wxBitmap *tmp2 = new wxBitmap(end);
      dc.DrawBitmap(*tmp2, wxPoint(width - 200, 0), false);
      wxBitmap *tmp = new wxBitmap(mid);

      for (int i = 200; i < width - 200; i += 200)
	dc.DrawBitmap(*tmp, wxPoint(i, 0), false);

      delete tmp;
      delete tmp1;
      delete tmp2;
      delete beg;
      delete mid;
      delete end;
    }

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
      if (LADSPA_IS_HINT_LOGARITHMIC(iter->second.Descriptor.RangeHint.HintDescriptor))
	{
	  //cout << "LOG" << endl;
	  Faders[i] = new KnobCtrl(this, i + 1, img_knob_bg, img_knob_fg, 
				   0, 100, 50, 10,
				   wxPoint(73 + i * interspace + interspace / 2 - 5, 11),
				   wxSize(img_knob_bg->GetWidth(), img_knob_bg->GetHeight()), this, GetPosition() + 
				   wxPoint(73 + i * interspace + interspace / 2 - 25, 35));
	}
      else
	{
	  // if LADSPA_IS_HINT_INTEGER
	 //  cout << hex << "LADSPA_IS_HINT_INTEGER " << iter->second.Descriptor.RangeHint.HintDescriptor  << endl;
// 	  cout << "Dans WiredExternalPluginGui :" << LADSPA_IS_HINT_INTEGER(iter->second.Descriptor.RangeHint.HintDescriptor) << endl;
	  Faders[i] = new FaderCtrl(this, i + 1, img_fader_bg, img_fader_fg,
				    (float)iter->second.Data.LowerBound, 
				    (float)iter->second.Data.UpperBound, 
				    (iter->second.Data.Data), (bool)LADSPA_IS_HINT_INTEGER(iter->second.Descriptor.RangeHint.HintDescriptor), 
				    wxPoint(73 + i * interspace + interspace / 2 - 5, 11),
				    wxSize(img_fader_bg->GetWidth(), 
					   img_fader_bg->GetHeight()), this,
				    GetPosition() + 
				    wxPoint(73 + i * interspace + interspace / 2 - 25, 35));
	  //Connect(i + 1, wxEVT_RIGHT_DOWN, /*(wxObjectEventFunction)(wxEventFunction) */
	  //  wxScrollEventHandler(WiredDSSIGui::OnFaderMove));
	  FaderIndex[i + 1] = iter->first;
	  // cout << "** " << iter->second.Data.LowerBound << "<" << *(iter->second.Data.Data) << "<" 
// 	       << iter->second.Data.UpperBound << endl;
	  //((HintedFader*)Faders[i])->SetValue((int)(*(iter->second.Data.Data) / (iter->second.Data.UpperBound - iter->second.Data.LowerBound) * 100));
	  //((FaderCtrl*)Faders[i])->SetValue((int)(*(iter->second.Data.Data)));
	  ((FaderCtrl*)Faders[i])->SetValue((*(iter->second.Data.Data)));
	  // gruik bis
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

// void		WiredDSSIGui::OnFaderMove(wxScrollEvent &e)
// {
//   int		id = e.GetId();

//   if (FaderIndex.find(id) != FaderIndex.end())
//     if (_GuiControls.find(FaderIndex[id]) != _GuiControls.end())
//       {
// 	float min = _GuiControls[FaderIndex[id]].Data.LowerBound;
// 	float max = _GuiControls[FaderIndex[id]].Data.UpperBound;
// 	float range = max - min;

// 	//cout << "* " << (*(_GuiControls[FaderIndex[id]].Data.Data)) * 100 << endl;
// 	*_GuiControls[FaderIndex[id]].Data.Data = ((FaderCtrl*)Faders[id - 1])->GetValue() / 100.f * range + min;
// 	cout << "Dans OnFaderMove : " << *_GuiControls[FaderIndex[id]].Data.Data << endl;
// 	//cout << (float)_GuiControls[FaderIndex[id]].Data.LowerBound << " < " <<  
// 	//Faders[id - 1]->GetValue() << " < " << (float)_GuiControls[FaderIndex[id]].Data.UpperBound << endl;
// 	//cout << "* " << (*(_GuiControls[FaderIndex[id]].Data.Data)) * 100 << endl;
// 	//cout << "dans le callback" << endl;
//       }
// }

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
