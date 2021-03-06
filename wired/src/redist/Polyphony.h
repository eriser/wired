// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __POLYPHONY_H__
#define __POLYPHONY_H__

#include <list>

typedef struct
{
  bool  IsFree;
  float **Buffer;
}	Poly;

class Polyphony
{
 public:
  Polyphony();
  ~Polyphony();
  
  void		SetVolume(float vol);
  void		SetPolyphony(int p);
  void		SetBufferSize(int size);

  float		**GetFreeBuffer();
  void		SetFreeBuffer(float **);
  
  void		GetMix(float **output);
  void		OnChangeSettings();
  int		GetCount() { return PolyphonyCount; }
 private:
  long		buffer_size;
  int		PolyphonyCount;
  float		Volume;
  std::list<Poly *> Voices;

};
#endif//__POLYPHONY_H__
