// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <iostream>
#include <string>
#include <vector>
#include <portaudio.h>

#define MIN	0
#define MAX	1
#define MAX_SAMPLE_FORMATS 6

using namespace std;

static const unsigned long standardSampleFormats[] =
{
  paFloat32, paInt32, 
  paInt24, paInt16, 
  paUInt8, paInt8
};

static const char *standardSampleFormats_str[] =
{
  "non-interleaved 32 bit float", "non-interleaved 32 bit int",
  "non-interleaved 24 bit int", "non-interleaved 16 bit int",
  "non-interleaved 8 bit unsigned int", "non-interleaved 8 bit int"
};

static double standardSampleRates[] =
{
  8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
  44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1 
  /* negative terminated  list */ 
};
 

class DeviceFormat
{
 public:
  DeviceFormat() {}
  ~DeviceFormat() {}
  
  long SampleFormat;
  vector<double> SampleRates;
};

class Device
{
 public:
  Device(int id, string name, int maxin, int maxout)	
				{					\
				  Id = id; Name = name;			\
				  MaxInputChannels = maxin;		\
				  MaxOutputChannels = maxout;		\
				}
  ~Device();
  
  int		Id;
  string	Name;
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
