// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __AUDIOCENTER_H__
#define __AUDIOCENTER_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <list>
#include "../audio/WaveFile.h"

class AudioCenter
{
 public:
  AudioCenter();
  ~AudioCenter();

  WaveFile*		AddWaveFile(wxString filename);
  void			RemoveWaveFile(WaveFile *File);
  void			Clear();

  std::list<WaveFile *> WaveFiles;
};

extern AudioCenter WaveCenter;

#endif
