// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "Device.h"

bool Device::GetSupportedSettings(void)
{
  const PaDeviceInfo	*info = 0x0;
  
  if ((info = Pa_GetDeviceInfo( Id )) == NULL)
    {
      cout << "[AUDIO] Error Pa_GetDeviceInfo() bad return value" 
	   << endl;
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
    
  InParams.device = Id;
  InParams.channelCount = MaxInputChannels;
  InParams.suggestedLatency = InputLatencyRange[MIN];
  
  OutParams.device = Id;
  OutParams.channelCount = MaxOutputChannels;
  
  OutParams.suggestedLatency = OutputLatencyRange[MIN];
  
  InParams.hostApiSpecificStreamInfo = NULL;
  OutParams.hostApiSpecificStreamInfo = NULL;

  PaError err;
  DeviceFormat *f;
  for (int sf = 0; sf < MAX_SAMPLE_FORMATS; sf++)
    {
      f = new DeviceFormat();
      OutParams.sampleFormat = standardSampleFormats[sf];
      InParams.sampleFormat = standardSampleFormats[sf];
      f->SampleFormat = standardSampleFormats[sf];
      // cout << "[DEVICE] Testing format index: " << sf << endl;
      vector<double> SampleRates;
      for( int i = 0; standardSampleRates[i] > 0; i++ )
	{
	  err = 
	    Pa_IsFormatSupported( &InParams, &OutParams, 
				  standardSampleRates[i] );
	  if( err == paFormatIsSupported )
	    {/*
	      cout << "[DEVICE] New Sample Rate Supported: " 
		   << standardSampleRates[i]
		   << endl; */
	      f->SampleRates.push_back(standardSampleRates[i]);
	    }
	}
      SupportedFormats.push_back(f);
    }
  return true;
}





