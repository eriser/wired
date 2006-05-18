// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"

#define PLUGIN_NAME	"Test Effect"

static PlugInitInfo info;

class TestPlugin: public Plugin
{
 public:
  TestPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~TestPlugin();

  void	Process(float **input, float **output, long sample_length);

  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  
  bool	 IsAudio();
  bool	 IsMidi();

  
  void OnButtonClick(wxCommandEvent &e);  
  void OnPaint(wxPaintEvent &event);
  wxBitmap *GetBitmap();

 protected:
  wxBitmap *bmp;   

  FaderCtrl *fader_test;
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;

  DECLARE_EVENT_TABLE()
};

/******** TestPlugin Implementation *********/

BEGIN_EVENT_TABLE(TestPlugin, wxWindow)
  EVT_PAINT(TestPlugin::OnPaint)
END_EVENT_TABLE()

TestPlugin::TestPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  wxImage *tr_bg = 
    new wxImage("data/FilterPlug.bmp", wxBITMAP_TYPE_BMP);
  TpBmp = new wxBitmap(tr_bg);

  bmp = new wxBitmap("plugins/test/effect_test.bmp", wxBITMAP_TYPE_BMP);  

  wxButton *b = new wxButton(this, 31, "YEAH", wxPoint(0,0), wxSize(40, 60));
  b->Show();

  img_bg = new wxImage("data/bg.png",wxBITMAP_TYPE_PNG );
  img_fg = new wxImage("data/fg.png",wxBITMAP_TYPE_PNG );
  fader_test = new FaderCtrl(this, -1, img_bg, img_fg,0,200, 200,
                             wxPoint(GetSize().x / 2, 10), wxSize(20,76));
  
  Connect(31, wxEVT_COMMAND_BUTTON_CLICKED , (wxObjectEventFunction)(wxEventFunction) (wxCommandEventFunction)&TestPlugin::OnButtonClick);
  SetBackgroundColour(wxColour(88, 106, 226));
}

void TestPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  int chan;
  short x;

#define keep 4

  for (chan = 0; chan < 2; chan++)
    for (i = 0; i < sample_length; i++)
      {
	/*x = (short)(input[chan][i] * 32767.f);
	output[chan][i] = x & (-1 << (16 - keep));
	output[chan][i] /= 32767.f;*/
	output[chan][i] = input[chan][i] * 1.25;
      }
}

TestPlugin::~TestPlugin()
{
  delete bmp;
}

bool TestPlugin::IsAudio()
{
  return (true);
}

bool TestPlugin::IsMidi()
{
  return (true);
}

wxBitmap *TestPlugin::GetBitmap()
{
  return (bmp);
}

void TestPlugin::OnButtonClick(wxCommandEvent &e)
{
  SetBackgroundColour(*wxRED);
  Refresh();
}

void TestPlugin::OnPaint(wxPaintEvent &event)
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
    WIRED_MAKE_STR(info.UniqueId, "PLEF");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;

    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new TestPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








