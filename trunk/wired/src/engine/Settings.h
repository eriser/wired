// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <wx/config.h>
#include <vector>

using namespace std;

#define WIRED_DATADIR   wxT("/share/wired/data/")
#define WIRED_DIRECTORY wxT(".wired/")
#define WIRED_CONF	wxT("wired.conf")
#define PLUG_CONF_FILE  wxT("wired_plugins.conf")

#define DEFAULT_MAXUNDOREDODEPTH (20)

#define WIRED_CONF_VERSION	wxT("1")

class Settings
{
 public:
  Settings();
  ~Settings();

  // Options
  bool		QuickWaveRender;
  bool		dbWaveRender;
  long		maxUndoRedoDepth;

  double	OutputLatency;
  double	InputLatency;
  
  long		SampleRate;
  long		SamplesPerBuffer;
  long		SampleFormat; 

  // string from conf file
  wxString	OutputSystemStr;
  wxString	InputSystemStr;

  wxString	OutputDeviceStr;
  wxString	InputDeviceStr;

  // id not in conf file
  long		OutputSystemId;
  long		InputSystemId;

  long		OutputDeviceId;
  long		InputDeviceId;

  vector<long>	OutputChannels;
  vector<long>	InputChannels;
  vector<long>	MidiIn;

  wxString	PlugConfFile;
  wxString	ConfDir;
  wxString	DataDir;
  wxString	WorkingDir;

  void		Load();
  void		Save();

  inline bool	IsFirstLaunch() { return(FirstLaunch); };
  inline bool	ConfIsDeprecated() { return(ConfDeprecated); };

 protected:
  wxConfig	*conf;

 private:
  bool		FirstLaunch;
  bool		ConfDeprecated;

  void		SetDefault();

  void		SaveChannels(wxString Group, vector<long>& list);
  void		ReadChannels(wxString Group, vector<long>& list);

  void		DeleteDeprecatedEntries();
};

extern Settings *WiredSettings;

#endif
