#include "ReverbPlug.h"

static PlugInitInfo info;

/******** ReverbPlugin Implementation *********/

BEGIN_EVENT_TABLE(ReverbPlugin, wxWindow)
  EVT_COMMAND_SCROLL(Reverb_RoomSize, ReverbPlugin::OnRoomSize)
  EVT_COMMAND_SCROLL(Reverb_Damp, ReverbPlugin::OnDamp)
  EVT_COMMAND_SCROLL(Reverb_DryWet, ReverbPlugin::OnDryWet)
  EVT_PAINT(ReverbPlugin::OnPaint)
END_EVENT_TABLE()

ReverbPlugin::ReverbPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_RV_BG)).c_str(), wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_RV_BMP)).c_str(), wxBITMAP_TYPE_BMP); 

  img_bg = new wxImage(string(GetDataDir() + string(IMG_RV_KNOB_BG)).c_str(),wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(string(GetDataDir() + string(IMG_RV_KNOB_FG)).c_str(),wxBITMAP_TYPE_PNG);
  
  RoomSizeKnob = new KnobCtrl(this, Reverb_RoomSize, img_bg, img_fg, 0, 100, 50, 1,
			      wxPoint(18, 8), wxSize(23, 23));
  DampKnob = new KnobCtrl(this, Reverb_Damp, img_bg, img_fg, 0, 100, 50, 1,
			  wxPoint(50, 8), wxSize(23, 23));
  DryWetKnob = new KnobCtrl(this, Reverb_DryWet, img_bg, img_fg, 0, 100, 50, 1,
			    wxPoint(80, 8), wxSize(23, 23));
  
  SetBackgroundColour(wxColour(237, 237, 237));
}


void ReverbPlugin::Init()
{
  model.setdamp(0.f);
  model.setdry(0.5f);
  model.setwet(1.0f);
  model.setroomsize(1.0f);
  model.setwidth(0.f);
}

void ReverbPlugin::Process(float **input, float **output, long sample_length)
{
  model.processmix(input[0], input[1], output[0], output[1], sample_length, 1);
}

ReverbPlugin::~ReverbPlugin()
{

}

bool ReverbPlugin::IsAudio()
{
  return (true);
}

bool ReverbPlugin::IsMidi()
{
  return (true);
}

wxBitmap *ReverbPlugin::GetBitmap()
{
  return (bmp);
}

void ReverbPlugin::OnRoomSize(wxScrollEvent &e)
{
  model.setroomsize(RoomSizeKnob->GetValue() / 100.f);
  cout << "room size: " << RoomSizeKnob->GetValue() / 100.f << endl;
}

void ReverbPlugin::OnDamp(wxScrollEvent &e)
{
  model.setdamp(DampKnob->GetValue() / 100.f);
  cout << "damp: " << DampKnob->GetValue() / 100.f << endl;
}

void ReverbPlugin::OnDryWet(wxScrollEvent &e)
{
  float d, w;

  w = DryWetKnob->GetValue() / 100.f;
  d = 1.f - w;
  model.setdry(d);
  model.setwet(w);
  cout << "dry: " << d << "; wet: " << w << endl;
}

void ReverbPlugin::OnPaint(wxPaintEvent &event)
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

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "VERB");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new ReverbPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








