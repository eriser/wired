#include "cImportMidiAction.h"
#include "WaveFile.h"
#include "Track.h"
#include "SequencerGui.h"
#include "akai.h"
#include "../engine/AudioCenter.h"
#include "../midi/MidiFile.h"
#include "../gui/Rack.h"

cImportWaveAction::cImportWaveAction (string path, bool kind)
{
  mTrackKindFlag = kind;
  mWavePath = path;
}

void cImportWaveAction::Do ()
{ 
  WaveFile *w = WaveCenter.AddWaveFile(mWavePath);

  if (w) 
    {
      Track *t = SeqPanel->AddTrack(mTrackKindFlag);
      t->AddPattern(w);
    }

  NotifyActionManager();
}
   
void cImportWaveAction::Redo ()
{ 
  Do();
}

void cImportWaveAction::Undo ()
{ 
  SeqPanel->RemoveTrack(); 
}

/////////////////////////////////////////////////////////////////

cImportMidiAction::cImportMidiAction (string path, bool kind)
{
  mTrackKindFlag = kind;
  mMidiPath = path;
}

void cImportMidiAction::Do ()
{
  MidiFile *m = new MidiFile(mMidiPath);

  if (m)
  {
	for (int i = 0; i < m->GetNumberOfTracks(); i++)
	{
	  if (m->GetTrack(i)->GetMaxPos() > 0)
	  {
		Track *t = SeqPanel->AddTrack(mTrackKindFlag);
		t->AddPattern(m->GetTrack(i));
	  }
	}
  }
  NotifyActionManager();
}
   
void cImportMidiAction::Redo ()
{ 
  Do();
}

void cImportMidiAction::Undo ()
{ 
  SeqPanel->RemoveTrack(); 
}

/////////////////////////////////////////////////////////////////

cImportAkaiAction::cImportAkaiAction (string path, bool kind)
{
  mTrackKindFlag = kind;
  mDevice = path.substr(0, path.find(":", 0));
  path = path.substr(path.find(":", 0) + 1, path.size() - path.find(":", 0));
  mPath = path.substr(10, path.size() - 10);
  int pos = mPath.find("/", 0);
  mPart = mPath.substr(0, pos).c_str()[0] - 64;
  mPath = mPath.substr(pos, mPath.size() - pos);
  int opos = 0;
  while ((pos = mPath.find("/", opos)) != string::npos)
	opos = pos + 1;

  mName = mPath.substr(opos, mPath.size() - opos);
  mPath = mPath.substr(1, opos - 2);
  cout << "device: " << mDevice << "; part: " << mPart << "; name: " << mName << "; path: " << mPath << endl;
}

void cImportAkaiAction::Do ()
{
  akaiImage *img = new akaiImage(mDevice);
  akaiSample *smp = img->getSample(mPart + "/" + mPath + "/" + mName);
  //t_akaiSample *sample = akaiGetSampleByName((char *)mDevice.c_str(), mPart, (char *)mPath.c_str(), (char *)mName.c_str());
  if (smp != NULL)
  {
    try
      {
	       WaveFile *w = new WaveFile(smp->getSample(), smp->getSize(), 2, smp->getRate());
	       Track *t = SeqPanel->AddTrack(true);
	       t->AddPattern(w);
      }
    catch (...)
      {	
	; // FIXME we want to do something here ..
      }
    delete smp;
  }
  delete img;
  NotifyActionManager();
}
   
void cImportAkaiAction::Redo ()
{ 
  Do();
}

void cImportAkaiAction::Undo ()
{ 
  SeqPanel->RemoveTrack(); 
}


/////////////////////////////////////////////////////////////////


cCreateEffectAction::cCreateEffectAction (PlugStartInfo* startInfo, PluginLoader* plugLoader)
{
  mPluginLoader = plugLoader;
  mStartInfo = startInfo;
}

void cCreateEffectAction::Do ()
{ 
  if (mPluginLoader)
    {
      RackPanel->AddToSelectedTrack(*mStartInfo, mPluginLoader);
      NotifyActionManager();
    }
}
   
void cCreateEffectAction::Redo ()
{ 
  Do();
}

void cCreateEffectAction::Undo ()
{ 
  RackPanel->RemoveFromSelectedTrack();
}


/////////////////////////////////////////////////////////////////

cCreateRackAction::cCreateRackAction (PlugStartInfo* startInfo, PluginLoader* plugLoader)
{
  mPluginLoader = plugLoader;
  mStartInfo = startInfo;
}

void cCreateRackAction::Do ()
{ 
  if (mPluginLoader)
  {
	RackPanel->AddTrack(*mStartInfo, mPluginLoader);
	NotifyActionManager();
  }
}
   
void cCreateRackAction::Redo ()
{ 
  Do();
}

void cCreateRackAction::Undo ()
{ 
  RackPanel->RemoveTrack();
}
