// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __AUDIOCENTER_H__
#define __AUDIOCENTER_H__

#include <list>
#include "WaveFile.h"

using std::list;

class AudioCenter
{
 public:
  AudioCenter();
  ~AudioCenter();

  WaveFile *AddWaveFile(string filename);
  void	    Clear();

  list<WaveFile *> WaveFiles;
};

extern AudioCenter WaveCenter;

#endif