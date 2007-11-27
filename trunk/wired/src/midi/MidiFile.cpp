// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MidiFile.h"
#include <fcntl.h>
#include <unistd.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <wx/filename.h>
#include <iostream>

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

unsigned long	SysExEvent::GetLen()
{
  return (len);
}

unsigned long	SysExEvent::GetPos()
{
  return (Pos);
}

size_t	SysExEvent::WriteData(wxFile &midiFileHandle)
{
  size_t	bytesWritten = 0;

  bytesWritten += midiFileHandle.Write(data, len);
  return (bytesWritten);
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

size_t	NonMidiEvent::WriteData(wxFile &midiFileHandle)
{
  size_t	bytesWritten = 0;

  bytesWritten += midiFileHandle.Write(data, len);
  return (bytesWritten);
}
 

/*************************************************************************************/
/*** Classe MidiTrack                                                              ***/
/*************************************************************************************/

MidiTrack::MidiTrack(unsigned long len, unsigned char *buffer, unsigned short PPQN,
		     wxString filename, unsigned int noTrack)
{
  // save parent's reference
  _filename = filename;
  _noTrack = noTrack;

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

size_t	MidiTrack::WriteChunk(wxFile &midiFileHandle)
{
  size_t					bytesWritten = 0;
  SysExEvent				*sysExEvent;
  NonMidiEvent				*nonMidiEvent;
  MidiFileEvent				*midiFileEvent;
  unsigned long				abs_pos;
  unsigned long				delta_pos;
  unsigned long				last_pos = 0;
  unsigned long				len;
  unsigned char				evt_type;
  unsigned char				uc;
  unsigned char				subtype;

  for (unsigned int i = 0; i < Events.size(); i++)
  {
	switch (Events[i]->GetType())
	{
	  case MIDI_EVENT_TYPE:
		midiFileEvent = (MidiFileEvent *)Events[i];

		// calculating delta position from absolute
		abs_pos = midiFileEvent->GetPos();
		delta_pos = abs_pos - last_pos;
		last_pos = abs_pos;

		// writing delta
		bytesWritten += WriteDelta(midiFileHandle, delta_pos);

		// type toto see IS_RUNNING()
		evt_type = midiFileEvent->GetID() | midiFileEvent->GetChannel();
		bytesWritten += midiFileHandle.Write(&evt_type, sizeof(evt_type));

        switch (ME_CODE(evt_type))
        {
          case ME_NOTEOFF:
          case ME_NOTEON:
          case ME_POLYPRESSURE:
          case ME_PITCHBEND:
          case ME_CTRLCHANGE:
			uc = midiFileEvent->GetParam(1);
			bytesWritten += midiFileHandle.Write(&uc, sizeof(uc));
            break;
          /*
           *case ME_PRGMCHANGE:
           *case ME_KEYPRESSURE:
           *  break;
           */
		  default:
			break;
        }
		break;
	  case SYSEX_EVENT_TYPE:
		sysExEvent = (SysExEvent *)Events[i];
		// calculating delta position from absolute
		abs_pos = sysExEvent->GetPos();
		delta_pos = abs_pos - last_pos;
		last_pos = abs_pos;

		// writing delta
		bytesWritten += WriteDelta(midiFileHandle, delta_pos);

		// writing event type
		uc = EVENT_SYSEX;
		bytesWritten += midiFileHandle.Write(&uc, sizeof(uc));

		// writing lenght and data
		len = sysExEvent->GetLen();
		bytesWritten += WriteDelta(midiFileHandle, len);
		bytesWritten += sysExEvent->WriteData(midiFileHandle);
		break;
	  case NONMIDI_EVENT_TYPE:
		nonMidiEvent = (NonMidiEvent *)Events[i];
		subtype = nonMidiEvent->GetID();
		bytesWritten += midiFileHandle.Write(&subtype, sizeof(subtype));
		len = nonMidiEvent->GetLen();
		bytesWritten += WriteDelta(midiFileHandle, len);
		bytesWritten += nonMidiEvent->WriteData(midiFileHandle);
		break;
	  default:
		cerr << "Unknown event number : " <<  (int)(Events[i]->GetType()) << endl;
		cerr << " after writing : " << (int)bytesWritten << " bytes" << endl;
		break;
	}
  }


  return (bytesWritten);
}

// from http://www.borg.com/~jglatt/tech/midifile.htm
size_t	MidiTrack::WriteDelta(wxFile &midiFileHandle, unsigned long value)
{
   unsigned long	buffer = value & 0x7F;
   unsigned char	c;
   size_t			bytesWritten = 0;

   while ((value >>= 7))
   {
     buffer <<= 8;
     buffer |= ((value & 0x7F) | 0x80);
   }
   while (true)
   {
	 c = (unsigned char)buffer;
	 bytesWritten += midiFileHandle.Write(&c,sizeof(c));
	 if (buffer & 0x80)
	   buffer >>= 8;
	 else
	   break;
   }
   return (bytesWritten);
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

static int		gl_MidiFileCounter;

MidiFile::MidiFile(wxString filename, bool newMidiFile)
{
  gl_MidiFileCounter++;
  NbTracks = 0;
  Division = 0;
  Type = 0;

  if (newMidiFile)
  {
	wxString ext = wxT("mid");
	wxString customFN_orig;

	customFN_orig.Clear();
	customFN_orig << DEFAULT_MIDI_PATH << wxFileName::GetPathSeparator() << DEFAULT_MIDI_NAME;
	filename = customFN_orig.BeforeLast('.');
	filename << gl_MidiFileCounter << wxT(".") << ext;
	while (wxFileName::FileExists(filename))
	{
	  gl_MidiFileCounter++;
	  filename = customFN_orig.BeforeLast('.');
	  filename << gl_MidiFileCounter << wxT(".") << ext;
	} 
  }
  this->filename = filename;
}

size_t	MidiFile::WriteMidiFile()
{
  wxFileName	wxFN(filename);
  size_t		bytesWritten = (size_t)0;

  if (wxFN.IsOk())
  {
	wxFile			midiFileHandle;
	t_chunk			tc_chunk;
	unsigned short	be_division, be_type, be_nbtracks;
	MidiTrack		*midiTrack;
	unsigned short	i;
	unsigned long	len;
	wxFileOffset	header_offset, after_chunk_offset;

	// create directory ?
	// check continualy for write return ? (running out of space stuff...)
	// or check at the end ?
	// create the file, destroy it if it exists
	midiFileHandle.Create(filename, true);

	// midi header
	MK_MIDI_HEADER(tc_chunk.ID);
	tc_chunk.Size = ntohl(6);
	bytesWritten += midiFileHandle.Write(&tc_chunk, sizeof(tc_chunk));
	be_type = ntohl(Type);
	bytesWritten += midiFileHandle.Write(&be_type, 2);
	be_nbtracks = ntohl(NbTracks);
	bytesWritten += midiFileHandle.Write(&be_nbtracks, 2);
	be_division = ntohl(Division);
	bytesWritten += midiFileHandle.Write(&be_division, 2);

	// cycle through all tracks
	MK_MIDI_TRK_HDR(tc_chunk.ID);
	for (i = 0; i < NbTracks; i++)
	{
	  midiTrack = Tracks[i]; //->GetMidiEvents();

	  // saving header offset to write it later
	  header_offset = midiFileHandle.Tell();

	  // skipping header (will be written later)
	  len = 0;
	  midiFileHandle.Seek(sizeof(tc_chunk), wxFromCurrent);

	  // writing chunk
	  len = midiTrack->WriteChunk(midiFileHandle);
	  bytesWritten += len;

	  // saving position
	  after_chunk_offset = midiFileHandle.Tell();

	  // back to current header position
	  midiFileHandle.Seek(header_offset);

	  // writing header
	  tc_chunk.Size = ntohl(len);
	  bytesWritten += midiFileHandle.Write(&tc_chunk, sizeof(tc_chunk));

	  // back to end of current chunk (end of file)
	  midiFileHandle.Seek(after_chunk_offset);
	}
  }
  else
	cerr << "[MidiFile::WriteMidiFile() Filename is not ok : " << filename.mb_str() << endl;
  return (bytesWritten);
}

bool	MidiFile::AppendMidiTrack(MidiTrack *track)
{
  //if pas trop de track
  //quelle est la limite ??
  //16 ?
  //unsigned short ?
  Tracks.push_back(track);
  NbTracks++;
  if (Division != track->GetPPQN())
  {
	if (NbTracks > 1)
	{
	  cout << "[MidiFile] AppendMidiTrack() New division, apparently... : from " << (int)Division
		<< " to " << (int) track->GetPPQN() << endl;
	}
	Division = track->GetPPQN();
  }
  if (NbTracks > 1)
	Type = 1;
}

void	MidiFile::ReadMidiFile()
{
  //this->filename = filename;
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
      for (unsigned short noTrack = 0; noTrack < NbTracks; noTrack++)
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
            Tracks.push_back(new MidiTrack(ch.Size, track, Division,
					   filename, noTrack));
          free(track);
        }
        else
          cout << "[MidiFile] Can't find track #" << noTrack << endl;
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
