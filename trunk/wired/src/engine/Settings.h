// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <wx/config.h>
#include <vector>
#include <string>

using namespace std;

#define DEFAULT_SAMPLE_RATE		44100.0
#define DEFAULT_SAMPLES_PER_BUFFER	2048

#define WIRED_DIRECTORY wxT(".wired")
#define WIRED_CONF	wxT(".wired/wired.conf")
#define PLUG_CONF_FILE  wxT("wired_plugins.conf")
#define WIRED_DATADIR   wxT("/share/wired/data/")

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

  wxString PlugConfFile;
  wxString ConfDir;
  wxString DataDir;
  wxString WorkingDir;

  void Load();
  void Save();

 protected:
  wxConfig *conf;

 private:
};

extern Settings *WiredSettings;

#endif
