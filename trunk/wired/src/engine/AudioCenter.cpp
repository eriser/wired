// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "AudioCenter.h"
#include <wx/wx.h>

AudioCenter::AudioCenter()
{

}
AudioCenter::~AudioCenter()
{
  Clear();
}

WaveFile *AudioCenter::AddWaveFile(wxString filename)
{
  WaveFile *w;
  
  try
    {
      w = new WaveFile(filename, false, WaveFile::rwrite);
      WaveFiles.push_back(w);
    }
  catch (...)
    {
    	cout << "Could not open file  !" << endl;
//      wxMessageDialog msg(0x0, 
//			  "Could not open file", "Wired", 
//			  wxOK | wxICON_EXCLAMATION |wxCENTRE);
//      msg.ShowModal();

      w = 0;
    }
  return (w);
}

void		AudioCenter::RemoveWaveFile(WaveFile *File)
{
  list<WaveFile *>::iterator i;

  for (i = WaveFiles.begin(); i != WaveFiles.end(); i++)
  	if (*i == File)
  	{
	  	WaveFiles.erase(i);
	    delete *i;
  	}
}

void AudioCenter::Clear()
{
  list<WaveFile *>::iterator i;

  for (i = WaveFiles.begin(); i != WaveFiles.end(); i++)
  	if (*i)
	    delete *i;
  WaveFiles.clear();
}
