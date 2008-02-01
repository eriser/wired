// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MidiFile.h"
#include  "Sequencer.h"
#include <fcntl.h>
#include <unistd.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <wx/filename.h>
#include <iostream>

void	MakeMidiFileName(wxString &filename, unsigned long seqTrackIndex)
{
  wxString ext = wxT("mid");

  filename.Clear();
  filename << DEFAULT_MIDI_PATH << wxFileName::GetPathSeparator()
    << DEFAULT_MIDI_NAME << seqTrackIndex << wxT(".") << ext;
}

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

MidiTrack::MidiTrack(vector<MidiEvent *> &evts, unsigned short PPQN, wxString filename,
    unsigned int noTrack)
{
  int			i;
  unsigned long		position;
  double		ratio;
  unsigned long max_position = 0;

  _noTrack = noTrack;
  _filename = filename;
  ppqn = PPQN;
  if (ppqn == 1)
    ppqn = DEFAULT_MIDI_PPQN;
  ratio = (double)Seq->SigNumerator * (double)ppqn;


  cout << "[MidiTrack] MidiTrack() creating MidiTrack from list of midi events" << endl;
  cout << " ppqn " << (int)ppqn << endl;
  cout << " position ratio " << ratio << endl;
  cout << " " << evts.size() << " events" << endl;
  Events.clear();
  for (i = 0; i < evts.size() ; i++)
  {
    position = (unsigned long)(evts[i]->Position * ratio);
#ifdef __DEBUG__
    cout << " adding event " << i << endl;
    cout << " wired position " << evts[i]->Position << endl;
    cout << " midi position " << position << endl;
#endif
    Events.push_back(
	new MidiFileEvent(position, evts[i]->Msg[0], 0, evts[i]->Msg[1], evts[i]->Msg[2])
	);
	if (evts[i]->Position > max_position)
	  max_position = evts[i]->Position;
  }
  MaxPos = max_position;
}

MidiTrack::MidiTrack(unsigned long len, unsigned short PPQN)
{
  _filename = wxT("");
  vector<MidiEvent *> evts;
  MaxPos = len;
  
  MidiTrack(evts, PPQN, _filename, Seq->Tracks.size());
}

MidiTrack::MidiTrack(unsigned long len, unsigned char *buffer, unsigned short PPQN,
		     wxString filename, unsigned int noTrack)
{
  // save parent's reference
  _noTrack = noTrack;

  _filename = filename;

#ifdef __DEBUG__
  cout << " noTrack " << _noTrack << endl;
#endif

  unsigned int abs = 0;
  unsigned char runningst = 0;
  ppqn = PPQN;
  for (wxUint32 ofs = 0; ofs < len; )
  {
    wxUint32 dt = GetVLQ(buffer, ofs);
#ifdef __DEBUG__
    cout << " delta " << dt << endl;
#endif
    abs += dt;
    unsigned char evttype = buffer[ofs++];
    if (IS_SYSEX(evttype))
    {
      wxUint32 len = GetVLQ(buffer, ofs);
      Events.push_back(new SysExEvent(abs, evttype, len, buffer + ofs));
      ofs += len;
    }
    else
      if (IS_NONMIDI(evttype))
      {
        unsigned char subtype = buffer[ofs++];
        wxUint32 len = GetVLQ(buffer, ofs);
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
#ifdef __DEBUG__
	    cout << " is midi 1" << endl;
#endif
	    Events.push_back(new MidiFileEvent(abs, ME_CODE(evttype), ME_CHANNEL(evttype), 
                                           p1, buffer[ofs++]));
            break;
          case ME_PRGMCHANGE:
          case ME_KEYPRESSURE:
#ifdef __DEBUG__
	    cout << " is midi 2" << endl;
#endif
	    Events.push_back(new MidiFileEvent(abs, ME_CODE(evttype), ME_CHANNEL(evttype), 
                                           p1, 0));
            break;
        }
      }
  }
  MaxPos = abs;
}

inline bool sort_event_by_pos(Event *a, Event *b)
{
	return (a->GetPos() <= b->GetPos());
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
  bool					is_running = false;

  cout << "[MidiTrack] WriteChunk() nb of events : '" << Events.size() << "'" << endl;
  stable_sort(Events.begin(), Events.end(), sort_event_by_pos);
  for (unsigned int i = 0; i < Events.size(); i++)
  {
#ifdef __DEBUG__
    cout << " writing event " << i << endl;
#endif
    switch (Events[i]->GetType())
    {
      case MIDI_EVENT_TYPE:
	midiFileEvent = (MidiFileEvent *)Events[i];

	// calculating delta position from absolute
	abs_pos = midiFileEvent->GetPos();
	delta_pos = abs_pos - last_pos;
#ifdef __DEBUG__
	cout << " delta " << delta_pos << endl;
	cout << " for abs " << abs_pos << endl;
	cout << " and last " << last_pos << endl;
#endif
	last_pos = abs_pos;

	// writing delta
	bytesWritten += WriteDelta(midiFileHandle, delta_pos);

	// type toto see IS_RUNNING()
	evt_type = midiFileEvent->GetID() | midiFileEvent->GetChannel();
	if (!is_running)
	  bytesWritten += midiFileHandle.Write(&evt_type, sizeof(evt_type));
	is_running = true;

	// writing p0
	uc = midiFileEvent->GetParam(0);
	bytesWritten += midiFileHandle.Write(&uc, sizeof(uc));
	switch (ME_CODE(evt_type))
	{
	  case ME_NOTEOFF:
	  case ME_NOTEON:
	  case ME_POLYPRESSURE:
	  case ME_PITCHBEND:
	  case ME_CTRLCHANGE:
	    // writing p1
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
	bytesWritten += WriteDelta(midiFileHandle, len); bytesWritten += sysExEvent->WriteData(midiFileHandle);
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

#ifdef __DEBUG__
  cout << "[MidiTrack] WriteChunk() end" << endl;
#endif

  return (bytesWritten);
}

// from http://www.borg.com/~jglatt/tech/midifile.htm
size_t	MidiTrack::WriteDelta(wxFile &midiFileHandle, wxUint32 value)
{
   wxUint32		buffer = value & 0x7F;
   unsigned char	c;
   size_t		bytesWritten = 0;

   while ((value >>= 7))
   {
     buffer <<= 8;
     buffer |= ((value & 0x7F) | 0x80);
   }
   while (true)
   {
     c = (unsigned char)buffer;
     bytesWritten += midiFileHandle.Write(&c,sizeof(c));
     //bytesWritten += midiFileHandle.Write(&buffer,sizeof(c));
     if (buffer & 0x80)
       buffer >>= 8;
     else
       break;
   }
   return (bytesWritten);
}

inline bool sort_fileevent_by_pos(MidiFileEvent *a, MidiFileEvent *b)
{
	return (a->GetPos() <= b->GetPos());
}

vector<MidiFileEvent *> MidiTrack::GetMidiEvents()
{
  vector<MidiFileEvent *> e;
  for (unsigned int i = 0; i < Events.size(); i++)
    if (Events[i]->GetType() == MIDI_EVENT_TYPE)
      e.push_back((MidiFileEvent *)Events[i]);
  stable_sort(e.begin(), e.end(), sort_fileevent_by_pos);
  return (e);
}

wxUint32 MidiTrack::GetVLQ(unsigned char *buf, wxUint32 &ofs)
{
  wxUint32 val;
  unsigned char c;

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

MidiFile::MidiFile(wxString filename, unsigned long long seqTrackIndex)
{
  NbTracks = 0;
  Division = 0;
  Type = 0;
  if (seqTrackIndex != -1)
    MakeMidiFileName(filename, seqTrackIndex);
  this->filename = filename;
}

size_t	MidiFile::WriteMidiFile()
{
  wxFileName	wxFN(filename);
  size_t		bytesWritten = (size_t)0;

  wxFileName	path(DEFAULT_MIDI_PATH);

  // just in case
  if (!path.DirExists())
    if (!path.Mkdir())
      cerr << "[MidiFile] WriteMidiFile() problem creating directory "
		<< path.GetFullPath().mb_str() << endl;
  if (wxFN.IsOk())
  {
    wxFile			midiFileHandle;
    t_chunk			tc_chunk;
    unsigned short		be_division, be_type, be_nbtracks;
    MidiTrack			*midiTrack;
    unsigned short		i;
    //unsigned long		len;
    wxUint32			len;
    wxFileOffset		header_offset, after_chunk_offset;

    cout << "[MidiFile] WriteMidiFile() writing to file '" << filename.mb_str() << "'" << endl;
    // check continualy for write return ? (running out of space stuff...)
    // or check at the end ?
    // create the file, destroy it if it exists
    midiFileHandle.Create(filename, true);

    // midi header
    MK_MIDI_HEADER(tc_chunk.ID);
    tc_chunk.Size = ntohs((unsigned short)6);
    bytesWritten += midiFileHandle.Write(&tc_chunk, sizeof(tc_chunk));
    be_type = ntohs(Type);
    bytesWritten += midiFileHandle.Write(&be_type, 2);
    be_nbtracks = ntohs(NbTracks);
    bytesWritten += midiFileHandle.Write(&be_nbtracks, 2);
    be_division = ntohs(Division);
    bytesWritten += midiFileHandle.Write(&be_division, 2);

    // cycle through all tracks
    MK_MIDI_TRK_HDR(tc_chunk.ID);
    for (i = 0; i < NbTracks; i++)
    {
      cout << "[MidiFile] WriteMidiFile() writing track '" << i << "'" << endl;
      midiTrack = Tracks[i]; //->GetMidiEvents();

      // saving header offset to write it later
      header_offset = midiFileHandle.Tell();

      // skipping header (will be written later)
      len = 0;
      //midiFileHandle.Seek(sizeof(tc_chunk), wxFromCurrent);
      midiFileHandle.Write(&tc_chunk, sizeof(tc_chunk));

      // writing chunk
      len = midiTrack->WriteChunk(midiFileHandle);
      bytesWritten += len;

      // saving position
      after_chunk_offset = midiFileHandle.Tell();

      // back to current header position
      if (midiFileHandle.Seek(header_offset) == -1)
	cout << "[MidiFile] WriteMidiFile() seek header failed" << endl;

      // writing header
      //tc_chunk.Size = ntohl(len);
      tc_chunk.Size = wxUINT32_SWAP_ALWAYS(len);
      bytesWritten += midiFileHandle.Write(&tc_chunk, sizeof(tc_chunk));

      // back to end of current chunk (end of file)
      if (midiFileHandle.Seek(after_chunk_offset) == -1)
	cout << "[MidiFile] WriteMidiFile() seek after chunk failed" << endl;
    }
  }
  else
	cerr << "[MidiFile] WriteMidiFile() Filename is not ok : " << filename.mb_str() << endl;
  return (bytesWritten);
}

bool	MidiFile::InsertMidiTrack(MidiTrack *track, unsigned short noTrack)
{
  //if not too many tracks
  //what's max ??
  //16 ?
  //unsigned short ?
  this->filename = (track->GetFileName().Cmp(wxT("")) == 0 ? this->filename : track->GetFileName());
  //Tracks.push_back(track);

  vector<MidiTrack *>::iterator vMTi = Tracks.begin();
  int							i = 0;
  while (i < noTrack && vMTi != Tracks.end())
  {
	i++;
	vMTi++;
  }
  Tracks.insert(vMTi, track);

  NbTracks++;
  if (Division != track->GetPPQN())
  {
	if (NbTracks > 1)
	{
	  cout << "[MidiFile] InsertMidiTrack() New division, apparently... : from " << (int)Division
		<< " to " << (int) track->GetPPQN() << endl;
	}
	Division = track->GetPPQN();
  }
  if (NbTracks > 1)
	Type = 1;
}

void	MidiFile::ReadMidiFile()
{
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
      //ch.Size = htonl(ch.Size);
      ch.Size = wxUINT32_SWAP_ALWAYS(ch.Size);
    } while ((len == sizeof(ch)) && (!IS_MIDI_HEADER(ch.ID)));
    if (IS_MIDI_HEADER(ch.ID))
    {
#ifdef __DEBUG__
      cout << "is midi header" << endl;
#endif
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
#ifdef __DEBUG__
      cout << NbTracks << " tracks" << endl;
      cout << "Division " << Division << endl;
#endif
      for (unsigned short noTrack = 0; noTrack < NbTracks; noTrack++)
      {
#ifdef __DEBUG__
	cout << "reading track " << noTrack << endl;
#endif
        ch.Size = 0;
        do
        {
          MIDIFile.Seek(ch.Size, wxFromCurrent);
          len = MIDIFile.Read(&ch, sizeof(ch));
          //ch.Size = htonl(ch.Size);
          ch.Size = wxUINT32_SWAP_ALWAYS(ch.Size);
        } while ((len == sizeof(ch)) && (!IS_MIDI_TRK(ch.ID)));
        if (IS_MIDI_TRK(ch.ID))
        {
#ifdef __DEBUG__
	  cout << "midi track" << endl;
#endif
          unsigned char *track = (unsigned char *)malloc(ch.Size);
          len = MIDIFile.Read(track, ch.Size);
          if (len == ch.Size)
            Tracks.push_back(new MidiTrack(ch.Size, track, Division,
					   filename, noTrack));
	  else
	    cout << "[MidiFile] ReadMidiFile() track " << noTrack << " not added becouse len = " << len << " and ch.Size = " << (int)ch.Size << endl;
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
