// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __SEQTRACKPATTERN_H__
#define __SEQTRACKPATTERN_H__

#include "SeqTrack.h"
#include "Pattern.h"
#include <wx/statline.h>

class SeqTrackPattern
{
 public:
  SeqTrackPattern(wxWindow *parent, SeqTrack *n, long length);
  ~SeqTrackPattern();

  void Update(long length);

  SeqTrack	  *TrackOpt;
  wxStaticLine	  *Horiz;
  wxWindow	  *Parent;
  
  vector<Pattern *> Patterns;
};

#endif
