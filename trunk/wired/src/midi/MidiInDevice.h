// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDIINDEVICE_H__
#define __MIDIINDEVICE_H__

#include "MidiDevice.h"

using namespace			std;

typedef int			MidiType;

class				MidiInDevice : public MidiDevice
{
 public:
  MidiInDevice();
  virtual ~MidiInDevice() {}
  int				OpenDevice(int ID);
  MidiDeviceList		*ListDevices(void);
  
  bool				Poll();
  bool				Read(MidiType *buf);
  
 protected:
  PmEvent			buffer;
};

#endif/*__MIDIINDEVICE_H__*/
