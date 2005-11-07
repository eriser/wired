#include "MidiThread.h"
#include "Sequencer.h"
#include "Settings.h"
#include "MidiController.h"
#include "MidiInDevice.h"

wxMutex					MidiMutex;
wxMutex					MidiDeviceMutex;
MidiThread				*MidiEngine;

MidiThread::MidiThread()
{
  ListDevices();
}

MidiThread::~MidiThread()
{
  vector<MidiInDevice *>:: iterator	i;

  for (i = MidiInDev.begin(); i != MidiInDev.end(); i++)
  	if (*i)
	    delete *i;
}

void MidiThread::OnExit()
{

}

void					MidiThread::OpenDevice(int id)
{
  MidiInDevice *dev;
  vector<MidiInDevice *>:: iterator	i;

  MidiMutex.Lock();
  dev = 0x0;
  for (i = MidiInDev.begin(); i != MidiInDev.end(); i++)
    if ((*i)->id == id)
      {
	dev = *i;
	break;
      }
  if (!dev)
    {
      dev = new MidiInDevice;
      MidiInDev.push_back(dev);
    }
  if (!(dev->OpenDevice(id)))
    RemoveDevice(id);
  else
    cout << "[MIDIINDEVICE] Opening device : " << dev->id << endl;
  MidiMutex.Unlock();
}

void					MidiThread::OpenDefaultDevices()
{
  MidiDeviceList::iterator		i;
  vector<long>:: iterator		j;
  long					k;
  
  for (j = WiredSettings->MidiIn.begin(); j != WiredSettings->MidiIn.end(); j++)
    for (k = 0, i = DeviceList.begin(); i != DeviceList.end(); i++, k++)
	if (k == *j)
	  {
	    OpenDevice((*i)->Id);       
	    break;
	  }
}

void					MidiThread::CloseDevice(int id)
{
  vector<MidiInDevice *>:: iterator	i;

  MidiMutex.Lock();
  for (i = MidiInDev.begin(); i != MidiInDev.end(); i++)
    if ((*i)->id == id)
      {
	((*i)->CloseDevice());
	break;
      }
  MidiMutex.Unlock();
}

void					MidiThread::RemoveDevice(int id)
{
  vector<MidiInDevice *>:: iterator	i;

  for (i = MidiInDev.begin(); i != MidiInDev.end(); i++)
    if ((*i)->id == id)
      {
      	if (*i)
			delete *i;
	MidiInDev.erase(i);
	break;
      }
}

MidiDeviceList				*MidiThread::ListDevices()
{
  const PmDeviceInfo			*pdi;
  MidiDeviceInfo			*d;
  int					numdev = Pm_CountDevices();
  
  for (unsigned int i = 0; i < numdev; i++)
    {
      pdi = Pm_GetDeviceInfo(i);
      if (pdi->input)
      	{
	  d = new MidiDeviceInfo(pdi->name, i);
	  DeviceList.push_back(d);
	}
    }
  return (&DeviceList);
}

void					*MidiThread::Entry()
{
  vector<MidiInDevice *>:: iterator	i;

  cout << "[MIDITHREAD] Thread started !" << endl;    
  while (1)
    {
      MidiDeviceMutex.Lock();
      for (i = MidiInDev.begin(); i != MidiInDev.end(); i++)
	{
	  if ((*i)->Poll())
	    {
	      (*i)->Read(midi_msg);
	      AnalyzeMidi((*i)->id);
	    }
	}
      MidiDeviceMutex.Unlock();
      if (TestDestroy() == true)
      	break;
      wxMilliSleep(1);
    }
  return (0x0);
}

void					MidiThread::AnalyzeMidi(int id)
{
  /*
  if ((midi_msg[0] == M_START) || (midi_msg[0] == M_CONT))
    {
      cout << "[MIDITHREAD] Received PLAY or CONTINUE" << endl;
    }
  else if (midi_msg[0] == M_STOP)
    {
      cout << "[MIDITHREAD] Received STOP" << endl;
    }
  else if ((STATUS(midi_msg[0]) == M_NOTEON1) ||  (STATUS(midi_msg[0]) == M_NOTEON2))
    {
      cout << "[MIDITHREAD] Received NOTE ON" << endl;
    }
  else if (STATUS(midi_msg[0]) == M_CONTROL)
    {
      cout << "[MIDITHREAD] Received CONTROL" << endl;
    }
  */

  MidiMutex.Lock();

  if (Controller)
    Controller->ProcessMidi(midi_msg);
  else
    Seq->AddMidiEvent(id, midi_msg);

  MidiMutex.Unlock();
}
