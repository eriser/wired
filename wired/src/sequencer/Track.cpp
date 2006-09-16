// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/colour.h>
#include "Track.h"
#include "Sequencer.h"
#include "../mixer/Mixer.h"
#include "../gui/Pattern.h"
#include "../gui/AudioPattern.h"
#include "../gui/MidiPattern.h"
#include "../gui/SeqTrack.h"
#include "../gui/SeqTrackPattern.h"

wxColour				PatternColours[MAX_AUTO_COLOURS] = 
  {  
    // blue
    wxColour( 50, 166, 211),				
    wxColour( 75, 133, 204),
    wxColour( 15,  98, 198),
    // purple
    wxColour(134, 112, 175),				
    wxColour(133,  96, 204),
    wxColour( 96,  47, 188),
    // orange
    wxColour(247, 186, 111),				
    wxColour(237, 134,  66),
    wxColour(239, 104,  75),
    // green
    wxColour(182, 226,  99),				
    wxColour(130, 206,  95),
    wxColour( 97, 183, 166),

    wxColour( 50, 166, 211),				
    wxColour( 75, 133, 204),
    wxColour( 15,  98, 198),
    wxColour( 15,  98, 198)    
  };

Track::Track(SeqTrack *n1, SeqTrackPattern *n2, char typ) 
{
  TrackOpt = n1;
  TrackPattern = n2;
  Wave = 0x0;
  Midi = 0x0;
  Index = 0;
  Type = typ;
  if (IsAudioTrack())
    {
      Output = Mix->AddStereoOutputChannel(true);
    }
  else
    Output = 0x0;

  ColourIndex = (AudioTrackCount + MidiTrackCount - 1) % MAX_AUTO_COLOURS;
}

Track::~Track() 
{ 
	if (TrackOpt)
	  delete TrackOpt;
    if (TrackPattern)
	  delete TrackPattern;
  if (Wave)
    delete Wave;
  if (Midi)
    delete Midi;
  if (Output)
    Mix->RemoveChannel(Output);
}

Track			Track::operator=(const Track& right)
{
	if (this != &right)
	{
		TrackOpt = right.TrackOpt;
		TrackPattern = right.TrackPattern;
		Wave = right.Wave;
		Midi = right.Midi;
		Index = right.Index;
		Output = right.Output;
		Type = right.Type;
		ColourIndex = right.ColourIndex;
	}
	return *this;
}

AudioPattern					*Track::AddPattern(WaveFile *w, double pos)
{
  AudioPattern					*a;

#ifdef __DEBUG__
  printf("Track::AddPattern(%d, %f) -- START (AUDIO) Index=%d\n", w, pos, Index);
#endif
  a = new AudioPattern(pos, w, Index);
  a->SetDrawColour(PatternColours[ColourIndex]);

  SeqMutex.Lock();
  TrackPattern->Patterns.push_back(a);
  if (a->GetEndPosition() > Seq->EndPos)
    Seq->EndPos = a->GetEndPosition();
  SeqMutex.Unlock();

  a->Update();
#ifdef __DEBUG__
  printf("Track::AddPattern(%d, %f) -- OVER (AUDIO)\n", w, pos);
#endif
  return (a);
}

void						Track::AddPattern(Pattern *p)
{
#ifdef __DEBUG__
  printf("Track::AddPattern(%d) -- START (PATTERN)\n", p);
#endif
  p->SetDrawColour(PatternColours[ColourIndex]);
  SeqMutex.Lock();
  TrackPattern->Patterns.push_back(p);
  SeqMutex.Unlock();
#ifdef __DEBUG__
  printf("Track::AddPattern(%d) -- OVER (PATTERN)\n", p);
#endif
}

MidiPattern					*Track::AddPattern(MidiTrack *t)
{
  MidiPattern					*a;

#ifdef __DEBUG__
  printf("Track::AddPattern(%d) -- START (MIDI)\n", t);
#endif
  a = new MidiPattern(0, t, Index);
  a->SetDrawColour(PatternColours[ColourIndex]);
  SeqMutex.Lock();
  TrackPattern->Patterns.push_back(a);
  a->Update();
  if (a->GetEndPosition() > Seq->EndPos)
    Seq->EndPos = a->GetEndPosition();
  SeqMutex.Unlock();
#ifdef __DEBUG__
  printf("Track::AddPattern(%d) -- OVER (MIDI)\n", t);
#endif
  return (a);
}

void						Track::UpdateIndex(long trackindex)
{  
  vector<Pattern *>::iterator			k;

#ifdef __DEBUG__
  printf("Track::UpdateIndex(%d) -- START\n", trackindex);
#endif
  Index = trackindex;
  for (k = TrackPattern->Patterns.begin(); k != TrackPattern->Patterns.end(); k++)
    (*k)->Modify(-1, -1, trackindex);
#ifdef __DEBUG__
  printf("Track::UpdateIndex(%d) -- OVER\n", trackindex);
#endif
}

void						Track::RefreshFullTrack()
{
  vector<Pattern *>::iterator			p;
  
  TrackOpt->Refresh();
  TrackPattern->Update();
  for (p = TrackPattern->Patterns.begin(); p != TrackPattern->Patterns.end(); p++)
    (*p)->Update();
}  

void						Track::ChangeTrackColour(wxColour &c)
{
  vector<Pattern *>::iterator			p;
  
  for (p = TrackPattern->Patterns.begin(); p != TrackPattern->Patterns.end(); p++)
    {
      (*p)->SetDrawColour(c);
      (*p)->Refresh();
    }
}

void						Track::AddColoredPattern(Pattern *p)
{
  SeqMutex.Lock();
  TrackPattern->Patterns.push_back(p);
  SeqMutex.Unlock();
}
