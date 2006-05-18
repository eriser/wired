// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIXER_H__
#define __MIXER_H__

using namespace std;

//#include <vector>
//#include <string>
//#include <iostream>
#include <list>


#define PREBUF_NUM 6

class Sequencer;
class Channel;

class Mixer
{
 public:
  Mixer();
  Mixer(const Mixer& copy){*this = copy;}
  Mixer operator=(const Mixer& right);
  ~Mixer();
  
  Channel*		AddMonoOutputChannel(void);
  Channel*		AddStereoOutputChannel(void);
  Channel*		AddMonoInputChannel(void);
  Channel*		AddStereoInputChannel(void);
  Channel*              OpenInput(long num);
  
  Channel*		AddMonoOutputChannel(bool visible);
  Channel*		AddStereoOutputChannel(bool visible);
  
  bool			RemoveChannel(Channel*);
  bool			InitOutputBuffers(void);
  
  void			MixOutput(bool soundcard); /* mixes the first buffer 
						      of each output channels
						      blocks until it writes the
						      result in each output 
						      ringbuffers 
						   */
  void			FlushInput(long num); 
  void			MixInput(void);
  
  float			Volume;
  float			VolumeLeft;
  float			VolumeRight;
  bool			MuteL;
  bool			MuteR;
  
  list<Channel*>	OutChannels;
  list<Channel*>	InChannels;
  
 protected:
  friend class		Sequencer;
  float			*OutputLeft;
  float			*OutputRight;
  float			**Input;
  float			*FillZero();
};

extern Mixer		*Mix;
extern wxMutex		MixMutex;

#endif
