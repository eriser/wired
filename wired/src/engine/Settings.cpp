// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "Settings.h"
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <iostream>

#include "config.h"

Settings *WiredSettings;

Settings::Settings() :
  QuickWaveRender(false), dbWaveRender(false), OutputDev(-1), InputDev(-1), OutputLatency(-1),
  InputLatency(-1), SampleRate(44100), SamplesPerBuffer(2048), SampleFormat(-1), maxUndoRedoDepth(20), WorkingDir("")
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

  f.AssignHomeDir();
  f.AppendDir(WIRED_DIRECTORY);
  
  if (f.Mkdir(0755, wxPATH_MKDIR_FULL))
    {
      f.SetName("wired.conf");  
      if (!f.FileExists())
	{
	  wxMessageDialog msg(0x0, "Welcome to version 0.2 of Wired.\nWired is currently a beta software, some of its features may not work completly yet.\nWe recommend to do not use the following features at this time :\n- Undo/Redo\n- Drag and drop of plugins\n- Codec management is known to be quite unstable at this time\n\nThe next step is to configure your soundcard settings in Wired Settings dialog.Select 32 bits float for sample format, 44100hz for sample rate (or whatever you prefer) and choose a latency which your soundcard is capable of. You can try different values (the lower the most realtime Wired will perform) and see which one is the best for your soundcard. Setting the latency too low will cause drops and glitch to appear in the sound output.\n\nYou will find in the Help menu, a \"Show integrated help\" item which will display an interactive help window on the bottom right corner of Wired. If you move your mouse over a control in Wired or in a plugin, it will show you the help associated with this item. \nIf you find any bugs in Wired, please make a bug report at :\nhttp://bloodshed.net/wired/bugs\n\nIf you need help or want to discuss about Wired, pleast visit :\nhttp://bloodshed.net/wired/forums", "Wired", wxOK | wxICON_INFORMATION | 
			      wxCENTRE);
	  msg.ShowModal();
	}
      conf = new wxConfig("Wired", "P31", WIRED_CONF, WIRED_CONF, wxCONFIG_USE_LOCAL_FILE);  
      Load();
    }
  else
    {
      cout << "[SETTINGS] Could not open config file, access was denied to " << f.GetFullPath().c_str() 
	   << endl;
      throw; // FIXME add a decent object to throw
    }
}

Settings::~Settings()
{
	if (conf)
		delete conf;  
}

void Settings::Load()
{
  int l, val;
  wxString s;

  conf->SetPath("/");
  conf->Read("QuickWaveRender", &QuickWaveRender, false);
  conf->Read("dbWaveRender", &dbWaveRender, false);
  conf->Read("OutputDev", &OutputDev, false);
  conf->Read("InputDev", &InputDev, false);
  conf->Read("SampleRate", &SampleRate, 0);
  conf->Read("SampleFormat", &SampleFormat, 0);
  conf->Read("SamplesPerBuffer", &SamplesPerBuffer, 4096);
  wxString		temp1("");
  wxString		temp2("WorkingDirectory");
  conf->Read(temp2, &WorkingDir, temp1);

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
  conf->Write("WorkingDirectory", WorkingDir);

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


