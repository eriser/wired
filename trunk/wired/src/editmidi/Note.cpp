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
  SeqMutex.Lock();

  p->SetToWrite();
  e->EndPosition = duration + e->Position;
  SeqMutex.Unlock();
}

double				Note::GetDuration()
{
  return e->EndPosition - e->Position;
}

void				Note::SetNote(int note)
{
  SeqMutex.Lock();

  p->SetToWrite();
  e->Msg[1] = note;
  SeqMutex.Unlock();
}

int				Note::GetNote()
{
  return(e->Msg[1]);
}

void				Note::SetPos(double pos)
{
  SeqMutex.Lock();

  p->SetToWrite();
  e->EndPosition = e->EndPosition - e->Position + pos;
  e->Position = pos;
  SeqMutex.Unlock();
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
  SeqMutex.Lock();

  p->SetToWrite();
  e->Msg[0] = ME_CODE(e->Msg[0]) | channel;
  SeqMutex.Unlock();
}

void				Note::SetVelocity(int velocity)
{
  SeqMutex.Lock();

  p->SetToWrite();
  e->Msg[2] = velocity;
  SeqMutex.Unlock();
}

int				Note::GetVelocity()
{
  return(e->Msg[2]);
}

void				Note::Erase()
{
  SeqMutex.Lock();

  p->SetToWrite();
  for (vector<MidiEvent *>::iterator i = p->Events.begin(); i != p->Events.end(); i++)
    {
      if (*i == e)
	{
	  p->Events.erase(i);
	  break;
	}//p->Events.begin() + num);
    }
  SeqMutex.Unlock();
}
