#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <unistd.h>

#include "Plugin.h"
#include "FaderCtrl.h"

#define PLUGIN_NAME	"Crusher"

#define IMG_CR_BG	"plugins/crusher/crusher.png"
#define IMG_CR_BMP	"plugins/crusher/CrusherPlug.bmp"
#define IMG_CR_FADER_BG	"plugins/crusher/fader_bg.png"
#define IMG_CR_FADER_FG	"plugins/crusher/fader_fg.png"

static PlugInitInfo info;

class CrusherPlugin: public Plugin
{
 public:
  CrusherPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~CrusherPlugin();

  void	 Init();
  void	 Process(float **input, float **output, long sample_length);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  void	 Load(int fd, long size);
  long   Save(int fd);

  std::string DefaultName() { return "Crusher"; }
  
  bool	 IsAudio();
  bool	 IsMidi();

  void OnButtonClick(wxCommandEvent &e); 
  void OnBits(wxScrollEvent &e);  
  void OnFreq(wxScrollEvent &e);  
  void OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();
  int		Bits;
  int		Freq;
  
 protected:
  wxBitmap *bmp;   

  FaderCtrl *BitsFader;
  FaderCtrl *FreqFader;
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;

  float		NormFreq;
  float		Step;
  float		Phasor[2];
  float		Last;

  DECLARE_EVENT_TABLE()  

};

enum
  {
    Crusher_Bits = 1,
    Crusher_Freq
  };

/******** CrusherPlugin Implementation *********/

BEGIN_EVENT_TABLE(CrusherPlugin, wxWindow)
  EVT_COMMAND_SCROLL(Crusher_Bits, CrusherPlugin::OnBits)
  EVT_COMMAND_SCROLL(Crusher_Freq, CrusherPlugin::OnFreq)
  EVT_PAINT(CrusherPlugin::OnPaint)
END_EVENT_TABLE()

CrusherPlugin::CrusherPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_CR_BG)).c_str(), wxBITMAP_TYPE_PNG);
  if (tr_bg)
    TpBmp = new wxBitmap(tr_bg);
  
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_CR_BMP)).c_str(), wxBITMAP_TYPE_BMP); 

  img_bg = new wxImage(string(GetDataDir() + string(IMG_CR_FADER_BG)).c_str(), wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(string(GetDataDir() + string(IMG_CR_FADER_FG)).c_str(), wxBITMAP_TYPE_PNG);
  
  BitsFader = new 
    FaderCtrl(this, Crusher_Bits, img_bg, img_fg, 0, 76, 16,
	      wxPoint(18, 8)/*wxPoint(GetSize().x / 2, 10)*/, wxSize(22,78));
  FreqFader = new 
    FaderCtrl(this, Crusher_Freq, img_bg, img_fg, 0, 44100, 4000,
	      wxPoint(142, 8)/*wxPoint(GetSize().x / 2 + 40, 10)*/, wxSize(22,78));
  
  SetBackgroundColour(wxColour(237, 237, 237));
}


void CrusherPlugin::Init()
{
  Bits = 16;
  Freq = 4000;
  NormFreq = 4000.f / 44100.f;
  Step = powf(0.5f, Bits);
  Phasor[0] = 0.f;
  Phasor[1] = 0.f;
  Last = 0.f;
}

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)

void CrusherPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  int chan;

  for (chan = 0; chan < 2; chan++)
    {
      for (i = 0; i < sample_length; i++)
	{
	  Phasor[chan] = Phasor[chan] + NormFreq;
	  if (Phasor[chan] >= 1.0f)
	    {
	      Phasor[chan] = Phasor[chan] - 1.0f;
	      Last = Step * floorf(input[chan][i] / Step + 0.5f);
	    }
	  output[chan][i] = Last; 
	}
    }
}

CrusherPlugin::~CrusherPlugin()
{

}

void CrusherPlugin::Load(int fd, long size)
{
  int filter;

  if (read(fd, &Bits, sizeof (Bits)) <= 0)
    {
      cout << "[CRUSHERPLUG] Error while loading patch !" << endl;
      return;
    }
  if (read(fd, &Freq, sizeof (Freq)) <= 0)
    {
      cout << "[CRUSHERPLUG] Error while loading patch !" << endl;
      return;
    }

  BitsFader->SetValue(Bits);
  FreqFader->SetValue(Freq);

  NormFreq = Freq / 44100.f;
  Step = powf(0.5f, Bits);
  Phasor[0] = 0.f;
  Phasor[1] = 0.f;
  Last = 0.f;
}
 
long CrusherPlugin::Save(int fd)
{
  long size;

  size = write(fd, &Bits, sizeof (Bits));
  size += write(fd, &Freq, sizeof (Freq));
  
  return (size);
}


bool CrusherPlugin::IsAudio()
{
  return (true);
}

bool CrusherPlugin::IsMidi()
{
  return (true);
}

wxBitmap *CrusherPlugin::GetBitmap()
{
  return (bmp);
}

void CrusherPlugin::OnBits(wxScrollEvent &WXUNUSED(e))
{
  Bits = BitsFader->GetValue();
  Step = powf(0.5f, Bits);
  //if (Bits <= 0.f)
  //Bits = 0.01f;
  cout << "Bits: " << Bits << "; Step: " << Step << endl;
}
  
void CrusherPlugin::OnFreq(wxScrollEvent &WXUNUSED(e))
{
  Freq = FreqFader->GetValue();
  NormFreq = Freq / 44100.f;
  cout << "Freq: " << Freq << "; NormFreq: " << NormFreq << endl;
}

void CrusherPlugin::OnPaint(wxPaintEvent &event)
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
    WIRED_MAKE_STR(info.UniqueId, "CRUS");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new CrusherPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








