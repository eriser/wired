// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <wx/filename.h>
#include "WiredSession.h"
#include "Sequencer.h"
#include "SequencerGui.h"
#include "Transport.h"
#include "Rack.h"
#include "AudioCenter.h"
#include "../plugins/PluginLoader.h"
#include "../sequencer/Track.h"
#include "../gui/SeqTrack.h"
#include "../gui/SeqTrackPattern.h"
#include "../gui/Pattern.h"
#include "../gui/AudioPattern.h"
#include "../gui/MidiPattern.h"
#include "../midi/midi.h"

extern vector<PluginLoader *>	LoadedPluginsList;
extern PlugStartInfo		StartInfo;

WiredSession::WiredSession(wxString filename, wxString audiodir)
  : FileName(filename), AudioDir(audiodir), fd(-1) 
{ 

}

WiredSession::~WiredSession()
{
  if (fd != -1)
    close(fd);
}

bool WiredSession::Load()
{
  if ((fd = open(FileName.mb_str(*wxConvCurrent), O_RDONLY)) != -1)
    {
      t_Header		header;
      t_RackTrack	racktrack;
      t_Plugin		plugin;
      t_Sequencer	sequencer;
      t_Track		track;
      t_Pattern		pattern;
      t_AudioPattern	audio_pattern;
      t_MidiPattern	midi_pattern;
      t_MidiEvent	midi_event;
      

      // Header
      read(fd, &header, sizeof (header));
      if ((header.Magic[0] != 'W') || (header.Magic[1] != 'I') ||
	  (header.Magic[2] != 'R') || (header.Magic[3] != 'E')) 
	{
	  cout << "[WIREDSESSION] Bad magic !" << endl;
	  return (false);
	}
      wxChar s[header.AudioDirLen + 1];
      read(fd, s, header.AudioDirLen);
      s[header.AudioDirLen] = 0;
      wxFileName session_dir(FileName.c_str());
      wxFileName f(s);

      f.MakeAbsolute(session_dir.GetPath());
      AudioDir = f.GetFullPath();
      cout << "[WIREDSESSION] Audio dir: " << AudioDir << endl;

      // RackTrack / Plugin
      long i;
      long j;
      RackTrack *r;
      vector<PluginLoader *>::iterator it;
      PluginLoader *p = 0x0;
      Plugin *plug;

      for (i = 0; i < header.NumberOfRackTracks; i++)
	{
	  read(fd, &racktrack, sizeof (racktrack));
	  r = RackPanel->AddTrack();
	  for (j = 0; j < racktrack.NumberOfPlugins; j++)
	    {
	      p = 0x0;
	      plug = 0x0;
	      read(fd, plugin.Id, sizeof (plugin.Id));
	      for (it = LoadedPluginsList.begin(); it != LoadedPluginsList.end(); it++)
		if (COMPARE_IDS((*it)->InitInfo.UniqueId, plugin.Id))
		  {
		    p = *it;
		    break;
		  }
	      if (p)
		{
		  cout << "[WIREDSESSION] Creating rack for plugin: " << p->InitInfo.Name << endl;
  		  plug = r->AddRack(StartInfo, p);
		}
	      else
		cout << "[WIREDSESSION] Plugin with Id  " << plugin.Id << " is not loaded" << endl;     
	      read(fd, &plugin.NameLen, sizeof (plugin.NameLen));
	      plugin.Name = new wxChar[plugin.NameLen + 1];
	      read(fd, plugin.Name, plugin.NameLen);
	      plugin.Name[plugin.NameLen] = 0;
	      read(fd, &plugin.DataLen, sizeof (plugin.DataLen));
	      if (plug)
		{
		  plug->Name = plugin.Name;
		  if (plugin.DataLen > 0)		    
		    plug->Load(fd, plugin.DataLen);
		}
	      delete plugin.Name;
	    }
	}

      // Sequencer
      read(fd, &sequencer, sizeof (sequencer));
      Seq->SetBPM(sequencer.BPM);
      Seq->SigNumerator = sequencer.SigNumerator;
      Seq->SigDenominator = sequencer.SigDenominator;

      Seq->SetCurrentPos(sequencer.CurrentPos);
      SeqPanel->SetCurrentPos(sequencer.CurrentPos);

      Seq->BeginLoopPos = sequencer.BeginLoopPos;
      Seq->EndLoopPos = sequencer.EndLoopPos;
      Seq->EndPos = sequencer.EndPos;
      Seq->Loop = sequencer.Loop;
      Seq->Click = sequencer.Click;

      SeqPanel->SetBeginLoopPos(sequencer.BeginLoopPos);
      SeqPanel->SetEndLoopPos(sequencer.EndLoopPos);
      SeqPanel->SetEndPos(sequencer.EndPos);

      TransportPanel->SetBpm(Seq->BPM);
      TransportPanel->SetSigNumerator(Seq->SigNumerator);
      TransportPanel->SetSigDenominator(Seq->SigDenominator);
      TransportPanel->SetLoop(Seq->Loop);
      TransportPanel->SetClick(Seq->Click);
      
      // Track / Pattern
      Track *t;
      for (i = 0; i < sequencer.NumberOfTracks; i++)
	{
	  read(fd, &(track.Type), sizeof (track.Type));
	  read(fd, &(track.Mute), sizeof (track.Mute));
	  read(fd, &(track.Record), sizeof (track.Record));
	  read(fd, &(track.DeviceId), sizeof (track.DeviceId));
	  read(fd, &(track.PluginId), sizeof (track.PluginId));
	  read(fd, &(track.NameLen), sizeof (track.NameLen));

	  track.Name = new wxChar[track.NameLen + 1];
	  read(fd, track.Name, track.NameLen);
	  track.Name[track.NameLen] = 0;
	  read(fd, &(track.NumberOfPatterns), sizeof (track.NumberOfPatterns));
	  t = SeqPanel->AddTrack(track.Type ? false : true);
	  t->TrackOpt->SetMute(track.Mute);
	  t->TrackOpt->SetRecording(track.Record);
	  t->TrackOpt->SetDeviceId(track.DeviceId);

	  list<RackTrack *>::iterator		it;
	  list<Plugin *>::iterator		it2;
	  int					id = 0;
	  bool					done = false;

	  for (it = RackPanel->RackTracks.begin(); 
	       (it != RackPanel->RackTracks.end()) && !done; it++)
	    for (it2 = (*it)->Racks.begin(); it2 != (*it)->Racks.end(); it2++)
	      {
		if (id == track.PluginId)
		  {
		    t->TrackOpt->ConnectTo(*it2);
		    done = true;
		    break;
		  }
		id++;
	      }

	  //t->TrackOpt->Text->SetValue(track.Name);
	  t->TrackOpt->SetName(track.Name);
	  
	  delete track.Name;
	  // Pattern
	  for (j = 0; j < track.NumberOfPatterns; j++)
	    {
	      read(fd, &pattern.Position, sizeof (pattern.Position));
	      read(fd, &pattern.EndPosition, sizeof (pattern.EndPosition));
	      read(fd, &pattern.NameLen, sizeof (pattern.NameLen));
	      pattern.Name = new wxChar[pattern.NameLen + 1];
	      read(fd, pattern.Name, pattern.NameLen);
	      pattern.Name[pattern.NameLen] = 0;
	      
	      // AudioPattern / MidiPattern
	      if (t->IsAudioTrack())
		{
		  read(fd, &audio_pattern.StartWavePos, sizeof (audio_pattern.StartWavePos));
		  read(fd, &audio_pattern.EndWavePos, sizeof (audio_pattern.EndWavePos));
		  read(fd, &audio_pattern.FilenameLen, sizeof (audio_pattern.FilenameLen));
		  audio_pattern.Filename = new wxChar[audio_pattern.FilenameLen + 1];
		  read(fd, audio_pattern.Filename, audio_pattern.FilenameLen);	  
		  audio_pattern.Filename[audio_pattern.FilenameLen] = 0;
		  AudioPattern	*p = new AudioPattern(pattern.Position, 
						      pattern.EndPosition, i);
		  WaveFile	*w = WaveCenter.AddWaveFile(audio_pattern.Filename);
		  if (w)
		    {		      
		      p->SetStartWavePos(audio_pattern.StartWavePos);
		      p->SetEndWavePos(audio_pattern.EndWavePos);
		      p->SetWave(w);
		      p->FileName = audio_pattern.Filename;
		      t->AddPattern(p);
		    }
		  else
		    cout << "[WIREDSESSION] Could not open file: " 
			 << audio_pattern.Filename << endl;
		  delete audio_pattern.Filename;
		}
	      else
		{
		  read(fd, &midi_pattern, sizeof (midi_pattern));	
		  MidiPattern *p = new MidiPattern(pattern.Position, 
						   pattern.EndPosition, i);
		  MidiEvent			*midi_e;
		  
		  p->SetPPQN(midi_pattern.PPQN);
		  for (int count = 0; count < midi_pattern.NumberOfEvents; count++)
		    {
		      read(fd, &midi_event, sizeof (midi_event));
		      midi_e = new MidiEvent(0, midi_event.Position, midi_event.Msg);
		      midi_e->EndPosition = midi_event.EndPosition;
		      p->AddEvent(midi_e);
		    }
		  t->AddPattern(p);
		} 
	      delete pattern.Name;	     
	    }
	}
      close(fd);
      return (true);
    }
  cout << "[WIREDSESSION] Loading failed for file: " << FileName << endl;
  return (false);
}

bool WiredSession::Save()
{
  if ((fd = open(FileName.mb_str(*wxConvCurrent), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) != -1)
    {
      t_Header		header;
      t_RackTrack	racktrack;
      t_Plugin		plugin;
      t_Sequencer	sequencer;
      t_Track		track;
      t_Pattern		pattern;
      t_AudioPattern	audio_pattern;
      t_MidiPattern	midi_pattern;
      t_MidiEvent	midi_event;
      
      wxFileName session_dir(FileName.c_str());

      // Header
      strcpy(header.Magic, "WIRE");

      // Make audio dir path absolute
      wxFileName f(AudioDir.c_str());

      f.MakeRelativeTo(session_dir.GetPath());

      header.AudioDirLen = f.GetFullPath().Length();// AudioDir.size();
      header.NumberOfRackTracks = RackPanel->RackTracks.size();
      write(fd, &header, sizeof (header));
      write(fd, f.GetFullPath().c_str(), header.AudioDirLen);

      // RackTrack / Plugin
      list<RackTrack *>::iterator i;
      list<Plugin *>::iterator j;
      unsigned int pos;

      for (i = RackPanel->RackTracks.begin(); i != RackPanel->RackTracks.end(); i++)
	{
	  racktrack.NumberOfPlugins = (*i)->Racks.size();
	  write(fd, &racktrack, sizeof (racktrack));
	  for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	    {
	      plugin.Id[0] = (*j)->InitInfo->UniqueId[0];
	      plugin.Id[1] = (*j)->InitInfo->UniqueId[1];
	      plugin.Id[2] = (*j)->InitInfo->UniqueId[2];
	      plugin.Id[3] = (*j)->InitInfo->UniqueId[3];
	      plugin.NameLen = (*j)->Name.size();
//	      plugin.Name = ()(*j)->Name;

	      write(fd, plugin.Id, sizeof (plugin.Id));
	      write(fd, &(plugin.NameLen), sizeof (plugin.NameLen));
	      write(fd, plugin.Name, plugin.NameLen);
	      
	      pos = lseek(fd, sizeof (long), SEEK_CUR) - sizeof (long);
	      plugin.DataLen = (*j)->Save(fd);
	      lseek(fd, pos, SEEK_SET);
	      write(fd, &(plugin.DataLen), sizeof (plugin.DataLen));
	      lseek(fd, plugin.DataLen, SEEK_CUR);
	    }
	}
      
      // Sequencer
      sequencer.BPM = Seq->BPM;
      sequencer.SigNumerator = Seq->SigNumerator;
      sequencer.SigDenominator = Seq->SigDenominator;
      sequencer.CurrentPos = Seq->CurrentPos;
      sequencer.BeginLoopPos = Seq->BeginLoopPos;
      sequencer.EndLoopPos = Seq->EndLoopPos;
      sequencer.EndPos = Seq->EndPos;
      sequencer.NumberOfTracks = Seq->Tracks.size();
      sequencer.Loop = Seq->Loop;
      sequencer.Click = Seq->Click;
      write(fd, &sequencer, sizeof (sequencer));

      // Track / Pattern
      vector<Track *>::iterator k;
      vector<Pattern *>::iterator l;
      bool found;
      wxString s;

      for (k = Seq->Tracks.begin(); k != Seq->Tracks.end(); k++)
	{
	  track.Type = (*k)->IsAudioTrack() ? 0 : 1;
	  track.Mute = (*k)->TrackOpt->Mute;
	  track.Record = (*k)->TrackOpt->Record;
	  track.DeviceId = (*k)->TrackOpt->DeviceId;

	  track.PluginId = 0; 	  
	  for (found = false, i = RackPanel->RackTracks.begin(); 
	       (i != RackPanel->RackTracks.end()) && !found; i++)
	    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	      {
		if (*j == (*k)->TrackOpt->Connected)
		  {
		    found = true;
		    break;
		  }
		(track.PluginId)++; 	  
	      }
	  

	  track.NameLen = (*k)->TrackOpt->Text->GetLineLength(0);
	  s = (*k)->TrackOpt->Text->GetValue();
//	  track.Name = wxString(s, *wxConvCurrent);
	  track.NumberOfPatterns = (*k)->TrackPattern->Patterns.size();
	  write(fd, &(track.Type), sizeof (track.Type));
	  write(fd, &(track.Mute), sizeof (track.Mute));
	  write(fd, &(track.Record), sizeof (track.Record));
	  write(fd, &(track.DeviceId), sizeof (track.DeviceId));
	  write(fd, &(track.PluginId), sizeof (track.PluginId));
	  write(fd, &(track.NameLen), sizeof (track.NameLen));
	  write(fd, track.Name, track.NameLen);
	  write(fd, &(track.NumberOfPatterns), sizeof (track.NumberOfPatterns));

	  for (l = (*k)->TrackPattern->Patterns.begin(); l != (*k)->TrackPattern->Patterns.end(); l++)
	    {
	      pattern.Position = (*l)->GetPosition();
	      pattern.EndPosition = (*l)->GetEndPosition();
	      pattern.NameLen = (*l)->GetName().size();
	//      pattern.Name = wxString((*l)->GetName(), *wxConvCurrent);
	      write(fd, &pattern.Position, sizeof (pattern.Position));
	      write(fd, &pattern.EndPosition, sizeof (pattern.EndPosition));
	      write(fd, &pattern.NameLen, sizeof (pattern.NameLen));
	      write(fd, pattern.Name, pattern.NameLen);
	      
	      // AudioPattern / MidiPattern
	      if ((*k)->IsAudioTrack())
		{
		  AudioPattern			*p = (AudioPattern *)*l;
		  audio_pattern.StartWavePos = p->GetStartWavePos();
		  audio_pattern.EndWavePos = p->GetEndWavePos();
		  audio_pattern.FilenameLen = p->FileName.size();
//		  audio_pattern.Filename = wxString(p->FileName, *wxConvCurrent);
		  write(fd, &audio_pattern.StartWavePos, sizeof (audio_pattern.StartWavePos));
		  write(fd, &audio_pattern.EndWavePos, sizeof (audio_pattern.EndWavePos));
		  write(fd, &audio_pattern.FilenameLen, sizeof (audio_pattern.FilenameLen));
//		  write(fd, (const char *)audio_pattern.Filename.mb_str(*wxConvCurrent), audio_pattern.FilenameLen);	  
		}
	      else
		{
		  vector<MidiEvent *>::iterator m;

		  MidiPattern *p = (MidiPattern *)*l;
		  midi_pattern.PPQN = p->GetPPQN();
		  midi_pattern.NumberOfEvents = p->Events.size();
		  write(fd, &midi_pattern, sizeof (midi_pattern));	
		  for (m = p->Events.begin(); m != p->Events.end(); m++)
		    {
		      midi_event.Position = (*m)->Position;
		      midi_event.EndPosition = (*m)->EndPosition;
		      memcpy(midi_event.Msg, (*m)->Msg, 3 * sizeof (int));
		      write(fd, &midi_event, sizeof (midi_event));
		    }
		}
	    }
	}
      close(fd);      
      return (true);
    }
  cout << "[WIREDSESSION] Writing failed for file: " << FileName << endl;
  return (false);
}
