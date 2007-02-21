// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

using namespace	std;

#include <vector>
#include <list>
#include <iostream>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
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

class					WiredSampleRate;

/** This class handles a sequencer channel buffer.*/
class ChanBuf
{
 public:
  /** Constructor.
   * Initializes Buffer from given argument
   */
  ChanBuf(float **buf){
    Buffer = buf;
    Chan = 0x0;
  }

  /** Constructor.
   * Initializes Buffer and Chan from given arguments
   */
  ChanBuf(float **buf, Channel *chan){
    Buffer = buf;
    Chan = chan;
  }

  /** Deletes the buffer.*/
  void					DeleteBuffer() 
    { 
      if (Buffer[0])
	delete[] Buffer[0];
      if (Buffer[1])
	delete[] Buffer[1];
      if (Buffer)
	delete[] Buffer;
      Buffer = NULL;
    }

  /** Points to the buffer.*/
  float					**Buffer;
  /** Points to the associated channel.*/
  Channel				*Chan;
};


/** This class handles the sequencer.
 */
class Sequencer : public wxThread
{
 public:

  /** Default constructor.
   * Initlializes the wired click sound from wired_click.wav in the data
   * directory. Creates the main output in the mixer. Initialiazes all buffers
   * to NULL.
   */
  Sequencer();

  /** Default destructor. */
  ~Sequencer();

  /** Executes sequencer thread.*/
  virtual void				*Entry();
  /** Kills sequencer thread.*/
  void					OnExit();
  /** Plays.*/
  void					Play();
  /** Stops.*/
  void					Stop();
  /** Records.*/
  void					Record();
  /** Stops recording.*/
  void					StopRecord();
  /** Exports project to a wave file.
   * \param filename is the filename (wxString) given to the file.
   */
  bool					ExportToWave(wxString &filename);
  /** Stops exporting.*/
  void					StopExport();
  /** Plays a file.
   * \param filename is the filename (wxString) of the file.
   * \param isakai is a flag determining if the flag is an Akai sample
   */
  void					PlayFile(wxString filename, bool isakai);
  /** Stops playinf of a file.*/
  void					StopFile();
  /** Adds a track to the sequencer.
   * \param t points to the track to add.
   */
  void					AddTrack(Track *t);
  void					RemoveTrack();
  /** Adds a MIDI event.
   * \param id is the MIDI device id.
   * \param MidiType is the MIDI event message.
   */
  void					AddMidiEvent(int id, MidiType midi_msg[3]);
  /** Adds a MIDI pattern to the sequencer.
   * \param l is a list of the MIDI events.
   * \param plug points to the associated plug-in.
   */
  void					AddMidiPattern(list<SeqCreateEvent *> *l, 
						       Plugin *plug);
  /** Adds a note to a MIDI track.
   * \param t points to the track.
   * \param event is the MIDI event to add.
   */
  void					AddNote(Track *t, MidiEvent &event);
  void					SetBPM(float bpm);
  void					SetSigNumerator(int signum);
  void					SetSigDenominator(int dennum);
  void					SetCurrentPos(double pos);
  /** Deletes a pattern.
   * \param p points to the pattern.
   */
  void					DeletePattern(Pattern *p);
  /** Sets flags and create pattern for a track that need recording.
   * \param T points the track
   */
  void					PrepareTrackForRecording(Track *T);
  /** Allocates buffer.
   * \param Buffer is the buffer to allocate
   * \param NbChannels is the number of channels
   */
  void					AllocBuffer(float** &Buffer, unsigned int NbChannels = 2);
  /** Deletes buffer.
   * \param Buffer is the buffere
   * \param NbChannels is the number of channels
   */
  void					DeleteBuffer(float** &Buffer, unsigned int NbChannels = 2);

  /** Flag indicating if in playing state.*/
  bool					Playing;
  /** Flag indicating if in recording state.*/
  bool					Recording;
  /** Number of beats per minute.*/
  float					BPM;
  /** Signature numerator.*/
  int					SigNumerator;
  /** Signature denominator.*/
  int					SigDenominator;
  /** Flag indicating if loop is activated.*/
  bool					Loop;
  /** Flag indicating if metronome activated.*/
  bool					Click;
  /** Flag indicating if in exporting state.*/
  bool					Exporting;
  /** Current audio position.*/
  unsigned long				CurAudioPos;
  /** Current position.*/
  double				CurrentPos;
  /** Loop start position.*/
  double				BeginLoopPos;
  /** Loop end position.*/
  double				EndLoopPos;
  /** End position.*/
  double				EndPos;
  /** Contains pointers to all the tracks.*/
  vector<Track *>			Tracks;
  /** Number of measures per sample.*/
  double				MeasurePerSample;
  /** Number of samples per measure.*/
  double				SamplesPerMeasure;
  /** List of patterns to resize.*/
  list<Pattern *>			PatternsToResize;
  /** List of patterns to refresh.*/
  list<MidiPattern *>			PatternsToRefresh;
  /** List of tracks to refresh.*/
  list<Track *>				TracksToRefresh;

 protected:

  void					SetCurrentPos();
  /** Resizes a pattern.
   * \param p points to the pattern to resize
   */
  void					ResizePattern(Pattern *p);
  /** Sets MeasurePerSample and SamplesPerMeasure from BPM, SigNumerator and SampleRate.*/
  void					CalcSpeed();
  /** Sets flags and create patterns for each track that need recording.*/
  void					PrepareRecording();
  /** Stops recording and unlocks mutexes for each track that were recording.*/
  void					FinishRecording();

  AudioPattern				*GetCurrentAudioPattern(Track *t);
  list<MidiPattern *>			GetCurrentMidiPatterns(Track *t);

  float					**GetCurrentAudioBuffer(AudioPattern *p);
  /** Processes MIDI events from a MIDI pattern.*/
  void					ProcessCurrentMidiEvents(Track *T, MidiPattern *p);
  /** Converts to correct sample rate then writes to file.*/
  void					WriteExport();
  /** Start position.*/
  double				StartAudioPos;
  /** List of MidiEvents.*/
  list<MidiEvent *>			MidiEvents;
  /** Wavefile of the metronome sound.*/
  WaveFile				*ClickWave;
  /** Output cahannel for the metronome.*/
  Channel				*ClickChannel;
  float					**AllocBuf1;
  float					**AllocBuf2;
  /** Buffer used for export.*/
  float					**ExportBuf;
  WriteWaveFile				*ExportWave;  
  /** FileLoader file.*/
  WaveFile				*PlayWave;  
  /** PlayWave position.*/
  long					PlayWavePos;
  /** PlayWave output channel.*/
  Channel				*PlayWaveChannel;
  
  WiredSampleRate			*SampleRateConverter;
};

static wxMutex				SeqMutex;
extern Sequencer			*Seq;

#endif
