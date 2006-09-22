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

Settings::Settings()
{
  SetDefault();

  ConfDir = wxString(wxT(INSTALL_PREFIX)) + wxString(wxT("/etc/"));
  PlugConfFile = ConfDir + PLUG_CONF_FILE;

  wxFileName f;

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

      if (!f.FileExists())
	FirstLaunch = true;

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

void Settings::SetDefault()
{
  QuickWaveRender = false;
  dbWaveRender = false;
  OutputSystemId = 0;
  InputSystemId = 0;
  OutputDeviceId = 0;
  InputDeviceId = 0;
  OutputLatency = -1;
  InputLatency = -1;
  SampleRate = DEFAULT_SAMPLE_RATE_INT;
  SamplesPerBuffer = DEFAULT_SAMPLES_PER_BUFFER;
  SampleFormat = DEFAULT_SAMPLE_FORMAT;
  maxUndoRedoDepth = DEFAULT_MAXUNDOREDODEPTH;
  WorkingDir = wxT("");

  // private vars
  FirstLaunch = false;
  ConfDeprecated = false;
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
  // general part
  wxString	version;
 
  conf->SetPath(wxT("/General"));
  conf->Read(wxT("confVersion"), &version, wxT("0"));
  if (version != WIRED_CONF_VERSION)
    {
      cout << "[SETTINGS] You've a deprecated conf " << version.mb_str() << endl;
      ConfDeprecated = true;
    }

  conf->Read(wxT("QuickWaveRender"), &QuickWaveRender, false);
  conf->Read(wxT("dbWaveRender"), &dbWaveRender, false);
  conf->Read(wxT("maxUndoRedoDepth"), &maxUndoRedoDepth, DEFAULT_MAXUNDOREDODEPTH);
  conf->Read(wxT("WorkingDirectory"), &WorkingDir, wxT(""));

  // audio part, system and device
  conf->SetPath(wxT("/AudioSoundCard"));
  conf->Read(wxT("OutputSystemStr"), &OutputSystemStr, wxT(""));
  conf->Read(wxT("InputSystemStr"), &InputSystemStr, wxT(""));

  conf->Read(wxT("OutputDeviceStr"), &OutputDeviceStr, wxT(""));
  conf->Read(wxT("InputDeviceStr"), &InputDeviceStr, wxT(""));

  // audio part, options
  conf->SetPath(wxT("/AudioOptions"));
  conf->Read(wxT("SampleRate"), &SampleRate, DEFAULT_SAMPLE_RATE_INT);
  conf->Read(wxT("SampleFormat"), &SampleFormat, 0);
  conf->Read(wxT("SamplesPerBuffer"), &SamplesPerBuffer, DEFAULT_SAMPLES_PER_BUFFER);

  ReadChannels(wxT("/OutputChannels"), OutputChannels);
  ReadChannels(wxT("/InputChannels"), InputChannels);
  ReadChannels(wxT("/MidiDevices"), MidiIn);
}

void Settings::Save()
{
  // erase all deprecated entries
  DeleteDeprecatedEntries();


  // general part
  conf->SetPath(wxT("/General"));
  conf->Write(wxT("confVersion"), WIRED_CONF_VERSION);
  conf->Write(wxT("QuickWaveRender"), QuickWaveRender);
  conf->Write(wxT("dbWaveRender"), dbWaveRender);
  conf->Write(wxT("maxUndoRedoDepth"), maxUndoRedoDepth);
  conf->Write(wxT("WorkingDirectory"), WorkingDir);

  // audio part, system and device
  conf->SetPath(wxT("/AudioSoundCard"));
  conf->Write(wxT("OutputSystemStr"), OutputSystemStr);
  conf->Write(wxT("InputSystemStr"), InputSystemStr);

  conf->Write(wxT("OutputDeviceStr"), OutputDeviceStr);
  conf->Write(wxT("InputDeviceStr"), InputDeviceStr);

  // audio part, options
  conf->SetPath(wxT("/AudioOptions"));
  conf->Write(wxT("SampleRate"), SampleRate);
  conf->Write(wxT("SampleFormat"), SampleFormat);
  conf->Write(wxT("SamplesPerBuffer"), SamplesPerBuffer);
 
  SaveChannels(wxT("/OutputChannels"), OutputChannels);
  SaveChannels(wxT("/InputChannels"), InputChannels);
  SaveChannels(wxT("/MidiDevices"), MidiIn);

  // be sure to save it
  conf->Flush();
  ConfDeprecated = false;
}

void Settings::DeleteDeprecatedEntries()
{
  conf->DeleteEntry(wxT("QuickWaveRender"));
  conf->DeleteEntry(wxT("dbWaveRender"));
  conf->DeleteEntry(wxT("maxUndoRedoDepth"));
  conf->DeleteEntry(wxT("WorkingDirectory"));
  conf->DeleteEntry(wxT("OutputSystem"));
  conf->DeleteEntry(wxT("InputSystem"));
  conf->DeleteEntry(wxT("OutputDevice"));
  conf->DeleteEntry(wxT("InputDevice"));
  conf->DeleteEntry(wxT("SampleRate"));
  conf->DeleteEntry(wxT("SampleFormat"));
  conf->DeleteEntry(wxT("SamplesPerBuffer"));
}
