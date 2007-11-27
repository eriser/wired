// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "cImportMidiAction.h"
#include "WaveFile.h"
#include "Track.h"
#include "SequencerGui.h"
#include "akai.h"
#include "AudioCenter.h"
#include "MidiFile.h"
#include "Rack.h"
#include "AudioPattern.h"

#ifdef DEBUG_UNDO
#include <wx/filename.h>
#endif

#ifdef DEBUG_UNDO
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : "  << __LINE__ << " : " << __FUNCTION__  << endl; }
#else
#define LOG
#endif

/********************   class cImportWaveAction   ********************/

cImportWaveAction::cImportWaveAction (const wxString& path, trackType kind, bool shouldAdd)
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
  WaveFile *w_tmp = WaveCenter.AddWaveFile(_WavePath);
  WaveFile *w;
  long		nb_channel;

  if (w_tmp)
    {
      for (nb_channel = 0; nb_channel < w_tmp->GetNumberOfChannels(); nb_channel++)
	{
	  _trackCreated = SeqPanel->CreateTrack(_TrackKindFlag);
	  w = WaveCenter.AddWaveFile(_WavePath);
	  w->SetChannelToRead(nb_channel);
	  _trackCreated->CreateAudioPattern(w);
	  _trackIndex = _trackCreated->GetIndex();
	  NotifyActionManager();
	}
    }
  WaveCenter.RemoveWaveFile(w_tmp);
}

void cImportWaveAction::AddWaveToEditor()
{
  WaveFile *w_tmp = WaveCenter.AddWaveFile(_WavePath);
  WaveFile *w;
  long	   nb_channel;

  if (w_tmp)
    {
      _trackCreated = SeqPanel->CreateTrack(_TrackKindFlag);
      for (nb_channel = 0; nb_channel < w_tmp->GetNumberOfChannels(); nb_channel++)
	{
	  AudioPattern  *pattern = _trackCreated->CreateAudioPattern(w);
	  pattern->OnDirectEdit();

	  _trackIndex = _trackCreated->GetIndex();
	  NotifyActionManager();
	}
    }
  WaveCenter.RemoveWaveFile(w_tmp);
}

void cImportWaveAction::RemoveWaveTrack(bool selectFromIndex)
{
  if (selectFromIndex == true)
    SeqPanel->DeleteTrack(_trackCreated);
  else
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

cImportMidiAction::cImportMidiAction (wxString& path, trackType kind)
{
  mTrackKindFlag = kind;
  mMidiPath = path;
  trackCreated = NULL;
}

void cImportMidiAction::Do ()
{
  LOG;
  MidiFile m(mMidiPath);
  m.ReadMidiFile();
  for (int i = 0; i < m.GetNumberOfTracks(); i++)
  {
    if (m.GetTrack(i)->GetMaxPos() > 0)
    {
      trackCreated = SeqPanel->CreateTrack(mTrackKindFlag);
      trackCreated->CreateMidiPattern(m.GetTrack(i));
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
  SeqPanel->DeleteTrack(trackCreated);
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

cImportAkaiAction::cImportAkaiAction (wxString& path, trackType kind)
{
  mTrackKindFlag = kind;
  mDevice = path.substr(0, path.find(wxT(":"), 0));
  path = path.substr(path.find(wxT(":"), 0) + 1, path.size() - path.find(wxT(":"), 0));
  mPath = path.substr(10, path.size() - 10);
  int pos = mPath.find(wxT("/"), 0);
  mPart = mPath.substr(0, pos).c_str()[0] - 64;
  mPath = mPath.substr(pos, mPath.size() - pos);
  int opos = 0;
  while ((pos = mPath.find(wxT("/"), opos)) != wxString::npos)
	opos = pos + 1;

  mName = mPath.substr(opos, mPath.size() - opos);
  mPath = mPath.substr(1, opos - 2);
  cout << "device: " << mDevice.mb_str() << "; part: " << mPart
       << "; name: " << mName.mb_str() << "; path: " << mPath.mb_str() << endl;
  trackCreated = NULL;
}

void cImportAkaiAction::Do ()
{
  t_akaiSample *sample = akaiGetSampleByName((char*)((const char*)mDevice.mb_str(*wxConvCurrent)),
					     mPart, (char*)((const char*)mPath.mb_str(*wxConvCurrent)),
					     (char*)((const char*)mName.mb_str(*wxConvCurrent)));
  if (sample != NULL)
  {
    try
      {
	WaveFile *w = new WaveFile(sample->buffer, sample->size, 2, sample->rate);
	trackCreated = SeqPanel->CreateTrack(eAudioTrack);
	trackCreated->CreateAudioPattern(w);
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
  SeqPanel->DeleteTrack(trackCreated);
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

	if (RackPanel->AddNewRack(*mStartInfo, mPluginLoader))
	  NotifyActionManager();
    }
}

const wxString		cCreateEffectAction::getHistoryLabel()
{
	wxString			result;

	result = HISTORY_LABEL_CREATE_EFFECT_ACTION;
	result += wxT(" ");
	result += mPluginLoader->InitInfo.Name;
	return result;
}

void cCreateEffectAction::RemoveRackEffect ()
{
	if (mRackIndex < 0)
	  RackPanel->RemoveSelectedRackTrack();
	else
	  RackPanel->RemoveRackTrack(mRackIndex);
	// TODO: Should be identified by StartInfo and PluginLoader
	// TODO bis : must delete this undo engine...
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
	RackPanel->AddNewRack(*mStartInfo, mPluginLoader);
	NotifyActionManager();
  }
}

void cCreateRackAction::Redo ()
{
  Do();
}

void cCreateRackAction::Undo ()
{
  RackPanel->RemoveLastRackTrack();
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
