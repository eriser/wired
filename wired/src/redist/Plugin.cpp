#include "Plugin.h"
#include <iostream>

using namespace std;

Plugin::Plugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo) 
  : wxWindow(startinfo.Rack, -1, startinfo.Pos, startinfo.Size)
{ 
  InitInfo = initinfo;
  StartInfo = startinfo;
  Connect(wxID_ANY, wxEVT_KEY_DOWN, (wxObjectEventFunction)(wxEventFunction)
          &Plugin::OnKeyEvent);
  Connect(wxID_ANY, wxEVT_MOUSEWHEEL, (wxObjectEventFunction)(wxEventFunction)
          &Plugin::OnMouseEvent);
  Connect(wxID_ANY, wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)
          &Plugin::OnMouseEvent);
  Connect(wxID_ANY, wxEVT_LEFT_UP, (wxObjectEventFunction)(wxEventFunction)
          &Plugin::OnMouseEvent);
  Connect(wxID_ANY, wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)
	 &Plugin::OnMouseEvent);
}

Plugin::~Plugin() 
{ 
  CloseOptionalView();
}

void Plugin::OnMouseEvent(wxMouseEvent &event)
{
  SendMouseEvent(event);
}

void Plugin::OnKeyEvent(wxKeyEvent &event)
{
  SendKeyEvent(event);
}

// Time events
float Plugin::GetBpm()
{
  float d;
  StartInfo.HostCallback(0x0, wiredGetBpm, (void *)&d);
  return (d);
}

int Plugin::GetSigNumerator()
{
  int d;
  StartInfo.HostCallback(0x0, wiredGetSigNumerator, (void *)&d);
  return (d);
}

int Plugin::GetSigDenominator()
{
  int d;
  StartInfo.HostCallback(0x0, wiredGetSigDenominator, (void *)&d);
  return (d);
}

unsigned long Plugin::GetSamplePos()
{
  unsigned long d;
  StartInfo.HostCallback(0x0, wiredGetSamplePos, (void *)&d);
  return (d);
}

double Plugin::GetSamplesPerBar()
{
  double spm;
  StartInfo.HostCallback(0x0, wiredGetSamplesPerBar, (void*)&spm);
  return (spm);
}

double Plugin::GetBarsPerSample()
{
  double spm;
  StartInfo.HostCallback(0x0, wiredGetBarsPerSample, (void*)&spm);
  return (spm);
}

double Plugin::GetBarPos()
{
  double d;
  StartInfo.HostCallback(0x0, wiredGetBarPos, (void *)&d);
  return (d);
}

// User interface events

void Plugin::SendHelp(std::string str)
{
  StartInfo.HostCallback(this, wiredSendHelp, (void *)&str);
}

void Plugin::SendMouseEvent(wxMouseEvent &event)
{
  StartInfo.HostCallback(this, wiredSendMouseEvent, (void *)&event);
}

void Plugin::SendKeyEvent(wxKeyEvent &event)
{
  StartInfo.HostCallback(this, wiredSendKeyEvent, (void *)&event);
}

void Plugin::SendClickEvent(wxMouseEvent &event)
{
  StartInfo.HostCallback(this, wiredSendClickEvent, (void *)&event);  
}

void Plugin::ShowOptionalView()
{
  StartInfo.HostCallback(this, wiredShowOptionalView, 0x0);
}

void Plugin::CloseOptionalView()
{
  StartInfo.HostCallback(this, wiredCloseOptionalView, 0x0);
}

// Host info
std::string Plugin::GetHostProductName()
{
  char str[256];
  std::string s;

  StartInfo.HostCallback(0x0, wiredHostProductName, (void *)str); 
  s = str;
  return (s);
}

float  Plugin::GetHostProductVersion()
{
  float f;

  StartInfo.HostCallback(0x0, wiredHostProductVersion, (void *)&f);
  return (f);
}

std::string Plugin::GetHostVendorName()
{
  char str[256];
  std::string s;

  StartInfo.HostCallback(0x0, wiredHostVendorName, (void *)str); 
  s = str;
  return (s);
}

std::string Plugin::GetDataDir()
{
  std::string s;
  
  StartInfo.HostCallback(0x0, wiredGetDataDir, (void *)&s); 
  return (s); 
}

bool Plugin::CreateMidiPattern(std::list<SeqCreateEvent *> *l)
{
  if (StartInfo.HostCallback(this, wiredCreateMidiPattern, (void *)l) > 0)
    return (true);
  else
    return (false);
}
