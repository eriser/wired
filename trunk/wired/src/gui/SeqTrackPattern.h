// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __SEQTRACKPATTERN_H__
#define __SEQTRACKPATTERN_H__

#include "SeqTrack.h"
#include "Pattern.h"

class SequencerView;

class SeqTrackPattern
{
 public:
  SeqTrackPattern(SequencerView *parent, SeqTrack *n, long length);
  ~SeqTrackPattern();
  
  void					Update();

  SeqTrack				*TrackOpt;
  SequencerView				*Parent;
  vector<Pattern *>			Patterns;
};

#endif