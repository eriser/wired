// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <iostream>
#include <vector>
#include "portaudio.h"
#include <wx/wx.h>

#define MIN	0
#define MAX	1
#define MAX_SAMPLE_FORMATS 6

using namespace std;
 

class DeviceFormat
{
 public:
  DeviceFormat() { SampleFormat = 0; }
  ~DeviceFormat() {}
  
  long SampleFormat;
  vector<double> SampleRates;
};

class AudioSystem;

class Device
{
 public:
  Device(int id, wxString name, int maxin, int maxout, AudioSystem* hostsystem)
				{					\
				  Id = id; Name = name;			\
				  MaxInputChannels = maxin;		\
				  MaxOutputChannels = maxout;		\
				  Host = hostsystem;			\
				};
  ~Device();
  
  int		Id;
  AudioSystem*	Host;
  wxString	Name;
  int		MaxInputChannels;
  int		MaxOutputChannels;
  double	SuggestedOutputLatency;
  double	OutputLatencyRange[2];
  double	InputLatencyRange[2];
  vector<DeviceFormat *> SupportedFormats;
  bool		GetSupportedSettings(void);
 private:
};

#endif//__DEVICE_H__
