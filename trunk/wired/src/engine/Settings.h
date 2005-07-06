// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <wx/config.h>
#include <vector>
#include <string>

using namespace std;

#define WIRED_DIRECTORY ".wired"
#define WIRED_CONF	".wired/wired.conf"
#define PLUG_CONF_FILE  "wired_plugins.conf"
#define WIRED_DATADIR   "/share/wired/data/"

class Settings
{
 public:
  Settings();
  ~Settings();

  // Options
  bool QuickWaveRender;
  bool dbWaveRender;
  unsigned int	maxUndoRedoDepth;
  
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

  string PlugConfFile;
  string ConfDir;
  string DataDir;

  void Load();
  void Save();

 protected:
  wxConfig *conf;

 private:
};

extern Settings *WiredSettings;

#endif
