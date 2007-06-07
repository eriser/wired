// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDITHREAD__H_
#define __MIDITHREAD__H_

#include <vector>
#include <wx/thread.h>
#include <wx/string.h>

class					MidiDeviceInfo;
class					MidiInDevice;
typedef int				MidiType;
typedef std::vector<MidiDeviceInfo *>	MidiDeviceList;

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
  //a simple call to ListDevices... is there a better way ? implementing accessors maybe...
  void					RefreshDevices();
  
  std::vector<int>				GetDevicesIdByName(std::vector<wxString>); 
  int					GetDeviceIdByName(wxString); 

  MidiDeviceList			DeviceList;
  std::vector<MidiInDevice *>		MidiInDev;

 private:
  void					AnalyzeMidi(int id);
  MidiDeviceList			*ListDevices();

  MidiType				midi_msg[3];
};

extern MidiThread			*MidiEngine;
extern wxMutex				MidiMutex;
extern wxMutex				MidiDeviceMutex;

#endif/*__MIDITHREAD_H__*/
