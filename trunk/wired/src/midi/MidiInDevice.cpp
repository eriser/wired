#include <stdio.h>
#include <porttime.h>
#include "MidiInDevice.h"
#include "midi.h"

MidiInDevice::MidiInDevice() : MidiDevice()
{

}
  
int				MidiInDevice::OpenDevice(int ID)
{
  int				err;
  
  MidiDevice::CloseDevice();
  Pt_Start(1, 0, 0); 
  if ((err = Pm_OpenInput(&stream, ID, NULL, MIDI_IN_BUFSIZE, NULL, NULL, NULL)) 
      != pmNoError)        
    {
      cout << "[MIDIINDEVICE] Could not open device: PortMidi error : " 
	   << Pm_GetErrorText((PmError)err) << endl;
      return (0);
    }
  id = ID;
  return (1);
}

MidiDeviceList			*MidiInDevice::ListDevices(void)
{
  const PmDeviceInfo		*pdi;
  MidiDeviceInfo		*d;
  int				numdev = Pm_CountDevices();
  
  MidiDevice::DeleteDevices();
  for (unsigned int i = 0; i < numdev; i++)
    {    
      pdi = Pm_GetDeviceInfo(i);	  
      if (pdi->input)
      	{
	  d = new MidiDeviceInfo(pdi->name, i);
	  devices.push_back(d);
	}
    }
  return (&devices);
}

bool				MidiInDevice::Poll()
{
  if (Pm_Poll(stream) == TRUE)
    return (true);
  else
    return (false);
}

bool				MidiInDevice::Read(MidiType *buf)
{
  int				err;
  
  if ((err = Pm_Read(stream, &buffer, 1)) > 0 && buf)
    {
      //printf("Got midi in : time %d, %2x %2x %2x\n",
//	     buffer.timestamp,
//	     Pm_MessageStatus(buffer.message),
//	     Pm_MessageData1(buffer.message),
//	     Pm_MessageData2(buffer.message));
      
      buf[0] = Pm_MessageStatus(buffer.message);
      buf[1] = Pm_MessageData1(buffer.message);
      buf[2] = Pm_MessageData2(buffer.message);
      return (true);
    } 
  else 
    {
      cout << "[MIDIINDEVICE] Error in Pm_Read: %s\n" << Pm_GetErrorText((PmError)err) << endl;
      return (false);
    }
}
