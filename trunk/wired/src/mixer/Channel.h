// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <vector>
#include <string>
#include <iostream>
#include "AudioEngine.h"

#define  NUM_BUFFERS  1

class Channel
{
 public:
  Channel(bool stereo);
  Channel(bool stereo, bool visible);
  Channel(const Channel& copy){*this = copy;}
  Channel	operator=(const Channel& right);
  ~Channel();
  
  bool		Stereo;
  bool		Mute;
  bool		MuteLeft;
  bool		MuteRight;
  float		Volume;
  float		VolumeLeft;
  float		VolumeRight;
 
  long		InputNum;
  
  void		PushBuffer(float* input, long bytes);

  void		PushBuffer(float **);
  void		PushBuffer(float *);
  void		AddBuffers(unsigned int);
  float		*PopBuffer(int i);
  void		RemoveFirstBuffer();
  void		ClearAllBuffers(bool renew = true);
  wxString	Label;
  bool		Visible;
  float		Lrms;
  float		Rrms;
  
  vector<float*> MonoBuffers;
  vector<float**> StereoBuffers;
  
  int		CurBuf;
  
  bool		Filled;
  
 private:
};

#endif //__CHANNEL_H__


