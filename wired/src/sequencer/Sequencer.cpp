#include <math.h>
#include "Sequencer.h"
#include "MainWindow.h"
#include "Rack.h"
#include "MidiThread.h"
#include "Mixer.h"
#include "AudioEngine.h"
#include "akai.h"

wxMutex SeqMutex;

Sequencer::Sequencer() 
  : wxThread(), BPM(96), SigNumerator(4), SigDenominator(4), Loop(false), 
    Exporting(false), ExportWave(0), PlayWave(0), PlayWavePos(0), Click(false),
    CurrentPos(0), BeginLoopPos(0), EndLoopPos(4), EndPos(16), Playing(false),
    Recording(false), CurAudioPos(0)
{
  try
    {
      ClickWave = new WaveFile(WiredSettings->DataDir + string("wired_click.wav"));
    }
  catch (...)
    {
      ClickWave = 0;
    }
  ClickChannel = Mix->AddStereoOutputChannel();
  PlayWaveChannel = Mix->AddStereoOutputChannel();

  CalcSpeed();
}

Sequencer::~Sequencer()
{
  
}

void					*Sequencer::Entry()
{
  bool					ok = true;
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
  float					**buf;
  float					**buf1;
  float					**buf2;
  
  bool off = true;
  while ( off )
    {
      SeqMutex.Lock();
      if ( Audio->IsOk )
	off = false;
      else
	wxUsleep(10);
      SeqMutex.Unlock();
    }
  
  if ( !Audio->StartStream() )
    cout << "[SEQUENCER] StartStream returned false" << endl;
  SetBufferSize();
  cout << "[SEQ] Thread started !" << endl;
  while (ok)
    {
      /* - Traitement des messages MIDI recus */
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
		  midievent.DeltaFrames = 0; // TODO ----- A REMPLIR ----
		  memcpy(midievent.MidiData, (*MidiMsg)->Msg, sizeof(int) * 3);
		  // Envoyer evenement midi au plugin connecte
		  if ((*T)->TrackOpt->Connected)
		    (*T)->TrackOpt->Connected->ProcessEvent(midievent);
		  // Ajout de l'evenement MIDI sur la piste, si requis
		  if (Playing && Recording && (*T)->TrackOpt->Record)
		    {
		      AddNote(*T, **MidiMsg);
		    }		  
		}	  	  
	  delete *MidiMsg;	  
	}
      SeqMutex.Unlock();
      MidiEvents.clear();
      MidiMutex.Unlock();
      SeqMutex.Lock();
      if (Playing)
	{
	  SeqMutex.Unlock();
	  SetCurrentPos();
	  SeqMutex.Lock();
	  /* Bouclage */
	  if (!Exporting && Loop && (CurrentPos >= EndLoopPos))
	    SetCurrentPos(BeginLoopPos);
	  if (Loop && ((EndLoopPos - CurrentPos) < (Audio->SamplesPerBuffer * MeasurePerSample)))
	    delta = (long)((EndLoopPos - CurrentPos) * SamplesPerMeasure);
	  /* Métronome */
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
	  /* Récupérations des patterns à jouer */
	  for (T = Tracks.begin(); T != Tracks.end(); T++)
	    {	      
	      if ((*T)->IsAudioTrack())
		{
		  /* - Enregistrement Audio */
		  if (Recording && (*T)->TrackOpt->Record)
		    {	  
		      Mix->MixInput(); // Mutex ou pas ? a prioris non
		      
		      /* - Recuperation des buffers d'enregistrement */
		      (*T)->Wave->GetRecordBuffer();
		      if ((*T)->Wave->GetEndPosition() < CurrentPos)
			ResizePattern((*T)->Wave);
		    }
		  if (!(*T)->TrackOpt->Mute)
		    {
		      /* Récupération des buffers Audio */
		      AudioP = (AudioPattern *)GetCurrentPattern(*T);  
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
		  /* Envoi des evenements MIDI (par rapport au timer) de chaque piste du 
		     sequenceur aux plugins concernes */
		  if (Recording && (*T)->Midi)
		    {
		      if ((*T)->Midi->GetEndPosition() < CurrentPos)
			ResizePattern((*T)->Midi);
		    }
		  if ((!(*T)->TrackOpt->Mute) && (MidiP = (MidiPattern *)GetCurrentPattern(*T)))
		    ProcessCurrentMidiEvents(*T, MidiP);
		}
	    }
	}
      SeqMutex.Unlock();
      SeqMutex.Lock();
      /* - Appel des fonctions Process de chaque Plugin de chaque piste de racks */
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
	      //  printf("[SEQ] PROCESS 1: %f\n",  Audio->GetTime());
	      (*Plug)->Process(buf1, buf2, delta);
	      //printf("[SEQ] PROCESS 2: %f\n",  Audio->GetTime());	    
	      buf = buf1;
	      buf1 = buf2;	      
	      buf2 = buf;
	      memset(buf2[0], 0, Audio->SamplesPerBuffer * sizeof(float));
	      memset(buf2[1], 0, Audio->SamplesPerBuffer * sizeof(float));
	    }
	  /*
	    for (int cpt = 0; cpt < Audio->SamplesPerBuffer; cpt++)
	    cout << "seq " << buf1[0][cpt] << " " << buf1[1][cpt] << endl;
	  */
	  (*RacksTrack)->Output->PushBuffer(buf1);
	  if ((*RacksTrack)->CurrentBuffer)
	    {
	      delete (*RacksTrack)->CurrentBuffer[0];
	      delete (*RacksTrack)->CurrentBuffer[1];
	      delete (*RacksTrack)->CurrentBuffer;
	      (*RacksTrack)->CurrentBuffer = 0x0;  
	    } 
	}
      SeqMutex.Unlock();
      delta = Audio->SamplesPerBuffer;
      /* Jouer fichier du FileLoader si besoin */
      if (PlayWave)
	{
	  buf = new float *[2];
	  buf[0] = new float[Audio->SamplesPerBuffer];
	  buf[1] = new float[Audio->SamplesPerBuffer];

	  size = PlayWave->GetNumberOfFrames() - PlayWavePos;
	  if (size > Audio->SamplesPerBuffer)
	    size = Audio->SamplesPerBuffer;
	  else
	    {
	      memset(buf[0], 0, Audio->SamplesPerBuffer * sizeof(float));
	      memset(buf[1], 0, Audio->SamplesPerBuffer * sizeof(float));
	    }	      
	  PlayWave->Read(buf, PlayWavePos, size);
	  ExtraBufs.push_back(new ChanBuf(buf, PlayWaveChannel));
	  PlayWavePos += size;
	  if (PlayWavePos >= PlayWave->GetNumberOfFrames())
	    {
	      delete PlayWave;
	      PlayWave = 0x0;
	    }
	}
      //LastCurBlock = CurBlock;
      /* Envoi des buffers supplémentaire */
      for (B = ExtraBufs.begin(); B != ExtraBufs.end(); B++)
	{
	  // Channel Mono
	  if (!(*B)->Buffer[1])
	    (*B)->Chan->PushBuffer((*B)->Buffer[0]);
	  else // Channel Stéréo
	    (*B)->Chan->PushBuffer((*B)->Buffer);
	}
      /* Appel de la fonction Mix->MixOutput(); */
      //SeqMutex.Lock();
      if (Exporting)
	{
	  if (CurrentPos >= EndLoopPos)
	    StopExport();
	  else
	    {
	      Mix->MixOutput(false);
	      WriteExport();
	    }
	}
      else
	Mix->MixOutput(true);
      //SeqMutex.Unlock();
      if (!Audio->StreamIsStarted)
	wxUsleep(1);      
      /* Cleanage des channels et buffers extra */
      for (B = ExtraBufs.begin(); B != ExtraBufs.end(); B++)
	{
	  (*B)->DeleteBuffer();
	  delete *B;
	}
      ExtraBufs.clear();      
    }
  cout << "[SEQ] Thread finished !" << endl;
  return (Wait());
}

void					Sequencer::OnExit()
{
  cout << "[SEQ] Thread terminated" << endl;
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
  SeqMutex.Unlock();
}

void					Sequencer::Stop()
{
  list<RackTrack *>::iterator		RacksTrack;
  list<Plugin *>::iterator		Plug;

  SeqMutex.Lock();

  for (RacksTrack = RackPanel->RackTracks.begin(); 
       RacksTrack != RackPanel->RackTracks.end(); RacksTrack++)
    for (Plug = (*RacksTrack)->Racks.begin(); Plug != (*RacksTrack)->Racks.end();
	 Plug++)
      (*Plug)->Stop();

  Playing = false;

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
  t->Index = Tracks.size();
  Tracks.push_back(t);
  SeqMutex.Unlock();
}

void					Sequencer::RemoveTrack()
{
  SeqMutex.Lock();  
  Track* track = Tracks.back();
  Tracks.pop_back();
  delete track;
  SeqMutex.Unlock();
}

void					Sequencer::PrepareRecording()
{
  vector<Track *>::iterator		T;      

  for (T = Tracks.begin(); T != Tracks.end(); T++)
    {
      cout << "preparing track: " << *T << endl;
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
      T->Wave = new AudioPattern(CurrentPos, CurrentPos + 0.1, T->Index); 
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
      T->Midi = new MidiPattern(CurrentPos, CurrentPos + 0.1, T->Index);
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
  printf("[SEQ] Received midi in for id %d : %2x %2x %2x\n", msg->Id, msg->Msg[0], msg->Msg[1], msg->Msg[2]);
  MidiEvents.push_back(msg);
}

void					Sequencer::AddNote(Track *t, MidiEvent &event)
{
  if (t->Midi)
    {
      cout << "[SEQ] Adding note to track" << endl;
      MidiEvent *e = new MidiEvent(event);
      e->Position = CurrentPos - t->Midi->GetPosition();
      t->Midi->AddEvent(e);
      PatternsToRefresh.push_back(t->Midi);
      // envoi evenement rafraichissement patter
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
    Calcul du coefficient de vitesse: Nombre de mesure par sample

    BPM = battement/minute
    SigNumerator = battement/mesure
    BPM/SigNumerator = mesure/minute
    (minute/mesure) / 60 = mesure/sec
    SampleRate = sample/sec
    (mesure/sec) / (sample/sec) = mesure/sample
  */

  MeasurePerSample = ((BPM / SigNumerator) / 60.0) / Audio->SampleRate;
  SamplesPerMeasure = 1 / MeasurePerSample;
  cout << "[SEQ] MeasurePerSample: " << MeasurePerSample << endl;
}

void					Sequencer::SetCurrentPos(double pos)
{
  //StartAudioPos = Audio->GetTime();
  CurAudioPos = (long)(pos / MeasurePerSample);
  CurrentPos = pos;
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
  // On update la taille des patterns audio
  for (i = Tracks.begin(); i != Tracks.end(); i++)
    if ((*i)->IsAudioTrack())
      for (j = (*i)->TrackPattern->Patterns.begin(); j != (*i)->TrackPattern->Patterns.end(); 
	   j++)
	(*j)->OnBpmChange();
}

void					Sequencer::SetSigNumerator(int signum)
{
  SigNumerator = signum;
  CalcSpeed();
}

Pattern					*Sequencer::GetCurrentPattern(Track *t)
{
  vector<Pattern *>::iterator		i;
  double				delta_mes = MeasurePerSample * Audio->SamplesPerBuffer;

  for (i = t->TrackPattern->Patterns.begin(); i != t->TrackPattern->Patterns.end(); i++)
    {
      if ((CurrentPos + delta_mes >= (*i)->GetPosition()) && 
	  (CurrentPos < (*i)->GetEndPosition()))
	{
	  return (*i);
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
  else 
    {
      p->LastBlock++;
      return (p->GetBlock(p->LastBlock));
    }
  //  cout << "[SEQ] Current Block: " << CurPatternBlock << endl;
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
	  T->TrackOpt->SetVuValue((*i)->Msg[2]);
	  if (T->TrackOpt->Connected)
	    {
	      curevent = new WiredEvent;
	      curevent->Type = WIRED_MIDI_EVENT;
	      curevent->DeltaFrames = (long)((p->GetPosition() + (*i)->Position - CurrentPos) 
					     * SamplesPerMeasure);
	      curevent->NoteLength = (long)((*i)->EndPosition * SamplesPerMeasure);
	      memcpy(curevent->MidiData, (*i)->Msg, sizeof(int) * 3);

	      // Envoi au plugin connecté
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

	    delete (p);

	    break;
	  }
    }
}

void					Sequencer::SetBufferSize()
{
  SeqMutex.Lock();
  AllocBuf1 = new float *[2];
  AllocBuf1[0] = new float [Audio->SamplesPerBuffer];
  AllocBuf1[1] = new float [Audio->SamplesPerBuffer];  
  AllocBuf2 = new float *[2];
  AllocBuf2[0] = new float [Audio->SamplesPerBuffer];
  AllocBuf2[1] = new float [Audio->SamplesPerBuffer];  
  SeqMutex.Unlock();
}

void					Sequencer::AddMidiPattern(list<SeqCreateEvent *> *l, Plugin *plug)
{
  vector<Track *>::iterator		i;
  list<SeqCreateEvent *>::iterator	j;
  Track					*t = 0x0;
  MidiPattern				*p;
  MidiEvent				*e;

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
  p = new MidiPattern(CurrentPos, CurrentPos + 1.0, t->TrackOpt->Index - 1);
  for (j = l->begin(); j != l->end(); j++)
    {
      e = new MidiEvent(0, (*j)->Position, (*j)->MidiMsg);
      e->EndPosition = (*j)->EndPosition;
      //      cout << "adding event: " << e->Position << " ; end: " << e->EndPosition << ", midimsg: " << e->Msg << endl;
      p->AddEvent(e);
    }
  t->AddPattern(p);
  p->DrawMidi();
}

void					Sequencer::ExportToWave(string filename)
{
  Seq->Stop();
  try
    {
      ExportWave = new WriteWaveFile(filename, (int)Audio->SampleRate, 2, 
				     SF_FORMAT_PCM_16);
      Seq->SetCurrentPos(BeginLoopPos);
      
      SeqMutex.Lock();
      Exporting = true;
      SeqMutex.Unlock();
      
      Seq->Play();
    } 
  catch (...)
    {
      cout << "ERROR EXPORTING" << endl; // FIXME do .. something ?
    } 
}

void					Sequencer::StopExport()
{
  Stop();
  SeqMutex.Lock();
  Exporting = false;
  if (ExportWave)
    {
      delete ExportWave;
      ExportWave = 0x0;
    }
  SeqMutex.Unlock();
}

void					Sequencer::WriteExport()
{
  long					i, j, chan;
  
  for (i = 0, j = 0; i < Audio->SamplesPerBuffer; j++)
    {
      AllocBuf1[0][i++] = Mix->OutputLeft[j];
      AllocBuf1[0][i++] = Mix->OutputRight[j];
    }
  ExportWave->WriteFloat(AllocBuf1[0], Audio->SamplesPerBuffer); 
  for (i = 0; i < Audio->SamplesPerBuffer; j++)
    {
      AllocBuf1[0][i++] = Mix->OutputLeft[j];
      AllocBuf1[0][i++] = Mix->OutputRight[j];  
    }
  ExportWave->WriteFloat(AllocBuf1[0], Audio->SamplesPerBuffer); 
}

void					Sequencer::PlayFile(string filename, bool isakai)
{
  StopFile();

  try
    {
      if (isakai)
	{
	  string mDevice, mFilename, mName;
	  int mPart;
	  	  
	  mDevice = filename.substr(0, filename.find(":", 0));
	  filename = filename.substr(filename.find(":", 0) + 1, filename.size() - filename.find(":", 0));
	  mFilename = filename.substr(10, filename.size() - 10);
	  int pos = mFilename.find("/", 0);
	  mPart = mFilename.substr(0, pos).c_str()[0] - 64;
	  mFilename = mFilename.substr(pos, mFilename.size() - pos);
	  int opos = 0;
	  while ((pos = mFilename.find("/", opos)) != string::npos)
	    opos = pos + 1;
	  
	  mName = mFilename.substr(opos, mFilename.size() - opos);
	  mFilename = mFilename.substr(1, opos - 2);
	  cout << "device: " << mDevice << "; part: " << mPart << "; name: " << mName << "; filename: " << mFilename << endl;
	  t_akaiSample *sample = akaiGetSampleByName((char *)mDevice.c_str(), mPart, 
						     (char *)mFilename.c_str(), 
						     (char *)mName.c_str());	  	
	  WaveFile *w = new WaveFile(sample->buffer, sample->size, 2, sample->rate);

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

      delete w;
    }
}
