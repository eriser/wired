#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

using namespace				std;

#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <wx/thread.h>

typedef struct s_SeqCreateEvent		SeqCreateEvent;
typedef int				MidiType;
class					Channel;
class					Track;
class					Plugin;
class					Track;
class					Pattern;
class					AudioPattern;
class					MidiPattern;
class					MidiEvent;
class					WaveFile;
class					WriteWaveFile;

class ChanBuf
{
 public:
  ChanBuf(float **buf) : Buffer(buf), Chan(0x0) {}
  ChanBuf(float **buf, Channel *chan) : Buffer(buf), Chan(chan) {}

  void					DeleteBuffer() 
    { 
      delete Buffer[0];
      if (Buffer[1])
	delete Buffer[1];
      delete Buffer;
    }

  float					**Buffer;
  Channel				*Chan;
};

class Sequencer : public wxThread
{
 public:
  Sequencer();
  ~Sequencer();

  virtual void				*Entry();
  void					OnExit();
  void					Play();
  void					Stop();
  void					Record();
  void					StopRecord();
  void					ExportToWave(string filename);
  void					StopExport();
  void					PlayFile(string filename, bool isakai);
  void					StopFile();
  void					AddTrack(Track *t);
  void					RemoveTrack();
  void					AddMidiEvent(int id, MidiType midi_msg[3]);
  void					AddMidiPattern(list<SeqCreateEvent *> *l, 
						       Plugin *plug);
  void					AddNote(Track *t, MidiEvent &event);
  void					SetBPM(float bpm);
  void					SetSigNumerator(int signum);
  void					SetSigDenominator(int dennum);
  void					SetCurrentPos(double pos);
  void					DeletePattern(Pattern *p);
  void					PrepareTrackForRecording(Track *T);
  void					SetBufferSize();

  bool					Playing;
  bool					Recording;
  float					BPM;
  int					SigNumerator;
  int					SigDenominator;
  bool					Loop;
  bool					Click;
  bool					Exporting;
  unsigned long				CurAudioPos;
  double				CurrentPos;
  double				BeginLoopPos;
  double				EndLoopPos;
  double				EndPos;
  vector<Track *>			Tracks;
  Track					*MidiRecord;
  double				MeasurePerSample;
  double				SamplesPerMeasure;
  list<Pattern *>			PatternsToResize;
  list<MidiPattern *>			PatternsToRefresh;

 protected:

  void					SetCurrentPos();
  void					ResizePattern(Pattern *p);
  void					CalcSpeed();
  void					PrepareRecording();
  void					FinishRecording();
  Pattern				*GetCurrentPattern(Track *t);
  float					**GetCurrentAudioBuffer(AudioPattern *p);
  void					ProcessCurrentMidiEvents(Track *T, MidiPattern *p);
  void					WriteExport();
  double				StartAudioPos;
  list<MidiEvent *>			MidiEvents;
  WaveFile				*ClickWave;
  Channel				*ClickChannel;
  float					**AllocBuf1;
  float					**AllocBuf2;
  WriteWaveFile				*ExportWave;  
  WaveFile				*PlayWave;  
  long					PlayWavePos;
  Channel				*PlayWaveChannel;
};

extern wxMutex				SeqMutex;
extern Sequencer			*Seq;

#endif
