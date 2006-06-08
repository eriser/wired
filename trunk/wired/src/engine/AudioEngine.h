// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __AUDIOENGINE_H__
#define __AUDIOENGINE_H__

#include <portaudio.h>
#include "Device.h"

//#include "MainWindow.h"


#include <iostream>
#include <string>
#include <vector>
#include <wx/thread.h>
#include "RingBuffer.h"
#include "Settings.h"

#define DEFAULT_SAMPLE_RATE		44100.0
#define DEFAULT_SAMPLES_PER_BUFFER	4096

using namespace std;


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

class AudioEngine
{
 public:
  AudioEngine(); 
  ~AudioEngine();
  
  vector<Device*>	DeviceList;
  
  bool			IsOk;
  bool			_paInit;
  
  Device		*SelectedOutputDevice;
  unsigned long		OutputSampleFormat;
  double		OutputLatency;
  int			OutputChannels;
  
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

  void			SetDefaultSettings(void);
  callback_t		*UserData;
  bool			StreamIsOpened;
  bool			StreamIsStarted;
 private:
  void			GetDevices();
  void			SetInputDevice(void);
  void			SetOutputDevice(void);
  Device		*GetDeviceById(PaDeviceIndex id);
  void			AlertDialog(const wxString& from, const wxString& msg);
  PaStream		*Stream;
};

extern AudioEngine	*Audio;

// AudioCallback
static int	AudioCallback(const void *input,
			      void *output,
			      unsigned long frameCount,
			      const PaStreamCallbackTimeInfo* timeInfo,
			      PaStreamCallbackFlags statusFlags, 
			      void *userData)
{
  if (!userData)
    return (0);

  callback_t *data = (callback_t*)userData;
  unsigned long bytes = frameCount, processed = 0;
  float **outputs = (float**)output;
  float **inputs  =(float**)input;
  
  
  int nchan = 0;
  vector<long>::iterator chan;
  
  if (data->SampleFormat & paFloat32)
    {
    if (data->OutFIFOVector.size() > 0)
    {
	      for (processed = 0, chan = data->Sets->OutputChannels.begin(); 
		   chan != data->Sets->OutputChannels.end();
		   chan++, nchan++)
		{
		  processed = data->OutFIFOVector[nchan]->Read(outputs[*chan], bytes);
		  if (processed < bytes)
		    for (; processed < bytes; processed++)
		      outputs[*chan][processed] = 0.f;
		}
    }
	if (data->InFIFOVector.size() > 0)
	{
	      nchan = 0;
	      for (processed = 0, chan = data->Sets->InputChannels.begin();
		   chan != data->Sets->InputChannels.end();
		   chan++, nchan++)
		{
		  processed = data->InFIFOVector[nchan]->Write(inputs[*chan], bytes);
		  /*if (processed != bytes)
		    cout << "[AUDIO] Frame drop while recording" << endl;
		  */
		}
	}
    }
  else if ( data->SampleFormat & paInt32 )
    {
      ;
    }
  else if ( data->SampleFormat & paInt24 )
    ;
  else if ( data->SampleFormat & paInt16 )
    ;
  else if ( data->SampleFormat & paUInt8 )
    ;
  else if ( data->SampleFormat & paInt8 )
    ;
  return (0);
}

#endif  // __AUDIOENGINE_H__
