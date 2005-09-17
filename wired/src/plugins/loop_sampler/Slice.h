// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __SLICE_H__
#define __SLICE_H__

#include <soundtouch/SoundTouch.h>

using namespace soundtouch;

class Slice
{
 public:
  Slice(long pos, double bar, int samplerate);
  ~Slice();

  void		SetNote(int n);
  void		SetOctave(float o);
  void		SetTempo(float t);

  long		Position;
  long		EndPosition;
  double	Bar;
  int		Note;
  float		Pitch;
  float		Volume;
  int		AffectMidi;
  bool		Invert;
  bool		Selected;

  SoundTouch	*LeftTouch;
  SoundTouch	*RightTouch;

  void		*Data;

 protected:
  
};

#endif
