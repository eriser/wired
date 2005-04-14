#include "Plugin.h"
#include <iostream>

using namespace std;

/* DO NOT MODIFY THIS FILE FOR MAKING A PLUGIN. JUST DERIVE FROM THAT CLASS */

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
  Connect(wxID_ANY, wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)
	 &Plugin::OnMouseEvent);
  //  Connect(wxID_ANY, wxEVT_PAINT, (wxObjectEventFunction)(wxEventFunction)
  //	 &Plugin::OnPaintEvent);
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

void Plugin::OnPaintEvent(wxPaintEvent &event)
{
  SendPaintEvent(event);
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

void Plugin::AskUpdate()
{
  StartInfo.HostCallback(this, wiredAskUpdateGui, 0x0);
}

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

void Plugin::SendPaintEvent(wxPaintEvent &event)
{
  StartInfo.HostCallback(this, wiredSendPaintEvent, (void *)&event);  
}

bool Plugin::ShowMidiController(int *MidiData[3])
{
  StartInfo.HostCallback(this, wiredShowMidiController, (void *)MidiData);  
  if (*MidiData[0] == -1)
    return (false);
  return (true);
}

void Plugin::ShowOptionalView()
{
  StartInfo.HostCallback(this, wiredShowOptionalView, 0x0);
}

void Plugin::CloseOptionalView()
{
  StartInfo.HostCallback(this, wiredCloseOptionalView, 0x0);
}

std::string Plugin::OpenFileLoader(std::string title, 
				   std::vector<std::string> *exts,
				   bool akai)
{
  struct
  {
    std::string *t;
    std::vector<std::string> *e;
    bool ak;
    std::string result;
  } w_filel;
 
  w_filel.t = &title;
  w_filel.e = exts;
  w_filel.ak = akai;
  StartInfo.HostCallback(this, wiredOpenFileLoader, (void *)&w_filel);
  return (w_filel.result);
}

std::string Plugin::SaveFileLoader(std::string title, 
				   std::vector<std::string> *exts)
{
  struct
  {
    std::string *t;
    std::vector<std::string> *e;
    std::string result;
  } w_filel;
 
  w_filel.t = &title;
  w_filel.e = exts;
  StartInfo.HostCallback(this, wiredSaveFileLoader, (void *)&w_filel);
  return (w_filel.result);
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

// WiredPluginData class implementation, used for Plugin parameters

WiredPluginData	WiredPluginData::operator=(const WiredPluginData& right)
{
	if (this != &right)
	{
		_Data = right._Data;
	}
	return *this;
}

bool			WiredPluginData::SaveValue(const std::string& Name, char *Value)
{
	if (_Data.find(Name) == _Data.end())
	{
		_Data[Name] = Value;
		return true;
	}
	return false;
}

char			*WiredPluginData::LoadValue(const std::string& Name)
{
	if (_Data.find(Name) != _Data.end())
		return _Data[Name];
	return NULL;
}

PluginParams	*WiredPluginData::GetParamsStack()
{
	if (_Data.empty() == false)
		return &_Data;
	return NULL;
}
