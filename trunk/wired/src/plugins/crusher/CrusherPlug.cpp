// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <unistd.h>

#include "Plugin.h"
#include "FaderCtrl.h"
#include "DownButton.h"
#include "midi.h"

#define PLUGIN_NAME	wxT("Crusher")

#define IMG_CR_BG	wxT("plugins/crusher/crusher_bg.png")
#define IMG_CR_BMP	wxT("plugins/crusher/CrusherPlug.bmp")
#define IMG_CR_FADER_BG	wxT("plugins/crusher/fader_bg.png")
#define IMG_CR_FADER_FG	wxT("plugins/crusher/fader_button.png")

#define IMG_LIQUID_ON	wxT("plugins/reverb/liquid-cristal_play.png")
#define IMG_LIQUID_OFF	wxT("plugins/reverb/liquid-cristal_stop.png")
#define IMG_BYPASS_ON	wxT("plugins/reverb/bypass_button_down.png")
#define IMG_BYPASS_OFF	wxT("plugins/reverb/bypass_button_up.png")

static PlugInitInfo info;

class		CrusherPlugin: public Plugin
{
 public:
  CrusherPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~CrusherPlugin();

  void		Init();
  void		Process(float **input, float **output, long sample_length);
  void		CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  void		Load(int fd, long size);
  long		Save(int fd);

  wxString	DefaultName() { return wxT("Crusher"); }
  
  bool		IsAudio();
  bool		IsMidi();

  void		OnButtonClick(wxCommandEvent &e); 
  void		OnBits(wxScrollEvent &e);  
  void		OnFreq(wxScrollEvent &e);  
  void		OnPaint(wxPaintEvent &event);
  void		OnBypass(wxCommandEvent &e);
  void		OnBypassController(wxMouseEvent &event);

  wxBitmap	*GetBitmap();
  float		Bits;
  float		Freq;
  
 protected:
  wxBitmap	*bmp;   

  int		MidiBypass[2];

  FaderCtrl	*BitsFader;
  FaderCtrl	*FreqFader;
  wxImage	*img_fg;
  wxImage	*img_bg;
  wxImage	*bypass_on;
  wxImage	*bypass_off;
  wxBitmap	*TpBmp;

  StaticBitmap	*Liquid;
  wxImage	*liquid_on;
  wxImage	*liquid_off;
  DownButton	*BypassBtn;
  bool		Bypass;

  float		NormFreq;
  float		Step;
  float		Phasor[2];
  float		Last;

  wxMutex	CrusherMutex;

  DECLARE_EVENT_TABLE()  
};

enum
  {
    Crusher_Bypass = 1,
    Crusher_Bits,
    Crusher_Freq
  };

/******** CrusherPlugin Implementation *********/

BEGIN_EVENT_TABLE(CrusherPlugin, wxWindow)
  EVT_BUTTON(Crusher_Bypass, CrusherPlugin::OnBypass)
  EVT_COMMAND_SCROLL(Crusher_Bits, CrusherPlugin::OnBits)
  EVT_COMMAND_SCROLL(Crusher_Freq, CrusherPlugin::OnFreq)
  EVT_PAINT(CrusherPlugin::OnPaint)
END_EVENT_TABLE()

CrusherPlugin::CrusherPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  Init();

  wxImage *tr_bg = 
    new wxImage(GetDataDir() + wxString(IMG_CR_BG), wxBITMAP_TYPE_PNG);
  if (tr_bg)
    TpBmp = new wxBitmap(tr_bg);
  
  bmp = new wxBitmap(GetDataDir() + wxString(IMG_CR_BMP), wxBITMAP_TYPE_BMP); 

  img_bg = new wxImage(GetDataDir() + wxString(IMG_CR_FADER_BG), wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(GetDataDir() + wxString(IMG_CR_FADER_FG), wxBITMAP_TYPE_PNG);

  bypass_on = new wxImage(GetDataDir() + wxString(IMG_BYPASS_ON), wxBITMAP_TYPE_PNG);
  bypass_off = new wxImage(GetDataDir() + wxString(IMG_BYPASS_OFF), wxBITMAP_TYPE_PNG);
  BypassBtn = new DownButton(this, Crusher_Bypass, wxPoint(21, 58), 
			     wxSize(bypass_on->GetWidth(), bypass_on->GetHeight()), bypass_off, bypass_on);  
  
  liquid_on = new wxImage(GetDataDir() + wxString(IMG_LIQUID_ON), wxBITMAP_TYPE_PNG);
  liquid_off = new wxImage(GetDataDir() + wxString(IMG_LIQUID_OFF), wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));

  BitsFader = new FaderCtrl(this, Crusher_Bits, img_bg, img_fg, 0, 76, &Bits,
			    true, wxPoint(83, 12), wxSize(22,78), 
			    this, GetPosition() + wxPoint(83, 35));
  FreqFader = new FaderCtrl(this, Crusher_Freq, img_bg, img_fg, 0, 44100, &Freq,
			    true, wxPoint(140, 12), wxSize(22,78), 
			    this, GetPosition() + wxPoint(140, 35));
  Connect(Crusher_Bypass, wxEVT_RIGHT_DOWN, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&CrusherPlugin::OnBypassController);
  SetBackgroundColour(wxColour(237, 237, 237));
}


void			CrusherPlugin::Init()
{
  Bits = 16;
  Freq = 4000;
  NormFreq = 4000.f / 44100.f;
  Step = powf(0.5f, Bits);
  Phasor[0] = 0.f;
  Phasor[1] = 0.f;
  Last = 0.f;
}

#define IS_DENORMAL(f)	(((*(unsigned int *)&f)&0x7f800000)==0)

void			CrusherPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  int chan;

  if (!Bypass)
    {
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
  else
    {
      memcpy(output[0], input[0], sample_length * sizeof(float));
      memcpy(output[1], input[1], sample_length * sizeof(float));
    }
}
CrusherPlugin::~CrusherPlugin()
{

}

void			CrusherPlugin::Load(int fd, long size)
{
  int			filter;

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

  //BitsFader->SetValue(Bits);
  //FreqFader->SetValue(Freq);

  NormFreq = Freq / 44100.f;
  Step = powf(0.5f, Bits);
  Phasor[0] = 0.f;
  Phasor[1] = 0.f;
  Last = 0.f;
}
 
long			CrusherPlugin::Save(int fd)
{
  long			size;

  size = write(fd, &Bits, sizeof (Bits));
  size += write(fd, &Freq, sizeof (Freq));
  
  return (size);
}


bool			CrusherPlugin::IsAudio()
{
  return (true);
}

bool			CrusherPlugin::IsMidi()
{
  return (true);
}

wxBitmap		*CrusherPlugin::GetBitmap()
{
  return (bmp);
}

void			CrusherPlugin::OnBits(wxScrollEvent &WXUNUSED(e))
{
  //Bits = BitsFader->GetValue();
  Step = powf(0.5f, Bits);
  //cout << "Bits = " << Bits << endl;
  //if (Bits <= 0.f)
  //Bits = 0.01f;
//  cout << "Bits: " << Bits << "; Step: " << Step << endl;
}
  
void			CrusherPlugin::OnFreq(wxScrollEvent &WXUNUSED(e))
{
  //*Freq = FreqFader->GetValue();
  NormFreq = Freq / 44100.f;
//  cout << "Freq: " << Freq << "; NormFreq: " << NormFreq << endl;
}

void			CrusherPlugin::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC	memDC;
  wxPaintDC	dc(this);
  
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

void			CrusherPlugin::OnBypass(wxCommandEvent &e)
{
  CrusherMutex.Lock();
  Bypass = BypassBtn->GetOn();
  Liquid->SetBitmap(wxBitmap((Bypass) ? liquid_off : liquid_on));
  CrusherMutex.Unlock();
}

void			CrusherPlugin::OnBypassController(wxMouseEvent &event)
{
  int			*midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      CrusherMutex.Lock();
      //CheckExistingControllerData(midi_data);
      MidiBypass[0] = midi_data[0];
      MidiBypass[1] = midi_data[1];
      CrusherMutex.Unlock();
    }
  delete midi_data;
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








