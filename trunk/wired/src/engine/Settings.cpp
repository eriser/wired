// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "Settings.h"
#include <wx/filename.h>
#include <iostream>

#include "config.h"

Settings *WiredSettings;

Settings::Settings() :
  QuickWaveRender(false), dbWaveRender(false), OutputDev(-1), InputDev(-1), OutputLatency(-1),
  InputLatency(-1), SampleRate(44100), SamplesPerBuffer(2048), SampleFormat(-1)
{
  wxFileName f;

  ConfDir = INSTALL_PREFIX;
  ConfDir += "/etc/";
  PlugConfFile = ConfDir;
  PlugConfFile += PLUG_CONF_FILE;

  f.Assign(PlugConfFile.c_str());
  if (!f.FileExists()) // if not found try /etc 
    {
      PlugConfFile = "/etc/";
      ConfDir = PlugConfFile;
      PlugConfFile += PLUG_CONF_FILE;
      f.Assign(PlugConfFile.c_str());
      if (!f.FileExists()) // if not found try in current directory
	{
	  PlugConfFile = PLUG_CONF_FILE;
	  ConfDir = "";
	  f.Assign(PlugConfFile.c_str());
	  if (!f.FileExists()) // plugin conf file not found
	    {
	      PlugConfFile = "";
	    }
	}
    }

  DataDir = INSTALL_PREFIX;
  DataDir += WIRED_DATADIR;
  
  cout << "[SETTINGS] DataDir=" << DataDir << endl;
  
  f.Assign(DataDir.c_str());
  if (!f.DirExists()) // if not found try /usr
    {
      DataDir = "/usr";
      DataDir += WIRED_DATADIR;
      f.Assign(DataDir.c_str());
      if (!f.DirExists()) // if not found try in current directory
	{
	  DataDir = WIRED_DATADIR;
	  f.Assign(DataDir.c_str());
	  if (!f.DirExists()) // plugin conf file not found
	    {
	      DataDir = "";
	    }
	}
    }
  cout << "[SETTINGS] DataDir=" << DataDir << endl;

  f.AssignHomeDir();
  f.AppendDir(WIRED_DIRECTORY);
  
  if (f.Mkdir(0755, wxPATH_MKDIR_FULL))
    {
      conf = new wxConfig("Wired", "P31", WIRED_CONF, WIRED_CONF, wxCONFIG_USE_LOCAL_FILE);  
      Load();
    }
  else
    {
      cout << "[SETTINGS] Could not open config file, access was denied to " << f.GetFullPath().c_str() 
	   << endl;
      throw; // FIXME add a decent object to throw
    }
  cout << "[SETTINGS] DataDir=" << DataDir << endl;

}

Settings::~Settings()
{
  delete conf;
}

void Settings::Load()
{
  int l, val;
  wxString s;

  cout << "[SETTINGS] Saving settings..."<< endl;

  conf->SetPath("/");
  conf->Read("QuickWaveRender", &QuickWaveRender, false);
  conf->Read("dbWaveRender", &dbWaveRender, false);
  conf->Read("OutputDev", &OutputDev, false);
  conf->Read("InputDev", &InputDev, false);
  conf->Read("SampleRate", &SampleRate, 0);
  conf->Read("SampleFormat", &SampleFormat, 0);
  conf->Read("SamplesPerBuffer", &SamplesPerBuffer, 4096);

  conf->SetPath("/OutputChannels");
  for (l = 0; ; l++)
    {
      s.Printf("%d", l);
      if (conf->Read(s, &val, -1))
	OutputChannels.push_back(val);
      else
	break;
    }  
  conf->SetPath("/InputChannels");
  for (l = 0; ; l++)
    {
      s.Printf("%d", l);
      if (conf->Read(s, &val, -1))
	InputChannels.push_back(val);
      else
	break;
    }  
  conf->SetPath("/MidiDevices");
  for (l = 0; ; l++)
    {
      s.Printf("%d", l);
      if (conf->Read(s, &val, -1))
	MidiIn.push_back(val);
      else
	break;
    }  
}

void Settings::Save()
{  
  vector<long>::iterator i;
  int l;
  wxString s;

  conf->SetPath("/");
  conf->Write("QuickWaveRender", QuickWaveRender);
  conf->Write("dbWaveRender", dbWaveRender);
  conf->Write("OutputDev", OutputDev);
  conf->Write("InputDev", InputDev);
  conf->Write("SampleRate", SampleRate);
  conf->Write("SampleFormat", SampleFormat);
  conf->Write("SamplesPerBuffer", SamplesPerBuffer);

  conf->DeleteGroup("/OutputChannels");
  conf->DeleteGroup("/InputChannels");
  conf->DeleteGroup("/MidiDevices");

  conf->SetPath("/OutputChannels");
  for (l = 0, i = OutputChannels.begin(); i != OutputChannels.end(); i++, l++)
    {
      s.Printf("%d", l);
      conf->Write(s, (*i));
    }
  conf->SetPath("/InputChannels");
  for (l = 0, i = InputChannels.begin(); i != InputChannels.end(); i++, l++)
    {
      s.Printf("%d", l);
      conf->Write(s, (*i));
    }
  conf->SetPath("/MidiDevices");
  for (l = 0, i = MidiIn.begin(); i != MidiIn.end(); i++, l++)
    {
      s.Printf("%d", l);
      conf->Write(s, (*i));
    }
}


