// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "AudioCenter.h"
#include <wx/wx.h>
#include "SaveCenter.h"
#include "MLTree.h"
//#include "<wx/filefn.h>"

extern SaveCenter *saveCenter;

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
        wxFileName onlyfile(filename);
        wxString to = saveCenter->getAudioDir() + onlyfile.GetFullName();
        for (list<WaveFile *>::iterator i = WaveFiles.begin(); i != WaveFiles.end(); i++)
            if ((*i)->Filename == to)
            {
                w = new WaveFile((*i)->Filename, false, WaveFile::rwrite);
                WaveFiles.push_back(w);
                return (w);
            }
        if (!wxFileExists(to))
            wxCopyFile(filename, to, false);
        MediaLibraryPanel->MLTreeView->AddFileInProject(to, true);
        w = new WaveFile(to, false, WaveFile::rwrite);
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

void		AudioCenter::RemoveWaveFile(WaveFile *file)
{
  list<WaveFile *>::iterator	i;
  int				cpt;
  list<WaveFile *>::iterator	save;

  for (cpt = 0, save = WaveFiles.end(), i = WaveFiles.begin(); i != WaveFiles.end(); i++)
      if ((*i)->Filename == file->Filename)
	{
	  cpt++;
	  if ((*i) == file)
	    save = i;
	}
  if (save != WaveFiles.end() && (*save)->GetAssociatedPattern() == 1)
    WaveFiles.erase(save);
  if (cpt == 1 && (*save)->GetAssociatedPattern() == 1)
    {
      MediaLibraryPanel->MLTreeView->DelFileInProject(file->Filename, true);
      wxRemoveFile(file->Filename);
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
