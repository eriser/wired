// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#include "ReverbPlug.h"
#include "midi.h"

static PlugInitInfo info;

/******** ReverbPlugin Implementation *********/

BEGIN_EVENT_TABLE(ReverbPlugin, wxWindow)
  EVT_BUTTON(Reverb_Bypass, ReverbPlugin::OnBypass)
  EVT_COMMAND_SCROLL(Reverb_Selrev, ReverbPlugin::OnSelrev)
  EVT_COMMAND_SCROLL(Reverb_Decay, ReverbPlugin::OnDecay)
  EVT_COMMAND_SCROLL(Reverb_Mix, ReverbPlugin::OnMix)
  EVT_PAINT(ReverbPlugin::OnPaint)
END_EVENT_TABLE()

ReverbPlugin::ReverbPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_RV_BG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);  
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_RV_BMP)).c_str(), 
		     wxBITMAP_TYPE_BMP); 
  img_bg = new wxImage(string(GetDataDir() + string(IMG_RV_FADER_BG)).c_str(),
		       wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(string(GetDataDir() + string(IMG_RV_FADER_FG)).c_str(),
		       wxBITMAP_TYPE_PNG);
  bypass_on = new wxImage(string(GetDataDir() + string(IMG_BYPASS_ON)).c_str(), 
			  wxBITMAP_TYPE_PNG);
  bypass_off = new wxImage(string(GetDataDir() + string(IMG_BYPASS_OFF)).c_str(), 
			   wxBITMAP_TYPE_PNG);
  BypassBtn = new DownButton(this, Reverb_Bypass, wxPoint(21, 58),
			     wxSize(bypass_on->GetWidth(), 
				    bypass_on->GetHeight()),
			     bypass_off, bypass_on);
  
  // bypass button's stuff

  cout << string(GetDataDir() + string(IMG_LIQUID_ON)).c_str() << endl;
  cout << string(GetDataDir() + string(IMG_LIQUID_OFF)).c_str() << endl;
  liquid_on = new wxImage(string(GetDataDir() + string(IMG_LIQUID_ON)).c_str(),
			  wxBITMAP_TYPE_PNG);
  liquid_off = new wxImage(string(GetDataDir() + string(IMG_LIQUID_OFF)).c_str(), wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));

  // Knobs' background

  SelrevKnob = 
    new FaderCtrl(this, Reverb_Selrev, img_bg, img_fg, 0, 2, 0,
		  wxPoint(73, 11), wxSize(img_bg->GetWidth() - 3, 
					  img_bg->GetHeight()));  
  DecayKnob = 
    new FaderCtrl(this, Reverb_Decay, img_bg, img_fg, 0, 30, 3,
		  wxPoint(110, 11), wxSize(img_bg->GetWidth() - 3, 
					   img_bg->GetHeight()));
  MixKnob = new FaderCtrl(this, Reverb_Mix, img_bg, img_fg, 0, 100, 50,
			  wxPoint(149, 11), wxSize(img_bg->GetWidth() - 3, 
						   img_bg->GetHeight()));
  
  Connect(Reverb_Bypass, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&ReverbPlugin::OnBypassController);    
  Connect(Reverb_Selrev, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&ReverbPlugin::OnSelrev); 
  Connect(Reverb_Decay, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&ReverbPlugin::OnDecay);    
  Connect(Reverb_Mix, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&ReverbPlugin::OnMix); 

  SetBackgroundColour(wxColour(237, 237, 237));
}

void ReverbPlugin::Load(int fd, long size)
{
  if (fd > 0)
    {
      if (read(fd, &param, size) <= 0)
	{
	  cout << "[REVERBPLUG] Error while loading patch !" << endl;
	  return;
	}
      MixKnob->SetValue((int)(param.Mix * EFFECT_MIX));
      DecayKnob->SetValue((int)param.Decay);
      SelrevKnob->SetValue(param.sel_rev);
      
      rev_sel = param.sel_rev;
      PRCreverb_stk.setT60(param.Decay);
      PRCreverb_stk.setEffectMix(param.Mix);
      JCreverb_stk.setT60(param.sel_rev);
      JCreverb_stk.setEffectMix(param.Mix);
      Nreverb_stk.setT60(param.sel_rev);
      Nreverb_stk.setEffectMix(param.Mix);
    }
}

long ReverbPlugin::Save(int fd)
{
  if (fd > 0)
    {
      param.sel_rev = rev_sel;
      param.Mix = (MixKnob->GetValue() / EFFECT_MIX);
      param.Decay = DecayKnob->GetValue();
      
      return (int)write(fd, &param, sizeof(t_param));
    }
  else
    return 0;
}

void ReverbPlugin::Init()
{
  PRCreverb_stk.setT60(3);
  PRCreverb_stk.setEffectMix(0.3f);
  JCreverb_stk.setT60(3);
  JCreverb_stk.setEffectMix(0.3f);
  Nreverb_stk.setT60(3);
  Nreverb_stk.setEffectMix(0.3f);
  rev_sel = 0;
}

void ReverbPlugin::Process(float **input, float **output, long sample_length)
{
  int	i;

  switch (rev_sel)
    {
    case 0 :
      for (i = 0; i <sample_length; i++)
	{
	  output[0][i] = PRCreverb_stk.tick(input[0][i]);
	  output[1][i] = PRCreverb_stk.tick(input[1][i]);
	}
      break;
    case 1 :
      for (i = 0; i <sample_length; i++)
	{
	  output[0][i] = JCreverb_stk.tick(input[0][i]);
	  output[1][i] = JCreverb_stk.tick(input[1][i]);
	}
      break;
    case 2 :
      for (i = 0; i <sample_length; i++)
	{
	  output[0][i] = Nreverb_stk.tick(input[0][i]);
	  output[1][i] = Nreverb_stk.tick(input[1][i]);
	}
      break;
    default :
      break;
    }
}

void ReverbPlugin::OnBypass(wxCommandEvent &e)
{
  ReverbMutex.Lock();
  Bypass = BypassBtn->GetOn();
  Liquid->SetBitmap(wxBitmap((Bypass) ? liquid_off : liquid_on));
  ReverbMutex.Unlock();
}

void ReverbPlugin::OnBypassController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      ReverbMutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiBypass[0] = midi_data[0];
      MidiBypass[1] = midi_data[1];

      ReverbMutex.Unlock();
    }
  delete midi_data;
}


ReverbPlugin::~ReverbPlugin()
{
  delete img_bg;
  delete img_fg;

  delete bypass_on;
  delete bypass_off;
  delete liquid_on;
  delete liquid_off;
}

bool ReverbPlugin::IsAudio()
{
  return (true);
}

bool ReverbPlugin::IsMidi()
{
  return (false);
}

wxBitmap *ReverbPlugin::GetBitmap()
{
  return (bmp);
}


void ReverbPlugin::OnSelrev(wxScrollEvent &e)
{
  rev_sel = SelrevKnob->GetValue();
  cout << "rev no: " << SelrevKnob->GetValue() << endl;
}

void ReverbPlugin::OnDecay(wxScrollEvent &e)
{
  PRCreverb_stk.setT60(DecayKnob->GetValue());
  JCreverb_stk.setT60(DecayKnob->GetValue());
  Nreverb_stk.setT60(DecayKnob->GetValue());
  cout << "Decay: " << DecayKnob->GetValue() << endl;
}

void ReverbPlugin::OnMix(wxScrollEvent &e)
{
  PRCreverb_stk.setEffectMix(MixKnob->GetValue() / EFFECT_MIX);
  JCreverb_stk.setEffectMix(MixKnob->GetValue() / EFFECT_MIX);
  Nreverb_stk.setEffectMix(MixKnob->GetValue() / EFFECT_MIX);
  cout << "Mix: " << MixKnob->GetValue() / EFFECT_MIX << endl;
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
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "REVERB");
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








