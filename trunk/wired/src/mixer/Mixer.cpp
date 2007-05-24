// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include <wx/thread.h>
#include <wx/utils.h>
#include "Mixer.h"
#include "MixerGui.h"
#include "MixerError.h"
#include "../mixer/Channel.h"
#include "../engine/AudioEngine.h"

using namespace			std;

extern Mixer			*Mix;
wxMutex				MixMutex(wxMUTEX_RECURSIVE);

Mixer::Mixer(WiredDocument* docParent) : WiredDocument(wxT("Mixer"), docParent)
{
  VolumeLeft = 1.f;
  VolumeRight = 1.f;
  MuteL = false;
  MuteR = false;

  OutputLeft = NULL;
  OutputRight = NULL;
  Input = NULL;

  InitBuffers();
  cout << "[MIXER] Mixer initialized" << endl;

  cout << "[MIXER] Mixer initialized" << endl;
}

Mixer::~Mixer()
{
  DeleteBuffers();
}

void				Mixer::DeleteBuffers()
{
  wxMutexLocker			locker(MixMutex);
  list<Channel*>::iterator	c;

  for (c = InChannels.begin(); c != InChannels.end(); c++)
    {
      if (*c)
	delete *c;
    }
  for (c = OutChannels.begin(); c != OutChannels.end(); c++)
    {
      if (*c)
	delete *c;
    }
  OutChannels.clear();
  InChannels.clear();

  if (OutputLeft)
    {
      delete[] OutputLeft;
      OutputLeft = NULL;
    }
  if (OutputRight)
    {
      delete[] OutputRight;
      OutputRight = NULL;
    }

  if (Input)
    {
      for (int i = 0; i < PREBUF_NUM; i++)
	{
	  if (Input[i])
	    delete [] Input[i];
	}
      delete [] Input;
      Input = NULL;
    }
}

/*
Mixer 	Mixer::operator=(const Mixer& right)
{
  cerr << "WARNING : Soon, Wired will miserably fail" << endl;

  // Ptr must NOT be copied, but content of data does.
  if (this != &right)
    {
      OutputLeft = right.OutputLeft;
      OutputRight = right.OutputRight;
      Input = right.Input;
      VolumeLeft = right.VolumeLeft;
      VolumeRight = right.VolumeLeft;
      MuteL = right.MuteL;
      MuteR = right.MuteR;
      OutChannels = right.OutChannels;
      InChannels = right.InChannels;
    }
  return *this;
}
*/

void				Mixer::Dump()
{
  cout << "===Mixer " << this << "dump begin===" << endl;
  cout << "VolumeLeft : {" << VolumeLeft << "}" << endl;
  cout << "VolumeRight : {" << VolumeRight << "}" << endl;
  cout << "MuteL : {" << MuteL << "}" << endl;
  cout << "MuteR : {" << MuteR << "}" << endl;
  cout << "OutChannels Size : {" << OutChannels.size() << "}" << endl;
  cout << "InChannels Size : {" << InChannels.size() << "}" << endl;
  cout << "OutputLeft Ptr : {" << OutputLeft << "}" << endl;
  cout << "OutputRight Ptr : {" << OutputRight << "}" << endl;
  cout << "Input Ptr : {" << Input << "}" << endl;
  cout << "===Mixer dump end===" << endl;
}

Channel*    	Mixer::AddChannel(list<Channel*>& list, bool stereo, bool visible)
{
  Channel*	chan;

  try
    {
      chan = new Channel(stereo, visible, this);
      list.push_back(chan);
    }
  catch (std::bad_alloc)
    {
      chan = NULL;
    }
  return (chan);
}

Channel*	    	Mixer::AddChannel(bool input, bool stereo, bool visible)
{
  if (input)
    return (AddChannel(InChannels, stereo, visible));
  return (AddChannel(OutChannels, stereo, visible));

}

bool				Mixer::RemoveChannel(Channel *chan)
{
  list<Channel*>::iterator	c;

  for (c = InChannels.begin(); c != InChannels.end(); c++)
    if ((*c) == chan)
      {
	if(*c)
		delete (*c);
	InChannels.erase(c);
	return true;
      }
  for (c = OutChannels.begin(); c != OutChannels.end(); c++)
    if ((*c) == chan)
      {
	//if ((*c)->Visible)
	//MixerPanel->RemoveChannel(*c);
	if(*c)
		delete (*c);
	OutChannels.erase(c);
	return true;
      }
  return false;
}

bool				Mixer::InitBuffers(void)
{
  DeleteBuffers();

  try
    {
      OutputLeft = new float[Audio->SamplesPerBuffer];
      OutputRight = new float[Audio->SamplesPerBuffer];
      if (!Input)
	Input = new float*[PREBUF_NUM];
      for (int i = 0; i < PREBUF_NUM; i++)
	Input[i] = new float[Audio->SamplesPerBuffer];
    }
  catch (std::bad_alloc)
    {
      DeleteBuffers();
      cout << "[MIXER] insufficient memory"<< endl;
      return false;
    }
  return true;
}

void				Mixer::MixOutput(bool soundcard, wxThread* caller)
{
  //struct timespec		t;
  float				Lrms = 0.f;
  float				Rrms = 0.f;
  unsigned int			i;

  //t.tv_sec = 0;
  //t.tv_nsec = 100;
  memset(OutputLeft, 0, Audio->SamplesPerBuffer * sizeof(float));
  memset(OutputRight, 0, Audio->SamplesPerBuffer * sizeof(float));

  //MixMutex.Lock();
  for (list<Channel*>::iterator c = OutChannels.begin();
       c != OutChannels.end(); c++)
    {
      //if (!(*c)->Mute)
      if ((*c)->Visible)
	{
	  if ((*c)->Stereo)
	    {
	    	if ((*c)->StereoBuffers[0])
	      for (i = 0; i < Audio->SamplesPerBuffer; i++)
		{
		  // Les Volumes sont appliques lors de PushBuffer
		  OutputLeft[i]  +=
		    ((*c)->StereoBuffers[0])[0][i];// * (*c)->VolumeLeft;
		  OutputRight[i] +=
		    ((*c)->StereoBuffers[0])[1][i];// * (*c)->VolumeRight;
		}
	    }
	  else
	    {
	    	if ((*c)->MonoBuffers[0])
	      for (i = 0; i < Audio->SamplesPerBuffer; i++)
		{
		  OutputLeft[i]  += ((*c)->MonoBuffers[0])[i];
		  OutputRight[i] += ((*c)->MonoBuffers[0])[i];
		}
	    }
	}
      else
	{
	  if ((*c)->Stereo)
	    {
	    	if ((*c)->StereoBuffers[0])
	      for (i = 0; i < Audio->SamplesPerBuffer; i++)
		{
		  OutputLeft[i]  +=
		    ((*c)->StereoBuffers[0])[0][i];// * (*c)->VolumeLeft;
		  OutputRight[i] +=
		    ((*c)->StereoBuffers[0])[1][i];// * (*c)->VolumeRight;
		}
	    }
	  else
	    {
	    	if ((*c)->MonoBuffers[0])
	      for (i = 0; i < Audio->SamplesPerBuffer; i++)
		{
		  OutputLeft[i]  +=
		    ((*c)->MonoBuffers[0])[i];// * (*c)->VolumeLeft;
		  OutputRight[i] +=
		    ((*c)->MonoBuffers[0])[i];// * (*c)->VolumeRight;
		}
	    }
	}
    }

  // additional stuff
  float				lvol;
  float				rvol;

  MixMutex.Lock(); // used by MixerGui for MasterChannelGui
  if (MuteL == true)
    lvol = 0.f;
  else
    lvol = VolumeLeft;
  if (MuteR == true)
    rvol = 0.f;
  else
    rvol = VolumeRight;
  MixMutex.Unlock();

  for (i = 0; i < Audio->SamplesPerBuffer; i++)
    {
      OutputLeft[i]  *= lvol;
      OutputRight[i] *= rvol;
      // CLIPPING OUTPUT
      if (OutputLeft[i] > 1.f)
	OutputLeft[i] = 1.f;
      else if (OutputLeft[i]  < -1.f)
	OutputLeft[i] = -1.f;
      if (OutputRight[i] > 1.f)
	OutputRight[i] = 1.f;
      else if (OutputRight[i] < -1.f)
	OutputRight[i] = -1.f;
      // Calcul du RMS
      Lrms += fabs(OutputLeft[i]);
      Rrms += fabs(OutputRight[i]);
    }
  Lrms /=  Audio->SamplesPerBuffer;
  Rrms /=  Audio->SamplesPerBuffer;
  MixMutex.Lock();
  MixerPanel->MasterLeft = Lrms; // used by MixerGui
  MixerPanel->MasterRight = Rrms;
  MixMutex.Unlock();
  if (soundcard && Audio->UserData)
    {
      long			bytes_written;
      float			*tmp;
      i = 0;

      for (vector<RingBuffer<float>*>::iterator chan =
	     Audio->UserData->OutFIFOVector.begin();
	   chan !=  Audio->UserData->OutFIFOVector.end(); chan++, i++)
	{
	  bytes_written = 0;
	  /* Left/Right fourberie */
	  tmp = ((i % 2) ?  OutputRight : OutputLeft);
	  /* Blocking write */
	  //cout << "[MIXER] blocking write BEGIN" << endl;
	  for (long spb = Audio->SamplesPerBuffer; spb > 0 && (*chan); )
	    {
	      bytes_written = (*chan)->Write(tmp, spb);
	      spb -= bytes_written;
	      tmp += bytes_written;
	      // worker thread must NOT call wxMilliSleep
	      if (caller)
		caller->Sleep(1);
	      else
		wxMilliSleep(1);
	    }
	  //cout << "[MIXER] blocking write END" << endl;
	}
    }
  /*
     Suppressing first buffer of each channel and allocating new buffers in
     RemoveFirstBuffer()
     function already Lock MixMutex because of rms value reset
  */
  long				spb = 0;
  for (list<Channel*>::iterator ch = OutChannels.begin();
       ch != OutChannels.end(); ch++)
    (*ch)->RemoveFirstBuffer();
}

Channel				*Mixer::OpenInput(long num)
{
  bool exist = false;
  for (vector<int>::iterator i = WiredSettings->InputChannels.begin();
       i != WiredSettings->InputChannels.end(); i++)
    {
      if ((*i) == num)
	{
	  exist = true;
	  break;
	}
    }
  if (!exist)
    return 0x0;
  Channel *c = AddChannel(true, false, true);
  if (!c)
    return 0x0;
  c->InputNum = num;
  return c;
}

void				Mixer::FlushInput(long num)
{
  long				bytes = 0;
  vector<int>::iterator		chan = WiredSettings->InputChannels.begin();

  for (vector<RingBuffer<float>*>::iterator c =
	 Audio->UserData->InFIFOVector.begin();
       c != Audio->UserData->InFIFOVector.end(); c++, chan++)
    {
      if (num == (*chan))
	{
	  // memset(Input, 0, Audio->SamplesPerBuffer * sizeof(float)); pas necessaire
	  for (bytes = (*c)->Read(Input[0], Audio->SamplesPerBuffer);
	       bytes > 0;// Audio->SamplesPerBuffer;
	       bytes = (*c)->Read(Input[0], Audio->SamplesPerBuffer));
	  break;
	}
    }
}

void				Mixer::MixInput(void)
{
  long				bytes = 0;
  vector<int>::iterator	chan = WiredSettings->InputChannels.begin();
  int				cpt = 0;

  for (vector<RingBuffer<float>*>::iterator c =
	 Audio->UserData->InFIFOVector.begin();
       c != Audio->UserData->InFIFOVector.end(); c++, chan++)
    {
      /*
	cout << "in mixinput chan:" << *chan
	   << ", which is connected to FIFO:"
	   << cpt++ << endl;
      */
      for (int i = 0; i < PREBUF_NUM; i++ )
	{
	  bytes = (*c)->Read(Input[i], Audio->SamplesPerBuffer);
	  if ( bytes != Audio->SamplesPerBuffer )
	    break;
	  for (list<Channel*>::iterator mix_chan = InChannels.begin();
	       mix_chan != InChannels.end(); mix_chan++)
	    {
	      if ( (*chan) == (*mix_chan)->InputNum )
		{

		  (*mix_chan)->CurBuf = i;
		  //cout << "push buffer from input "
		  //   <<  << endl;
		  (*mix_chan)->PushBuffer(Input[i], bytes);
		}
	    }
//	  if (i + 1 == PREBUF_NUM)
//	    throw MixerError::InputBuffersFull();
	    //	    cout << "MIX INPUT ERROR\n";
	}
      //cout << << endl;
    }
}


void			Mixer::Save()
{
  wxMutexLocker		locker(MixMutex);
  SaveElement		*savedElem;

  //VolumeLeft
  savedElem = new SaveElement(wxT("volumeLeft"), this->VolumeLeft);
  saveDocData(savedElem);

  //VolumeRight
  savedElem = new SaveElement(wxT("volumeRight"), this->VolumeRight);
  saveDocData(savedElem);

  //MuteL
  if (this->MuteL)
    savedElem = new SaveElement(wxT("muteL"), 1);
  else
    savedElem = new SaveElement(wxT("muteL"), 0);
  saveDocData(savedElem);

  //MuteR
  if (this->MuteR)
    savedElem = new SaveElement(wxT("muteR"), 1);
  else
    savedElem = new SaveElement(wxT("muteR"), 0);
  saveDocData(savedElem);

}

void			Mixer::Load(SaveElementArray data)
{
  wxMutexLocker		locker(MixMutex);
  int			dataCompt;

  for (dataCompt = 0; dataCompt < data.GetCount(); dataCompt++)
    {
      if (data[dataCompt]->getKey() == wxT("volumeLeft"))
	this->VolumeLeft = data[dataCompt]->getValueFloat();
      else if (data[dataCompt]->getKey() == wxT("volumeRight"))
	this->VolumeRight = data[dataCompt]->getValueFloat();
      else if (data[dataCompt]->getKey() == wxT("muteL"))
	{
	  if (!data[dataCompt]->getValueInt())
	    this->MuteL = false;
	  else
	    this->MuteL = true;
	}
      else if (data[dataCompt]->getKey() == wxT("muteR"))
	{
	  if (!data[dataCompt]->getValueInt())
	    this->MuteR = false;
	  else
	    this->MuteR = true;
	}
    }
}

void			Mixer::CleanChildren()
{

}
