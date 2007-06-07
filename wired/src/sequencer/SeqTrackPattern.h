// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SEQTRACKPATTERN_H__
#define __SEQTRACKPATTERN_H__

#include <vector>
#include "../gui/Pattern.h"

class	Pattern;

class	SeqTrackPattern
{
 public:
  SeqTrackPattern();
  ~SeqTrackPattern();

  std::vector<Pattern *>			Patterns;
};

#endif
