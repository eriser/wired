// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDIFILE_H__
#define __MIDIFILE_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <vector>

using namespace std;
using std::vector;


// CHUNK MIDI HEADER
#define MIDI_HDR_1			'M'
#define MIDI_HDR_2			'T'
#define MIDI_HDR_3			'h'
#define MIDI_HDR_4			'd'

// CHUNK MIDI TRACK
#define MIDI_TRK_1			'M'
#define MIDI_TRK_2			'T'
#define MIDI_TRK_3			'r'
#define MIDI_TRK_4			'k'

// TYPES D EVENEMENTS
#define EVENT_SYSEX			0xF0
#define EVENT_NONMIDI			0xFF

#define	IS_MIDI_HEADER(hdr)		((hdr[0] == MIDI_HDR_1) &&\
					 (hdr[1] == MIDI_HDR_2) &&\
					 (hdr[2] == MIDI_HDR_3) &&\
					 (hdr[3] == MIDI_HDR_4))

#define IS_MIDI_TRK(hdr)		((hdr[0] == MIDI_TRK_1) &&\
					 (hdr[1] == MIDI_TRK_2) &&\
					 (hdr[2] == MIDI_TRK_3) &&\
					 (hdr[3] == MIDI_TRK_4))

#define IS_SYSEX(evt)			((evt & 0xFF) == EVENT_SYSEX)
#define IS_NONMIDI(evt)			((evt & 0xFF) == EVENT_NONMIDI)
#define IS_RUNNING(evt)			((evt & 0xFF) < 0x80)

#define ME_CODE(evt)			(evt & 0xF0)
#define ME_CHANNEL(evt)			(evt & 0x0F)

#ifndef MAKEWORD
#define MAKEWORD(lsb, msb)		((lsb & 0xFF) + ((msb & 0xFF) << 8))
#endif

// MIDI EVENTS
#define ME_NOTEOFF			0x80
#define ME_NOTEON			0x90
#define ME_POLYPRESSURE			0xA0
#define ME_CTRLCHANGE			0xB0
#define ME_PRGMCHANGE			0xC0
#define ME_KEYPRESSURE			0xD0
#define ME_PITCHBEND			0xE0

// NON MIDI EVENTS
#define NME_SEQNUMBER			0x00
#define NME_TEXT			0x01
#define NME_COPYRIGHT			0x02
#define NME_TRKNAME			0x03
#define NME_INSTRNAME			0x04
#define NME_LYRICS			0x05
#define NME_MARKER			0x06
#define NME_CUEPOINT			0x07
#define NME_PRGMNAME			0x08
#define NME_DEVNAME			0x09
#define NME_ENDOFTRK			0x2F
#define NME_TEMPO			0x51
#define NME_SMTPEOFFSET			0x54
#define NME_TIMESIG			0x58
#define NME_KEYSIG			0x59
#define NME_PROPRIETARY			0x7F

#define IS_ME_NOTEON(id)		(ME_CODE(id) == ME_NOTEON)
#define IS_ME_NOTEOFF(id)		(ME_CODE(id) == ME_NOTEOFF)

// Obselete events
#define NME_MIDICHANNEL			0x20
#define NME_MIDIPORT			0x21

// Type d events
#define EVENT_TYPE			0
#define MIDI_EVENT_TYPE			1
#define SYSEX_EVENT_TYPE		2
#define NONMIDI_EVENT_TYPE		3

// Basic chunk
typedef struct	s_chunk
{
  unsigned char	ID[4];
  unsigned long Size;
}		t_chunk;

// Base class for Midi event handling
class Event
{
 public:
  Event(unsigned long pos, unsigned char ID);
    virtual ~Event();
    unsigned char GetID() { return ID; }
    unsigned long GetPos() { return Pos; }
    unsigned char GetType() { return Type; }

  protected:
    unsigned char ID;
    unsigned long Pos;
    unsigned char Type;
};

// Describes a midi event
class MidiFileEvent : public Event
{
  public:
  MidiFileEvent(unsigned long pos, unsigned char ID, unsigned char channel,
		  unsigned char p1, unsigned char p2);
    unsigned char GetChannel() { return (Channel); }
    unsigned char GetParam(int num) { return (num == 0) ? p1 : p2; }

  private:
    unsigned char Channel;
    unsigned char p1;
    unsigned char p2;
};

// Describes a sysex event
class SysExEvent : public Event
{
  public:
    SysExEvent(unsigned long pos, unsigned char ID, unsigned long len,
               unsigned char *data);
    ~SysExEvent();

  private:
    unsigned char *data;
    unsigned long len;
};

// Describes a Non Midi Event (ex: TrackName)
class NonMidiEvent : public Event
{
  public:
    NonMidiEvent(unsigned long pos, unsigned char ID, unsigned long len,
                 unsigned char *data);
    ~NonMidiEvent();
    wxString GetBufferAsString() { return wxString(wxString((const char *)data, *wxConvCurrent), 0, len); }
    unsigned char GetBufferAt(unsigned long pos) { return (pos < len) ? data[pos] : 0; }

  private:
    unsigned char *data;
    unsigned long len;
};

// All the events together is a track
class MidiTrack
{
 private:
  wxString	_filename;
  unsigned int	_noTrack;

 public:
  MidiTrack(unsigned long len, unsigned char *buffer, unsigned short PPQN,
	    wxString filename, unsigned int noTrack);
  ~MidiTrack();
  unsigned long GetMaxPos() { return (MaxPos); }
  vector<MidiFileEvent *> GetMidiEvents();
  unsigned short GetPPQN() { return ppqn; }

  inline wxString&	GetFileName() { return (_filename); };
  inline unsigned int	GetNoTrack() { return (_noTrack); };

  protected:
    unsigned long GetVLQ(unsigned char *buf, unsigned long &ofs);
    vector<Event *> Events;
    unsigned long MaxPos;
    unsigned short ppqn;
};

// The midifile containing 1 or several tracks
class MidiFile
{
 public:
  MidiFile(wxString filename);
  ~MidiFile();

  long GetNumberOfTracks() { return NbTracks; }
  long GetDivision()   { return Division; }
  MidiTrack *GetTrack(int num) { if ((num >= 0) && (num < NbTracks))
                                   return Tracks[num]; else return NULL; }

 protected:
   wxString filename;
   unsigned short Division;
   unsigned short Type;
   unsigned short NbTracks;
   vector<MidiTrack *> Tracks;
};

#endif
