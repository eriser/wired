// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include "Sequencer.h"
#include "MainWindow.h"
#include "Rack.h"
#include "MidiThread.h"
#include "Mixer.h"
#include "AudioEngine.h"
#include "akai.h"
#include "Track.h"
#include "../gui/SequencerGui.h"
#include "../gui/SeqTrack.h"
#include "../gui/SeqTrackPattern.h"
#include "../gui/Pattern.h"
#include "../gui/AudioPattern.h"
#include "../gui/MidiPattern.h"
#include "../mixer/Channel.h"
#include "../midi/midi.h"
#include "../audio/WaveFile.h"
#include "../redist/Plugin.h"
#include "../audio/WriteWaveFile.h"
#include "../samplerate/WiredSampleRate.h"
#include "../gui/Threads.h"

Sequencer::Sequencer() 
  : wxThread(), BPM(96), SigNumerator(4), SigDenominator(4), Loop(false), 
    Exporting(false), ExportWave(0), PlayWave(0), PlayWavePos(0), Click(false),
    CurrentPos(0), BeginLoopPos(0), EndLoopPos(4), EndPos(16), Playing(false),
    Recording(false), CurAudioPos(0)
{
  try
    {
      ClickWave = new WaveFile(WiredSettings->DataDir + wxString(wxT("wired_click.wav"), *wxConvCurrent));
    }
  catch (...)
    {
      ClickWave = 0;
    }
  ClickChannel = Mix->AddStereoOutputChannel();
  PlayWaveChannel = Mix->AddStereoOutputChannel();

  CalcSpeed();
  ExportBuf = NULL;
  SampleRateConverter = NULL;
  AllocBuf1 = NULL;
  AllocBuf2 = NULL;
  cout << "[SEQUENCER] Sequencer initialized !" << endl;
}

Sequencer::~Sequencer()
{
  if (ClickWave)
    delete ClickWave;
}

void					*Sequencer::Entry()
{
  list<RackTrack *>::iterator		RacksTrack;
  list<Plugin *>::iterator		Plug;
  list<MidiEvent *>::iterator		MidiMsg;
  vector<Track *>::iterator		T;
  vector<ChanBuf *>			ExtraBufs;
  vector<ChanBuf *>::iterator		B;
  AudioPattern				*AudioP;
  MidiPattern				*MidiP;
  long					size, click_pos = 0;;
  long					click_coeff, click_dec = 0;
  long					delta = Audio->SamplesPerBuffer;
  WiredEvent				midievent;
  float					**buf = NULL;
  float					**buf1 = NULL;
  float					**buf2 = NULL;

  AllocBuffer(AllocBuf1);
  AllocBuffer(AllocBuf2);
  cout << "[SEQUENCER] Thread started !" << endl;

  while (!TestDestroy())
    {
      // to prevent playing without a valid config
      AudioMutex.Lock();
      if (!Audio->IsOk)
	{
	  // main thread needs Yield to do something
	  Yield();
	  SeqStopped->Signal();
	  AudioMutex.Unlock();

	  Sleep(500); // in milliseconds
	  continue;
	}
      AudioMutex.Unlock();

      /* - gotten MIDI messages analyze */
      MidiMutex.Lock();
      SeqMutex.Lock();
      for (MidiMsg = MidiEvents.begin(); MidiMsg != MidiEvents.end(); MidiMsg++)
	{
	  if (((*MidiMsg)->Msg[0] == M_START) || ((*MidiMsg)->Msg[0] == M_CONT))
	    {
	      SeqMutex.Unlock();
	      Play();
	      SeqMutex.Lock();
	    }
	  else if ((*MidiMsg)->Msg[0] == M_STOP)
	    {
	      SeqMutex.Unlock();
	      Stop();
	      SeqMutex.Lock();  
	    }
	  else
	    for (T = Tracks.begin(); T != Tracks.end(); T++)
	      if ((*T)->IsMidiTrack() && ((*T)->TrackOpt->DeviceId == (*MidiMsg)->Id))
		{		  
		  midievent.Type = WIRED_MIDI_EVENT;
		  midievent.NoteLength = CurAudioPos;
		  midievent.DeltaFrames = 0; //**TODO ----- TO FILL ----
		  memcpy(midievent.MidiData, (*MidiMsg)->Msg, sizeof(int) * 3);
		  // Sends MIDI event to the connected plug-in
		  if ((*T)->TrackOpt->Connected)
		    (*T)->TrackOpt->Connected->ProcessEvent(midievent);
		  // Adds MIDI event on the track if needed
		  if (Playing && Recording && (*T)->TrackOpt->Record)
		    {
		      AddNote(*T, **MidiMsg);
		    }
		}
		if (*MidiMsg)
		  delete *MidiMsg;
	}
      SeqMutex.Unlock();
      Yield();
      MidiEvents.clear();
      MidiMutex.Unlock();
      SeqMutex.Lock();
      if (Playing)
	{
	  /* moved to end of loop
	    SeqMutex.Unlock();
	  SetCurrentPos();
	  SeqMutex.Lock();
	  */
	  /* Looping */
	  if (!Exporting && Loop && (CurrentPos >= EndLoopPos))
	    SetCurrentPos(BeginLoopPos);
	  if (Loop && ((EndLoopPos - CurrentPos) < (Audio->SamplesPerBuffer * MeasurePerSample)))
	    delta = (long)((EndLoopPos - CurrentPos) * SamplesPerMeasure);
	  /* Metronome */
	  if (Click)
	    {
	      click_coeff = (long)(fmod(CurrentPos, 1.0 / SigNumerator) * SamplesPerMeasure);
	      if (click_coeff < Audio->SamplesPerBuffer)
		{
		  click_dec = Audio->SamplesPerBuffer - click_coeff;
		  click_pos = 0;
		}
	      if (ClickWave && (click_pos < ClickWave->GetNumberOfFrames()))
		{
		  size = ClickWave->GetNumberOfFrames() - click_pos - click_dec;
		  if (size > Audio->SamplesPerBuffer)
		    size = Audio->SamplesPerBuffer - click_dec; 
		  else
		    {
		      memset(AllocBuf1[0] + size, 0, (Audio->SamplesPerBuffer - size) * sizeof(float));
		      memset(AllocBuf1[1] + size, 0, (Audio->SamplesPerBuffer - size) * sizeof(float));
		    }
		  if (click_dec > 0)
		    {
		      /*
			cout << "[SEQ] click_dec: " << click_dec << "; CurAudioPos: " << 
			CurAudioPos << "; Sig: " <<
			(long)((1.0 / SigNumerator) / MeasurePerSample) << endl;
		      */
		      memset(AllocBuf1[0], 0, click_dec * sizeof(float));
		      memset(AllocBuf1[1], 0, click_dec * sizeof(float));
		      memcpy(AllocBuf1[0] + click_dec, ClickWave->Data[0] + click_pos, size * sizeof(float));
		      memcpy(AllocBuf1[1] + click_dec, ClickWave->Data[1] + click_pos, size * sizeof(float)); 
		      click_dec = 0;
		    }
		  else
		    {
		      memcpy(AllocBuf1[0], ClickWave->Data[0] + click_pos, size * sizeof(float));
		      memcpy(AllocBuf1[1], ClickWave->Data[1] + click_pos, size * sizeof(float));
		    }
		  ClickChannel->PushBuffer(AllocBuf1); 
		  
		  click_pos += size;
		}
	    }
	  /* Gets patterns that must be played */
	  for (T = Tracks.begin(); T != Tracks.end(); T++)
	    {	      
	      if ((*T)->IsAudioTrack())
		{
		  /* - Audio recording */
		  if (Recording && (*T)->TrackOpt->Record)
		    {	 
		      //cout << "MixInput()"<< endl;
		      Mix->MixInput();	// Mutex ou pas ? a prioris non

		      /* - Gets recording buffers */
		      //cout << "GetRecordBuffer" << endl;
		      (*T)->Wave->GetRecordBuffer();
		      if ((*T)->Wave->GetEndPosition() < CurrentPos)
			ResizePattern((*T)->Wave);
		    }
		  // plays only if NOT mute
		  if (!(*T)->TrackOpt->Mute)
		    {
		      /* Gets audio buffers */
		      AudioP = GetCurrentAudioPattern(*T);
		      if (AudioP)
			{
			  buf = GetCurrentAudioBuffer(AudioP);
			  if ((*T)->TrackOpt->ConnectedRackTrack)
			    (*T)->TrackOpt->ConnectedRackTrack->CurrentBuffer = buf;
			  else if (buf)
			    ExtraBufs.push_back(new ChanBuf(buf, (*T)->Output));
			}
		    }
		}
	      else
		{	  
		  /* Sends each sequencer track MIDI events to related plug-ins
		     depending on the timer*/
		  if (Recording && (*T)->Midi)
		    {
		      if ((*T)->Midi->GetEndPosition() < CurrentPos)
			ResizePattern((*T)->Midi);
		    }
		  list<MidiPattern *> l;
		  list<MidiPattern *>::iterator midi_it;

		  if (!((*T)->TrackOpt->Mute))
		    {
		      l = GetCurrentMidiPatterns(*T);
		      for (midi_it = l.begin(); midi_it != l.end(); midi_it++)
			ProcessCurrentMidiEvents(*T, *midi_it);
		    }
		}
	    }

	  SetCurrentPos();
	}

      //SeqMutex.Unlock();
      //SeqMutex.Lock();
      /* - Calls each rack track plug-in's Process function */
      for (RacksTrack = RackPanel->RackTracks.begin(); RacksTrack != RackPanel->RackTracks.end(); 
	   RacksTrack++)
	{
	  if (!(buf1 = (*RacksTrack)->CurrentBuffer))
	    {
	      buf1 = AllocBuf1;
	      memset(buf1[0], 0, Audio->SamplesPerBuffer * sizeof(float));
	      memset(buf1[1], 0, Audio->SamplesPerBuffer * sizeof(float));
	    }
	  buf2 = AllocBuf2;
	  memset(buf2[0], 0, Audio->SamplesPerBuffer * sizeof(float));
	  memset(buf2[1], 0, Audio->SamplesPerBuffer * sizeof(float));
	  for (Plug = (*RacksTrack)->Racks.begin(); Plug != (*RacksTrack)->Racks.end(); Plug++)
	    {
	      if ((*Plug))
		{
		  //  printf("[SEQ] PROCESS 1: %f\n",  Audio->GetTime());
		  (*Plug)->Process(buf1, buf2, delta);
		  //printf("[SEQ] PROCESS 2: %f\n",  Audio->GetTime());	    
		  buf = buf1;
		  buf1 = buf2;	      
		  buf2 = buf;
		  memset(buf2[0], 0, Audio->SamplesPerBuffer * sizeof(float));
		  memset(buf2[1], 0, Audio->SamplesPerBuffer * sizeof(float));
		}
	    }
	  /*
	    for (int cpt = 0; cpt < Audio->SamplesPerBuffer; cpt++)
	    cout << "seq " << buf1[0][cpt] << " " << buf1[1][cpt] << endl;
	  */
	  (*RacksTrack)->Output->PushBuffer(buf1);
	  if ((*RacksTrack)->CurrentBuffer)
	    {
    	if ((*RacksTrack)->CurrentBuffer[0])
	      delete[] (*RacksTrack)->CurrentBuffer[0];
	    if ((*RacksTrack)->CurrentBuffer[1])	      
	      delete[] (*RacksTrack)->CurrentBuffer[1];
	    if ((*RacksTrack)->CurrentBuffer)
	      delete[] (*RacksTrack)->CurrentBuffer;
	      (*RacksTrack)->CurrentBuffer = 0x0;  
	    } 
	}

      delta = Audio->SamplesPerBuffer;
      /* Plays FileLoader files if needed */
      if (PlayWave)
	{
	  float	**fl_buf = NULL;

	  AllocBuffer(fl_buf, 2);
	  size = PlayWave->GetNumberOfFrames() - PlayWavePos;
	  if (size > Audio->SamplesPerBuffer)
	    size = Audio->SamplesPerBuffer;
	  else
	    {
	      memset(fl_buf[0], 0, Audio->SamplesPerBuffer * sizeof(float));
	      memset(fl_buf[1], 0, Audio->SamplesPerBuffer * sizeof(float));
	    }	      
	  PlayWave->Read(fl_buf, PlayWavePos, size);
      	  ExtraBufs.push_back(new ChanBuf(fl_buf, PlayWaveChannel));
	  PlayWavePos += size;
	  if (PlayWavePos >= PlayWave->GetNumberOfFrames())
	    {
	    if (PlayWave)
	      delete PlayWave;
	      PlayWave = 0x0;
	    }
	}
      //LastCurBlock = CurBlock;
      /* Sends additionnal buffers */
      for (B = ExtraBufs.begin(); B != ExtraBufs.end(); B++)
	{
	  // Mono Channel
	  if (!(*B)->Buffer[1])
	    (*B)->Chan->PushBuffer((*B)->Buffer[0]);
	  else // Stereo Channel
	    (*B)->Chan->PushBuffer((*B)->Buffer);
	}
      /* Mix->MixOutput() function call */
      //SeqMutex.Lock();
      if (Exporting)
	{
	  if (CurrentPos >= EndLoopPos)
	    StopExport();
	  else
	    {
	      Mix->MixOutput(false, this);
	      WriteExport();
	    }
	}
      else
	Mix->MixOutput(true, this);
      SeqMutex.Unlock();

      /* Channel and extra buffers cleaning */
      for (B = ExtraBufs.begin(); B != ExtraBufs.end(); B++)
	{
	  if(*B)
	    {
	      (*B)->DeleteBuffer();
	      delete *B;
	    }
	}
      ExtraBufs.clear();
    }
  cout << "[SEQ] Thread finished !" << endl;
  return NULL;
}

void					Sequencer::Play()
{
  list<RackTrack *>::iterator		RacksTrack;
  list<Plugin *>::iterator		Plug;

  SeqMutex.Lock();
  for (RacksTrack = RackPanel->RackTracks.begin(); 
       RacksTrack != RackPanel->RackTracks.end(); RacksTrack++)
    for (Plug = (*RacksTrack)->Racks.begin(); Plug != (*RacksTrack)->Racks.end();
	 Plug++)
      (*Plug)->Play();
  Playing = true;
  StartAudioPos = Audio->GetTime();
  /* CurAudioPos = 0;
     CurrentPos = 0.f; */
  if (Recording)
    PrepareRecording();
  //if (WiredVideoObject->asFile == true) WiredVideoObject->PlayFile();
  SeqMutex.Unlock();
}

void					Sequencer::Stop()
{
  list<RackTrack *>::iterator		RacksTrack;
  list<Plugin *>::iterator		Plug;
  vector<Track *>::iterator		T;

  SeqMutex.Lock();

  for (RacksTrack = RackPanel->RackTracks.begin(); 
       RacksTrack != RackPanel->RackTracks.end(); RacksTrack++)
    for (Plug = (*RacksTrack)->Racks.begin(); Plug != (*RacksTrack)->Racks.end();
	 Plug++)
      (*Plug)->Stop();

  for (T = Tracks.begin(); T != Tracks.end(); T++)
    if ((*T)->IsMidiTrack())
      {	
	(*T)->TrackOpt->VuValue = 0;
	TracksToRefresh.push_back(*T);
      }  

  Playing = false;
  //if (WiredVideoObject->asFile == true) WiredVideoObject->StopFile();
  if (Recording)
    {
      SeqMutex.Unlock();

      FinishRecording();

      SeqMutex.Lock(); 
    }
  Recording = false;    
  SeqMutex.Unlock();
}

void					Sequencer::Record()
{
  SeqMutex.Lock();
  Recording = true;
  if (Playing)
    PrepareRecording();
  SeqMutex.Unlock();
}

void					Sequencer::StopRecord()
{
  SeqMutex.Lock();
  Recording = false;
  SeqMutex.Unlock();

  FinishRecording();
}

void					Sequencer::AddTrack(Track *t)
{
  SeqMutex.Lock();
  t->SetIndex(Tracks.size());
  Tracks.push_back(t);
  SeqMutex.Unlock();
}

void					Sequencer::RemoveTrack()
{
  SeqMutex.Lock();  
  Track* track = Tracks.back();
  Tracks.pop_back();
  if (track)
    delete track;
  SeqMutex.Unlock();
}

void					Sequencer::PrepareRecording()
{
  vector<Track *>::iterator		T;      

  for (T = Tracks.begin(); T != Tracks.end(); T++)
    {
      if ((*T)->TrackOpt->Record)
	PrepareTrackForRecording(*T);
    }
}

void					Sequencer::PrepareTrackForRecording(Track *T)
{
  int					type;

  PatternsToResize.clear();  
  if (T->TrackOpt->Record && (T->TrackOpt->DeviceId == -1))
    {
      T->TrackOpt->SetRecording(false);
      return;
    }
  type = Audio->GetLibSndFileFormat();
  if (T->IsAudioTrack())
    {
      if (T->Wave)
	delete T->Wave;
      T->Wave = new AudioPattern(CurrentPos, CurrentPos + 0.1, T->GetIndex()); 
      if (!T->Wave->PrepareRecord(type))
	{
	  delete T->Wave;
	  T->Wave = 0x0;
	  T->TrackOpt->SetRecording(false);
	  return;
	}
      else
	T->TrackPattern->Patterns.push_back(T->Wave);	      

    }
  else if (T->IsMidiTrack())
    {
      T->Midi = new MidiPattern(CurrentPos, CurrentPos + 0.1, T->GetIndex());
      T->TrackPattern->Patterns.push_back(T->Midi);
    }  
}

void					Sequencer::FinishRecording()
{
  vector<Track *>::iterator		T; 

  for (T = Tracks.begin(); T != Tracks.end(); T++)
    {
      if ((*T)->TrackOpt->Record)
	{
	  if ((*T)->IsAudioTrack())
	    {	
	      if ((*T)->Wave)
		(*T)->Wave->StopRecord();
	      SeqMutex.Lock();

	      (*T)->Wave = 0x0;
	      
	      SeqMutex.Unlock();
		
	    }
	  else if ((*T)->IsMidiTrack())
	    {
	      SeqMutex.Lock();
	      
	      (*T)->Midi = 0x0;
	      
	      SeqMutex.Unlock();  
	    }
	}
    }  
}

void					Sequencer::AddMidiEvent(int id, MidiType midi_msg[3])
{
  MidiEvent				*msg;

  msg = new MidiEvent(id, CurrentPos, midi_msg);
  // printf("[SEQ] Received midi in for id %d : %2x %2x %2x\n", msg->Id, msg->Msg[0], msg->Msg[1], msg->Msg[2]);
  MidiEvents.push_back(msg);
}

void					Sequencer::AddNote(Track *t, MidiEvent &event)
{
	if (!t)	return;
  if (t->Midi)
    {
      cout << "[SEQ] Adding note to track" << endl;
      MidiEvent *e = new MidiEvent(event);
      e->Position = CurrentPos - t->Midi->GetPosition();
      t->Midi->AddEvent(e);
      PatternsToRefresh.push_back(t->Midi);
      // Sends pattern refresh event
      /*
      wxCommandEvent evt(ID_SEQ_DRAWMIDI, TYPE_SEQ_DRAWMIDI);
      evt.SetId(ID_SEQ_DRAWMIDI);
      evt.SetEventType(TYPE_SEQ_DRAWMIDI);
      evt.SetEventObject((wxObject *)t->Midi);
      wxPostEvent(SeqPanel, evt);*/
    }
}

void					Sequencer::CalcSpeed()
{
  /*
    Speed coefficients calculation : mesure per sample number

    BPM = beat/minute
    SigNumerator = beat/mesure
    BPM/SigNumerator = mesure/minute
    (minute/mesure) / 60 = mesure/sec
    SampleRate = sample/sec
    (mesure/sec) / (sample/sec) = mesure/sample
  */

  MeasurePerSample = ((BPM / SigNumerator) / 60.0) / Audio->SampleRate;
  SamplesPerMeasure = 1 / MeasurePerSample;
}

void					Sequencer::SetCurrentPos(double pos)
{
  //StartAudioPos = Audio->GetTime();
  CurAudioPos = (long)(pos / MeasurePerSample);
  CurrentPos = pos;
  //if (WiredVideoObject->asFile == true) WiredVideoObject->SeekFile(absolute, pos);
  /*CursorEvent event(wxSetCursorPos, wxSetCursorPos);
  event.SetEventObject(SeqPanel);
  event.Position = CurrentPos;
  wxPostEvent(SeqPanel, event);*/
}

void					Sequencer::SetCurrentPos()
{
  CurAudioPos += Audio->SamplesPerBuffer;//(long)((Audio->GetTime() - StartAudioPos) * Audio->SampleRate);// + (Audio->SamplesPerBuffer * 2);
  CurrentPos = (CurAudioPos * MeasurePerSample);// + (Audio->SamplesPerBuffer * MeasurePerSample);
  //cout << "[SEQ] CurPos: " << CurrentPos << endl;	   
  //CursorEvent event(101010, wxSetCursorPos);
  //event.Position = CurrentPos;
  wxCommandEvent event(ID_SEQ_SETPOS, TYPE_SEQ_SETPOS);
  event.SetId(ID_SEQ_SETPOS);
  event.SetEventType(TYPE_SEQ_SETPOS);
  //  event.SetEventObject(MainWin);
  //SeqPanel->SetCurrentPos(CurrentPos - (Audio->SamplesPerBuffer * MeasurePerSample * 2));

  SeqPanel->CurrentPos = CurrentPos - (Audio->SamplesPerBuffer * MeasurePerSample * 2);
  //wxPostEvent(SeqPanel, event);
}

void					Sequencer::ResizePattern(Pattern *p)
{
  PatternsToResize.push_back(p);
  /*wxCommandEvent event(ID_SEQ_RESIZE, TYPE_SEQ_RESIZE);
  event.SetId(ID_SEQ_RESIZE);
  event.SetEventType(TYPE_SEQ_RESIZE);
  event.SetEventObject((wxObject *)p);
  wxPostEvent(SeqPanel, event);*/
}

void					Sequencer::SetBPM(float bpm)
{
  vector<Track *>::iterator		i;
  vector<Pattern *>::iterator		j;

  BPM = bpm;
  CalcSpeed();
  // updates audio pattern size
  for (i = Tracks.begin(); i != Tracks.end(); i++)
    if ((*i)->IsAudioTrack())
      for (j = (*i)->TrackPattern->Patterns.begin(); j != (*i)->TrackPattern->Patterns.end(); 
	   j++)
	(*j)->OnBpmChange();
  // notify the plug-ins
  list<RackTrack *>::iterator		RacksTrack;
  list<Plugin *>::iterator		Plug;

  for (RacksTrack = RackPanel->RackTracks.begin(); 
       RacksTrack != RackPanel->RackTracks.end(); RacksTrack++)
    for (Plug = (*RacksTrack)->Racks.begin(); Plug != (*RacksTrack)->Racks.end(); Plug++)
      (*Plug)->SetBPM(bpm);
}

void					Sequencer::SetSigNumerator(int signum)
{
  list<RackTrack *>::iterator i;
  list<Plugin *>::iterator j;

  if (signum != 0)
    SigNumerator = signum;
  CalcSpeed();
  for (i = RackPanel->RackTracks.begin(); i != RackPanel->RackTracks.end(); i++)
    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
      (*j)->SetSignature(SigNumerator, SigDenominator);
}

void					Sequencer::SetSigDenominator(int dennum)
{
  list<RackTrack *>::iterator i;
  list<Plugin *>::iterator j;

  SigDenominator = dennum;
  for (i = RackPanel->RackTracks.begin(); i != RackPanel->RackTracks.end(); i++)
    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
      (*j)->SetSignature(SigNumerator, SigDenominator);
}

// Called only in case of a MIDI track
list<MidiPattern *>			Sequencer::GetCurrentMidiPatterns(Track *t)
{
  vector<Pattern *>::iterator		i;
  vector<Pattern *>::iterator		ret;
  double				delta_mes = MeasurePerSample * Audio->SamplesPerBuffer;
  list<MidiPattern *>			l;

  for (i = t->TrackPattern->Patterns.begin(); i != t->TrackPattern->Patterns.end(); i++)
    {
      if ((CurrentPos + delta_mes >= (*i)->GetPosition()) && 
	  (CurrentPos < (*i)->GetEndPosition()))
	{
	  ret = i;
	  
	  // Check if the next pattern starts when this one finishes
	  //i++;
	  /*	  if (i != t->TrackPattern->Patterns.end())
	    {
	      if (((*ret)->GetEndPosition() == (*i)->GetPosition()) &&
		  ((CurrentPos + delta_mes > (*ret)->GetEndPosition())))
		l.push_ (*i);
		}*/
	  l.push_back((MidiPattern *)*ret);	  
	}
    }
  return (l);
}

// Called only with in case of an audio track
AudioPattern				*Sequencer::GetCurrentAudioPattern(Track *t)
{
  vector<Pattern *>::iterator		i;
  vector<Pattern *>::iterator		ret;
  double				delta_mes = MeasurePerSample * Audio->SamplesPerBuffer;

  for (i = t->TrackPattern->Patterns.begin(); i != t->TrackPattern->Patterns.end(); i++)
    {
      if ((CurrentPos + delta_mes >= (*i)->GetPosition()) && 
	  (CurrentPos < (*i)->GetEndPosition()))
	{
	  ret = i;
	  
	  // Check if the next pattern starts when this one finishes
	  i++;
	  if (i != t->TrackPattern->Patterns.end())
	    {
	      if (((*ret)->GetEndPosition() == (*i)->GetPosition()) &&
		  ((CurrentPos + delta_mes > (*ret)->GetEndPosition())))
		return ((AudioPattern *)*i);
	    }
	  return ((AudioPattern *)*ret);	  
	}
    }
  return (0x0);
}

float					**Sequencer::GetCurrentAudioBuffer(AudioPattern *p)
{
  long					CurPatternBlock;

  CurPatternBlock = (long)((double)((CurrentPos - (p->GetPosition()/* + p->BeginPosition*/)) * SamplesPerMeasure) 
			   / (double)Audio->SamplesPerBuffer);
  if (p->LastBlock != CurPatternBlock)
    {
      p->LastBlock = CurPatternBlock;
      return (p->GetBlock(CurPatternBlock));
    }
  p->LastBlock++;
  return (p->GetBlock(p->LastBlock));
}

void					Sequencer::ProcessCurrentMidiEvents(Track *T, MidiPattern *p)
{
  double				delta_mes;
  WiredEvent				*curevent;
  vector<MidiEvent *>::iterator		i;
  
  //  cout << "POSITION [ " << p->GetPosition() << " ] youpla :D" << endl;
  //  T->TrackOpt->SetVuValue(0);
  delta_mes = MeasurePerSample * Audio->SamplesPerBuffer;
  
  for (i = p->Events.begin(); i != p->Events.end(); i++)
    {
      if ((p->GetPosition() + (*i)->Position >= CurrentPos) && 
	  (p->GetPosition() + (*i)->Position < CurrentPos + delta_mes))
	{
	  T->TrackOpt->VuValue = (*i)->Msg[2];
	  TracksToRefresh.push_back(T);

	  if (T->TrackOpt->Connected)
	    {
	      curevent = new WiredEvent; //** really needed to dyn alloc ?
	      curevent->Type = WIRED_MIDI_EVENT;
	      curevent->DeltaFrames = (long)((p->GetPosition() + (*i)->Position - CurrentPos) 
					     * SamplesPerMeasure);
	      curevent->NoteLength = (long)(((*i)->EndPosition - (*i)->Position) 
					    * SamplesPerMeasure);
	      memcpy(curevent->MidiData, (*i)->Msg, sizeof(int) * 3);

	      // sent to connected plug-in
	      T->TrackOpt->Connected->ProcessEvent(*curevent);
	      delete curevent;
	    }
	}
    }
}

void					Sequencer::DeletePattern(Pattern *p)
{
  SeqTrackPattern			*t;
  vector<Pattern *>::iterator		k;

  if (p->GetTrackIndex() < Tracks.size())
    {
      t = Tracks[p->GetTrackIndex()]->TrackPattern;
      for (k = t->Patterns.begin(); k != t->Patterns.end(); k++)
	if ((*k) == p)
	  {
	    SeqMutex.Lock();
	    t->Patterns.erase(k);
	    SeqMutex.Unlock();
	    if (p)
	      {
		//RemoveWaveFile(p->Wave);
		delete (p);
	      }
	    break;
	  }
    }
}

void					Sequencer::DeleteBuffer(float** &Buffer, unsigned int NbChannels)
{
  if (NbChannels == 0)
    return;
  try
    {
      for (unsigned int CurrentChannel = 0; CurrentChannel < NbChannels; CurrentChannel++)
	{
	  if (Buffer)
	    if (Buffer[CurrentChannel])
	      delete[] Buffer[CurrentChannel];
	}
      if (Buffer)
	delete [] Buffer;
      Buffer = NULL;
    }
  catch (...)
  {
	//TO FIX : It's always a bad idea not to manage exceptions....
  }
}

void					Sequencer::AllocBuffer(float** &Buffer, unsigned int NbChannels)
{
  if (NbChannels == 0)
    return;
  DeleteBuffer(Buffer, NbChannels);
  Buffer = new float *[NbChannels];
  for (unsigned int CurrentChannel = 0; CurrentChannel < NbChannels; CurrentChannel++)
  {
    Buffer[CurrentChannel] = new float [Audio->SamplesPerBuffer];
  }
}

void					Sequencer::AddMidiPattern(list<SeqCreateEvent *> *l, 
								  Plugin *plug)
{
  vector<Track *>::iterator		i;
  list<SeqCreateEvent *>::iterator	j;
  Track					*t = 0x0;
  MidiPattern				*p;
  MidiEvent				*e;
  double				max_end = 0.0;

  for (i = Tracks.begin(); i != Tracks.end(); i++)
    if ((*i)->TrackOpt->GetSelected() && (*i)->IsMidiTrack())
      {
	t = *i;
	break;
      }
  if (!t)
    {
      t = SeqPanel->AddTrack(false);
      t->TrackOpt->ConnectTo(plug);
    }
  p = new MidiPattern(CurrentPos, CurrentPos, t->TrackOpt->Index - 1);
  for (j = l->begin(); j != l->end(); j++)
    {
      e = new MidiEvent(0, (*j)->Position, (*j)->MidiMsg);
      e->EndPosition = (*j)->EndPosition;
      if (max_end < e->EndPosition)
	max_end = e->EndPosition;
      //      cout << "adding event: " << e->Position << " ; end: " << e->EndPosition << ", midimsg: " << e->Msg << endl;
      p->AddEvent(e);
    }
  p->Modify(-1, -1, -1, max_end);
  p->Update();
  t->AddPattern(p);
}

bool					Sequencer::ExportToWave(wxString &filename)
{
  Seq->Stop();
  if (SampleRateConverter)
    delete SampleRateConverter;
  SampleRateConverter = new WiredSampleRate;
  t_samplerate_info	Info;
  
  Info.SampleRate = (int) Audio->SampleRate;
  Info.Format = Audio->UserData->SampleFormat;
  Info.SamplesPerBuffer = Audio->SamplesPerBuffer;
  SampleRateConverter->Init(&Info);
  if (SampleRateConverter->SaveFile(filename, 2, Audio->SamplesPerBuffer, true))
    {
      SeqMutex.Lock();
      AllocBuffer(ExportBuf);
      SetCurrentPos(BeginLoopPos);
      Exporting = true;
      Loop = false;
      SeqMutex.Unlock();
      Play();      
      return true;
    }
  else
    {
      wxMutexGuiLeave();
      return false;
    }

//  try
//    {
//      ExportWave = new WriteWaveFile(filename, (int)Audio->SampleRate, 2, 
//				     SF_FORMAT_PCM_16);
//      SetCurrentPos(BeginLoopPos);
//      
//      SeqMutex.Lock();
//      Exporting = true;
//      SeqMutex.Unlock();
//      
//      Play();
//    } 
//  catch (...)
//    {
//      cout << "[SEQUENCER] Could not create export file" << endl; // FIXME error dialog box
//    } 
//    return false;
}

void					Sequencer::StopExport()
{
  Stop();
  SeqMutex.Lock();
  Exporting = false;
  if (SampleRateConverter)
  {
  	SampleRateConverter->EndSaveFile(2);
  	delete SampleRateConverter;
  	SampleRateConverter = NULL;
    DeleteBuffer(ExportBuf);
  }
  SeqMutex.Unlock();
  return;  
}

void					Sequencer::WriteExport()
{
  if (SampleRateConverter)
    {
//		bcopy(Mix->OutputLeft, ExportBuf[0], Audio->SamplesPerBuffer);
//		bcopy(Mix->OutputRight, ExportBuf[1], Audio->SamplesPerBuffer);
      long				j;
		
      for (j = 0; j < Audio->SamplesPerBuffer; j++)
	{
	  ExportBuf[0][j] = Mix->OutputLeft[j];
	  ExportBuf[1][j] = Mix->OutputRight[j];
	}
      SampleRateConverter->WriteToFile((unsigned long) Audio->SamplesPerBuffer, ExportBuf, 2);
    }
  return;
}

void					Sequencer::PlayFile(wxString filename, bool isakai)
{
  StopFile();

  try
    {
      if (isakai)
	{
	  wxString mDevice, mFilename, mName;
	  int mPart;
	  
	  mDevice = filename.substr(0, filename.find(wxT(":"), 0));
	  filename = filename.substr(filename.find(wxT(":"), 0) + 1, filename.size() - filename.find(wxT(":"), 0));
	  mFilename = filename.substr(10, filename.size() - 10);
	  int pos = mFilename.find(wxT("/"), 0);
	  mPart = mFilename.substr(0, pos).c_str()[0] - 64;
	  mFilename = mFilename.substr(pos, mFilename.size() - pos);
	  int opos = 0;
	  while ((pos = mFilename.find(wxT("/"), opos)) != wxString::npos)
	    opos = pos + 1;
	  
	  mName = mFilename.substr(opos, mFilename.size() - opos);
	  mFilename = mFilename.substr(1, opos - 2);
	  cout << "device: " << mDevice << "; part: " << mPart << "; name: " << mName << "; filename: " << mFilename << endl;
	  t_akaiSample *sample = akaiGetSampleByName((char*)((const char*)mDevice.mb_str(*wxConvCurrent)),  mPart,
						     (char*)((const char*)mFilename.mb_str(*wxConvCurrent)),
						     (char*)((const char*)mName.mb_str(*wxConvCurrent)));
    //akaiImage *img = new akaiImage(mDevice);
    //akaiSample *smp = img->getSample(mPart + "/" + mName + "/" + mFilename);
	  WaveFile *w = NULL;
    //if (smp)
	  if (sample)
	    {
	    //w = new WaveFile(smp->getSample(), smp->getSize(), 2, smp->getRate());
	      w = new WaveFile(sample->buffer, sample->size, 2, sample->rate);
      //delete smp;
	      free(sample->name);
	      free(sample);
	    }
    //delete img;

	  SeqMutex.Lock();
	  
	  PlayWavePos = 0;
	  PlayWave = w;
    
	  SeqMutex.Unlock();	  	  
	}
      else
	{
	  WaveFile *w = new WaveFile(filename, false);
	  SeqMutex.Lock();
	  PlayWavePos = 0;
	  PlayWave = w;
	  SeqMutex.Unlock();
	}
    }
  catch (...)
    {
      SeqMutex.Lock();
      PlayWave = 0;
      SeqMutex.Unlock();
    }
}

void					Sequencer::StopFile()
{
  if (PlayWave)
    {
      WaveFile *w = PlayWave;

      SeqMutex.Lock();
      PlayWave = 0x0;
      SeqMutex.Unlock();
      if (w)
	delete w;
    }
}

void                    Sequencer::OnExit()
{
  wxMutexLocker		locker(wxGetApp().m_mutex);
  wxArrayThread&	threads = wxGetApp().m_threads;

  threads.Remove(this);
  if (threads.IsEmpty())
    wxGetApp().m_condAllDone->Signal();
  cout << "[SEQ] Thread terminated" << endl;
}
