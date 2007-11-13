// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "version.h"
#include "Settings.h"
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <iostream>

#include "AudioEngine.h"

#include <config.h>

// we can't move this global var until plugins doesn't use it anymore
// TODO : write missing API functions to handle this
Settings		*WiredSettings = NULL;

Settings::Settings()
{
  wxFileName f;

  SetDefault();

  ConfDir = wxString(wxT(SYSCONF_DIR)) + wxT("/");
  PlugConfFile = ConfDir + PLUG_CONF_FILE;

  f.Assign(PlugConfFile);
  if (!f.FileExists())
    {
      // if not found let hope it belongs the current directory
      ConfDir = wxT("./");
      PlugConfFile = ConfDir + PLUG_CONF_FILE;
    }

  DataDir = wxString(wxT(DATA_DIR)) + wxT("/wired/");
  f.Assign(DataDir);

  if (!f.DirExists())
    {
      // if not found let hope it belongs the current directory
      DataDir = wxT("./");
      f.Assign(DataDir);
    }

  f.AssignDir(f.GetHomeDir() + wxString(wxT("/")) + wxString(WIRED_DIRECTORY));

  HomeDir = wxString(f.GetFullPath());

  if (f.Mkdir(0755, wxPATH_MKDIR_FULL))
    {
      f.SetName(WIRED_CONF);

      if (!f.FileExists())
	FirstLaunch = true;

      conf = new wxFileConfig(WIRED_NAME, wxT("P31"),
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

void Settings::ReadChannels(wxString Group, vector<int>& list)
{
  wxString			s;
  int				l;
  int				val;

  conf->SetPath(Group);
  for (l = 0; ; l++)
    {
      s.Printf(wxT("%d"), l);
      if (!conf->Read(s, &val, -1))
	return;
      list.push_back(val);
    }
}

void Settings::SaveChannels(wxString Group, vector<int>& list)
{
  vector<int>::iterator	i;
  wxString			s;
  int				l;

  if (list.size() == 0)
    return;

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
      cout << "[SETTINGS] Your configuration file is deprecated" << version.mb_str() << endl;
      ConfDeprecated = true;
      return;
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
  conf->Read(wxT("SampleRate"), &SampleRate, (int)DEFAULT_SAMPLE_RATE);
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

vector<wxFileName>	Settings::GetRecentDirs()
{
  vector<wxFileName>	pathList;
  int					i;
  wxString				recent, tmpstr;
  wxFileName			path;
  wxString				Group = wxT("/Recent");
  int					max_recent;
  long					l;

  if(conf)
  {
	conf->SetPath(Group);
	conf->Read(wxT("max_recent"), &tmpstr, wxT(""));
	if (tmpstr != wxT(""))
	  tmpstr.ToLong(&l);
	else
	  l = MAX_RECENT;
	max_recent = (int)l;
	for (i = 0; i < max_recent; i++)
	{
	  recent.Clear();
	  recent << wxT("Recent") << i;
	  conf->Read(recent, &tmpstr, wxT(""));
	  path.Assign(tmpstr);
	  if (path.DirExists())
		pathList.push_back(path);
	}
  }
  else
	cout << "[SETTINGS] GetRecentDirs() : Can't access conf" << endl;
  return (pathList);
}

void	Settings::RemoveRecentDir(int idx)
{
  vector<wxFileName>	pathList;
  int					i;
  long					l;
  wxString				recent, tmpstr;
  wxFileName			path;
  wxString				Group = wxT("/Recent");
  int					max_recent;

  if(conf)
  {
	conf->SetPath(Group);
	conf->Read(wxT("max_recent"), &tmpstr, wxT(""));
	if (tmpstr != wxT(""))
	  tmpstr.ToLong(&l);
	else
	  l = MAX_RECENT;
	max_recent = (int)l;
	for (i = 0; i < max_recent; i++)
	{
	  recent.Clear();
	  recent << wxT("Recent") << i;
	  conf->Read(recent, &tmpstr, wxT(""));
	  path.Assign(tmpstr);
	  if (path.DirExists() && i != idx)
		pathList.push_back(path);
	}
	conf->DeleteGroup(Group);
	if (pathList.size() > 0)
	  for (i = 0; i < pathList.size() && i < max_recent; i++)
	  {
		recent.Clear();
		recent << wxT("Recent") << i;
		conf->Write(recent, pathList[i].GetFullPath());
	  }
  }
  else
	cout << "[SETTINGS] RemoveRecentDir() : Can't access conf" << endl;
}

bool Settings::AddDirToRecent(wxString pathstr)
{
  vector<wxFileName>	pathList;
  wxString				tmpstr, recent;
  wxFileName			newpath, path;
  int					i;
  wxString				Group = wxT("/Recent");
  int					max_recent;
  long					l;

  if(conf)
  {
	conf->SetPath(Group);
	conf->Read(wxT("max_recent"), &tmpstr, wxT(""));
	if (tmpstr != wxT(""))
	  tmpstr.ToLong(&l);
	else
	  l = MAX_RECENT;
	max_recent = (int)l;
	newpath.Assign(pathstr);
	newpath.MakeAbsolute();
	cout << "[SETTINGS] Adding '" << newpath.GetFullPath().mb_str() << "' to the recent list" << endl;
	pathList.push_back(newpath);
	for (i = 0; i < max_recent; i++)
	{
	  recent.Clear();
	  recent << wxT("Recent") << i;
	  conf->Read(recent, &tmpstr, wxT(""));
	  path.Assign(tmpstr);
	  // DirExists or IsOk ?
	  if (path.DirExists())
		if (newpath != path)
		  pathList.push_back(path);
	}
	conf->DeleteGroup(Group);
	for (i = 0; i < pathList.size() && i < max_recent; i++)
	{
	  recent.Clear();
	  recent << wxT("Recent") << i;
	  conf->Write(recent, pathList[i].GetFullPath());
	}
  }
  else
	cout << "[SETTINGS] AddDirToRecent() : Can't access conf" << endl;
}
