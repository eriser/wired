#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "midi.h"

#include <iostream>
using namespace std;

#define PLUGIN_NAME	"Test Instrument"

static PlugInitInfo info;

class TestPlugin: public Plugin
{
 public:
  TestPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~TestPlugin();

  void   Process(float **input, float **output, long sample_length);
  void   ProcessEvent(WiredEvent &event);

  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  bool	 IsInstrument();

  bool	 IsAudio();
  bool	 IsMidi();

  wxBitmap *GetBitmap();

protected:
  bool	 NoteIsOn;
  float	 Freq;

  void OnButtonClick(wxCommandEvent &e);  
  wxBitmap *bmp;  
};

#define SAW 

/******** TestPlugin Implementation *********/

TestPlugin::TestPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  cout << "[TESTINSTR] Host is " << GetHostProductName()
       << " version " << GetHostProductVersion() << endl;
  NoteIsOn = false;
  Freq = 0.f;

  bmp = new wxBitmap("plugins/test/instr_test.bmp", wxBITMAP_TYPE_BMP);  

  wxButton *b = new wxButton(this, 31, "YEAH", wxPoint(0,0), wxSize(40, 60));
  b->Show();
  
  Connect(31, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&TestPlugin::OnButtonClick);
  SetBackgroundColour(wxColour(77, 31, 77));

}

void TestPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  int chan;

  Plugin::Process(input, output, sample_length);
  
#ifdef DSF
  double x;  // input
  double a = 0.5;  // a<1.0
  double N = 440;  // N<SmplFQ/2, 
  double fi = 0.5;// phase

  double s1;
  double s2;
  double s3;
  double s4;
  
  for (chan = 0; chan < 2; chan++)
    for (i = 0; i < sample_length; i++)
      {
	x = input[chan][i];
	s1 = pow(a,N-1.0)*sin((N-1.0)*x+fi);
	s2 = pow(a,N)*sin(N*x+fi);
	s3 = a*sin(x+fi);
	s4 =1.0 - (2*a*cos(x)) +(a*a);
  
	if (s4==0)
	  output[chan][i] = 0.f;
	else
	  output[chan][i] = (sin(fi) - s3 - s2 +s1)/s4; 
      }

#endif

#ifdef SAW
  float p=0.0f;      //current position
  float dp=1.0f;     //change in postion per sample
  float pmax;        //maximum position
  float x;           //position in sinc function
  float leak=0.995f; //leaky integrator
  float dc;          //dc offset
  static float saw[2];         //output
  
  //set frequency...
  
  pmax = 0.5f * 44100.f / Freq;
  dc = -0.498f/pmax;

  if (NoteIsOn)
  for (chan = 0; chan < 2; chan++)
    for (i = 0; i < sample_length; i++)
      {
	p += dp;
	if(p < 0.0f)
	  {
	    p = -p;
	    dp = -dp;
	  }
	else if(p > pmax)
	  {
	    p = pmax + pmax - p;
	    dp = -dp;
	  }

	x= M_PI * p;
	if(x < 0.00001f)
	  x=0.00001f; //don't divide by 0
	
	saw[chan] = (leak*saw[chan] + dc + (float)sin(x)/(x)) * 0.25f;
	output[chan][i] = saw[chan];
      }
#endif
}

void TestPlugin::ProcessEvent(WiredEvent &event)
{
  if ((event.MidiData[0] == M_NOTEON1) || (event.MidiData[0] == M_NOTEON2))
    {
      Freq = 440.f * (float)pow(2, (float)(event.MidiData[1] - 69)/12.0f);
      if (!event.MidiData[2]) 
	NoteIsOn = false;
      else
	NoteIsOn = true;
      cout << "Note frequency: " << Freq << endl;
    }
  
 printf("[TESTPLUGIN] Got midi in : %2x %2x %2x\n", event.MidiData[0], event.MidiData[1], event.MidiData[2]);
}

TestPlugin::~TestPlugin()
{
  delete bmp;
}

bool TestPlugin::IsInstrument()
{
  return (true);
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
  SetBackgroundColour(*wxBLUE);
  Refresh();
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "PLIN");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_INSTR;  
    info.UnitsX = 2;
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








