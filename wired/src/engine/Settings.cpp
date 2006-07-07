// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "version.h"
#include "Settings.h"
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <iostream>

#include "AudioEngine.h"

#include "config.h"

// we can't move this global var until plugins doesn't use it anymore
// TODO : write missing API functions to handle this
Settings		*WiredSettings = NULL;

Settings::Settings() :
  QuickWaveRender(false), dbWaveRender(false), OutputSystem(0), InputSystem(0),
  OutputDev(0), InputDev(0), OutputLatency(-1), InputLatency(-1),
  SampleRate(DEFAULT_SAMPLE_RATE_INT), SamplesPerBuffer(DEFAULT_SAMPLES_PER_BUFFER),
  SampleFormat(DEFAULT_SAMPLE_FORMAT), maxUndoRedoDepth(20), WorkingDir(wxT(""))
{
  wxFileName f;

  ConfDir = wxString(wxT(INSTALL_PREFIX)) + wxString(wxT("/etc/"));
  PlugConfFile = ConfDir + PLUG_CONF_FILE;

  f.Assign(PlugConfFile);
  if (!f.FileExists()) // if not found try /etc 
    {
      ConfDir = wxT("/etc/");
      PlugConfFile = ConfDir + wxString(PLUG_CONF_FILE);
      f.Assign(PlugConfFile);
      if (!f.FileExists()) // if not found let hope it belongs the current directory
	{
	  ConfDir = wxT("./");
	  PlugConfFile = ConfDir + wxString(PLUG_CONF_FILE);
	}
    }

  DataDir = wxT(INSTALL_PREFIX);
  DataDir += WIRED_DATADIR;
  
  f.Assign(DataDir);
  if (!f.DirExists()) // if not found try /usr
    {
      DataDir = wxT("/usr");
      DataDir += WIRED_DATADIR;
      f.Assign(DataDir);
      if (!f.DirExists()) // if not found let hope it belongs the current directory
	DataDir = WIRED_DATADIR;
    }

  f.AssignDir(f.GetHomeDir() + wxString(wxT("/")) + wxString(WIRED_DIRECTORY));

  if (f.Mkdir(0755, wxPATH_MKDIR_FULL))
    {
      f.SetName(WIRED_CONF);

      cout << "test path : " << f.GetPath().mb_str() << endl;
      cout << "test file : " << f.GetName().mb_str() << endl;
      if (!f.FileExists())
	{
	  wxString	welcome;

	  welcome = wxT("Welcome to version WIRED_VERSION of WIRED_NAME.\nWIRED_NAME is currently a beta software, some of its features may not work completly yet.\nWe recommend to do not use the following features at this time :\n- Undo/Redo\n- Drag and drop of plugins\n- Codec management is known to be quite unstable at this time\n\nThe next step is to configure your soundcard settings in WIRED_NAME Settings dialog.Select 32 bits float for sample format, 44100hz for sample rate (or whatever you prefer) and choose a latency which your soundcard is capable of. You can try different values (the lower the most realtime WIRED_NAME will perform) and see which one is the best for your soundcard. Setting the latency too low will cause drops and glitch to appear in the sound output.\n\nYou will find in the Help menu, a \"Show integrated help\" item which will display an interactive help window on the bottom right corner of WIRED_NAME. If you move your mouse over a control in WIRED_NAME or in a plugin, it will show you the help associated with this item. \nIf you find any bugs in WIRED_NAME, please make a bug report at :\nWIRED_BUGS\n\nIf you need help or want to discuss about WIRED_NAME, pleast visit :\nWIRED_FORUMS");
	  welcome.Replace(wxT("WIRED_VERSION"), WIRED_VERSION);
	  welcome.Replace(wxT("WIRED_NAME"), WIRED_NAME);
	  welcome.Replace(wxT("WIRED_BUGS"), WIRED_BUGS);
	  welcome.Replace(wxT("WIRED_FORUMS"), WIRED_FORUMS);

	  wxMessageDialog msg(0x0, welcome, WIRED_NAME, wxOK | wxICON_INFORMATION | wxCENTRE);
	  msg.ShowModal();
	}

      conf = new wxConfig(WIRED_NAME, wxT("P31"),
			  wxString(WIRED_DIRECTORY) + wxString(WIRED_CONF),
			  wxString(WIRED_DIRECTORY) + wxString(WIRED_CONF),
			  wxCONFIG_USE_LOCAL_FILE);  
      Load();
    }
  else
    {
      cout << "[SETTINGS] Could not open config file, access was denied to " <<
	f.GetFullPath().mb_str() << endl;
      throw; // FIXME add a decent object to throw
    }
}

Settings::~Settings()
{
	if (conf)
		delete conf;  
}

void Settings::ReadChannels(wxString Group, vector<long>& list)
{
  wxString			s;
  long				l;
  long				val;

  conf->SetPath(Group);
  for (l = 0; ; l++)
    {
      s.Printf(wxT("%d"), l);
      if (!conf->Read(s, &val, -1))
	return;
      list.push_back(val);
    }
}

void Settings::SaveChannels(wxString Group, vector<long>& list)
{
  vector<long>::iterator	i;
  wxString			s;
  long				l;

  conf->DeleteGroup(Group);
  conf->SetPath(Group);
  for (l = 0, i = list.begin(); i != list.end(); i++, l++)
    {
      s.Printf(wxT("%d"), l);
      conf->Write(s, (*i));
    }
}

void Settings::Load()
{
  conf->SetPath(wxT("/"));
  conf->Read(wxT("QuickWaveRender"), &QuickWaveRender, false);
  conf->Read(wxT("dbWaveRender"), &dbWaveRender, false);

  conf->Read(wxT("OutputSystem"), &OutputSystem, 0);
  conf->Read(wxT("InputSystem"), &InputSystem, 0);

  conf->Read(wxT("OutputDev"), &OutputDev, 0);
  conf->Read(wxT("InputDev"), &InputDev, 0);

  conf->Read(wxT("SampleRate"), &SampleRate, DEFAULT_SAMPLE_RATE_INT);
  conf->Read(wxT("SampleFormat"), &SampleFormat, 0);
  conf->Read(wxT("SamplesPerBuffer"), &SamplesPerBuffer, DEFAULT_SAMPLES_PER_BUFFER);
  conf->Read(wxT("WorkingDirectory"), &WorkingDir, wxT(""));

  ReadChannels(wxT("/OutputChannels"), OutputChannels);
  ReadChannels(wxT("/InputChannels"), InputChannels);
  ReadChannels(wxT("/MidiDevices"), MidiIn);
}

void Settings::Save()
{  
  conf->SetPath(wxT("/"));
  conf->Write(wxT("QuickWaveRender"), QuickWaveRender);
  conf->Write(wxT("dbWaveRender"), dbWaveRender);

  conf->Write(wxT("OutputSystem"), OutputSystem);
  conf->Write(wxT("InputSystem"), InputSystem);

  conf->Write(wxT("OutputDev"), OutputDev);
  conf->Write(wxT("InputDev"), InputDev);

  conf->Write(wxT("SampleRate"), SampleRate);
  conf->Write(wxT("SampleFormat"), SampleFormat);
  conf->Write(wxT("SamplesPerBuffer"), SamplesPerBuffer);
  conf->Write(wxT("WorkingDirectory"), WorkingDir);
 
  // save output channels
  SaveChannels(wxT("/OutputChannels"), OutputChannels);

  // save input channels
  SaveChannels(wxT("/InputChannels"), InputChannels);

  // save midi settings 
  SaveChannels(wxT("/MidiDevices"), MidiIn);

  conf->Flush();
}
