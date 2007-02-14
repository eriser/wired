// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDI_H__
#define __MIDI_H__

class				MidiEvent
{
 public:
  MidiEvent(int id, double pos, int msg[3]) 
    {
      Id = id;
      Position = pos;
      EndPosition = 0.0;
      Msg[0] = msg[0];
      Msg[1] = msg[1];
      Msg[2] = msg[2];
    }  	   
  ~MidiEvent() {}

  int				Id;		// Id du controlleur MIDI
  // Les positions sont relatives a celles du MidiPattern
  double			Position;	// position dans la mesure de l'evenement
  double			EndPosition;	// position de fin
  int				Msg[3];		// data midi
};

#define MIDI_IN_BUFSIZE		(96)
#define	STATUS(x)		(x & 0xF0)
#define CHANNEL(x)		(x & 0x0F)
#define M_START			(0xFA)
#define M_CONT			(0xFB)
#define M_STOP			(0xFC)
#define M_NOTEON1		(0x80)
#define M_NOTEON2		(0x90)
#define M_CONTROL		(0xB0)

typedef int			MidiType;

#endif
