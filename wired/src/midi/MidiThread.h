#ifndef __MIDITHREAD__H_
#define __MIDITHREAD__H_

#include <string>
#include "Sequencer.h"
#include "MidiInDevice.h"
#include "midi.h"

extern wxMutex MidiMutex;
extern wxMutex MidiDeviceMutex;

class MidiThread : public wxThread
{
 public: 
  MidiThread();
  ~MidiThread();

  virtual void *Entry();
  virtual void  OnExit();
  
  void	    OpenDevice(int id);
  void	    OpenDefaultDevices();
  void	    CloseDevice(int id);
  void	    RemoveDevice(int id);
  
  MidiDeviceList	 DeviceList;
  vector<MidiInDevice *> MidiInDev;

 private:
  MidiType  midi_msg[3];

  void AnalyzeMidi(int id);
  MidiDeviceList *ListDevices();
};

extern MidiThread *MidiEngine;

#endif
