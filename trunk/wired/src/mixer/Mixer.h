// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIXER_H__
#define __MIXER_H__

using namespace std;

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
  
  void			Dump();

  Channel*		AddMonoInputChannel(void);
  Channel*		AddStereoInputChannel(void);
  Channel*              OpenInput(long num);
  
  Channel*		AddMonoOutputChannel(bool visible = true);
  Channel*		AddStereoOutputChannel(bool visible = true);
  
  bool			RemoveChannel(Channel*);
  bool			InitOutputBuffers(void);
  
 /* mixes the first buffer 
    of each output channels
    blocks until it writes the
    result in each output 
    ringbuffers 
 */

  void			MixOutput(bool soundcard, wxThread* caller = NULL);

  void			FlushInput(long num); 
  void			MixInput(void);

  float			VolumeLeft;
  float			VolumeRight;
  bool			MuteL;
  bool			MuteR;
  
  list<Channel*>	OutChannels;
  list<Channel*>	InChannels;

 private:
  Channel*		AddChannel(list<Channel*>& listm,
				   bool stereo, bool visible = true);

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
