// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIXER_H__
#define __MIXER_H__

#include <list>
#include "../save/WiredDocument.h"

#define PREBUF_NUM 6

class Sequencer;
class Channel;

class Mixer : public WiredDocument
{
 public:
  Mixer(WiredDocument* docParent = NULL);
  // Mixer(const Mixer& copy){*this = copy;}
  //Mixer operator=(const Mixer& right);
  ~Mixer();

  void			Dump();

  Channel*              OpenInput(long num);

  Channel*		AddChannel(bool input, bool stereo, bool visible = true);

  bool			RemoveChannel(Channel*);
  bool			InitBuffers(void);
  void			DeleteBuffers(void);

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

  std::list<Channel*>	OutChannels;
  std::list<Channel*>	InChannels;

  /**
   * WiredDocument implementation
   */
  void			Save();

  /**
   * WiredDocument implementation
   */
  void			Load(SaveElementArray data);

  /**
   * WiredDocument implementation
   */
  void			CleanChildren();

 private:
  Channel*		AddChannel(std::list<Channel*>& listm,
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
