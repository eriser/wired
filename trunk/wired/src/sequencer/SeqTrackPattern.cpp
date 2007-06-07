// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "SeqTrackPattern.h"
#include "../gui/SequencerGui.h"

SeqTrackPattern::SeqTrackPattern()
{

}

SeqTrackPattern::~SeqTrackPattern()
{
  //int				size;
  vector<Pattern*>::iterator	it;
  vector<Pattern*>::iterator	next;

  //size = 0;
  it = Patterns.begin();
  while (it != Patterns.end() )//&& size < Patterns.size())
    {
      //next = it + 1;
      SeqPanel->DeletePattern(*it);
      //it = next;
      it = Patterns.begin();
      //size++;
    }
}
