// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MidiThread.h"
#include "Sequencer.h"
#include "Settings.h"
#include "MidiController.h"
#include "MidiInDevice.h"
#include "../gui/Threads.h"

wxMutex					MidiMutex(wxMUTEX_RECURSIVE);
wxMutex					MidiDeviceMutex(wxMUTEX_RECURSIVE);
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
  vector<int>:: iterator		j;
  long					k;
  
  for (j = WiredSettings->MidiIn.begin(); j != WiredSettings->MidiIn.end(); j++)
    for (k = 0, i = DeviceList.begin(); i != DeviceList.end(); i++, k++)
      if (k == (long)*j)
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
  
  DeviceList.clear();
  
  std::cerr << "[MIDITHREAD] ListDevices : numdev = " << numdev << std::endl;

  for (unsigned int i = 0; i < numdev; i++)
    {
      pdi = Pm_GetDeviceInfo(i);
      if (pdi->input)
      	{
	  d = new MidiDeviceInfo(wxString(pdi->name, *wxConvCurrent), i);
	  DeviceList.push_back(d);
	}
    }
  return (&DeviceList);
}

void					*MidiThread::Entry()
{
  vector<MidiInDevice *>:: iterator	i;

  cout << "[MIDITHREAD] Thread started !" << endl;    
  while (!TestDestroy())
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
      Sleep(1);
    }
  cout << "[MIDITHREAD] Thread finished !" << endl;    
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

void			MidiThread::RefreshDevices()
{
  ListDevices();
}

int			MidiThread::GetDeviceIdByName(wxString name)
{
  MidiDeviceList::iterator	midiDeviceIt;

  for (midiDeviceIt = DeviceList.begin();
       midiDeviceIt != DeviceList.end();
       midiDeviceIt++)
    if ((*midiDeviceIt)->Name == name)
      return (*midiDeviceIt)->Id;
    
  return -1;
}

vector<int>		MidiThread::GetDevicesIdByName(vector<wxString> names)
{
  vector<int>			ids;
  int				id;
  vector<wxString>::iterator	namesIt;

  for (namesIt = names.begin(); 
       namesIt != names.end();
       namesIt++)
    {
      id = GetDeviceIdByName(*namesIt);
      if (id != -1)
	ids.push_back(id);
    }

  return ids;
}

void                    MidiThread::OnExit()
{
  //It freezes the application on Windows and don't
  //makes difference on Linux
  //wxMutexLocker		locker(wxGetApp().m_mutex);
  wxArrayThread& threads = wxGetApp().m_threads;

  threads.Remove(this);
  if (threads.IsEmpty())
    wxGetApp().m_condAllDone->Signal();
  cout << "[MIDITHREAD] Thread terminated !" << endl;    
}
