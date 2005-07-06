#include <math.h>
#include "Channel.h"
#include "Mixer.h"

Channel::Channel(bool stereo)
  : VolumeLeft(1.f), VolumeRight(1.f), /*Label(""),*/Visible(false),
    MuteLeft(false), MuteRight(false), Stereo(stereo), 
    Filled(false), Lrms(0.f), Rrms(0.f)
{
  CurBuf = 0;
  
  if (!Stereo)
    AddBuffers(PREBUF_NUM);
  else
    AddBuffers(NUM_BUFFERS);
}

Channel::Channel(bool stereo, bool visible)
  : Volume(1.f), VolumeLeft(1.f), VolumeRight(1.f),// Label(label), 
    Visible(visible),
    Mute(false), Stereo(stereo), Filled(false), Lrms(0.f), Rrms(0.f)
{
  CurBuf = 0;
  AddBuffers(NUM_BUFFERS);
}

Channel::~Channel()
{
  if (!Stereo)
    for (vector<float*>::iterator b = MonoBuffers.begin(); 
	 b != MonoBuffers.end(); b++)
      delete *b;
  else
    for (vector<float**>::iterator b = StereoBuffers.begin(); 
	 b != StereoBuffers.end(); b++)
      {
	delete (*b)[0];
	delete (*b)[1];
	delete (*b);
      }
  MonoBuffers.clear();
  StereoBuffers.clear();
}

void Channel::AddBuffers(unsigned int num)
{
  if (Stereo)
    {
      float **tmp;
      while (num > 0)
	{
	  tmp = new float *[2];
	  tmp[0] = new float [Audio->SamplesPerBuffer];
	  tmp[1] = new float [Audio->SamplesPerBuffer];
	  memset(tmp[0], 0, Audio->SamplesPerBuffer * sizeof(float));
	  memset(tmp[1], 0, Audio->SamplesPerBuffer * sizeof(float));
	  StereoBuffers.push_back(tmp);
	  num--;
	}
    }
  else
    {
      float *tmp;
      while (num > 0)
	{
	  tmp = new float [Audio->SamplesPerBuffer];
	  memset(tmp, 0, Audio->SamplesPerBuffer * sizeof(float));
	  MonoBuffers.push_back(tmp);
	  num--;
	}
    }
}

void Channel::PushBuffer(float *buffer)
{
  float rms = 0.f, vol;
  
  MixMutex.Lock();
  vol = VolumeLeft;
  //rvol = VolumeRight;
  MixMutex.Unlock();
  
  if (!MonoBuffers.empty())
    {
      if (Visible)
	{
	  int abs;
	  for (unsigned long i = 0; i < Audio->SamplesPerBuffer; i++)
	    {
	      (MonoBuffers[0])[i] = buffer[i] * vol;
	      rms += fabsf( (MonoBuffers[0])[i] );
	      
	      //abs = ((*(int*)&((MonoBuffers[0])[i]))&0x7fffffff);
	      //Lrms += (float)(*(float*)&abs);
	    }	  
	  rms /= Audio->SamplesPerBuffer;
	  
	  MixMutex.Lock();
	  Rrms = Lrms = rms;
	  MixMutex.Unlock();
	}
      else
	{
	  memcpy(MonoBuffers[0], buffer,
		 Audio->SamplesPerBuffer * sizeof(float));
	}
    }
}

void Channel::PushBuffer(float **buffer)
{
  float lrms = 0.f, rrms = 0.f, lvol, rvol;
  bool ml, mr;
  
  MixMutex.Lock();		//used in ChannelGui::OnFader....()
  ml = MuteLeft;
  mr = MuteRight;
  lvol = VolumeLeft;
  rvol = VolumeRight;
  MixMutex.Unlock();
  
  if (ml == true)
    lvol = 0.f;
  if (mr == true)
    rvol = 0.f;
  if (!StereoBuffers.empty())
    {
      if (Visible)
	{
	  for (unsigned long i = 0; i < Audio->SamplesPerBuffer; i++)
	    {
	      (StereoBuffers[0])[0][i] = buffer[0][i] * lvol;
	      (StereoBuffers[0])[1][i] = buffer[1][i] * rvol;
	      
	      lrms += fabsf( (StereoBuffers[0])[0][i] );
	      rrms += fabsf( (StereoBuffers[0])[1][i] );
	    }
	  lrms /= Audio->SamplesPerBuffer;
	  rrms /= Audio->SamplesPerBuffer;
	  
	  MixMutex.Lock();	//mutex used by ChannelGui::UpdateScreen
	  Lrms = lrms;
	  Rrms = rrms;
	  MixMutex.Unlock();
	}
      else
	{
	  memcpy((StereoBuffers[0])[0], buffer[0], 
		 Audio->SamplesPerBuffer * sizeof(float));
	  memcpy((StereoBuffers[0])[1], buffer[1], 
		 Audio->SamplesPerBuffer * sizeof(float));
	}
    }
}

void  Channel::PushBuffer(float* input, long bytes)
{
  memcpy( MonoBuffers[CurBuf], input, Audio->SamplesPerBuffer * sizeof(float) );
}

float *Channel::PopBuffer(int i)
{
  if (MonoBuffers.size() < i)
    return (0x0);
  
  return (MonoBuffers[i]);
  /*
    float *buf = MonoBuffers.front();
    MonoBuffers.erase(MonoBuffers.begin());
    if (MonoBuffers.size() < NUM_BUFFERS)
    {
    AddBuffers(NUM_BUFFERS);
    }
    return (buf);  
  */
}

void Channel::RemoveFirstBuffer()
{
  
  MixMutex.Lock();
  Rrms = 0.f;
  Lrms = 0.f;
  MixMutex.Unlock();
  
  if (!StereoBuffers.empty())
    {
      memset(StereoBuffers[0][0], 0, Audio->SamplesPerBuffer*sizeof(float));
      memset(StereoBuffers[0][1], 0, Audio->SamplesPerBuffer*sizeof(float));
      /*
	float **tmp = StereoBuffers.front();
	delete tmp[0];
	delete tmp[1];
	delete tmp;
	StereoBuffers.erase(StereoBuffers.begin());
	if (StereoBuffers.size() <  NUM_BUFFERS)
	AddBuffers(NUM_BUFFERS);
      */
    }
  else if (!MonoBuffers.empty())
    {
      memset(MonoBuffers[0], 0, Audio->SamplesPerBuffer * sizeof(float));
      /*
	delete (MonoBuffers.front());
	MonoBuffers.erase(MonoBuffers.begin());
	if (MonoBuffers.size() < NUM_BUFFERS)
	AddBuffers(NUM_BUFFERS);
      */
    }
}

void Channel::ClearAllBuffers(void)
{
  
  MixMutex.Lock();
  Rrms = 0.f;
  Lrms = 0.f;
  MixMutex.Unlock();

  if (!Stereo)
    for (vector<float*>::iterator b = MonoBuffers.begin(); 
	 b != MonoBuffers.end(); b++)
      delete *b;
  else
    for (vector<float**>::iterator b = StereoBuffers.begin(); 
	 b != StereoBuffers.end(); b++)
      {
	delete (*b)[0];
	delete (*b)[1];
	delete (*b);
      }
  MonoBuffers.clear();
  StereoBuffers.clear();
  AddBuffers(NUM_BUFFERS);
}










