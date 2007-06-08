// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __TRACK_H__
#define __TRACK_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

enum trackType
  {
    eUnknownTrack = 0,
    eMidiTrack,
    eAudioTrack,
    eEndTrackType
  };

#include "SeqTrack.h"
#include "SeqTrackPattern.h"
#include "Pattern.h"
#include "SequencerGui.h"
#include "ChannelGui.h"
#include "WiredDocument.h"

#define MAX_AUTO_COLOURS		(16)		

class					WaveFile;
class					MidiTrack;
class					Pattern;
class					AudioPattern;
class					MidiPattern;
class					SeqTrack;
class					SeqTrackPattern;
class					Channel;
class					ChannelGui;
class					SequencerView;

class					Track : public WiredDocument
{
 private:
  long					Index;
  trackType				Type;

  SeqTrack*				TrackOpt;
  Channel*				Output;
  ChannelGui*				ChanGui;

  // They ARE elements in TrackPattern vector, they DONT need to be deleted.
  // These pointers are used when we record on this Track.
  AudioPattern*				Wave;
  MidiPattern*				Midi;

 public:
  Track(WiredDocument* parentDoc, trackType type, wxPoint& pos, wxSize& size,
	wxWindow* trackview); 
  ~Track(); 
  Track	operator=(const Track& right);

  //super degueu
  SeqTrackPattern*			TrackPattern;


  void					Dump();

  AudioPattern				*CreateAudioPattern(WaveFile *w, double pos = 0);
  MidiPattern				*CreateMidiPattern(MidiTrack *t);

  // this function should be used ONLY in Pattern constructor/destructor
  void					AddPattern(Pattern *p);
  AudioPattern*				AddPattern(WaveFile *w, double pos);
  void					DelPattern(Pattern *p);


  void					UpdateIndex(long trackindex);
  void					RefreshFullTrack();
  void					ChangeTrackColour(wxColour &c);
  bool					IsAudioTrack() { return ((Type == eAudioTrack) ? (true) : (false)); }
  bool					IsMidiTrack() { return ((Type == eMidiTrack) ? (true) : (false)); }
  trackType				GetType() { return (Type); }

  inline long				GetIndex() { return (Index); };

  inline SeqTrack*			GetTrackOpt() { return (TrackOpt); };
  inline AudioPattern*			GetAudioPattern() { return (Wave); };
  inline MidiPattern*			GetMidiPattern() { return (Midi); };
  inline SeqTrackPattern*		GetTrackPattern() { return (TrackPattern); };
  inline Channel*			GetOutputChannel() { return (Output); };

  // used to set the newly created pattern where Record will be done
  void					SetMidiPattern(MidiPattern* mp);
  void					SetAudioPattern(AudioPattern* ap);

  // WiredDocument implementation
  void					Save();
  void					Load(SaveElementArray data);

  /**
   * This method is set only once, and just after his creation.
   * To change his index after, you need to use UpdateIndex()
   * \param trackindex First index of the Track
   * \see UpdateIndex().
   */
  inline void				SetIndex(long trackindex) { Index = trackindex; };

 protected:
  unsigned char				ColourIndex;
};

#endif
