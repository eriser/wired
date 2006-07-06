// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include	<sndfile.h>
#include	"AudioEngine.h"
#include	"MainWindow.h"
#include	"EngineError.h"

AudioEngine::AudioEngine() 
{
  IsOk = false;
  StreamIsOpened = false;
  StreamIsStarted = false;
  Stream = NULL;

  _paInit = false;
  PaError err = Pa_Initialize();
  if ( err != paNoError ) 
    throw Error::InitFailure(wxString(Pa_GetErrorText (err), *wxConvCurrent));
  _paInit = true;

  cout << "[AUDIO] Portaudio initialized" << endl; 
  UserData = new callback_t;

  GetAudioSystems();
  GetDevices();
  SetDefaultSettings();

  cout << "[AUDIO] AudioEngine initialized" << endl; 
}

AudioEngine::~AudioEngine()
{
  PaError err;

  if (_paInit)
    {
      CloseStream();
      err = Pa_Terminate();
      if (err != paNoError)
	cout << "[AUDIO] Error Terminate(): " << Pa_GetErrorText (err) << endl; 
      else
	cout << "[AUDIO] Portaudio unloaded" << endl;
      ResetChannels();
      if (UserData)
	delete UserData;
    }
}

void AudioEngine::SetDefaultSettings(void)
{
  SelectedOutputDevice = NULL;
  SelectedInputDevice = NULL;
  
  SamplesPerBuffer = DEFAULT_SAMPLES_PER_BUFFER;
  SampleRate = DEFAULT_SAMPLE_RATE;
  UserData->SampleFormat = DEFAULT_SAMPLE_FORMAT;
}


void AudioEngine::SetOutputDevice(void)
{
  if ((SelectedOutputDevice = GetDevice(WiredSettings->OutputDev,
					WiredSettings->OutputSystem))
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
       << SelectedOutputDevice->OutputLatencyRange[MIN] * 1000 << " msec" << endl; 
  cout << "[AUDIO] OUTPUT MAX LATENCY " 
       << SelectedOutputDevice->OutputLatencyRange[MAX] * 1000 << " msec" << endl; 
  cout << "[AUDIO] SUGGESTED LATENCY: " 
       << (double)((double)SamplesPerBuffer / (double)SampleRate) * 1000 << " msec"
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
      if (( SelectedInputDevice = GetDevice(WiredSettings->InputDev,
					    WiredSettings->InputSystem))
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
  Latency = (double)((double)SamplesPerBuffer / (double)SampleRate);

  return (SetChannels(UserData->InputChannels,UserData->OutputChannels));
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
       << SelectedOutputDevice->OutputLatencyRange[MIN] * 1000 << " msec" << endl; 
  cout << "[AUDIO] OUTPUT MAX LATENCY " 
       << SelectedOutputDevice->OutputLatencyRange[MAX] * 1000 << " msec" << endl;
  
  cout << "[AUDIO] INPUT MIN LATENCY " 
       << SelectedInputDevice->InputLatencyRange[MIN] * 1000 << " msec" << endl; 
  cout << "[AUDIO] INPUT MAX LATENCY "
       << SelectedInputDevice->InputLatencyRange[MAX] * 1000 << " msec" << endl; 
  
  
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

void AudioEngine::GetAudioSystems()
{
  PaHostApiIndex	Num = Pa_GetHostApiCount();
  int			i;

  for (i = 0; i < Num; i++)
    {
      const PaHostApiInfo	*Infos = Pa_GetHostApiInfo(i);

      if (Infos)
	{
	  cout << "[AUDIO] Host API Number " << i << " is " << Infos->name 
	       << ", devices count : " << Infos->deviceCount << endl;

	  SystemList.push_back(new AudioSystem(i, wxString(Infos->name, *wxConvCurrent)));
	}
    }
}

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
      if (!(info = Pa_GetDeviceInfo( i )))
	{
	  cout << "[AUDIO] Error Pa_GetDeviceInfo() bad return value" 
	       << endl;
	  throw Error::NoDevice();
	}
      dev = new Device(i, wxString(info->name, *wxConvCurrent),
		       info->maxInputChannels,
		       info->maxOutputChannels, info->hostApi);
      DeviceList.push_back(dev);
      cout << "[AUDIO] New device found #" << dev->Id << " for host " << dev->AudioSystem
	   << " : " << dev->Name.mb_str() << endl
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

  if (WiredSettings->OutputChannels.size())
    {
      OutputParameters.device = SelectedOutputDevice->Id;
      OutputParameters.channelCount = SelectedOutputDevice->MaxOutputChannels;
      OutputParameters.sampleFormat = UserData->SampleFormat | paNonInterleaved;
      OutputParameters.suggestedLatency = Latency;
      OutputParameters.hostApiSpecificStreamInfo = NULL;
    }
  
  if (WiredSettings->InputChannels.size())
    {
      InputParameters.device = SelectedInputDevice->Id;
      InputParameters.channelCount = SelectedInputDevice->MaxInputChannels;
      InputParameters.sampleFormat = UserData->SampleFormat | paNonInterleaved;
      InputParameters.suggestedLatency  = Latency;
      InputParameters.hostApiSpecificStreamInfo = NULL;
    }

  /* 
     InputChannels * FramesPerBuffer
     OutputChannels * FramesPerBuffer
     is the number of bytes to be processed by the callback
     Usually 2 is the value of default stereo output.
  */

  err = Pa_OpenStream((PaStream **)&Stream,
		      (const PaStreamParameters*)
		      ( WiredSettings->InputChannels.empty() ? 
			NULL : &InputParameters ),
		      (const PaStreamParameters*)
		      ( WiredSettings->OutputChannels.empty() ?
			NULL : &OutputParameters ),
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
  if (!WiredSettings->OutputChannels.empty())
    cout <<"[AUDIO] Output Device is " << SelectedOutputDevice->Name.mb_str() << endl;
  if (!WiredSettings->InputChannels.empty())
    cout << "[AUDIO] Input Device is " << SelectedInputDevice->Name.mb_str() << endl;
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
  Stream = NULL;
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

Device*			AudioEngine::GetDevice(int dev_id, int system_id)
{
  vector<Device*>::iterator		it;
  int					n;

  // index 0 is a null system/device, but not in DeviceList nor AudioSystem attribute
  dev_id--;
  system_id--;

  n = 0;
  for (it = DeviceList.begin(); it != DeviceList.end(); it++)
    {
      if (system_id  == (*it)->AudioSystem)
	{
	  if (n == dev_id)
	    return ((*it));
	  n++;
	}
    }
  return (NULL);
}

int			AudioEngine::GetDeviceIdByTrueId(Device* dev)
{
  vector<Device*>::iterator	it;
  int				n;

  if (!dev)
    return (0);

  n = 1;
  for (it = DeviceList.begin(); it != DeviceList.end(); it++)
    {
      if (dev->AudioSystem == (*it)->AudioSystem)
	{
	  if (dev->Id == (*it)->Id)
	    return (n);
	  n++;
	}
    }
  return (0);
}

int			AudioEngine::GetDefaultInputDevice()
{
  return (GetDeviceIdByTrueId(GetDeviceById(Pa_GetDefaultInputDevice())));
}

int			AudioEngine::GetDefaultOutputDevice()
{
  return (GetDeviceIdByTrueId(GetDeviceById(Pa_GetDefaultOutputDevice())));
}

int			AudioEngine::GetDefaultAudioSystem()
{
  return (Pa_GetDefaultHostApi() + 1);
}
