#include "cImportMidiAction.h"
#include "WaveFile.h"
#include "Track.h"
#include "SequencerGui.h"
#include "akai.h"
#include "../engine/AudioCenter.h"
#include "../midi/MidiFile.h"
#include "../gui/Rack.h"

/********************   class cImportWaveAction   ********************/

cImportWaveAction::cImportWaveAction (const string& path, bool kind, bool shouldAdd)
{
	_TrackKindFlag = kind;
	_WavePath = path;
	_trackIndex = INVALID_VALUE;
	_ShouldAdd = shouldAdd;
}

void cImportWaveAction::Do ()
{
	if (_ShouldAdd == true)
		AddWaveTrack();	
	else
		RemoveWaveTrack(false);
}

void cImportWaveAction::AddWaveTrack()
{
	WaveFile *w = WaveCenter.AddWaveFile(_WavePath);
	
	if (w) 
    {
    	Track *t = SeqPanel->AddTrack(_TrackKindFlag);
		t->AddPattern(w);
		_trackIndex = t->Index;
		NotifyActionManager();
    }
}

void cImportWaveAction::RemoveWaveTrack(bool selectFromIndex)
{
	if (selectFromIndex == true)
		SeqPanel->SelectTrack(_trackIndex);
	SeqPanel->DeleteSelectedTrack();
}

void cImportWaveAction::Redo ()
{ 
	Do();
}

void cImportWaveAction::Undo ()
{
	if (_ShouldAdd == true)
		RemoveWaveTrack(true);	
	else
		AddWaveTrack();
}

cImportWaveAction			cImportWaveAction::operator=(const cImportWaveAction& right)
{
	if (this != &right)
	{
		_TrackKindFlag = right._TrackKindFlag;
		_WavePath = right._WavePath;
		_trackIndex = right._trackIndex;
		_ShouldAdd = right._ShouldAdd;
	}
	return *this;
}


/********************   class cImportMidiAction   ********************/

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

cImportMidiAction			cImportMidiAction::operator=(const cImportMidiAction& right)
{
	if (this != &right)
	{
		mTrackKindFlag = right.mTrackKindFlag;
		mMidiPath = right.mMidiPath;
	}
	return *this;
}

/********************   class cImportAkaiAction   ********************/

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
  t_akaiSample *sample = akaiGetSampleByName((char *)mDevice.c_str(), mPart, (char *)mPath.c_str(), (char *)mName.c_str());
  if (sample != NULL)
  {
    try
      {
	WaveFile *w = new WaveFile(sample->buffer, sample->size, 2, sample->rate);
	Track *t = SeqPanel->AddTrack(true);
	t->AddPattern(w);
      }
    catch (...)
      {	
	; // FIXME we want to do something here ..
      }
    delete sample;
  }
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

cImportAkaiAction			cImportAkaiAction::operator=(const cImportAkaiAction& right)
{
	if (this != &right)
	{
		mTrackKindFlag = right.mTrackKindFlag;
		mDevice = right.mDevice;
		mPart = right.mPart;
		mPath = right.mPath;
		mName = right.mName;
	}
	return *this;
}

/********************   class cChangeParamsEffectAction   ********************/

cChangeParamsEffectAction::cChangeParamsEffectAction (Plugin* plugin, bool shouldSave)
{
  mPlugin = plugin;
  mShouldSave = shouldSave;
}

void cChangeParamsEffectAction::Do ()
{ 
	if (mShouldSave)
		SaveDatas();
	else
		LoadDatas();
}
   
void cChangeParamsEffectAction::Redo ()
{ 
	Do();
}

void cChangeParamsEffectAction::Undo ()
{
	if (!mShouldSave)
		SaveDatas();
	else
		LoadDatas();
}

// TODO : Be sure that only the concerned plugin is saved
void cChangeParamsEffectAction::SaveDatas()
{
	if (mPlugin)
		{
			std::cout << "SavePluginsDatas" << endl;
//			mPlugin->Save(&mDatas);
		}
    NotifyActionManager();
}

// TODO : Be sure that only the concerned plugin is loaded
void cChangeParamsEffectAction::LoadDatas()
{
	if (mPlugin)
		{
			std::cout << "LoadPluginsDatas" << endl;
//			mPlugin->Load(&mDatas);
		}
    NotifyActionManager();
}

void cChangeParamsEffectAction::Dump()
{
	std::cout << "    Dumping cChangeParamsEffectAction : "	<< this << std::endl;
	std::cout << "      Plugin* mPlugin : "	<< mPlugin << std::endl;
	std::cout << "      bool mShouldSave : " << mShouldSave << std::endl;
	cAction::Dump();
	std::cout << "      End Dumping cChangeParamsEffectAction" << std::endl;
}

cChangeParamsEffectAction			cChangeParamsEffectAction::operator=(const cChangeParamsEffectAction& right)
{
	if (this != &right)
	{
		mPlugin = right.mPlugin;
		mShouldSave = right.mShouldSave;
	}
	return *this;
}

/********************   class cCreateEffectAction   ********************/

cCreateEffectAction::cCreateEffectAction (PlugStartInfo* startInfo, PluginLoader* plugLoader, 
											bool shouldAdd)
{
  mPluginLoader = plugLoader;
  mStartInfo = startInfo;
  mShouldAdd = shouldAdd;
  mRackIndex = INVALID_VALUE;
}

void cCreateEffectAction::Do ()
{ 
	if (mShouldAdd == true)
		AddRackEffect();	
	else
		RemoveRackEffect();
}
   
void cCreateEffectAction::Redo ()
{ 
	Do();
}

void cCreateEffectAction::Undo ()
{
	if (mShouldAdd == true)
		RemoveRackEffect();
	else
		AddRackEffect();
}

void cCreateEffectAction::AddRackEffect ()
{ 
	if (mPluginLoader)
    {
    	if (mRackIndex < 0)
			mRackIndex = RackPanel->RackTracks.size();
    	//RackPanel->AddToSelectedTrack(*mStartInfo, mPluginLoader);
    	RackPanel->AddRackAndChannel(*mStartInfo, mPluginLoader);
	    NotifyActionManager();
    }
}

std::string		cCreateEffectAction::getHistoryLabel()		
{
	std::string			result;
	
	result = HISTORY_LABEL_CREATE_EFFECT_ACTION;
	result += " ";
	result += mPluginLoader->InitInfo.Name; 
	return result;
}

void cCreateEffectAction::RemoveRackEffect ()
{ 
	if (mRackIndex < 0)
		RackPanel->RemoveSelectedRackAndChannel();
	else
		RackPanel->RemoveTrack(mRackIndex);				// TODO: Should be identified by StartInfo and PluginLoader
	NotifyActionManager();
}

void cCreateEffectAction::Dump()
{
	std::cout << "    Dumping cCreateEffectAction : "	<< this << std::endl;
	std::cout << "      PluginLoader *mPluginLoader : " << mPluginLoader << std::endl;
	std::cout << "      PlugStartInfo *mStartInfo : "	<< mStartInfo << std::endl;
	std::cout << "      bool mShouldAdd : "	<< mShouldAdd << std::endl;
	cAction::Dump();
	std::cout << "      End Dumping cCreateEffectAction" << std::endl;
}

cCreateEffectAction			cCreateEffectAction::operator=(const cCreateEffectAction& right)
{
	if (this != &right)
	{
		mPluginLoader = right.mPluginLoader;
		mStartInfo = right.mStartInfo;
		mShouldAdd = right.mShouldAdd;
		mRackIndex = right.mRackIndex;
	}
	return *this;
}

/********************   class cCreateRackAction   ********************/

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

cCreateRackAction			cCreateRackAction::operator=(const cCreateRackAction& right)
{
	if (this != &right)
	{
		mPluginLoader = right.mPluginLoader;
		mStartInfo = right.mStartInfo;
	}
	return *this;
}
