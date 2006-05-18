// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MidiDevice.h"

void MidiDevice::CloseDevice()
{
  int err;

  if (id != -1)
    {
#ifdef DEBUG
      cout << "[MIDIDEVICE] closing midi device : " << id << endl;
#endif
      if ((err = Pm_Close(stream)) != pmNoError)
      	cerr << "[MIDIDEVICE] PortMidi error while closing device: " << Pm_GetErrorText((PmError)err) << endl;
      id = -1;
    }
}

void MidiDevice::DeleteDevices()
{
  for (MidiDeviceList::iterator i = devices.begin(); i != devices.end(); i++)
	if (*i)
	    delete (*i);
  devices.clear();
}

int MidiDevice::GetDeviceIDFromName(string s)
{
  MidiDeviceList::iterator i;

  for (i = devices.begin(); i != devices.end(); i++)
    if (s == string((*i)->Name.mb_str(*wxConvCurrent)))
      return ((*i)->Id);
  return (0);
}
