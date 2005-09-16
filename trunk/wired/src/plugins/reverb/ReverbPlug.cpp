// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#include "ReverbPlug.h"
#include "midi.h"

static PlugInitInfo info;

/******** ReverbPlugin Implementation *********/

BEGIN_EVENT_TABLE(ReverbPlugin, wxWindow)
  EVT_BUTTON(Reverb_Bypass, ReverbPlugin::OnBypass)
  //EVT_COMMAND_SCROLL(Reverb_Selrev, ReverbPlugin::OnSelrev)
  EVT_COMMAND_SCROLL(Reverb_Decay, ReverbPlugin::OnDecay)
  EVT_COMMAND_SCROLL(Reverb_Mix, ReverbPlugin::OnMix)

  EVT_BUTTON(Reverb_Select, ReverbPlugin::OnSelect)
  EVT_BUTTON(Reverb_A, ReverbPlugin::OnASelect)
  EVT_BUTTON(Reverb_B, ReverbPlugin::OnBSelect)
  EVT_BUTTON(Reverb_C, ReverbPlugin::OnCSelect)

  EVT_PAINT(ReverbPlugin::OnPaint)
END_EVENT_TABLE()

ReverbPlugin::ReverbPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo), Bypass(false)
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
  
  //reverb type selector button

  wxImage** imgs;

  imgs = new wxImage*[3];
  imgs[0] = new wxImage(_T(string(GetDataDir() + string(IMG_FL_KNOB_LP)).c_str()));
  imgs[1] = new wxImage(_T(string(GetDataDir() + string(IMG_FL_KNOB_BP)).c_str()));
  imgs[2] = new wxImage(_T(string(GetDataDir() + string(IMG_FL_KNOB_HP)).c_str()));
  // imgs[3] = new wxImage(_T(string(GetDataDir() + string(IMG_FL_KNOB_NOTCH)).c_str()));

  printf("a\n");
  a_rev_on = new wxImage(string(GetDataDir() + string(IMG_FL_HP)).c_str(), wxBITMAP_TYPE_PNG);
  a_rev_off = new wxImage(string(GetDataDir() + string(IMG_FL_HP_UP)).c_str(), wxBITMAP_TYPE_PNG);
  AReverbBtn = new DownButton(this, Reverb_A, wxPoint(70, 43),
			      wxSize(a_rev_off->GetWidth(), a_rev_off->GetHeight()), a_rev_off, a_rev_on);

  printf("b\n");
  b_rev_on = new wxImage(string(GetDataDir() + string(IMG_FL_LP)).c_str(), wxBITMAP_TYPE_PNG);
  b_rev_off = new wxImage(string(GetDataDir() + string(IMG_FL_LP_UP)).c_str(), wxBITMAP_TYPE_PNG);
  BReverbBtn = new DownButton(this, Reverb_B, wxPoint(81, 47),
			      wxSize(b_rev_off->GetWidth(), b_rev_off->GetHeight()), b_rev_off, b_rev_on);

  printf("c\n");
  c_rev_on = new wxImage(string(GetDataDir() + string(IMG_FL_BP)).c_str(), wxBITMAP_TYPE_PNG);
  c_rev_off = new wxImage(string(GetDataDir() + string(IMG_FL_BP_UP)).c_str(), wxBITMAP_TYPE_PNG);
  CReverbBtn = new DownButton(this, Reverb_C, wxPoint(92, 47),
			      wxSize(c_rev_off->GetWidth(), c_rev_off->GetHeight()), c_rev_off, c_rev_on);

  AReverbBtn->SetOn();
  SelrevKnob = new StaticPosKnob(this, Reverb_Select, 3, imgs, 15, 0, 2, 0, wxPoint(68, 7), wxDefaultSize);
  
  // bypass button's stuff

  liquid_on = new wxImage(string(GetDataDir() + string(IMG_LIQUID_ON)).c_str(),
			  wxBITMAP_TYPE_PNG);
  liquid_off = new wxImage(string(GetDataDir() + string(IMG_LIQUID_OFF)).c_str(), wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));

  // Knobs' background

  DecayKnob = 
    new FaderCtrl(this, Reverb_Decay, img_bg, img_fg, 0, 30, 3,
		  wxPoint(118, 12), wxSize(img_bg->GetWidth() - 3, 
					   img_bg->GetHeight()));
  MixKnob = new FaderCtrl(this, Reverb_Mix, img_bg, img_fg, 0, 100, 50,
			  wxPoint(153, 12), wxSize(img_bg->GetWidth() - 3, 
						   img_bg->GetHeight()));
  
  Connect(Reverb_Bypass, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&ReverbPlugin::OnBypassController);    
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
      JCreverb_stk.setT60(param.Decay);
      JCreverb_stk.setEffectMix(param.Mix);
      Nreverb_stk.setT60(param.Decay);
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

void ReverbPlugin::Load(WiredPluginData& Datas)
{
	char		*buffer;
	
	ReverbMutex.Lock();
	buffer = strdup(Datas.LoadValue(std::string(STR_REVERB_SELECTED)));
	if (buffer != NULL)
	{
		rev_sel = atoi(buffer);
		SelrevKnob->SetValue(rev_sel);
	}
	free(buffer);
	buffer = strdup(Datas.LoadValue(std::string(STR_MIX_LEVEL)));
	if (buffer != NULL)
	{
		float Mix = strtof(buffer, NULL);
		MixKnob->SetValue((int)(Mix * EFFECT_MIX));
		PRCreverb_stk.setEffectMix(Mix);
		JCreverb_stk.setEffectMix(Mix);
		Nreverb_stk.setEffectMix(Mix);
	}
	free(buffer);
	buffer = strdup(Datas.LoadValue(std::string(STR_DECAY)));
	if (buffer != NULL)
	{
		float Decay = strtof(buffer, NULL);
		DecayKnob->SetValue((int)Decay);
		PRCreverb_stk.setT60(Decay);
		JCreverb_stk.setT60(Decay);
		Nreverb_stk.setT60(Decay);
	}
	free(buffer);
	ReverbMutex.Unlock();
}

void ReverbPlugin::Save(WiredPluginData& Datas)
{
	std::ostringstream 	oss;

	oss << rev_sel;
	Datas.SaveValue(std::string(STR_REVERB_SELECTED), std::string(oss.str()));
	oss.seekp(ios_base::beg);
	oss << (MixKnob->GetValue() / EFFECT_MIX);
	Datas.SaveValue(std::string(STR_MIX_LEVEL), std::string(oss.str()));
	oss.seekp(ios_base::beg);
	oss << DecayKnob->GetValue();
	Datas.SaveValue(std::string(STR_DECAY), std::string(oss.str()));
	oss.seekp(ios_base::beg);
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

  if (!Bypass)
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
  else
    {
      memcpy(output[0], input[0], sample_length * sizeof(float));
      memcpy(output[1], input[1], sample_length * sizeof(float));
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


void ReverbPlugin::OnSelect(wxCommandEvent &e)
{
  int i;

  i = SelrevKnob->GetValue();
  cout << "rev no: " << SelrevKnob->GetValue() << endl;
  
  if (i == 0)
    {
      AReverbBtn->SetOn();
      BReverbBtn->SetOff();
      CReverbBtn->SetOff();
    }
  else if (i == 1)
    {
      AReverbBtn->SetOff();
      BReverbBtn->SetOn();
      CReverbBtn->SetOff();
    }
  else if (i == 2)
    {
      AReverbBtn->SetOff();
      BReverbBtn->SetOff();
      CReverbBtn->SetOn();
    }
}

void ReverbPlugin::OnASelect(wxCommandEvent &e)
{
  if (!AReverbBtn->GetOn())
    return ;
  BReverbBtn->SetOff();
  CReverbBtn->SetOff();
  // = 0;
  cout << "testA" << endl;
  SelrevKnob->SetValue(0);
  AReverbBtn->SetOn();
  
}

void ReverbPlugin::OnBSelect(wxCommandEvent &e)
{
  if (!BReverbBtn->GetOn())
    return ;
  AReverbBtn->SetOff();
  CReverbBtn->SetOff();
  cout << "testB" << endl;
  SelrevKnob->SetValue(1);
  BReverbBtn->SetOn();
}

void ReverbPlugin::OnCSelect(wxCommandEvent &e)
{
  if (!CReverbBtn->GetOn())
    return ;
  AReverbBtn->SetOff();
  BReverbBtn->SetOff();
  cout << "testC" << endl;
  SelrevKnob->SetValue(2);
  CReverbBtn->SetOn();
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
  Plugin::OnPaintEvent(event);
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








