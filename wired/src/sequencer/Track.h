#ifndef __TRACK_H__
#define __TRACK_H__

#include "SeqTrack.h"
#include "SeqTrackPattern.h"
#include "WriteWaveFile.h"
#include "WaveFile.h"
#include "Pattern.h"
#include "AudioPattern.h"
#include "MidiPattern.h"
#include "Channel.h"

#define IS_MIDI_TRACK					0x0
#define IS_AUDIO_TRACK					0x1

class Track
{
 public:
  Track(SeqTrack *n1, SeqTrackPattern *n2, char typ = IS_MIDI_TRACK); 
  ~Track(); 

  AudioPattern				*AddPattern(WaveFile *w, double pos = 0);
  MidiPattern				*AddPattern(MidiTrack *t);
  void					AddPattern(Pattern *p);
  void					UpdateIndex(long trackindex);
  bool					IsAudioTrack() { return (Type == IS_AUDIO_TRACK); }
  bool					IsMidiTrack() { return (Type == IS_MIDI_TRACK); }
  SeqTrack				*TrackOpt;
  SeqTrackPattern			*TrackPattern;
  AudioPattern				*Wave;
  MidiPattern				*Midi;
  long					Index;
  Channel				*Output;

 protected:
  char Type;
};

#endif
