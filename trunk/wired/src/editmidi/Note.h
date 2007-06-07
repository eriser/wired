// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __NOTE_H__
#define __NOTE_H__
#include <vector>
#include "../midi/midi.h"
#include "../gui/MidiPattern.h"

using std::vector;

class Note 
{
 public:
  Note(MidiPattern *p, unsigned int num);

  void		SetDuration(double duration);
  void		Erase();
  double	GetDuration();
  void		SetNote(int note);
  int		GetNote();
  void		SetPos(double pos);
  double	GetPos();
  int		GetChannel();
  void		SetChannel(int channel);
  void		SetVelocity(int velocity);
  int		GetVelocity();

 private:

  MidiEvent	*e;
  MidiPattern	*p;
  unsigned int	num;
};


#endif
