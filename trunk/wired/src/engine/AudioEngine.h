// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __AUDIOENGINE_H__
#define __AUDIOENGINE_H__

#include <vector>

#include "portaudio.h"
#include <wx/thread.h>

#include "Device.h"
#include "RingBuffer.h"
#include "Settings.h"

#define DEFAULT_SAMPLE_RATE_INT		44100
#define DEFAULT_SAMPLE_RATE		44100.0
#define DEFAULT_SAMPLES_PER_BUFFER	2048
#define DEFAULT_SAMPLE_FORMAT		paFloat32

using namespace std;

extern const char *standardSampleFormats_str[];
extern const unsigned long standardSampleFormats[];
extern const double standardSampleRates[];

typedef struct
{
  int		OutputChannels;
  int		InputChannels;
  unsigned long SampleFormat;
  unsigned long InputSampleFormat;
  unsigned long OutputSampleFormat;
  Settings	*Sets;
  RingBuffer<float>		*OutFIFO;
  vector<RingBuffer<float>*>	OutFIFOVector;
  vector<RingBuffer<float>*>	InFIFOVector;
}		callback_t;

class AudioSystem
{
 private:
  int		_id;
  wxString	_name;

 public:
  AudioSystem(int id, wxString name)
    {
      _id = id;
      _name = name;
    }
  ~AudioSystem();

  wxString	GetName() { return (_name); };
  int		GetId() { return (_id); };
};

class AudioEngine
{
 public:
  AudioEngine();
  ~AudioEngine();

  vector<Device*>	DeviceList;
  vector<AudioSystem*>	SystemList;

  bool			IsOk;
  bool			_paInit;

  AudioSystem		*SelectedOutputSystem;
  Device		*SelectedOutputDevice;
  unsigned long		OutputSampleFormat;
  double		OutputLatency;
  int			OutputChannels;

  AudioSystem		*SelectedInputSystem;
  Device		*SelectedInputDevice;
  unsigned long		InputSampleFormat;
  double		InputLatency;
  int			InputChannels;

  unsigned long		SampleFormat;

  double		SampleRate;
  unsigned long		SamplesPerBuffer;
  double		Latency;

  void			Restart(void);
  void			OpenStream(void);
  bool			CloseStream(void);
  bool			StartStream(void);
  bool			StopStream(void);
  double		GetTime(void);
  double		GetCpuLoad(void);
  void			SetChannels(int in, int out);
  bool			SetOutputChannels(int num);
  bool			SetInputChannels(int num);
  void			ResetChannels(void);
  void			GetDeviceSettings(void);
  int			GetLibSndFileFormat();

  int			GetDefaultAudioSystem();
  int			GetDefaultInputDevice();
  int			GetDefaultOutputDevice();
  Device*		GetDevice(int dev_id, int system_id);

  AudioSystem*		GetAudioSystemByName(wxString name);
  Device*		GetDeviceByName(wxString name);
  int			GetDeviceIdByTrueId(Device* dev);

  void			SetDefaultSettings(void);
  callback_t		*UserData;
  bool			StreamIsOpened;
  bool			StreamIsStarted;

 private:
  void			GetAudioSystems();
  void			GetDevices();
  void			SetInputDevice(void);
  void			SetOutputDevice(void);
  Device*		GetDeviceById(PaDeviceIndex id);
  AudioSystem*		GetAudioSystemById(int id);

  void			AlertDialog(const wxString& from, const wxString& msg);
  PaStream		*Stream;
};

extern AudioEngine	*Audio;

// AudioCallback
int	AudioCallback(const void *input,
			      void *output,
			      unsigned long frameCount,
			      const PaStreamCallbackTimeInfo* timeInfo,
			      PaStreamCallbackFlags statusFlags,
			      void *userData);

// Useful functions for debugging
void		DumpStereoBuffer(float **buffer);


#endif  // __AUDIOENGINE_H__
