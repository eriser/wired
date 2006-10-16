// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __TRACK_H__
#define __TRACK_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#define IS_MIDI_TRACK			(0x0)
#define IS_AUDIO_TRACK		      	(0x1)
#define MAX_AUTO_COLOURS		(16)		

class					WaveFile;
class					MidiTrack;
class					Pattern;
class					AudioPattern;
class					MidiPattern;
class					SeqTrack;
class					SeqTrackPattern;
class					Channel;

class					Track
{
 private:
  long					Index;

 public:
  Track(SeqTrack *n1, SeqTrackPattern *n2, char typ = IS_MIDI_TRACK); 
  ~Track(); 
  Track(const Track& copy) {*this = copy;}
  Track	operator=(const Track& right);

  void					Dump();

  AudioPattern				*AddPattern(WaveFile *w, double pos = 0);
  MidiPattern				*AddPattern(MidiTrack *t);
  void					AddPattern(Pattern *p);
  void					DelPattern(Pattern *p);
  void					AddColoredPattern(Pattern *p);

  void					UpdateIndex(long trackindex);
  void					RefreshFullTrack();
  void					ChangeTrackColour(wxColour &c);
  bool					IsAudioTrack() { return (Type == IS_AUDIO_TRACK); }
  bool					IsMidiTrack() { return (Type == IS_MIDI_TRACK); }

  inline long				GetIndex() { return (Index); };

  /**
   * This method is set only once, and just after his creation.
   * To change his index after, you need to use UpdateIndex()
   * \param trackindex First index of the Track
   * \see UpdateIndex().
   */
  inline void				SetIndex(long trackindex) { Index = trackindex; };

  SeqTrack				*TrackOpt;
  SeqTrackPattern			*TrackPattern;
  AudioPattern				*Wave;
  MidiPattern				*Midi;
  Channel				*Output;

 protected:
  char					Type;
  char					ColourIndex;
};

#endif
