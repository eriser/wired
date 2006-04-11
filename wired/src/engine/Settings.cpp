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
  InputLatency(-1), SampleRate(44100), SamplesPerBuffer(2048), SampleFormat(-1), maxUndoRedoDepth(20), WorkingDir(wxT(""))
{
  wxFileName f;

  ConfDir = wxT(INSTALL_PREFIX);
  ConfDir += wxT("/etc/");
  PlugConfFile = ConfDir;
  PlugConfFile += PLUG_CONF_FILE;

  f.Assign(PlugConfFile);
  if (!f.FileExists()) // if not found try /etc 
    {
      PlugConfFile = wxT("/etc/");
      ConfDir = PlugConfFile;
      PlugConfFile += PLUG_CONF_FILE;
      f.Assign(PlugConfFile);
      if (!f.FileExists()) // if not found try in current directory
	{
	  PlugConfFile = PLUG_CONF_FILE;
	  ConfDir = wxT("");
	  f.Assign(PlugConfFile);
	  if (!f.FileExists()) // plugin conf file not found
	    {
	      PlugConfFile = wxT("");
	    }
	}
    }

  DataDir = wxT(INSTALL_PREFIX);
  DataDir += WIRED_DATADIR;
  
  f.Assign(DataDir.c_str());
  if (!f.DirExists()) // if not found try /usr
    {
      DataDir = wxT("/usr");
      DataDir += WIRED_DATADIR;
      f.Assign(DataDir);
      if (!f.DirExists()) // if not found try in current directory
	{
	  DataDir = WIRED_DATADIR;
	  f.Assign(DataDir.c_str());
	  if (!f.DirExists()) // plugin conf file not found
	    {
	      DataDir = wxT("");
	    }
	}
    }

  f.AssignHomeDir();
  f.AppendDir(WIRED_DIRECTORY);
  
  if (f.Mkdir(0755, wxPATH_MKDIR_FULL))
    {
      f.SetName(wxT("wired.conf"));
      if (!f.FileExists())
	{
	  wxMessageDialog msg(0x0, wxString("Welcome to version 0.2 of Wired.\nWired is currently a beta software, some of its features may not work completly yet.\nWe recommend to do not use the following features at this time :\n- Undo/Redo\n- Drag and drop of plugins\n- Codec management is known to be quite unstable at this time\n\nThe next step is to configure your soundcard settings in Wired Settings dialog.Select 32 bits float for sample format, 44100hz for sample rate (or whatever you prefer) and choose a latency which your soundcard is capable of. You can try different values (the lower the most realtime Wired will perform) and see which one is the best for your soundcard. Setting the latency too low will cause drops and glitch to appear in the sound output.\n\nYou will find in the Help menu, a \"Show integrated help\" item which will display an interactive help window on the bottom right corner of Wired. If you move your mouse over a control in Wired or in a plugin, it will show you the help associated with this item. \nIf you find any bugs in Wired, please make a bug report at :\nhttp://bloodshed.net/wired/bugs\n\nIf you need help or want to discuss about Wired, pleast visit :\nhttp://bloodshed.net/wired/forums", *wxConvCurrent), 
                    wxT("Wired"), wxOK | wxICON_INFORMATION | wxCENTRE);
	  msg.ShowModal();
	}
      conf = new wxConfig(wxT("Wired"), wxT("P31"), WIRED_CONF, WIRED_CONF, wxCONFIG_USE_LOCAL_FILE);  
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

  conf->SetPath(wxT("/"));
  conf->Read(wxT("QuickWaveRender"), &QuickWaveRender, false);
  conf->Read(wxT("dbWaveRender"), &dbWaveRender, false);
  conf->Read(wxT("OutputDev"), &OutputDev, false);
  conf->Read(wxT("InputDev"), &InputDev, false);
  conf->Read(wxT("SampleRate"), &SampleRate, 0);
  conf->Read(wxT("SampleFormat"), &SampleFormat, 0);
  conf->Read(wxT("SamplesPerBuffer"), &SamplesPerBuffer, 4096);
  wxString		temp1(wxT(""));
  wxString		temp2(wxT("WorkingDirectory"));
  conf->Read(temp2, &WorkingDir, temp1);

  conf->SetPath(wxT("/OutputChannels"));
  for (l = 0; ; l++)
    {
      s.Printf(wxT("%d"), l);
      if (conf->Read(s, &val, -1))
	OutputChannels.push_back(val);
      else
	break;
    }  
  conf->SetPath(wxT("/InputChannels"));
  for (l = 0; ; l++)
    {
      s.Printf(wxT("%d"), l);
      if (conf->Read(s, &val, -1))
	InputChannels.push_back(val);
      else
	break;
    }  
  conf->SetPath(wxT("/MidiDevices"));
  for (l = 0; ; l++)
    {
      s.Printf(wxT("%d"), l);
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

  conf->SetPath(wxT("/"));
  conf->Write(wxT("QuickWaveRender"), QuickWaveRender);
  conf->Write(wxT("dbWaveRender"), dbWaveRender);
  conf->Write(wxT("OutputDev"), OutputDev);
  conf->Write(wxT("InputDev"), InputDev);
  conf->Write(wxT("SampleRate"), SampleRate);
  conf->Write(wxT("SampleFormat"), SampleFormat);
  conf->Write(wxT("SamplesPerBuffer"), SamplesPerBuffer);
  conf->Write(wxT("WorkingDirectory"), WorkingDir);
  
 
  conf->DeleteGroup(wxT("/OutputChannels"));
  conf->DeleteGroup(wxT("/InputChannels"));
  conf->DeleteGroup(wxT("/MidiDevices"));

  conf->SetPath(wxT("/OutputChannels"));
  for (l = 0, i = OutputChannels.begin(); i != OutputChannels.end(); i++, l++)
    {
      s.Printf(wxT("%d"), l);
      conf->Write(s, (*i));
    }
  conf->SetPath(wxT("/InputChannels"));
  for (l = 0, i = InputChannels.begin(); i != InputChannels.end(); i++, l++)
    {
      s.Printf(wxT("%d"), l);
      conf->Write(s, (*i));
    }
  conf->SetPath(wxT("/MidiDevices"));
  for (l = 0, i = MidiIn.begin(); i != MidiIn.end(); i++, l++)
    {
      s.Printf(wxT("%d"), l);
      conf->Write(s, (*i));
      }
}


