// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include	<unistd.h>
#include	<sndfile.h>
#include	"AudioEngine.h"
#include	"MainWindow.h"
#include	"EngineError.h"

wxMutex		AudioMutex;

AudioEngine::AudioEngine() 
{
  IsOk = true;
  StreamIsOpened = false;
  StreamIsStarted = false;
  UserData = new callback_t;

  PaError err = Pa_Initialize();
  if ( err != paNoError ) 
    throw Error::InitFailure(Pa_GetErrorText (err));
  cout << "[AUDIO] Portaudio initialized" << endl; 
}

AudioEngine::~AudioEngine()
{
  StopStream();
  CloseStream();
  
  PaError err = Pa_Terminate();
  if (err != paNoError)
    {
      cout << "[AUDIO] Error Terminate(): " << Pa_GetErrorText (err) << endl; 
    }
  else
    {
      cout << "[AUDIO] Portaudio unloaded" << endl;
    }
}


void AudioEngine::SetDefaultSettings(void)
{
  WiredSettings->OutputDev = 1;//Pa_GetDefaultOutputDevice();
  WiredSettings->InputDev = 1;//Pa_GetDefaultOutputDevice();
  SelectedOutputDevice = GetDeviceById(Pa_GetDefaultOutputDevice());
  SelectedInputDevice = SelectedOutputDevice;
  
  WiredSettings->OutputChannels.push_back(0);
  WiredSettings->OutputChannels.push_back(1);
  //WiredSettings->InputChannels = 0;
  
  SamplesPerBuffer = 4096;
  SampleRate = 44100.0;
  UserData->SampleFormat = paFloat32;
  
  /*int i = 0;
  for (vector<DeviceFormat*>::iterator df = 
	 SelectedOutputDevice->SupportedFormats.begin();
       df != SelectedOutputDevice->SupportedFormats.end();
       df++)
    {
      
      cout << "id " << i++ << " " << (*df)->SampleRates.size() << endl;
      for (vector<double>::iterator it = (*df)->SampleRates.begin(); 
	   it != (*df)->SampleRates.end();
	   it++ )
	{
	  cout << *it << "\t"<< endl;
	}
      
    }
  */
  
}


void AudioEngine::SetOutputDevice(void)
{
  if ((SelectedOutputDevice = GetDeviceById(WiredSettings->OutputDev))
      == 0x0)
    if ( (SelectedOutputDevice = GetDeviceById(Pa_GetDefaultOutputDevice())) 
	 == 0x0)
      {
	throw Error::InvalidDeviceSettings();
      }
  
  /*
    cout << "num format supported "
       << SelectedOutputDevice->SupportedFormats.size() << endl;
       for (vector<DeviceFormat*>::iterator df = 
	 SelectedOutputDevice->SupportedFormats.begin();
       df != SelectedOutputDevice->SupportedFormats.end();
       df++)
    {
      cout << (*df)->SampleRates.size() << endl;
      for (vector<double>::iterator it = (*df)->SampleRates.begin(); 
	   it != (*df)->SampleRates.end();
	   it++ )
	{
	  cout << *it << "\t"<< endl;
	}
      
	}
  */
  if ( SelectedOutputDevice->SupportedFormats.size() > 
       WiredSettings->SampleFormat )
    {
      UserData->SampleFormat = SelectedOutputDevice->
	SupportedFormats[WiredSettings->SampleFormat]->SampleFormat;
      //cout << "sample format " << WiredSettings->SampleFormat << endl;
      //cout << "num rates supported " << SelectedOutputDevice->
      //SupportedFormats[WiredSettings->SampleFormat]->SampleRates.size() 
      //<< endl;
      if ( SelectedOutputDevice->
	   SupportedFormats[WiredSettings->SampleFormat]->SampleRates.size() 
	   > WiredSettings->SampleRate )
	SampleRate = SelectedOutputDevice->
	  SupportedFormats[WiredSettings->SampleFormat]->
	  SampleRates[WiredSettings->SampleRate];
      else
	{
	  cout << "output sample rate"  << endl;
	  throw Error::InvalidDeviceSettings();
	}
    }
  else
    {
      cout << "output sample format"  << endl;
      throw Error::InvalidDeviceSettings();
    }
  
  
  
  cout << "[AUDIO] OUTPUT MIN LATENCY " 
       << SelectedOutputDevice->OutputLatencyRange[MIN] << endl; 
  cout << "[AUDIO] OUTPUT MAX LATENCY " 
       << SelectedOutputDevice->OutputLatencyRange[MAX] << endl; 
  cout << "[AUDIO] SUGGESTED LATENCY: " 
       << (double)((double)SamplesPerBuffer / (double)SampleRate)
       << endl;
  if ( (WiredSettings->OutputLatency > 
	SelectedOutputDevice->OutputLatencyRange[MAX]) ||
       (WiredSettings->OutputLatency < 
	SelectedOutputDevice->OutputLatencyRange[MIN]) )
    {
      //TO FIX, calculer la latence par rapport au SamplesPerBuffer
      //TO FIX, utiliser eventuellement la variable 
      
      OutputLatency =  SelectedOutputDevice->OutputLatencyRange[MIN];
    }
  else
    {
      OutputLatency = WiredSettings->OutputLatency;
    }
  
  if (WiredSettings->OutputChannels.size() <= 0)
    UserData->OutputChannels = 2;//SelectedOutputDevice->MaxOutputChannels;
  else
    UserData->OutputChannels = UserData->Sets->OutputChannels.size();
}

void AudioEngine::SetInputDevice(void)
{
  if (WiredSettings->InputChannels.size() > 0)
    {
      if (( SelectedInputDevice = GetDeviceById(WiredSettings->InputDev))
	  == 0x0)
	if ( (SelectedInputDevice = 
	      GetDeviceById(Pa_GetDefaultInputDevice())) 
	     == 0x0)
	  {
	    cout << "input" << endl;
	    throw Error::InvalidDeviceSettings();
	  }
    }
  UserData->InputChannels = UserData->Sets->InputChannels.size();
  
}

void AudioEngine::GetDeviceSettings()
{
  UserData->Sets = WiredSettings;
  
  // assimile a la latence possible
  if (WiredSettings->SamplesPerBuffer)
    SamplesPerBuffer = WiredSettings->SamplesPerBuffer;
  else
    SamplesPerBuffer = DEFAULT_SAMPLES_PER_BUFFER;
  
  SetOutputDevice();
  SetInputDevice();
  
  //SampleRate = 44100.0;
  Latency = (double)((double)SamplesPerBuffer / (double)SampleRate);
    
  
  
  return SetChannels(UserData->InputChannels, 
		     UserData->OutputChannels);
}

#if 0
void AudioEngine::GetDeviceSettings()
{
  UserData->Sets = WiredSettings;
  
  SetOutputDevice();
  SetInputDevice();
  
  if ((SelectedOutputDevice = GetDeviceById(WiredSettings->OutputDev))
      == 0x0)
    if ( (SelectedOutputDevice = GetDeviceById(Pa_GetDefaultOutputDevice())) 
	 == 0x0)
      throw Error::InvalidDeviceSettings();
  
  if (WiredSettings->InputChannels.size() > 0)
    if (( SelectedInputDevice = GetDeviceById(WiredSettings->InputDev))
	== 0x0)
      if ( (SelectedInputDevice = GetDeviceById(Pa_GetDefaultInputDevice())) 
	   == 0x0)
	throw Error::InvalidDeviceSettings();
  
  cout << "[AUDIO] OUTPUT MIN LATENCY " 
       << SelectedOutputDevice->OutputLatencyRange[MIN] << endl; 
  cout << "[AUDIO] OUTPUT MAX LATENCY " 
       << SelectedOutputDevice->OutputLatencyRange[MAX] << endl; 
  
  cout << "[AUDIO] INPUT MIN LATENCY " 
       << SelectedInputDevice->InputLatencyRange[MIN] << endl; 
  cout << "[AUDIO] INPUT MAX LATENCY "
       << SelectedInputDevice->InputLatencyRange[MAX] << endl; 
  
  if ( (WiredSettings->OutputLatency > 
	SelectedOutputDevice->OutputLatencyRange[MAX]) ||
       (WiredSettings->OutputLatency < 
	SelectedOutputDevice->OutputLatencyRange[MIN]) ||
       (WiredSettings->InputLatency > 
	SelectedInputDevice->InputLatencyRange[MAX]) ||
       (WiredSettings->InputLatency < 
	SelectedInputDevice->InputLatencyRange[MIN]) )
    {
      OutputLatency =  SelectedOutputDevice->OutputLatencyRange[MIN];
      InputLatency = SelectedInputDevice->InputLatencyRange[MIN];
    }
  else 
    {
      OutputLatency = WiredSettings->OutputLatency;
      InputLatency = WiredSettings->InputLatency;
    }
  
  if ( SelectedOutputDevice->SupportedFormats.size() > 
       WiredSettings->SampleFormat )
    {
      UserData->SampleFormat = SelectedOutputDevice->
	SupportedFormats[WiredSettings->SampleFormat]->SampleFormat;
      if ( SelectedOutputDevice->
	   SupportedFormats[WiredSettings->SampleFormat]->SampleRates.size() 
	   > WiredSettings->SampleRate )
	SampleRate = SelectedOutputDevice->
	  SupportedFormats[WiredSettings->SampleFormat]->
	  SampleRates[WiredSettings->SampleRate];
      else
	throw Error::InvalidDeviceSettings();
    }
  else if (!SelectedOutputDevice->SupportedFormats.empty())
    {
      /*
	if ( !(SelectedOutputDevice->SupportedFormats[0])->SampleRates.empty() )
	{
	UserData->SampleFormat = 
	(SelectedOutputDevice->SupportedFormats[0])->SampleFormat;
	SampleRate = 
	(SelectedOutputDevice->SupportedFormats[0])->SampleRates.front();
	}
	else
      */
	throw Error::InvalidDeviceSettings();
      /*{
	SampleRate = 44100.0;
	cout << "[AUDIO] Sample rate not defined: default selected: "
	<< SampleRate << endl;
	return false;
	}*/
    }
  else
    throw Error::InvalidDeviceSettings(); // FIXME add a better error msg
  /*{
    SampleRate = 44100.0;
    cout << "[AUDIO] Sample rate not defined: default selected: "
    << SampleRate << endl;
    return false;
    }*/
  if (WiredSettings->SamplesPerBuffer)
    SamplesPerBuffer = WiredSettings->SamplesPerBuffer;
  else
    SamplesPerBuffer = DEFAULT_SAMPLES_PER_BUFFER;
  
  //   UserData->OutputChannels = UserData->Sets->OutputChannels.size();
  //   UserData->InputChannels = UserData->Sets->InputChannels.size();
  
  if (UserData->Sets->OutputChannels.size() <= 0)
    UserData->OutputChannels = 2;//SelectedOutputDevice->MaxOutputChannels;
  else
    UserData->OutputChannels = UserData->Sets->OutputChannels.size();
  
  UserData->InputChannels = UserData->Sets->InputChannels.size();
  
  //UserData->InputChannels = DeviceList.back()->MaxInputChannels;   //FIXME
  //UserData->OutputChannels = DeviceList.back()->MaxOutputChannels; //FIXME
  return SetChannels(UserData->InputChannels, 
		     UserData->OutputChannels);
}
#endif

void AudioEngine::GetDevices()
{
  int			n = Pa_GetDeviceCount();
  const PaDeviceInfo	*info;
  Device		*dev;
  
  if (!(n > 0))
    {
      cout << "[AUDIO] No device found" << endl;
      throw Error::NoDevice();
    }
  for (int i = 0; i < n; i++)
    {
      //      info = 0x0;
      if (!(info = Pa_GetDeviceInfo( i )))
	{
	  cout << "[AUDIO] Error Pa_GetDeviceInfo() bad return value" 
	       << endl;
	  throw Error::NoDevice();
	}
      //      dev = 0x0;
      dev = new Device(i, string(info->name),
		       info->maxInputChannels,
		       info->maxOutputChannels);
      DeviceList.push_back(dev);
      cout << "[AUDIO] New device found #" << dev->Id 
	   << " : " << dev->Name << endl
	   << "[AUDIO] Max Input Channels: " << dev->MaxInputChannels 
	   << endl
	   << "[AUDIO] Max Output Channels: " << dev->MaxOutputChannels
	   << endl;
      dev->GetSupportedSettings();
    }
}

void AudioEngine::OpenStream()
{
  if (StreamIsOpened)
    return ;
  if (!SelectedOutputDevice || 
      (WiredSettings->InputChannels.size() && !SelectedInputDevice))
    {
      cout << "[AUDIO] No default device was found" << endl; 
      throw Error::StreamNotOpen();
    }
  
  PaStreamParameters	OutputParameters, InputParameters;
  PaError err;
  
  OutputParameters.device = SelectedOutputDevice->Id;
  OutputParameters.channelCount = SelectedOutputDevice->MaxOutputChannels;//UserData->OutputChannels;//
  OutputParameters.sampleFormat = UserData->SampleFormat | paNonInterleaved;
  
  if (WiredSettings->InputChannels.size())
    {
      InputParameters.device = SelectedInputDevice->Id;
      InputParameters.channelCount = SelectedOutputDevice->MaxInputChannels;// UserData->InputChannels;//
      InputParameters.sampleFormat = 
	UserData->SampleFormat | paNonInterleaved;
    }
    
  /* 
     InputChannels * FramesPerBuffer
     OutputChannels * FramesPerBuffer
     is the number of bytes to be processed by the callback
     Usually 2 is the value of default stereo output.
  */
  
  
  
  
  OutputParameters.suggestedLatency = Latency;
  InputParameters.suggestedLatency  = Latency;
  
  OutputParameters.hostApiSpecificStreamInfo = NULL;
  InputParameters.hostApiSpecificStreamInfo = NULL;
  
  err = Pa_OpenStream((PaStream **)&Stream,
		      (const PaStreamParameters*)
		      ( WiredSettings->InputChannels.empty() ? 
			0 : &InputParameters ),
		      (const PaStreamParameters*)&OutputParameters,
		      (double)SampleRate,
		      (unsigned long)SamplesPerBuffer,
		      (PaStreamFlags)paClipOff, 
  /* we won't output out of range samples so don't bother clipping them */
		      AudioCallback,
		      (void*)UserData);
  
  if( err != paNoError)
    {
      cout << "[AUDIO] Error using portaudio OpenStream(): " 
	   << Pa_GetErrorText(err) << endl;
      cout << "[AUDIO] Sample rate is: " << SampleRate 
	   << " Samples per buffer is: " << SamplesPerBuffer 
	   << endl;
      AlertDialog("AUDIO", wxString("Error using portaudio OpenStream(): ") + 
		  wxString(Pa_GetErrorText(err)));
      throw Error::StreamNotOpen();
    }
  cout << "[AUDIO] Stream opened:" 
       << " Sample rate is " << SampleRate 
       << " Samples/sec, Samples per buffer is: " << SamplesPerBuffer
       << endl << "[AUDIO] Sample format is: ";
  for (int i = 0; i < MAX_SAMPLE_FORMATS; i++)
    if (standardSampleFormats[i] == UserData->SampleFormat)
      { cout << standardSampleFormats_str[i]; break; }
  cout << endl;
  cout <<"[AUDIO] Output Device is " << SelectedOutputDevice->Name;
  if (!WiredSettings->InputChannels.empty())
    cout << ", Input Device is " << SelectedInputDevice->Name;
  cout << endl;
  StreamIsOpened = true;
}

bool AudioEngine::CloseStream()
{
  if (!StreamIsOpened)
    return true;
  
  PaError err = Pa_IsStreamActive(Stream);
  if (err == 1)
    StopStream();
  else if (err < 0)
    cout << "[AUDIO] Error using portaudio IsStreamActive(): " 
	 << Pa_GetErrorText(err) << endl;
  
  err = Pa_CloseStream(Stream);
  if( err != paNoError )
    {
      cout << "[AUDIO] Error using portaudio CloseStream(): " 
	   << Pa_GetErrorText(err) << endl;
      return (false);
    }
  StreamIsOpened = false;
  cout << "[AUDIO] Stream closed" << endl;
  return (true);
}

bool AudioEngine::StartStream()
{
  if (!StreamIsOpened)
    {
      cout << "[AUDIO] Error, Stream not opened" << endl;
      return (StreamIsStarted = false);
    }
  PaError err;
  if ( (err = Pa_IsStreamActive(Stream)) != paNoError )
    {
      if (err == 1)
	return (true);
      if (err < 0)
	cout << "[AUDIO] Error using portaudio IsStreamActive(): " 
	     << Pa_GetErrorText(err) << endl;
      return (false);
    }
  
  err = Pa_StartStream( Stream );
  if( err != paNoError )
    {
      if (err == paTimedOut)
	{
	  cout << "[AUDIO] Timed out returned by Pa_StartStream" << endl;
	  // FIXME this does not depend on the objet but on the stream
	  StartStream();
	}
      cout << "[AUDIO] Error using portaudio StartStream(): [" 
	   << err << "] " << Pa_GetErrorText(err) << endl;
      return (false);
    }
  cout << "[AUDIO] Stream started" << endl;
  
  return (StreamIsStarted = true);
}

bool AudioEngine::StopStream()
{
  if (!StreamIsStarted)
    return true;
  PaError err = Pa_IsStreamActive( Stream );
  if (err != 1) {
    cout << "[AUDIO] Pa_IsStreamActive(): " <<  Pa_GetErrorText(err) 
	 << endl;
    return (false);
  }
  
  //PaError err = Pa_StopStream( Stream );
  err = Pa_AbortStream( Stream );
  if ( err != paNoError )
    {    
      cout << "[AUDIO] Error using portaudio StopStream(): " 
	   << Pa_GetErrorText(err) << endl;
      //Terminate();
      return (false);
    }
  cout << "[AUDIO] Stream stopped" << endl;
  StreamIsStarted = false;
  return (true);
}

void AudioEngine::AlertDialog(const wxString& from, const wxString& msg)
{
  
  //wxMutexGuiEnter();    
  MainWin->AlertDialog(from, msg);
  //wxMutexGuiLeave();
  
}

void AudioEngine::SetChannels(int in, int out)
{
  //CloseStream();
  if (!in && !out)
    {
      cout << "[AUDIO] Set 0 Channel?"<< endl;
      throw Error::ChannelsNotSet();
    }
  if (StreamIsOpened)
    { 
      cout << "[AUDIO] Stream is opened: couldn't SetChannels" << endl; 
      throw Error::ChannelsNotSet();
    }
  
  ResetChannels();
  
  if ( !SetOutputChannels(out) )
    {
      cout << "[AUDIO] Error Setting " << out << " Output Channels" << endl;
      ResetChannels();
      throw Error::ChannelsNotSet();
    }
  else if ( !SetInputChannels(in) )
    {
      cout << "[AUDIO] Error Setting " << out << " Input Channels" << endl;
      ResetChannels();
      throw Error::ChannelsNotSet();
    }
  cout << "[AUDIO] Channels setted, in: " << in << ", out: " << out << endl;
}

void AudioEngine::ResetChannels(void)
{
  for (vector<RingBuffer<float>*>::iterator 
	 r = UserData->InFIFOVector.begin(); 
       r != UserData->InFIFOVector.end(); 
       r++)
    delete (*r);
  UserData->InFIFOVector.clear();
  UserData->InputChannels = 0;

  for (vector<RingBuffer<float>*>::iterator 
	 r = UserData->OutFIFOVector.begin(); 
       r != UserData->OutFIFOVector.end(); 
       r++)
    delete (*r);
  UserData->OutFIFOVector.clear();
  UserData->OutputChannels = 0;
}

bool AudioEngine::SetOutputChannels(int num)
{
  long numB = SamplesPerBuffer * 2;
  
  RingBuffer<float> *rbuf;
  
  while (num)
    {
      try
	{
	  rbuf = new RingBuffer<float>(numB);
	}
      catch (std::bad_alloc)
      	{ 
	  cout << "[AUDIO] Could not allocate memory" << endl;
	  return false;
	}
      catch (RingBufferError::NumBytesError)
	{
	  cout << "[AUDIO] Couldn't Intialise FIFO: " 
	       << numB << " is not power of 2" << endl;
	  delete rbuf;
	  return false;
	}

      num--;
      UserData->OutFIFOVector.push_back(rbuf);
    }
  
  UserData->OutputChannels = UserData->OutFIFOVector.size();
  return true;
}

bool AudioEngine::SetInputChannels(int num)
{
  long numB = SamplesPerBuffer * 8;
  
  RingBuffer<float> *rbuf;
  while (num)
    {
      try
	{
	  rbuf = new RingBuffer<float>(numB);
	}
      catch (std::bad_alloc)
	{ 
	  cout << "[AUDIO] Could not allocate memory" << endl;
	  return false;
	}
      catch (RingBufferError::NumBytesError)
	{
	  cout << "[AUDIO] Couldn't Intialise FIFO: " 
	       << numB<< " is not power of 2" << endl;
	  delete rbuf;
	  return false;
	}

      num--;
      UserData->InFIFOVector.push_back(rbuf);
    }
  
  UserData->InputChannels = UserData->InFIFOVector.size();
  return true;
}


double AudioEngine::GetTime(void)
{
  return ((double)Pa_GetStreamTime(Stream));
}
  
double AudioEngine::GetCpuLoad(void)
{
  return (Pa_GetStreamCpuLoad(Stream));
}

Device *AudioEngine::GetDeviceById(PaDeviceIndex id)
{
  if (id == paNoDevice)
    return (0x0);
  for (vector<Device*>::iterator dev = DeviceList.begin();
       dev != DeviceList.end(); dev++)
    if ((*dev)->Id == id)
      return (*dev);
  return (0x0);
}


// non-interleaved callback:
/*
static int AudioCallback(const void *input,
		  void *output,
		  unsigned long frameCount,
		  const PaStreamCallbackTimeInfo* timeInfo,
		  PaStreamCallbackFlags statusFlags, 
		  void *userData)
{
  callback_t *data = (callback_t*)userData;
  unsigned long bytes = frameCount, processed = 0;
  float **outputs = (float**)output;
  float **inputs  =(float**)input;
  
  
  int nchan = 0;
  vector<long>::iterator chan;
  
  if (data->SampleFormat & paFloat32)
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
      nchan = 0;
      for (processed = 0, chan = data->Sets->InputChannels.begin();
	   chan != data->Sets->InputChannels.end();
	   chan++, nchan++)
	{
	  processed = data->InFIFOVector[nchan]->Write(inputs[*chan], bytes);
	  //if (processed != bytes)
	  //cout << "[AUDIO] Frame drop while recording" << endl;
	  
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
*/
  
int AudioEngine::GetLibSndFileFormat()
{
  int type;

  switch (SampleFormat)
    {
    case paInt8 : 
      type = SF_FORMAT_PCM_S8; 
      break;
    case paUInt8 : 
      type = SF_FORMAT_PCM_U8;
      break;
    case paInt16 : 
      type = SF_FORMAT_PCM_16;
      break;
    case paInt24 : 
      type = SF_FORMAT_PCM_24;
      break;
    case paInt32 : 
      type = SF_FORMAT_PCM_32;
      break;
    case paFloat32 : 
      type = SF_FORMAT_FLOAT;
      break;
    default:
      type = SF_FORMAT_PCM_16;
      break;
    }
  return (type);
}

