// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "Note.h"
#include "midi.h"
#include "EditMidi.h"
#include "Sequencer.h"
#include "MidiFile.h"

Note::Note(MidiPattern *p, unsigned int num)
{
  this->num = num;
  e = p->Events[num];
  this->p = p;
}

void				Note::SetDuration(double duration)
{
  wxMutexLocker			m(SeqMutex);

  p->SetToWrite();
  e->EndPosition = duration + e->Position;
}

double				Note::GetDuration()
{
  return e->EndPosition - e->Position;
}

void				Note::SetNote(int note)
{
  wxMutexLocker			m(SeqMutex);

  if (note > 127)
	note = 127;
  if (note < 0)
	note = 0;
  p->SetToWrite();
  e->Msg[1] = note;
}

int				Note::GetNote()
{
  return(e->Msg[1]);
}

void				Note::SetPos(double pos)
{
  wxMutexLocker			m(SeqMutex);

  p->SetToWrite();
  e->EndPosition = e->EndPosition - e->Position + pos;
  e->Position = pos;
}

double				Note::GetPos()
{
  return e->Position;
}

int				Note::GetChannel()
{
  return (ME_CHANNEL(e->Msg[0]));
}

void				Note::SetChannel(int channel)
{
  wxMutexLocker			m(SeqMutex);

  if (channel > 15)
	channel = 15;
  if (channel < 0)
	channel = 0;
  p->SetToWrite();
  e->Msg[0] = ME_CODE(e->Msg[0]) | channel;
}

void				Note::SetVelocity(int velocity)
{
  wxMutexLocker			m(SeqMutex);

  if (velocity > 127)
	velocity = 127;
  if (velocity < 0)
	velocity = 0;
  p->SetToWrite();
  e->Msg[2] = velocity;
}

int				Note::GetVelocity()
{
  return(e->Msg[2]);
}

void				Note::Erase()
{
  wxMutexLocker			m(SeqMutex);

  p->SetToWrite();
  for (vector<MidiEvent *>::iterator i = p->Events.begin(); i != p->Events.end(); i++)
    {
      if (*i == e)
	{
	  p->Events.erase(i);
	  break;
	}//p->Events.begin() + num);
    }
}
