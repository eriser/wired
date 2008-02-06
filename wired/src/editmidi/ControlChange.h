// Copyright (C) 2004-2008 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CONTROLCHANGE_H__
#define __CONTROLCHANGE_H__

#include <vector>
#include "midi.h"
#include "MidiPattern.h"

using std::vector;

class ControlChange
{
  public:
    ControlChange(MidiPattern *p, unsigned int num);

    void   SetChannel   (int channel);
    void   SetController(int note);
    void   SetPos       (double pos);
    void   SetValue     (int value);

    int    GetChannel   ();
    int    GetController();
    double GetPos       ();
    int    GetValue     ();

    void   Erase        ();

  private:
    MidiEvent	*e;
    MidiPattern	*p;
    unsigned int	num;
};

#endif
