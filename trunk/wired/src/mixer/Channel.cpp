// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <cmath>
#include "Channel.h"
#include "Mixer.h"

using namespace std;

Channel::Channel(bool stereo, bool visible, WiredDocument* docParent) :
  WiredDocument(wxT("Channel"), docParent)
{
  Stereo = stereo;
  VolumeLeft = 100.f;
  VolumeRight = 100.f;

  InputNum = 0;

  Visible = visible;
  Lrms = 0.f;
  Rrms = 0.f;

  CurBuf = 0;
  Filled = false;

  MuteLeft = false;
  MuteRight = false;

  if (!stereo)
    AddBuffers(PREBUF_NUM);
  else
    AddBuffers(NUM_BUFFERS);
}

Channel::~Channel()
{
  ClearAllBuffers(false);
}

/*
Channel		Channel::operator=(const Channel& right) :
  WiredDocument(wxT("Channel"), right.docParent)
{
  cerr << "WARNING : Soon, Wired will fail" << endl;

  // Ptr must NOT be copied, but content of data does.
  if (this != &right)
    {
      Label = right.Label;
      Visible = right.Visible;
      Lrms = right.Lrms;
      Rrms = right.Rrms;
      MonoBuffers = right.MonoBuffers;
      StereoBuffers = right.StereoBuffers;
      CurBuf = right.CurBuf;
      Filled = right.Filled;
      Stereo = right.Stereo;
      MuteLeft = right.MuteLeft;
      MuteRight = right.MuteRight;
      VolumeLeft = right.VolumeLeft;
      VolumeRight = right.VolumeRight;
      InputNum = right.InputNum;
    }
  return *this;
}
*/

void		Channel::Dump()
{
  cout << "===Channel " << this << " dump begin===" << endl;
  cout << "VolumeLeft : {" << VolumeLeft << "}" << endl;
  cout << "VolumeRight : {" << VolumeRight << "}" << endl;
  cout << "Stereo : {" << Stereo << "}" << endl;
  cout << "InputNum : {" << InputNum << "}" << endl;
  cout << "Label : {" << Label.mb_str() << "}" << endl;
  cout << "Visible : {" << Visible << "}" << endl;
  cout << "Lrms : {" << Lrms << "}" << endl;
  cout << "Rrms : {" << Rrms << "}" << endl;
  cout << "CurBuf : {" << CurBuf << "}" << endl;
  cout << "Filled : {" << Filled << "}" << endl;
  cout << "MuteLeft : {" << MuteLeft << "}" << endl;
  cout << "MuteRight : {" << MuteRight << "}" << endl;
  cout << "===Channel dump end===" << endl;
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
  float rms = 0.f;

  MixMutex.Lock();
  MixMutex.Unlock();

  if (!MonoBuffers.empty())
    {
      if (Visible)
	{
	  int abs;
	  for (unsigned long i = 0; i < Audio->SamplesPerBuffer; i++)
	    {
	      (MonoBuffers[0])[i] = buffer[i] * VolumeLeft / 100.f;
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
	memcpy(MonoBuffers[0], buffer, Audio->SamplesPerBuffer*sizeof(float));
    }
}

void Channel::PushBuffer(float **buffer)
{
  float lrms = 0.f, rrms = 0.f, lvol, rvol;
  bool ml, mr;

  MixMutex.Lock();		//used in ChannelGui::OnFader....()
  ml = MuteLeft;
  mr = MuteRight;
  lvol = VolumeLeft / 100.f;
  rvol = VolumeRight / 100.f;
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

void Channel::ClearAllBuffers(bool renew)
{
  vector<float*>::iterator	mon;
  vector<float**>::iterator	ste;

  MixMutex.Lock();
  Rrms = 0.f;
  Lrms = 0.f;
  MixMutex.Unlock();

  if (!Stereo)
    {
      for (mon = MonoBuffers.begin(); mon != MonoBuffers.end(); mon++)
	if (*mon)
	  delete [] *mon;
    }
  else
    {
      for (ste = StereoBuffers.begin(); ste != StereoBuffers.end(); ste++)
	if (*ste)
	  {
	    if ((*ste)[0])
	      delete [] (*ste)[0];
	    if ((*ste)[1])
	      delete [] (*ste)[1];
	    delete [] (*ste);
	  }
    }
  MonoBuffers.clear();
  StereoBuffers.clear();

  if (renew)
    {
      if (!Stereo)
	AddBuffers(PREBUF_NUM);
      else
	AddBuffers(NUM_BUFFERS);
    }
}

void			Channel::Save()
{
  SaveElement	*savedElem;

  //Stereo
  if (this->Stereo)
    savedElem = new SaveElement(wxT("stereo"), 1);
  else
    savedElem = new SaveElement(wxT("stereo"), 0);
  saveDocData(savedElem);

  //VolumeLeft
  savedElem = new SaveElement(wxT("volumeLeft"), this->VolumeLeft);
  saveDocData(savedElem);

  //VolumeRight
  savedElem = new SaveElement(wxT("volumeRight"), this->VolumeRight);
  saveDocData(savedElem);

  //MuteLeft
  if (this->MuteLeft)
    savedElem = new SaveElement(wxT("muteLeft"), 1);
  else
    savedElem = new SaveElement(wxT("muteLeft"), 0);
  saveDocData(savedElem);

  //MuteRight
  if (this->MuteRight)
    savedElem = new SaveElement(wxT("MuteRight"), 1);
  else
    savedElem = new SaveElement(wxT("MuteRight"), 0);
  saveDocData(savedElem);

  //InputNum
  savedElem = new SaveElement(wxT("inputNum"), (int)this->InputNum);
  saveDocData(savedElem);

  //Label
  savedElem = new SaveElement(wxT("label"), this->Label);
  saveDocData(savedElem);

  //Visible
  savedElem = new SaveElement(wxT("visible"), this->Visible);
  saveDocData(savedElem);

  //Lrms
  savedElem = new SaveElement(wxT("lrms"), this->Lrms);
  saveDocData(savedElem);

  //Rrms
  savedElem = new SaveElement(wxT("rrms"), this->Rrms);
  saveDocData(savedElem);

  //CurBuf
  savedElem = new SaveElement(wxT("curBuf"), this->CurBuf);
  saveDocData(savedElem);

  //Filled
  if (this->Filled)
    savedElem = new SaveElement(wxT("filled"), 1);
  else
    savedElem = new SaveElement(wxT("filled"), 0);
  saveDocData(savedElem);
}

void			Channel::Load(SaveElementArray data)
{
  int		dataCompt;

  for (dataCompt = 0; dataCompt < data.GetCount(); dataCompt++)
    {
      if (data[dataCompt]->getKey() == wxT("stereo"))
	{
	  if (!data[dataCompt]->getValueInt())
	    this->Stereo = false;
	  else
	    this->Stereo = true;
	}
      else if (data[dataCompt]->getKey() == wxT("volumeLeft"))
	this->VolumeLeft = data[dataCompt]->getValueFloat();
      else if (data[dataCompt]->getKey() == wxT("volumeRight"))
	this->VolumeRight = data[dataCompt]->getValueFloat();
      else if (data[dataCompt]->getKey() == wxT("muteLeft"))
	{
	  if (!data[dataCompt]->getValueInt())
	    this->MuteLeft = false;
	  else
	    this->MuteLeft = true;
	}
      else if (data[dataCompt]->getKey() == wxT("muteRight"))
	{
	  if (!data[dataCompt]->getValueInt())
	    this->MuteRight = false;
	  else
	    this->MuteRight = true;
	}
      else if (data[dataCompt]->getKey() == wxT("inputNum"))
	this->InputNum = (long)data[dataCompt]->getValueInt();
      else if (data[dataCompt]->getKey() == wxT("label"))
	this->Label = data[dataCompt]->getValue();
      else if (data[dataCompt]->getKey() == wxT("visible"))
	{
	   if (!data[dataCompt]->getValueInt())
	    this->Visible = false;
	  else
	    this->Visible = true;
	}
      else if (data[dataCompt]->getKey() == wxT("lrms"))
	this->Lrms = data[dataCompt]->getValueFloat();
      else if (data[dataCompt]->getKey() == wxT("rrms"))
	this->Rrms = data[dataCompt]->getValueFloat();
      else if (data[dataCompt]->getKey() == wxT("curBuf"))
	this->CurBuf = data[dataCompt]->getValueInt();
      else if (data[dataCompt]->getKey() == wxT("filled"))
	{
	  if (!data[dataCompt]->getValueInt())
	    this->Filled = false;
	  else
	    this->Filled = true;
	}
    }
}

void			Channel::CleanChildren()
{
  cout << "Channel::CleanChildren" << endl;

}
