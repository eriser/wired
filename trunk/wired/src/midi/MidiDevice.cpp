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
    if (s == (*i)->Name)
      return ((*i)->Id);
  return (0);
}
