// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include	<sndfile.h>
#include	"AudioEngine.h"
#include	"MainWindow.h"
#include	"EngineError.h"

const unsigned long standardSampleFormats[] =
{
  paFloat32, paInt32, 
  paInt24, paInt16, 
  paUInt8, paInt8
};

const char *standardSampleFormats_str[] =
{
  "non-interleaved 32 bit float", "non-interleaved 32 bit int",
  "non-interleaved 24 bit int", "non-interleaved 16 bit int",
  "non-interleaved 8 bit unsigned int", "non-interleaved 8 bit int"
};

const double standardSampleRates[] =
{
  8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
  44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1 
  /* negative terminated  list */ 
};

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

      // todo :
      // delete DeviceList and SystemList
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
  if (!(SelectedOutputDevice = GetDevice(WiredSettings->OutputDeviceId,
					 WiredSettings->OutputSystemId)))
    if (!(SelectedOutputDevice = GetDeviceById(Pa_GetDefaultOutputDevice())))
      {
	throw Error::InvalidDeviceSettings();
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
    throw Error::InvalidDeviceSettings();
    //UserData->OutputChannels = 2;//SelectedOutputDevice->MaxOutputChannels;
  else
    UserData->OutputChannels = UserData->Sets->OutputChannels.size();
}

void AudioEngine::SetInputDevice(void)
{
  // we check input device only if its id are not null
  if (WiredSettings->InputChannels.size() > 0 &&
      WiredSettings->InputDeviceId > 0 &&
      WiredSettings->InputSystemId > 0)
    {
      if (!(SelectedInputDevice = GetDevice(WiredSettings->InputDeviceId,
					    WiredSettings->InputSystemId)))
	if (!(SelectedInputDevice = GetDeviceById(Pa_GetDefaultInputDevice())))
	  {
	    cout << "input" << endl;
	    throw Error::InvalidDeviceSettings();
	  }
      UserData->InputChannels = UserData->Sets->InputChannels.size();
    }
  else
    {
      SelectedInputDevice = NULL;
      UserData->InputChannels = 0;
    }  
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
  if (!SampleRate)
    Latency = 0;
  else
    Latency = (double)((double)SamplesPerBuffer / (double)SampleRate);

  return (SetChannels(UserData->InputChannels,UserData->OutputChannels));
}

void AudioEngine::GetAudioSystems()
{
  PaHostApiIndex	deviceNumber = Pa_GetHostApiCount();
  int			deviceIndex;

  if (deviceNumber < 0)
    cout << "[AUDIO] Portaudio ERROR : " << Pa_GetErrorText(deviceNumber) << endl;
  else if (!deviceNumber)
    cout << "[AUDIO] No audio system found" << endl;    
  else
    for (deviceIndex = 0; deviceIndex < deviceNumber; deviceIndex++)
      {
	// retrieve host API informations
	const PaHostApiInfo	*Infos = Pa_GetHostApiInfo(deviceIndex);

	if (Infos)
	  {
	    cout << "[AUDIO] Host API Number " << deviceIndex
		 << " is " << Infos->name
		 << " with " << Infos->deviceCount << " devices" << endl;

	    // insert AudioSystem object in our list
	    SystemList.push_back(new AudioSystem(deviceIndex, wxString(Infos->name, *wxConvCurrent)));
	  }
      }
}

void AudioEngine::GetDevices()
{
  int			deviceIndex;
  int			deviceNumber = Pa_GetDeviceCount();
  const PaDeviceInfo	*info;
  Device		*dev;
  
  if (deviceNumber < 0)
    cout << "[AUDIO] Portaudio ERROR: " << Pa_GetErrorText(deviceNumber) << endl;
  else if (!deviceNumber)
    cout << "[AUDIO] No device found" << endl;
  else
    for (deviceIndex = 0; deviceIndex < deviceNumber; deviceIndex++)
      {
	// if for some reasons index is misunderstood by Pa function..
	if (!(info = Pa_GetDeviceInfo(deviceIndex)))
	  {
	    cout << "[AUDIO] Pa_GetDeviceInfo() failed with " << deviceIndex
		 << endl;
	  }
	else
	  {
	    // create device node
	    dev = new Device(deviceIndex, wxString(info->name, *wxConvCurrent),
			     info->maxInputChannels,
			     info->maxOutputChannels, GetAudioSystemById(info->hostApi));
	    dev->GetSupportedSettings();

	    // insert device node in our list 
	    DeviceList.push_back(dev);

	    // print device informations
	    cout << "[AUDIO] New device found #" << dev->Id << " for host " << dev->Host->GetId()
		 << " : " << dev->Name.mb_str() << endl
		 << "[AUDIO] Max Input Channels: " << dev->MaxInputChannels 
		 << endl
		 << "[AUDIO] Max Output Channels: " << dev->MaxOutputChannels
		 << endl;
	  }
      }
}

void AudioEngine::OpenStream()
{
  if (StreamIsOpened)
    return ;
  if (!SelectedOutputDevice || 
      (UserData->InputChannels > 0 && !SelectedInputDevice))
    {
      cout << "[AUDIO] No default device was found (Unreachable error)" << endl; 
      throw Error::StreamNotOpen();
    }
  
  PaStreamParameters	OutputParameters, InputParameters;
  PaError err;

  if (UserData->OutputChannels)
    {
      OutputParameters.device = SelectedOutputDevice->Id;
      OutputParameters.channelCount = SelectedOutputDevice->MaxOutputChannels;
      OutputParameters.sampleFormat = UserData->SampleFormat | paNonInterleaved;
      OutputParameters.suggestedLatency = Latency;
      OutputParameters.hostApiSpecificStreamInfo = NULL;
    }
  
  if (UserData->InputChannels)
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
		      ( UserData->InputChannels > 0 ? 
			&InputParameters : NULL ),
		      (const PaStreamParameters*)
		      ( UserData->OutputChannels > 0 ?
			&OutputParameters : NULL ),
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
  if (UserData->OutputChannels > 0 && SelectedOutputDevice)
    cout <<"[AUDIO] Output Device is " << SelectedOutputDevice->Name.mb_str() << endl;
  if (UserData->InputChannels > 0 && SelectedInputDevice)
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
  cout << "[AUDIO] Channels set, in: " << in << ", out: " << out << endl;
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
      if (system_id  == (*it)->Host->GetId())
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
      if (dev->Host == (*it)->Host)
	{
	  if (dev->Id == (*it)->Id)
	    return (n);
	  n++;
	}
    }
  return (0);
}

AudioSystem*				AudioEngine::GetAudioSystemById(int id)
{
  vector<AudioSystem*>::iterator	it;

  for (it = SystemList.begin(); it != SystemList.end(); it++)
    if ((*it)->GetId() == id)
      return (*it);
  return (NULL);
}

AudioSystem*		AudioEngine::GetAudioSystemByName(wxString name)
{
  vector<AudioSystem*>::iterator	it;

  for (it = SystemList.begin(); it != SystemList.end(); it++)
    if ((*it)->GetName() == name)
      return (*it);
  return (NULL);
}

Device*			AudioEngine::GetDeviceByName(wxString name)
{
  vector<Device*>::iterator	it;

  for (it = DeviceList.begin(); it != DeviceList.end(); it++)
    if ((*it)->Name == name)
      return (*it);
  return (NULL);
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

void			DumpStereoBuffer(float **buffer)
{
  cout << "===DumpBuffer dump begin (first 10bytes)===" << endl;
  if (buffer)
    {
      int	i;

      if (buffer[0] && buffer[1])
	for (i = 0; i < 10; i++)
	  cout << buffer[0][i] << "\t" << buffer[1][i] << endl;
    }
  cout << "===DumpBuffer dump end===" << endl;
}
