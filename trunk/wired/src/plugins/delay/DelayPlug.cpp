#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"
#include "StaticBitmap.h"
#include "DownButton.h"

#define PLUGIN_NAME	"Delay"

static PlugInitInfo info;

class DelayPlugin: public Plugin
{
 public:
  DelayPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~DelayPlugin();

  void	 Init();
  void	 Process(float **input, float **output, long sample_length);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  
  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Delay"; }

  void	 OnBypass(wxCommandEvent &e);  
  void	 OnDelayTime(wxScrollEvent &e);  
  void   OnFeedback(wxScrollEvent &e);  
  void   OnDryWet(wxScrollEvent &event);
  void   OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();

  float		DelayTime;
  float		Feedback;
  float		WetLevel;
  float		DryLevel;
  
 protected:
  bool	   Bypass;

  wxBitmap *bmp;   

  FaderCtrl *TimeFader;
  FaderCtrl *FeedbackFader;
  FaderCtrl *DryWetFader;
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;
  wxImage *bypass_on;
  wxImage *bypass_off;
  wxImage *liquid_on;;
  wxImage *liquid_off;;
  StaticBitmap *Liquid;
  DownButton *BypassBtn;

  float *DelayBuffer;
  float *BufStart[2];
  float *BufEnd[2];
  float *BufPtr[2];

  wxMutex DelayMutex;

  void AllocateMem();

  DECLARE_EVENT_TABLE()  
};

enum
  {
    Delay_Bypass = 1,
    Delay_Time,
    Delay_Feedback,
    Delay_DryWet,
  };

/******** DelayPlugin Implementation *********/

BEGIN_EVENT_TABLE(DelayPlugin, wxWindow)
  EVT_BUTTON(Delay_Bypass, DelayPlugin::OnBypass)
  EVT_COMMAND_SCROLL(Delay_Time, DelayPlugin::OnDelayTime)
  EVT_COMMAND_SCROLL(Delay_Feedback, DelayPlugin::OnFeedback)
  EVT_COMMAND_SCROLL(Delay_DryWet, DelayPlugin::OnDryWet)
  EVT_PAINT(DelayPlugin::OnPaint)
END_EVENT_TABLE()

#define IMG_DL_BG	"plugins/delay/delay_bg.png"
#define IMG_DL_BMP	"plugins/delay/DelayPlug.bmp"
#define IMG_DL_FADER_BG	"plugins/delay/fader_bg.png"
#define IMG_DL_FADER_FG	"plugins/delay/fader_button.png"
#define IMG_LIQUID_ON	"plugins/delay/liquid-cristal_play.png"
#define IMG_LIQUID_OFF	"plugins/delay/liquid-cristal_stop.png"
#define IMG_BYPASS_ON	"plugins/delay/bypass_button_down.png"
#define IMG_BYPASS_OFF	"plugins/delay/bypass_button_up.png"

DelayPlugin::DelayPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo), Bypass(false)
{
  BufStart[0] = 0x0;
  BufStart[1] = 0x0;
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_BG)).c_str(), wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);

  liquid_on = new wxImage(string(GetDataDir() + string(IMG_LIQUID_ON)).c_str(), 
			  wxBITMAP_TYPE_PNG);
  liquid_off = new wxImage(string(GetDataDir() + string(IMG_LIQUID_OFF)).c_str(), 
			   wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));

  bypass_on = new wxImage(string(GetDataDir() + string(IMG_BYPASS_ON)).c_str(), 
			  wxBITMAP_TYPE_PNG);
  bypass_off = new wxImage(string(GetDataDir() + string(IMG_BYPASS_OFF)).c_str(), 
			   wxBITMAP_TYPE_PNG);
  BypassBtn = new DownButton(this, Delay_Bypass, wxPoint(21, 58),
			     wxSize(bypass_on->GetWidth(), bypass_on->GetHeight()),
			     bypass_off, bypass_on);

  bmp = new wxBitmap(string(GetDataDir() + string(IMG_DL_BMP)).c_str(), 
		     wxBITMAP_TYPE_BMP); 

  img_bg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_BG)).c_str(),
		       wxBITMAP_TYPE_PNG );
  img_fg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_FG)).c_str(),
		       wxBITMAP_TYPE_PNG );
  
  TimeFader = new 
    FaderCtrl(this, Delay_Time, img_bg, img_fg, 0, 5000, 1000,
	      wxPoint(73, 11), wxSize(img_bg->GetWidth(), img_bg->GetHeight()));
  FeedbackFader = new 
    FaderCtrl(this, Delay_Feedback, img_bg, img_fg, 0, 100, 50,
	      wxPoint(110, 11), wxSize(img_bg->GetWidth(), img_bg->GetHeight()));
  DryWetFader = new 
    FaderCtrl(this, Delay_DryWet, img_bg, img_fg, 0, 100, 50,
	      wxPoint(149, 11), wxSize(img_bg->GetWidth(), img_bg->GetHeight()));
  
  SetBackgroundColour(wxColour(237, 237, 237));
}

DelayPlugin::~DelayPlugin()
{
  delete TpBmp;
  delete bmp;
  delete img_bg;
  delete img_fg;

  delete bypass_on;
  delete bypass_off;
  delete liquid_on;
  delete liquid_off;
}

void DelayPlugin::Init()
{
  DelayMutex.Lock();

  DelayTime = 1000;
  Feedback = 0.5f;
  DryLevel = 0.5f;
  WetLevel = 0.5f;
  
  AllocateMem();

  DelayMutex.Unlock();

}

void DelayPlugin::AllocateMem()
{
  for (int i = 0; i < 2; i++)
    if (BufStart[i])
      delete BufStart[i];

  int len = (int)(DelayTime * 44100.f / 1000.f);
  if (len >= 0)
    {
      for (int i = 0; i < 2; i++)
	{
	  BufStart[i] = new float[len];
	  if (BufStart[i] == 0x0)
	    cout << "[DELAYPLUG] Error allocating memory" << endl;
	  
	  BufEnd[i] = BufStart[i] + len;
	  BufPtr[i] = BufStart[i];
	  
	  memset(BufPtr[i], 0, len * sizeof(float));
	}
    }
}

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)

void DelayPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  int chan;
  float out[2];

  DelayMutex.Lock();

  if (!Bypass)
    for (chan = 0; chan < 2; chan++)
      {
	for (i = 0; i < sample_length; i++)
	  {
	    out[chan] = *BufPtr[chan];
	    
	    output[chan][i] = DryLevel * input[chan][i] + 
	      WetLevel * out[chan];
	    
	    *BufPtr[chan] = input[chan][i] + Feedback * out[chan];
	    
	    if (++(BufPtr[chan]) >= BufEnd[chan])
	      BufPtr[chan] = BufStart[chan];
	  }
      }
  else
    {
      memcpy(output[0], input[0], sample_length * sizeof(float));
      memcpy(output[1], input[1], sample_length * sizeof(float));
    }

  DelayMutex.Unlock();
}

bool DelayPlugin::IsAudio()
{
  return (true);
}

bool DelayPlugin::IsMidi()
{
  return (true);
}

wxBitmap *DelayPlugin::GetBitmap()
{
  return (bmp);
}

void DelayPlugin::OnBypass(wxCommandEvent &e)
{
  DelayMutex.Lock();

  Bypass = BypassBtn->GetOn();
  if (Bypass)
    Liquid->SetBitmap(wxBitmap(liquid_off));
  else
    Liquid->SetBitmap(wxBitmap(liquid_on));	      

  DelayMutex.Unlock();
}

void DelayPlugin::OnDelayTime(wxScrollEvent &WXUNUSED(e))
{
  DelayMutex.Lock();

  DelayTime = TimeFader->GetValue();
  AllocateMem();

  DelayMutex.Unlock();

  cout << "Time: " << DelayTime << endl;
}
  
void DelayPlugin::OnFeedback(wxScrollEvent &WXUNUSED(e))
{
  Feedback = FeedbackFader->GetValue() / 100.f;
  cout << "Feedback: " << Feedback << endl;
}

void DelayPlugin::OnDryWet(wxScrollEvent &WXUNUSED(e))
{
  DryLevel = (100 - DryWetFader->GetValue()) / 100.f;
  WetLevel = DryWetFader->GetValue() / 100.f;
  cout << "DryLevel: " << DryLevel << "; WetLevel: " << WetLevel << endl;
}

void DelayPlugin::OnPaint(wxPaintEvent &WXUNUSED(event))
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
    WIRED_MAKE_STR(info.UniqueId, "DELA");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new DelayPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








