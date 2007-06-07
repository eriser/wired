// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <wx/fileconf.h>
#include <vector>

#define WIRED_DIRECTORY wxT(".wired/")
#define WIRED_CONF	wxT("wired.conf")
#define PLUG_CONF_FILE  wxT("wired_plugins.conf")

#define DEFAULT_MAXUNDOREDODEPTH (20)

#define WIRED_CONF_VERSION	wxT("1")

/**
 * Setting for wired.
 * Set Default,read and save channels,Load and save the settings.
 */
class Settings
{
 public:

  /**
   * Default constructor.
   * checking if config file existed.
   */
  Settings();

  /**
   * The main destructor.
   */
  ~Settings();

  /**
   * Quickly render waveforms.
   * The default setting is false.
   */
  bool		QuickWaveRender;

  /**
   * Render waveforms in db mode.
   * The default setting is false.
   */
  bool		dbWaveRender;

  /**
   * Undo Redo maximum depth.
   * The default value is DEFAULT_MAXUNDOREDODEPTH (20).
   */
  long		maxUndoRedoDepth;

  /**
   * the default value is -1.
   */
  double	OutputLatency;

  /**
   * the default value is -1.
   */
  double	InputLatency;

  /**
   * Setting the sample rate.
   * the default value is DEFAULT_SAMPLE_RATE_INT.
   */
  long		SampleRate;

  /**
   * how many samples per buffer.
   * the default value is DEFAULT_SAMPLES_PER_BUFFER.
   */
  long		SamplesPerBuffer;

  /**
   * The sample format.
   * The default value is DEFAULT_SAMPLE_FORMAT.
   */
  long		SampleFormat;

  /**
   * The string of output system from config file.
   */
  wxString	OutputSystemStr;

  /**
   * The string of input system from config file.
   */
  wxString	InputSystemStr;

  /**
   * The string of output device from config file.
   */
  wxString	OutputDeviceStr;

  /**
   * The string of input device from config file.
   */
  wxString	InputDeviceStr;

  /**
   * The default value of output system id is 0.
   */
  long		OutputSystemId;

  /**
   * The default value of input system id is 0.
   */
  long		InputSystemId;

  /**
   * The default value of output device id is 0.
   */
  long		OutputDeviceId;

  /**
   * The default value of input device id is 0.
   */
  long		InputDeviceId;

  /**
   * Output channels.
   */
  std::vector<int>	OutputChannels;

  /**
   * Input channels.
   */
  std::vector<int>	InputChannels;

  /**
   * The midi devices.
   */
  std::vector<int>	MidiIn;

  std::vector<wxString> MidiInStr;

  /**
   * Setting a path to put a config file of plugin.
   */
  wxString	PlugConfFile;

  /**
   * Setting a directory of config file.
   */
  wxString	ConfDir;

  /**
   * Setting a directory of data.
   */
  wxString	DataDir;

  /**
   * Setting a working directory.
   */
  wxString	WorkingDir;

  /**
   * Setting a user home directory.
   */
  wxString	HomeDir;

  /**
   * Loading the settings.
   * setting the path and checking if it's wired config version.
   */
  void		Load();

  /**
   * Delete deprecated entries and saving all settings.
   */
  void		Save();

  /**
   * Checking if it's first launch.
   * \return returns a bool,if it's first launch,then it returns TRUE.
   * else it returns FALSE.
   */
  inline bool	IsFirstLaunch() { return(FirstLaunch); };

  /**
   * Checking if the configuration is deprecated.
   * \return returns a bool,if it's deprecated then it returns TRUE.
   * else it returns FALSE.
   */
  inline bool	ConfIsDeprecated() { return(ConfDeprecated); };

 protected:
  wxFileConfig	*conf;

 private:

  /**
   * Default value is false.
   */
  bool		FirstLaunch;

  /**
   * Default value is false.
   */
  bool		ConfDeprecated;

  /**
   * Setting all defaults.
   */
  void		SetDefault();

  /**
   * Saving channels.
   * \param Group a wxString,the name of title.
   * \param list a vector<int>&,the list of channels.
   */
  void		SaveChannels(wxString Group, std::vector<int>& list);

  /**
   * Reading channels.
   * \param Group a wxString,the name of title.
   * \param list a vector<int>&,the list of channels.
   */
  void		ReadChannels(wxString Group, std::vector<int>& list);

  /**
   * Delete deprecated entries.
   */
  void		DeleteDeprecatedEntries();
};


extern Settings *WiredSettings;

#endif
