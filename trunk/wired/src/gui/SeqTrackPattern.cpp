// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "SeqTrackPattern.h"

SeqTrackPattern::SeqTrackPattern(wxWindow *parent, SeqTrack *n, long length)
{ 
  Parent = parent;
  TrackOpt = n;
  Horiz = new wxStaticLine(Parent, -1, 
			   wxPoint(0, n->GetPosition().y + n->GetSize().y),
			   wxSize(length, 1), wxLI_HORIZONTAL);
}

SeqTrackPattern::~SeqTrackPattern()
{
  vector<Pattern *>::iterator k;

  for (k = Patterns.begin(); k != Patterns.end(); k++)
    delete *k;
  Horiz->Destroy();
}

void					SeqTrackPattern::Update(long length)
{
  Horiz->SetPosition(wxPoint(0, TrackOpt->GetPosition().y 
			     + TrackOpt->GetSize().y));
  Horiz->SetSize(wxSize(length, 1));
}
