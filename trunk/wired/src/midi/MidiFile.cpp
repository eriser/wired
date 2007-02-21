// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MidiFile.h"
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <wx/file.h>

/*************************************************************************************/
/*** Classe Event                                                                  ***/
/*************************************************************************************/

Event::Event(unsigned long pos, unsigned char ID)
{
  this->ID = ID;
  this->Pos = pos;
  this->Type = EVENT_TYPE;
}

Event::~Event()
{
}


/*************************************************************************************/
/*** Classe MidiFileEvent                                                              ***/
/*************************************************************************************/

MidiFileEvent::MidiFileEvent(unsigned long pos, unsigned char ID, unsigned char channel, 
                     unsigned char p1, unsigned char p2) : Event(pos, ID)
{
  this->Channel = channel;
  this->Type = MIDI_EVENT_TYPE;
  this->p1 = p1;
  this->p2 = p2;
}


/*************************************************************************************/
/*** Classe SysExEvent                                                             ***/
/*************************************************************************************/

SysExEvent::SysExEvent(unsigned long pos, unsigned char ID, unsigned long len, 
                       unsigned char *data) : Event(pos, ID)
{
  this->len = len; 
  this->Type = SYSEX_EVENT_TYPE;
  this->data = (unsigned char *)malloc(len);
  this->Pos = pos;
  this->ID = ID;
  memcpy(this->data, data, len);
}

SysExEvent::~SysExEvent()
{
	if (data)
	  free(data);
}
 

/*************************************************************************************/
/*** Classe NonMidiEvent                                                           ***/
/*************************************************************************************/

NonMidiEvent::NonMidiEvent(unsigned long pos, unsigned char ID, unsigned long len, 
                           unsigned char *data) : Event(pos, ID) 
{
  this->len = len; 
  this->Type = NONMIDI_EVENT_TYPE;
  this->data = (unsigned char *)malloc(len);
  this->Pos = pos;
  this->ID = ID;
  memcpy(this->data, data, len);
}

NonMidiEvent::~NonMidiEvent()
{
	if (data)
	  free(data);
}


/*************************************************************************************/
/*** Classe MidiTrack                                                              ***/
/*************************************************************************************/

MidiTrack::MidiTrack(unsigned long len, unsigned char *buffer, unsigned short PPQN)
{
  unsigned int abs = 0;
  unsigned char runningst = 0;
  ppqn = PPQN;
  for (unsigned long ofs = 0; ofs < len; )
  {
    unsigned long dt = GetVLQ(buffer, ofs);
    abs += dt;
    unsigned char evttype = buffer[ofs++];
    if (IS_SYSEX(evttype))
    {
      unsigned long len = GetVLQ(buffer, ofs);
      Events.push_back(new SysExEvent(abs, evttype, len, buffer + ofs));
      ofs += len;
    }
    else
      if (IS_NONMIDI(evttype))
      {
        unsigned char subtype = buffer[ofs++];
        unsigned long len = GetVLQ(buffer, ofs);
        Events.push_back(new NonMidiEvent(abs, subtype, len, buffer + ofs));
        ofs += len;
      }
      else
      {
        unsigned char p1 = 0;
        if (IS_RUNNING(evttype))
        {
          p1 = evttype;
          evttype = runningst;
        }
        else
        {
          runningst = evttype;
          p1 = buffer[ofs++];
        }
        switch (ME_CODE(evttype))
        {
          case ME_NOTEOFF:
          case ME_NOTEON:
          case ME_POLYPRESSURE:
          case ME_PITCHBEND:
          case ME_CTRLCHANGE:
	    Events.push_back(new MidiFileEvent(abs, ME_CODE(evttype), ME_CHANNEL(evttype), 
                                           p1, buffer[ofs++]));
            break;
          case ME_PRGMCHANGE:
          case ME_KEYPRESSURE:
	    Events.push_back(new MidiFileEvent(abs, ME_CODE(evttype), ME_CHANNEL(evttype), 
                                           p1, 0));
            break;
        }
      }
  }
  MaxPos = abs;
}

inline bool sort_by_pos(MidiFileEvent *a, MidiFileEvent *b)
{
	return (a->GetPos() <= b->GetPos());
}

vector<MidiFileEvent *> MidiTrack::GetMidiEvents()
{
  vector<MidiFileEvent *> e;
  for (unsigned int i = 0; i < Events.size(); i++)
    if (Events[i]->GetType() == MIDI_EVENT_TYPE)
      e.push_back((MidiFileEvent *)Events[i]);
  stable_sort(e.begin(), e.end(), sort_by_pos);
  return (e);
}

unsigned long MidiTrack::GetVLQ(unsigned char *buf, unsigned long &ofs)
{
  register unsigned long val;
  register unsigned char c;

  if ((val = buf[ofs++]) & 0x80)
  {
    val &= 0x7F;
    do
    {
      val = (val << 7) + ((c = buf[ofs++]) & 0x7F);
    } while (c & 0x80);
  }
  return (val);
}

MidiTrack::~MidiTrack()
{
  for (unsigned int i = 0; i < Events.size(); i++)
	  if (Events[i])
	    delete(Events[i]);
}


/*************************************************************************************/
/*** Classe MidiFile                                                               ***/
/*************************************************************************************/

MidiFile::MidiFile(wxString filename)
{
  this->filename = filename;
  NbTracks = 0;
  Division = 0;
  Type = 0;

  cout << "[MidiFile] Loading " << filename.mb_str() << "..." << endl;
  wxFile MIDIFile;
  MIDIFile.Open(filename);
  if (MIDIFile.IsOpened())
  {
    t_chunk ch;
    ch.Size = 0;
    unsigned long len = 0;
    do
    {
      MIDIFile.Seek(ch.Size, wxFromCurrent);
      len = MIDIFile.Read(&ch, sizeof(ch));
      ch.Size = htonl(ch.Size);
    } while ((len == sizeof(ch)) && (!IS_MIDI_HEADER(ch.ID)));
    if (IS_MIDI_HEADER(ch.ID))
    {
      if ((MIDIFile.Read(&Type, 2) != 2) ||
          (MIDIFile.Read(&NbTracks, 2) != 2) ||
          (MIDIFile.Read(&Division, 2) != 2))
      {
        perror("[MidiFile] read");
        MIDIFile.Close();
        return; 
      }
      Type = htons(Type);
      NbTracks = htons(NbTracks);
      Division = htons(Division);
      /*cout << "[MidiFile] Type: " << Type << endl;
      cout << "[MidiFile] NbTracks: " << NbTracks << endl;
      if (Division < 0x8000)
        cout << "[MidiFile] Division: " << Division << " PPQN" << endl;
      else
      {
        char f = (Division >> 8) & 0xFF;
        char s = Division & 0xFF;
        cout << "[MidiFile] Division :" << f << " frames " << s << " subframes" << endl;
      }*/
      for (unsigned short t = 0; t < NbTracks; t++)
      {
        ch.Size = 0;
        do
        {
          MIDIFile.Seek(ch.Size, wxFromCurrent);
          len = MIDIFile.Read(&ch, sizeof(ch));
          ch.Size = htonl(ch.Size);
        } while ((len == sizeof(ch)) && (!IS_MIDI_TRK(ch.ID)));
        if (IS_MIDI_TRK(ch.ID))
        {
          unsigned char *track = (unsigned char *)malloc(ch.Size);
          len = MIDIFile.Read(track, ch.Size);
          if (len == ch.Size)
            Tracks.push_back(new MidiTrack(ch.Size, track, Division));
          free(track);
        }
        else
          cout << "[MidiFile] Can't find track #" << t << endl;
      }
    }
   else
      cout << "[MidiFile] " << filename.mb_str() << " is not a valid midi file." << endl;
    MIDIFile.Close();
  }
  else
    perror("[MidiFile] open");
}

MidiFile::~MidiFile()
{
  for (unsigned int i = 0; i < Tracks.size(); i++)
  	if (Tracks[i])
	    delete(Tracks[i]);
}
