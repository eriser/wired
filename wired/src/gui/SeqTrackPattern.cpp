// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include "SeqTrackPattern.h"
#include "SequencerGui.h"

SeqTrackPattern::SeqTrackPattern(SequencerView *parent, SeqTrack *n, long length)
{ 


   TrackOpt = n;
   Parent = parent;
   //   Horiz = new wxStaticLine(Parent, -1, 
   //wxPoint(0, n->GetPosition().y + n->GetSize().y),
   //		   wxSize(length, 1), wxLI_HORIZONTAL);
}

SeqTrackPattern::~SeqTrackPattern()
{
  vector<Pattern *>::iterator		k;

  for (k = Patterns.begin(); k != Patterns.end(); k++)
    delete *k;
  //Horiz->Destroy();
}

void					SeqTrackPattern::Update()
{
  //Horiz->SetSize(Parent->GetXScroll(), TrackOpt->GetPosition().y + TrackOpt->GetSize().y,
  //	 Parent->GetTotalWidth(), 1, wxSIZE_USE_EXISTING);
}
