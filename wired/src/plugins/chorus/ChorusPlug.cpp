// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License


#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"
#include "DownButton.h"
#include "WvIn.h"
#include "WaveLoop.h"
#include "Effect.h"
#include "Delay.h"
#include "DelayL.h"
#include "Chorus.h"
#include "ChorusPlug.h"

#include <iostream>
#include <sstream>
#include <string>

ChorusPlugin::ChorusPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo), Bypass(false)
{
  BufStart[0] = 0x0;
  BufStart[1] = 0x0;
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_BG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_DL_BMP)).c_str(), 
		     wxBITMAP_TYPE_BMP); 
  img_bg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_BG)).c_str(),
		       wxBITMAP_TYPE_PNG );
  img_fg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_FG)).c_str(),
		       wxBITMAP_TYPE_PNG );
  bypass_on = new wxImage(string(GetDataDir() + string(IMG_BYPASS_ON)).c_str(),
			  wxBITMAP_TYPE_PNG);
  bypass_off = 
    new wxImage(string(GetDataDir() + string(IMG_BYPASS_OFF)).c_str(), 
		wxBITMAP_TYPE_PNG);
  BypassBtn = 
    new DownButton(this, Chorus_Bypass, wxPoint(21, 58), 
		   wxSize(bypass_on->GetWidth(), bypass_on->GetHeight()), 
		   bypass_off, bypass_on);

  //bypass button's stuff

  liquid_on = new wxImage(string(GetDataDir() + string(IMG_LIQUID_ON)).c_str(),
			  wxBITMAP_TYPE_PNG);
  liquid_off = 
    new wxImage(string(GetDataDir() + string(IMG_LIQUID_OFF)).c_str(), 
		wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));
  BaseLengthFader = new FaderCtrl(this, Chorus_Feedback, img_bg, img_fg, 0, 
				  10000, (float*)&BaseLength, true, wxPoint(73, 11), 
				  wxSize(22,78), this, 
				  GetPosition() + wxPoint(58, 25));
  ModDepthFader = new FaderCtrl(this, Chorus_Stage, img_bg, img_fg, 0, 10, 
				(float*)&ModDepth, true, wxPoint(110, 11), wxSize(22,78), 
				this, GetPosition() + wxPoint(95, 25));
  EffectMixFader = new FaderCtrl(this, Chorus_DryWet, img_bg, img_fg, 0, 100, 
				 (float*)&EffectMix, true, wxPoint(149, 11), wxSize(22, 78), 
				 this, GetPosition() + wxPoint(135, 25));  

  cout << "BaseLength : " << BaseLength << endl;
  cout << "ModDepth : " << ModDepth << endl;
  cout << "Frequency : " << Frequency << endl;
  cout << "EffectMix : " << EffectMix << endl;

  SetBackgroundColour(wxColour(237, 237, 237));
}

ChorusPlugin::~ChorusPlugin()
{
  delete chorus1;
  delete chorus2;
  delete TpBmp;
  delete bmp;
  delete img_bg;
  delete img_fg;
}

void ChorusPlugin::Init()
{
  ChorusMutex.Lock();

  BaseLength = 10000;
  ModDepth = 0.03;
  Frequency = 0.003;
  EffectMix = 0.5;
  
  Stk::setSampleRate( 44100.0 );

  chorus1 = new Chorus(BaseLength, GetDataDir());
  chorus1->setModDepth(ModDepth);
  chorus1->setModFrequency(Frequency);
  chorus1->setEffectMix(EffectMix);

  chorus2 = new Chorus(BaseLength, GetDataDir());
  chorus2->setModDepth(ModDepth);
  chorus2->setModFrequency(Frequency);
  chorus2->setEffectMix(EffectMix);

  
  ChorusMutex.Unlock();
}

void ChorusPlugin::AllocateMem()
{
  ;
}

void ChorusPlugin::Process(float **input, float **output, long sample_length)
{
  long c1;

  ChorusMutex.Lock();

  if (!Bypass)
    for (c1 = 0; c1 < sample_length; c1++)
      {
	output[0][c1] = chorus1->tick(input[0][c1]);
	output[1][c1] = chorus2->tick(input[1][c1]);
      }
  else
    {
      memcpy(output[0], input[0], sample_length * sizeof(float));
      memcpy(output[1], input[1], sample_length * sizeof(float));
    }
  ChorusMutex.Unlock();
}

void ChorusPlugin::OnBypass(wxCommandEvent &e)
{
  ChorusMutex.Lock();
  Bypass = BypassBtn->GetOn();
  Liquid->SetBitmap(wxBitmap((Bypass) ? liquid_off : liquid_on));
  ChorusMutex.Unlock();
}

void ChorusPlugin::Load(int fd, long size)
{
  if (read(fd, &BaseLength, sizeof (BaseLength)) <= 0)
    {
      cout << "[CHORUSPLUG] Error while loading patch !" << endl;
      return;
    }
  if (read(fd, &ModDepth, sizeof (ModDepth)) <= 0)
    {
      cout << "[CHORUSPLUG] Error while loading patch !" << endl;
      return;
    }

  if (read(fd, &Frequency, sizeof (Frequency)) <= 0)
     {
      cout << "[CHORUSPLUG] Error while loading patch !" << endl;
      return;
    }

  if (read(fd, &EffectMix, sizeof (EffectMix)) <= 0)
    {
      cout << "[CHORUSPLUG] Error while loading patch !" << endl;
      return;
    }

  BaseLengthFader->SetValue(int(BaseLength));
  ModDepthFader->SetValue(int(ModDepth * 100));
  FrequencyFader->SetValue(int(Frequency * 1000));
  EffectMixFader->SetValue(int(100 - (EffectMix * 100)));


  chorus1->setBaseLength(BaseLength);
  chorus1->setModDepth(ModDepth);
  chorus1->setModFrequency(Frequency);
  chorus1->setEffectMix(EffectMix);

  chorus2->setBaseLength(BaseLength);
  chorus2->setModDepth(ModDepth);
  chorus2->setModFrequency(Frequency);
  chorus2->setEffectMix(EffectMix);
  

}

void ChorusPlugin::Load(WiredPluginData& Datas)
{
	char	*buffer;
	
	buffer = strdup(Datas.LoadValue(std::string(STR_BASE_LENGHT)));
	if (buffer != NULL)
	  {
	    BaseLength = atof(buffer);
	    BaseLengthFader->SetValue(int(BaseLength));
	    chorus1->setBaseLength(BaseLength);
	    chorus2->setBaseLength(BaseLength);
	  }
	free(buffer);
	buffer = strdup(Datas.LoadValue(std::string(STR_MODE_DEPTH)));
	if (buffer != NULL)
	  {
	    ModDepth = atof(buffer);
	    ModDepthFader->SetValue(int(ModDepth * 100)); 
	  chorus1->setModDepth(ModDepth);
	  chorus2->setModDepth(ModDepth);
	  }
	free(buffer);
	buffer = strdup(Datas.LoadValue(std::string(STR_FREQUENCY)));
	if (buffer != NULL)
	  {
	    Frequency = atof(buffer);
	    FrequencyFader->SetValue(int(Frequency * 1000));
	    chorus1->setModFrequency(Frequency);
	    chorus2->setModFrequency(Frequency);
	  }
	free(buffer);
	buffer = strdup(Datas.LoadValue(std::string(STR_EFFECT_MIX)));
	if (buffer != NULL)
	  {
	    EffectMix = atof(buffer);
	    EffectMixFader->SetValue(int(100 - (EffectMix * 100)));
	    chorus1->setEffectMix(EffectMix);  
	    chorus2->setEffectMix(EffectMix);
	  }
	free(buffer);
}

long ChorusPlugin::Save(int fd)
{
  long size;
  
  size = write(fd, &BaseLength, sizeof (BaseLength));
  size += write(fd, &ModDepth, sizeof (ModDepth));
  size += write(fd, &Frequency, sizeof (Frequency));
  size += write(fd, &EffectMix, sizeof (EffectMix));
  
  return (size);
}

void ChorusPlugin::Save(WiredPluginData& Datas)
{
  std::ostringstream 	oss;

  oss << BaseLength;
  Datas.SaveValue(std::string(STR_BASE_LENGHT), std::string(oss.str()));
  oss.seekp(ios_base::beg);
  oss << ModDepth;
  Datas.SaveValue(std::string(STR_MODE_DEPTH), std::string(oss.str()));
  oss.seekp(ios_base::beg);
  oss << Frequency;
  Datas.SaveValue(std::string(STR_FREQUENCY), std::string(oss.str()));
  oss.seekp(ios_base::beg);
  oss << EffectMix;
  Datas.SaveValue(std::string(STR_EFFECT_MIX), std::string(oss.str()));
}

bool ChorusPlugin::IsAudio()
{
  return (true);
}

bool ChorusPlugin::IsMidi()
{
  return (false);
}

wxBitmap *ChorusPlugin::GetBitmap()
{
  return (bmp);
}


void ChorusPlugin::OnChorusTime(wxScrollEvent &WXUNUSED(e))
{
  ChorusMutex.Lock();
  
  //Frequency = FrequencyFader->GetValue()/1000.f;
  AllocateMem();

  ChorusMutex.Unlock();
  chorus1->setModFrequency(Frequency);
  chorus2->setModFrequency(Frequency);
  //cout << "Frequency: " << Frequency << endl;
}

void ChorusPlugin::OnFeedback(wxScrollEvent &WXUNUSED(e))
{
  //BaseLength = BaseLengthFader->GetValue();
  
  chorus1->setBaseLength(BaseLength);
  chorus2->setBaseLength(BaseLength);
  //cout << "Delay: " << BaseLength << endl;
}

void ChorusPlugin::OnChorusStage(wxScrollEvent &WXUNUSED(e))
{
  ChorusMutex.Lock();
  
  //ModDepth = ModDepthFader->GetValue()/100.f;
  
  //cout << "ModDepth:" << ModDepth << endl;
  chorus1->setModDepth(ModDepth);
  chorus2->setModDepth(ModDepth);
  ChorusMutex.Unlock();
}


void ChorusPlugin::OnDryWet(wxScrollEvent &WXUNUSED(e))
{
  EffectMix = (100 - EffectMixFader->GetValue()) / 100.f;
  chorus1->setEffectMix(EffectMix);
  chorus2->setEffectMix(EffectMix);
  cout << "EffectMix: " << EffectMix << endl;
  cout << "Delay: " << BaseLength << endl;
  cout << "Frequency: " << Frequency << endl;
}

void ChorusPlugin::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*TpBmp);    
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, 
	      upd.GetX(), upd.GetY(), wxCOPY, FALSE);      
      upd++;
    }
  Plugin::OnPaintEvent(event);
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "CHOR");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
    return (info);
  }
  
  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new ChorusPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }
}








