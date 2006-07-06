// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <wx/config.h>
#include <vector>
#include <string>

using namespace std;

#define WIRED_DATADIR   wxT("/share/wired/data/")
#define WIRED_DIRECTORY wxT(".wired/")
#define WIRED_CONF	wxT("wired.conf")
#define PLUG_CONF_FILE  wxT("wired_plugins.conf")

class Settings
{
 public:
  Settings();
  ~Settings();

  // Options
  bool QuickWaveRender;
  bool dbWaveRender;
  unsigned int	maxUndoRedoDepth;

  long OutputSystem;
  long InputSystem;

  long OutputDev;
  long InputDev;

  double OutputLatency;
  double InputLatency;
  
  long		SampleRate;
  long		SamplesPerBuffer;
  long		SampleFormat;
 
  vector<long>	OutputChannels;
  vector<long>	InputChannels;
  vector<long>	MidiIn;

  wxString PlugConfFile;
  wxString ConfDir;
  wxString DataDir;
  wxString WorkingDir;

  void Load();
  void Save();

 protected:
  wxConfig *conf;

 private:
  void	SaveChannels(wxString Group, vector<long>& list);
  void	ReadChannels(wxString Group, vector<long>& list);
};

extern Settings *WiredSettings;

#endif
