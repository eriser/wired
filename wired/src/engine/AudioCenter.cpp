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

WaveFile *AudioCenter::AddWaveFile(string filename)
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

void AudioCenter::Clear()
{
  list<WaveFile *>::iterator i;

  for (i = WaveFiles.begin(); i != WaveFiles.end(); i++)
    delete *i;
  WaveFiles.clear();
}
