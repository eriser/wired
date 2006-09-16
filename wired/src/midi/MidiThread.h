// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDITHREAD__H_
#define __MIDITHREAD__H_

#include <vector>
#include <wx/thread.h>

using namespace				std;

class					MidiDeviceInfo;
class					MidiInDevice;
typedef int				MidiType;
typedef vector<MidiDeviceInfo *>	MidiDeviceList;

class					MidiThread : public wxThread
{
 public: 
  MidiThread();
  ~MidiThread();

  virtual void				*Entry();
  void          				OnExit();
  
  void					OpenDevice(int id);
  void					OpenDefaultDevices();
  void					CloseDevice(int id);
  void					RemoveDevice(int id);
  
  MidiDeviceList			DeviceList;
  vector<MidiInDevice *>		MidiInDev;

 private:
  void					AnalyzeMidi(int id);
  MidiDeviceList			*ListDevices();

  MidiType				midi_msg[3];
};

extern MidiThread			*MidiEngine;
extern wxMutex				MidiMutex;
extern wxMutex				MidiDeviceMutex;

#endif/*__MIDITHREAD_H__*/
