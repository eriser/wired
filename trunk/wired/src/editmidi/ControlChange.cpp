// Copyright (C) 2004-2008 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "ControlChange.h"
#include "midi.h"
#include "EditMidi.h"
#include "Sequencer.h"
#include "MidiFile.h"

ControlChange::ControlChange(MidiPattern *p, unsigned int num)
{
  this->num = num;
  e = p->Events[num];
  this->p = p;
}

void				ControlChange::SetChannel(int channel)
{
  SeqMutex.Lock();

  if (channel > 15)
    channel = 15;
  if (channel < 0)
    channel = 0;
  p->SetToWrite();
  e->Msg[0] = ME_CODE(e->Msg[0]) | channel;

  SeqMutex.Unlock();
}

void				ControlChange::SetController(int controller)
{
  SeqMutex.Lock();

  if (controller > 127)
    controller = 127;
  if (controller < 0)
    controller = 0;
  p->SetToWrite();
  e->Msg[1] = controller;

  SeqMutex.Unlock();
}

void				ControlChange::SetPos(double pos)
{
  SeqMutex.Lock();

  p->SetToWrite();
  e->EndPosition = pos;
  e->Position = pos;

  SeqMutex.Unlock();
}

void				ControlChange::SetValue(int value)
{
  SeqMutex.Lock();

  if (value > 127)
    value = 127;
  if (value < 0)
    value = 0;
  p->SetToWrite();
  e->Msg[2] = value;

  SeqMutex.Unlock();
}

int				ControlChange::GetChannel()
{
  return (ME_CHANNEL(e->Msg[0]));
}

int				ControlChange::GetController()
{
  return (e->Msg[1]);
}

double		ControlChange::GetPos()
{
  return (e->Position);
}

int				ControlChange::GetValue()
{
  return (e->Msg[2]);
}

void			ControlChange::Erase()
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

