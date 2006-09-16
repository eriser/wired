// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDIDEVICE_H__
#define __MIDIDEVICE_H__

#include <vector>
#include <iostream>
#include <portmidi.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

using namespace				std;

class MidiDeviceInfo
{
 public:
  MidiDeviceInfo(wxString n, int i) : Name(n), Id(i) {}
  ~MidiDeviceInfo() {}

  wxString Name;
  int	 Id;
};

typedef vector<MidiDeviceInfo *> MidiDeviceList;

class MidiDevice
{
 public:
  MidiDevice() { id = -1; }
  virtual ~MidiDevice() { CloseDevice(); DeleteDevices(); };
  virtual int				OpenDevice(int _id) = 0;
  virtual void				CloseDevice();
  virtual MidiDeviceList		*ListDevices(void) = 0;
  int					GetDeviceIDFromName(wxString s);

  int					id;
 
 protected:

  void					DeleteDevices();
  
  MidiDeviceList devices;
  PmStream				*stream;
};

#endif/*__MIDIDEVICE_H__*/
