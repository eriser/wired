// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "Slice.h"

Slice::Slice(long pos, double bar, int samplerate)
{
  Position = pos;
  EndPosition = 0;
  Bar = bar;
  Note = 0;
  Pitch = 1.f;
  Volume = 1.f;
  AffectMidi = 0x48; //C3
  Invert = false;
  Selected = false;

  LeftTouch = 0x0;
  RightTouch = 0x0;

  LeftTouch = new SoundTouch();
  RightTouch = new SoundTouch();

  LeftTouch->setSampleRate(samplerate);
  LeftTouch->setChannels(1);
  //LeftTouch->setTempo(2.f);
  //LeftTouch->setSetting(SETTING_USE_QUICKSEEK, 1);
  //  LeftTouch->setPitch(.5f);
  
  //LeftTouch->setPitchSemiTones(params->pitchDelta);

  RightTouch->setSampleRate(samplerate);
  RightTouch->setChannels(1);
  //RightTouch->setTempo(2.f);
  //RightTouch->setSetting(SETTING_USE_QUICKSEEK, 1);
  //RightTouch->setPitch(.5f);
  //LeftTouch->setPitchSemiTones(params->pitchDelta);

  Data = 0x0;
}

Slice::~Slice()
{
  delete LeftTouch;
  delete RightTouch;
  
  // TODO: delete Data
}

void Slice::SetNote(int n)
{
  Note = n;
  LeftTouch->setPitchSemiTones(n);
  RightTouch->setPitchSemiTones(n);  
}

void Slice::SetOctave(float o)
{
  LeftTouch->setPitchOctaves(o);
  RightTouch->setPitchOctaves(o);
  //LeftTouch->setPitchSemiTones(Note);
  //RightTouch->setPitchSemiTones(Note);  
}
  
void Slice::SetTempo(float t)
{
  LeftTouch->setTempo(t);
  RightTouch->setTempo(t);
}
