// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License


#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"
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
  : Plugin(startinfo, initinfo)
{
  BufStart[0] = 0x0;
  BufStart[1] = 0x0;
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_BG)).c_str(), wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_DL_BMP)).c_str(), 
		     wxBITMAP_TYPE_BMP); 
  img_bg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_BG)).c_str(),
		       wxBITMAP_TYPE_PNG );
  img_fg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_FG)).c_str(),
		       wxBITMAP_TYPE_PNG );
  
  FrequencyFader = new FaderCtrl(this, Chorus_Time, img_bg, img_fg, 0, 10, 3, 
				 wxPoint(18, 8), wxSize(22,78));
  BaseLengthFader = new FaderCtrl(this, Chorus_Feedback, img_bg, img_fg, 0, 10000, 5000,
				  wxPoint(50, 8), wxSize(22,78));
  ModDepthFader = new FaderCtrl(this, Chorus_Stage, img_bg, img_fg, 0, 10, 3,
				wxPoint(82, 8), wxSize(22,78));
  EffectMixFader = new FaderCtrl(this, Chorus_DryWet, img_bg, img_fg, 0, 100, 50,
				 wxPoint(142, 8), wxSize(22,78));  
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
  unsigned long c1;

  ChorusMutex.Lock();

  for (c1 = 0; c1 < sample_length; c1++)
    {
      output[0][c1] = chorus1->tick(input[0][c1]);
      output[1][c1] = chorus2->tick(input[1][c1]);
    }

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

long ChorusPlugin::Save(int fd)
{
  long size;

  size = write(fd, &BaseLength, sizeof (BaseLength));
  size += write(fd, &ModDepth, sizeof (ModDepth));
  size += write(fd, &Frequency, sizeof (Frequency));
  size += write(fd, &EffectMix, sizeof (EffectMix));

  return (size);
}





bool ChorusPlugin::IsAudio()
{
  return (true);
}

bool ChorusPlugin::IsMidi()
{
  return (true);
}

wxBitmap *ChorusPlugin::GetBitmap()
{
  return (bmp);
}


void ChorusPlugin::OnChorusTime(wxScrollEvent &WXUNUSED(e))
{
  ChorusMutex.Lock();

  Frequency = FrequencyFader->GetValue()/1000.f;
  AllocateMem();

  ChorusMutex.Unlock();
  chorus1->setModFrequency(Frequency);
  chorus2->setModFrequency(Frequency);
  cout << "Frequency: " << Frequency << endl;
}
  
void ChorusPlugin::OnFeedback(wxScrollEvent &WXUNUSED(e))
{
  BaseLength = BaseLengthFader->GetValue();

  chorus1->setBaseLength(BaseLength);
  chorus2->setBaseLength(BaseLength);
  cout << "Delay: " << BaseLength << endl;
}

void ChorusPlugin::OnChorusStage(wxScrollEvent &WXUNUSED(e))
{
  ChorusMutex.Lock();

  ModDepth = ModDepthFader->GetValue()/100.f;

  cout << "ModDepth:" << ModDepth << endl;
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
}

void ChorusPlugin::OnPaint(wxPaintEvent &WXUNUSED(event))
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








