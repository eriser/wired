// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <vector>
#include <iostream>
#include "AudioEngine.h"

#define  NUM_BUFFERS  1

class Channel
{
 public:
  Channel(bool stereo, bool visible = true);
  Channel(const Channel& copy){*this = copy;}
  Channel	operator=(const Channel& right);
  ~Channel();

  void		Dump();

  bool		Stereo;
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
  
  inline void	SetMuteLeft(bool muted)
    { MuteLeft = muted; };
  inline void	SetMuteRight(bool muted)
    { MuteRight = muted; };

 private:
  bool		MuteLeft;
  bool		MuteRight;
};

#endif //__CHANNEL_H__


