// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <iostream>
#include "AudioEngine.h"
#include "Device.h"

using namespace std;

Device::~Device()
{
	vector<DeviceFormat *>::iterator	Iter;
	for (Iter = SupportedFormats.begin(); Iter != SupportedFormats.end(); Iter++)
		if (*Iter)
			delete *Iter;
}

bool Device::GetSupportedSettings(void)
{
  const PaDeviceInfo	*info = NULL;
  
  info = Pa_GetDeviceInfo(Id);

  if (info == NULL)
    {
      cout << "[AUDIO] Error Pa_GetDeviceInfo() bad return value" << endl;
      return (false);
    }
  /*cout << "[DEVICE] info Low In Latency: " << info->defaultLowInputLatency 
       << ", " << "High In Latency: " << info->defaultHighInputLatency 
       << endl;
  cout << "[DEVICE] info Low Out Latency: " << info->defaultLowOutputLatency 
       << ", " << "High Out Latency: " << info->defaultHighOutputLatency 
       << endl;
  */
  
  OutputLatencyRange[MIN] = info->defaultLowInputLatency;
  OutputLatencyRange[MAX] = info->defaultHighInputLatency;
  
  InputLatencyRange[MIN] = info->defaultLowOutputLatency;
  InputLatencyRange[MAX] = info->defaultHighOutputLatency;
  
  PaStreamParameters InParams, OutParams;

  memset(&InParams, 0, sizeof(InParams));
  InParams.device = Id;
  InParams.channelCount = MaxInputChannels;
  InParams.suggestedLatency = InputLatencyRange[MIN];
  InParams.hostApiSpecificStreamInfo = NULL;

  memset(&OutParams, 0, sizeof(OutParams));
  OutParams.device = Id;
  OutParams.channelCount = MaxOutputChannels;
  OutParams.suggestedLatency = OutputLatencyRange[MIN];
  OutParams.hostApiSpecificStreamInfo = NULL;
  
  PaError err;
  DeviceFormat *f;
  for (int sf = 0; sf < MAX_SAMPLE_FORMATS; sf++)
    {
      InParams.sampleFormat = standardSampleFormats[sf];
      OutParams.sampleFormat = standardSampleFormats[sf];

      f = new DeviceFormat();
      f->SampleFormat = standardSampleFormats[sf];
      for (int i = 0; standardSampleRates[i] > 0; i++)
	{
	  //If Max*Channels is 0, we have to give NULL to Portaudio.
	  err = Pa_IsFormatSupported((MaxInputChannels > 0 ? &InParams : NULL),
				     (MaxOutputChannels > 0 ? &OutParams : NULL),
				     standardSampleRates[i]);
	  if( err == paFormatIsSupported )
	    f->SampleRates.push_back(standardSampleRates[i]);
	}
      SupportedFormats.push_back(f);
    }
  return true;
}
