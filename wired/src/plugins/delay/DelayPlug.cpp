#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"

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
  wxBitmap *bmp;   

  FaderCtrl *TimeFader;
  FaderCtrl *FeedbackFader;
  FaderCtrl *DryWetFader;
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;

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
    Delay_Time = 1,
    Delay_Feedback,
    Delay_DryWet,
  };

/******** DelayPlugin Implementation *********/

BEGIN_EVENT_TABLE(DelayPlugin, wxWindow)
  EVT_COMMAND_SCROLL(Delay_Time, DelayPlugin::OnDelayTime)
  EVT_COMMAND_SCROLL(Delay_Feedback, DelayPlugin::OnFeedback)
  EVT_COMMAND_SCROLL(Delay_DryWet, DelayPlugin::OnDryWet)
  EVT_PAINT(DelayPlugin::OnPaint)
END_EVENT_TABLE()

#define IMG_DL_BG	"plugins/delay/delay.png"
#define IMG_DL_BMP	"plugins/delay/DelayPlug.bmp"
#define IMG_DL_FADER_BG "plugins/delay/fader_bg.png"
#define IMG_DL_FADER_FG	"plugins/delay/fader_fg.png"

DelayPlugin::DelayPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  BufStart[0] = 0x0;
  BufStart[1] = 0x0;
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_DL_BG)).c_str(), wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_DL_BMP)).c_str(), wxBITMAP_TYPE_BMP); 

  img_bg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_BG)).c_str(),wxBITMAP_TYPE_PNG );
  img_fg = new wxImage(string(GetDataDir() + string(IMG_DL_FADER_FG)).c_str(),wxBITMAP_TYPE_PNG );
  
  TimeFader = new 
    FaderCtrl(this, Delay_Time, img_bg, img_fg, 0, 5000, 1000,
	      wxPoint(18, 8)/*wxPoint(GetSize().x / 2, 10)*/, wxSize(22,78));
  FeedbackFader = new 
    FaderCtrl(this, Delay_Feedback, img_bg, img_fg, 0, 100, 50,
	      wxPoint(74, 8)/*wxPoint(GetSize().x / 2 + 40, 10)*/, wxSize(22,78));
  DryWetFader = new 
    FaderCtrl(this, Delay_DryWet, img_bg, img_fg, 0, 100, 50,
	      wxPoint(142, 8)/*wxPoint(GetSize().x / 2 + 40, 10)*/, wxSize(22,78));
  
  SetBackgroundColour(wxColour(237, 237, 237));
}

DelayPlugin::~DelayPlugin()
{
  //delete tr_bg;
  delete TpBmp;
  delete bmp;
  delete img_bg;
  delete img_fg;
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








