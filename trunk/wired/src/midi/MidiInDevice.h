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