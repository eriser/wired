// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WiredSessionXml.h"

extern std::vector<PluginLoader *>	LoadedPluginsList;
extern WiredExternalPluginMgr		*LoadedExternalPlugins;
extern PlugStartInfo				StartInfo;


WiredSessionXml::~WiredSessionXml()
{
	;
}

WiredSessionXml::WiredSessionXml(const WiredSessionXml& copy)
{
	*this = copy;	
}

WiredSessionXml	WiredSessionXml::operator=(const WiredSessionXml& right)
{
	if (this != &right)
	{
		this->WiredXml::operator=(right);
		_WorkingDir = right._WorkingDir;
	}
	return *this;
}

WiredSessionXml	WiredSessionXml::Clone()
{
	return *this;
}

bool			WiredSessionXml::Load(const wxString& FileName)
{
	if (FileName != wxT(""))
		_DocumentFileName = FileName;
	if (OpenDocument(_DocumentFileName) == true)
	{
		if (OpenDtd(wxString(DTD_FILENAME)) == true)
		{
			if (ValidDocument() == true)
				return ParseWiredSession();
			else
				std::cout << "[WIREDSESSION] File {" << _DocumentFileName.mb_str() 
					<< "} does not comply with DTD {" << DTD_FILENAME << "}" 
					<< std::endl;
		}
		else
		{
			std::cout << "[WIREDSESSION] Unable to load DTD file {" 
				<< DTD_FILENAME << "}" << std::endl << "error == " << strerror(errno) << std::endl;
		}
	}
	else
	{
		std::cout << "[WIREDSESSION] Unable to load file {" 
			<< _DocumentFileName.mb_str() << "}" << std::endl;
	}
	return false;
}

bool			WiredSessionXml::Save()
{
	if (CreateFile() == true)
	{
		if (_DocumentWriter != NULL)
		{
			wxFileName 	session_dir(_DocumentWriterName);
			wxFileName 	f(_WorkingDir);
			TrackIter	Ts;

			f.MakeRelativeTo(session_dir.GetPath());
			StartElement(STR_ROOT_NODE_NAME);
			StartElement(STR_WORKING_DIR);
			WriteString(f.GetFullPath());
			EndElement();
			SaveSeq();
			for (Ts = Seq->Tracks.begin(); Ts != Seq->Tracks.end(); Ts++)
			{
				SaveTrack(*Ts);
			}
			SaveFreePlugins();
			EndElement();
			EndDocumentWriter(true, true);
		}
		return true;
	}
	std::cout << "[WIREDSESSION] Writing XML failed for file: " << _DocumentFileName.mb_str() << std::endl;
	return false;
}


bool			WiredSessionXml::SaveSeq()
{
	int			Res = 0;
	wxChar		Buffer[20];
	
	Res += this->StartElement(STR_SEQUENCEUR);
	Res += this->WriteAttribute(STR_LOOP, 
		Seq->Loop == true ? STR_TRUE : STR_FALSE, false);
	Res += this->WriteAttribute(STR_CLICK, 
		Seq->Click == true ? STR_TRUE : STR_FALSE);
	Res += this->StartElement(STR_BPM);
	wxSnprintf(Buffer, 20, wxT("%.2f"), Seq->BPM);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_SIG_NUM);
	wxSnprintf(Buffer, 20, wxT("%d"), Seq->SigNumerator);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_SIG_DEN);
	wxSnprintf(Buffer, 20, wxT("%d"), Seq->SigDenominator);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_CURRENT_POS);
	wxSnprintf(Buffer, 20, wxT("%g"), Seq->CurrentPos);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_END_POS);
	wxSnprintf(Buffer, 20, wxT("%g"), Seq->EndPos);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_BEGIN_LOOP);
	wxSnprintf(Buffer, 20, wxT("%g"), Seq->BeginLoopPos);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_END_LOOP);
	wxSnprintf(Buffer, 20, wxT("%g"), Seq->EndLoopPos);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->EndElement();
	return Res == 0;
}

bool			WiredSessionXml::SaveTrack(Track* TrackInfo)
{
	if (TrackInfo == NULL)
		return false;
	int			Res = 0;
	wxChar		Buffer[20];
	PatternIter	Piter;

	Res += this->StartElement(STR_TRACK);
	Res += this->WriteAttribute(STR_TRACK_TYPE, 
		(TrackInfo->IsAudioTrack() == true ? STR_AUDIO : STR_MIDI), false);
	Res += this->WriteAttribute(STR_MUTED,
		TrackInfo->TrackOpt->Mute == true ? STR_TRUE : STR_FALSE, false);
	Res += this->WriteAttribute(STR_RECORDING,
		 TrackInfo->TrackOpt->Record == true ? STR_TRUE : STR_FALSE);
	Res += this->StartElement(STR_DEVIDE_ID);
	wxSnprintf(Buffer, 20, wxT("%dl"), TrackInfo->TrackOpt->DeviceId);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_NAME);
	Res += this->WriteString((wxString)TrackInfo->TrackOpt->Text->GetValue());
	Res += this->EndElement();
	if (TrackInfo->TrackPattern != NULL)
	{
		for (Piter = TrackInfo->TrackPattern->Patterns.begin(); 
			Piter != TrackInfo->TrackPattern->Patterns.end(); Piter++)
			{
				SavePattern(*Piter, TrackInfo->IsAudioTrack());
			}
	}
	SaveTrackPlugins(TrackInfo);
	Res += this->EndElement();
	return Res == 0;
}

bool			WiredSessionXml::SaveTrackPlugins(Track* TrackInfo)
{
	PluginIter	IterPlugins;
	RackTrack	*PluginsConnected = TrackInfo->TrackOpt->ConnectedRackTrack;

	if (PluginsConnected == NULL)
		return true;
	for (IterPlugins = PluginsConnected->Racks.begin(); IterPlugins != PluginsConnected->Racks.end(); IterPlugins++)
	{
		if (*IterPlugins == TrackInfo->TrackOpt->Connected)
			SavePlugin(*IterPlugins);
	}
	return true;
}

bool			WiredSessionXml::IsPluginConnected(Plugin *Plug)
{
	TrackIter	Ts;
			
	for (Ts = Seq->Tracks.begin(); Ts != Seq->Tracks.end(); Ts++)
	{
		if ((*Ts)->TrackOpt->Connected != NULL)
			return true;
	}
	return false;
}

bool			WiredSessionXml::SaveFreePlugins()
{
	PluginIter								IterPlugins;
	bool									Result = false;
	list<RackTrack *>::iterator 			It;

	for (It = RackPanel->RackTracks.begin(); It != RackPanel->RackTracks.end(); It++)  
	{
		for (IterPlugins = (*It)->Racks.begin(); IterPlugins != (*It)->Racks.end(); IterPlugins++)  
		{
			if (IsPluginConnected(*IterPlugins) == false)
			{
				SavePlugin(*IterPlugins);
				Result = true;
			}
		}
	}
	return true;
}

bool			WiredSessionXml::SavePlugin(Plugin* PluginInfo)
{
	int					Res = 0;
	WiredPluginData		Params;
	PluginParamsIter	IterParam;
	wxChar				Buffer[20];
		
	PluginInfo->Save(Params);
	Res += this->StartElement(STR_PLUGIN);
	Res += this->StartElement(STR_PLUGIN_ID);
	wxSnprintf(Buffer, 20, wxT("%ld"), PluginInfo->InitInfo->UniqueExternalId);
	Res += this->WriteString(PluginInfo->InitInfo->UniqueExternalId == 0 ? 
							wxString(PluginInfo->InitInfo->UniqueId, *wxConvCurrent).substr(0, 4) :
							wxString(Buffer));
	Res += this->EndElement();
	Res += this->StartElement(STR_NAME);
	Res += this->WriteString(PluginInfo->Name);
	Res += this->EndElement();
	Res += this->StartElement(STR_WIDTH);
	wxSnprintf(Buffer, 20, wxT("%d"), PluginInfo->InitInfo->UnitsX);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_HEIGHT);
	wxSnprintf(Buffer, 20, wxT("%d"), PluginInfo->InitInfo->UnitsY);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	if (Params.GetParamsStack() != NULL)
		for (IterParam = Params.GetParamsStack()->begin(); IterParam != Params.GetParamsStack()->end(); IterParam++)
		{
			Res += this->StartElement(STR_PLUGIN_DATA);
			Res += this->StartElement(STR_PLUGIN_DATA_PARAM_NAME);
			Res += this->WriteString(IterParam->first);
			Res += this->EndElement();
			Res += this->StartElement(STR_PLUGIN_DATA_PARAM_VALUE);
			Res += this->WriteString(IterParam->second);
			Res += this->EndElement();
			Res += this->EndElement();
		}
	Res += this->EndElement();
	return Res == 0;
}


bool			WiredSessionXml::SavePattern(Pattern* PatternInfo, bool AudioTrack)
{
	int			Res = 0;
	wxChar		Buffer[20];

	Res += this->StartElement(STR_PATTERN);
	Res += this->StartElement(STR_START_POS);
	wxSnprintf(Buffer, 20, wxT("%g"), PatternInfo->GetPosition());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();	
	wxSnprintf(Buffer, 20, wxT("%g"), PatternInfo->GetEndPosition());
	Res += this->StartElement(STR_END_POS);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_NAME);
	Res += this->WriteString((wxString)PatternInfo->GetName());
	Res += this->EndElement();
	if (AudioTrack == true)
		SavePatternAudioData((AudioPattern*)PatternInfo);
	else
		SavePatternMIDIData((MidiPattern*)PatternInfo);
	Res += this->EndElement();
	return Res == 0;
}

bool			WiredSessionXml::SavePatternAudioData(AudioPattern* PatternInfo)
{
	int			Res = 0;
	wxChar		Buffer[20];

	Res += this->StartElement(STR_PATTERN_DATA);
	Res += this->StartElement(STR_PATTERN_AUDIO_DATA);
	Res += this->StartElement(STR_START_POS);
	wxSnprintf(Buffer, 20, wxT("%ld"), PatternInfo->GetStartWavePos());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_END_POS);
	wxSnprintf(Buffer, 20, wxT("%ld"), PatternInfo->GetEndWavePos());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(STR_FILENAME);
	Res += this->WriteString(PatternInfo->FileName);
	Res += this->EndElement();
	Res += this->EndElement();
	Res += this->EndElement();
	return Res == 0;
}

bool			WiredSessionXml::SavePatternMIDIData(MidiPattern* PatternInfo)
{
	int				Res = 0;
	wxChar			Buffer[20];
	MidiEventIter	MidiIter;

	Res += this->StartElement(STR_PATTERN_DATA);
	Res += this->StartElement(STR_PATTERN_MIDI_DATA);
	Res += this->StartElement(STR_PPQN);
	wxSnprintf(Buffer, 20, wxT("%hd"), PatternInfo->GetPPQN());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	for (MidiIter = PatternInfo->Events.begin(); MidiIter != PatternInfo->Events.end(); MidiIter++)
	{
		Res += this->StartElement(STR_START_POS);
		wxSnprintf(Buffer, 20, wxT("%g"), (*MidiIter)->Position);
		Res += this->WriteString(Buffer);
		Res += this->EndElement();
		Res += this->StartElement(STR_END_POS);
		wxSnprintf(Buffer, 20, wxT("%g"), (*MidiIter)->EndPosition);
		Res += this->WriteString(Buffer);
		Res += this->EndElement();
		Res += this->StartElement(STR_MIDI_MESSAGE);
		wxSnprintf(Buffer, 20, wxT("%d %d %d"), (*MidiIter)->Msg[0], (*MidiIter)->Msg[1], (*MidiIter)->Msg[2]);
		Res += this->WriteBin(Buffer, 0, (3 * sizeof(int)));
		Res += this->EndElement();
	}
	Res += this->EndElement();
	Res += this->EndElement();
	return Res == 0;
}


bool			WiredSessionXml::CreateFile()
{
	int			fd = INVALID_FD;

	fd = open(_DocumentFileName.mb_str(*wxConvCurrent), FLAGS_OPEN_CW);	
	if (fd != INVALID_FD)
	{
		close(fd);
		return CreateDocument(_DocumentFileName);
	}
	std::cout << "[WIREDSESSION] Could not open file : " << strerror(errno) << std::endl;
	return false;
}


bool			WiredSessionXml::ParseWiredSession()
{
	int			TrackNumber = 0;
	bool		IsInProject = false;
	bool		HasFoundProject = false;
	
	while (Read() == true)
	{
		wxString 	Name = GetNodeName();
		
		if (Name.Cmp(STR_ROOT_NODE_NAME) == 0)
		{
			HasFoundProject = true;
			IsInProject = IsInProject ? false : true;
			if (IsInProject == false)
				break;
		}
		if (Name.Cmp(STR_WORKING_DIR) == 0 && IsInProject)
			LoadWorkingDir();
		else if (Name.Cmp(STR_SEQUENCEUR) == 0 && IsInProject)
			LoadSeq();
		else if (Name.Cmp(STR_TRACK) == 0 && IsInProject)
			LoadTrack(TrackNumber++);
		else if (Name.Cmp(STR_PLUGIN) == 0 && IsInProject)
			LoadPlugin(NULL);
	}
	return HasFoundProject;
}


void			WiredSessionXml::LoadWorkingDir()
{
	Read();
	wxString	Value = GetNodeValue();

	if (!Value.IsEmpty())
	{
		wxFileName 		session_dir(_DocumentFileName);
		wxFileName 		f(Value);

		f.MakeAbsolute(session_dir.GetPath());
		_WorkingDir = f.GetFullPath();
		cout << "[WIREDSESSION] Audio dir: " << _WorkingDir.mb_str() << endl;
	}	
	Read();	
}


void			WiredSessionXml::LoadSeq()
{
  wxString	Buffer;
  wxString	Value;
  long		number;
  double	floatnb;

  Buffer = GetAttribute(STR_LOOP);
  if (!Buffer.IsEmpty())
    {
      if (Buffer.Cmp(STR_TRUE) == 0)
	{
	  Seq->Loop = true;
	  TransportPanel->SetLoop(true);
	}
      else
	{
	  Seq->Loop = false;
	  TransportPanel->SetLoop(false);
	}
    }
  else
    {		
      Seq->Loop = false;
      TransportPanel->SetLoop(false);
    }
  Buffer = GetAttribute(STR_CLICK);
  if (!Buffer.IsEmpty())
    {
      if (Buffer.Cmp(STR_TRUE) == 0)
	{
	  Seq->Click = true;
	  TransportPanel->SetClick(true);
	}
      else
	{
	  Seq->Click = false;
	  TransportPanel->SetClick(false);
	}
    }
  else
    {
      Seq->Click = false;
      TransportPanel->SetClick(false);
    }
  while (Read() == true)
    {
      Buffer = GetNodeName();
      if (Buffer.Cmp(STR_SEQUENCEUR) == 0)
	break;
      Read();
      Value = GetNodeValue();
      if (Buffer.Cmp(STR_BPM) == 0)
	{
	  Value.ToDouble(&floatnb);
	  Seq->SetBPM(floatnb);
	  TransportPanel->SetBpm(floatnb);
	}
      else if (Buffer.Cmp(STR_SIG_NUM) == 0)
	{
	  Value.ToLong(&number);
	  Seq->SetSigNumerator(number);
	  TransportPanel->SetSigNumerator(number);
	}
      else if (Buffer.Cmp(STR_SIG_DEN) == 0)
	{
	  Value.ToLong(&number);
	  Seq->SetSigDenominator(number);
	  TransportPanel->SetSigDenominator(number);
	}
      else if (Buffer.Cmp(STR_BEGIN_LOOP) == 0)
	Value.ToDouble(&Seq->BeginLoopPos);
      else if (Buffer.Cmp(STR_END_LOOP) == 0)
	Value.ToDouble(&Seq->EndLoopPos);
      else if(Buffer.Cmp(STR_CURRENT_POS) == 0)
	Value.ToDouble(&Seq->CurrentPos);
      else if (Buffer.Cmp(STR_END_POS) == 0)
	Value.ToDouble(&Seq->EndPos);
      else
	continue;
      Read();
    }
}


void			WiredSessionXml::LoadTrack(int Number)
{
	wxString	Buffer;
	wxString	Value;
	Track		*NewTrack;
	
	Buffer = GetAttribute(STR_TRACK_TYPE);	
	if (!Buffer.IsEmpty())
	{
		if (Buffer.Cmp(STR_AUDIO) == 0)
			NewTrack = SeqPanel->AddTrack(true);
		else
			NewTrack = SeqPanel->AddTrack(false);
	}
	else
		NewTrack = SeqPanel->AddTrack();
	Buffer = GetAttribute(STR_MUTED);
	if (!Buffer.IsEmpty())
	{
		if (Buffer.Cmp(STR_TRUE) == 0)
			NewTrack->TrackOpt->SetMute(true);
		else
			NewTrack->TrackOpt->SetMute(false);
	}
	else
		NewTrack->TrackOpt->SetMute(false);
	Buffer = GetAttribute(STR_RECORDING);	
	if (!Buffer.IsEmpty())
	{
		if (Buffer.Cmp(STR_TRUE) == 0)
			NewTrack->TrackOpt->SetRecording(true);
		else
			NewTrack->TrackOpt->SetRecording(false);
	}
	else
		NewTrack->TrackOpt->SetRecording(false);
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.Cmp(STR_TRACK) == 0)
			break;
		if (Buffer.Cmp(STR_DEVIDE_ID) == 0)
		{
		  long	tmp;

		  Read();
		  Value = GetNodeValue();
		  if (!Value.IsEmpty())
		    {
		      Value.ToLong(&tmp);
		      NewTrack->TrackOpt->SetDeviceId(tmp);
		    }
		}
		else if (Buffer.Cmp(STR_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
				NewTrack->TrackOpt->SetName(Value);
		}
		else if (Buffer.Cmp(STR_PATTERN) == 0)
		{
			LoadPattern(NewTrack, Number);
			continue;
		}
		else if (Buffer.Cmp(STR_PLUGIN) == 0)
		{
			LoadPlugin(NewTrack);
			continue;
		}
		else
			continue;
		Read();
	}
}


void			WiredSessionXml::LoadPlugin(Track* TrackInfo)
{
	wxString		Buffer;
	wxString		Value;
	t_PluginXml		Plugin;
	
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.Cmp(STR_PLUGIN) == 0)
			break;
		if (Buffer.Cmp(STR_PLUGIN_ID) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
				Plugin.Id = Value;
		}
		else if (Buffer.Cmp(STR_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
				Plugin.Name = Value;
		}
		else if (Buffer.Cmp(STR_WIDTH) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  Value.ToLong((long*)&Plugin.Width);
		}
		else if (Buffer.Cmp(STR_HEIGHT) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  Value.ToLong((long*)&Plugin.Height);
		}
		else if (Buffer.Cmp(STR_PLUGIN_DATA) == 0)
		{
			LoadPluginData(&Plugin);
			continue;
		}
		else
			continue;
		Read();
	}
	LoadTrackPlugin(TrackInfo, &Plugin);
}


void			WiredSessionXml::LoadTrackPlugin(Track* TrackInfo, t_PluginXml *PluginInfo)
{
	RackTrack								*NewRack;
	Plugin 									*NewPlugin;
	std::vector<PluginLoader *>::iterator 	it;
	PluginLoader 							*p = 0x0;
	unsigned long							Id;

	PluginInfo->Id.ToLong((long*)&Id);
	NewRack = RackPanel->AddTrack();
	if (Id == 0)
	{
		for (it = LoadedPluginsList.begin(); it != LoadedPluginsList.end(); it++)
			if (COMPARE_IDS((*it)->InitInfo.UniqueId, PluginInfo->Id.mb_str()))
			{
				p = *it;
				break;
			}
	}
	else
	{
		p = new PluginLoader(LoadedExternalPlugins, Id);
	}
	if (p)
	  {
	    cout << "[WIREDSESSION] Creating rack for plugin: " << p->InitInfo.Name.mb_str() << endl;
	    NewPlugin = NewRack->AddRack(StartInfo, p);
	  }
    else
		cout << "[WIREDSESSION] Plugin with Id  " << PluginInfo->Id.mb_str() << " is not loaded" << endl;     
    if (NewPlugin)
		NewPlugin->Name = PluginInfo->Name;
	if (TrackInfo != NULL)
		TrackInfo->TrackOpt->ConnectTo(NewPlugin);	
	NewPlugin->Load(PluginInfo->Data);

}


void			WiredSessionXml::LoadPluginData(t_PluginXml *Params)
{
	wxString		Buffer;
	wxString		Value;
	wxString		ParamValue;
	wxString		ParamName;
	
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.Cmp(STR_PLUGIN_DATA) == 0)
			break;
		if (Buffer.Cmp(STR_PLUGIN_DATA_PARAM_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
				ParamName = Value;
		}
		else if (Buffer.Cmp(STR_PLUGIN_DATA_PARAM_VALUE) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
				ParamValue = Value;
		}
		else
			continue;
		Read();
	}
	if (ParamValue.size() > 0 && ParamName.size() > 0)
		Params->Data.SaveValue(ParamName, ParamValue);
}


void			WiredSessionXml::LoadPattern(Track *AddedTrack, int TrackNumber)
{
	wxString		Buffer;
	wxString		Value;
	t_PatternXml	PatternInfo;
	
	PatternInfo.TrackNumber = TrackNumber;
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.Cmp(STR_PATTERN) == 0)
			break;
		if (Buffer.Cmp(STR_START_POS) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  Value.ToDouble(&PatternInfo.Position);
		}
		else if (Buffer.Cmp(STR_END_POS) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  Value.ToDouble(&PatternInfo.EndPosition);
		}
		else if (Buffer.Cmp(STR_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  PatternInfo.Name = Value;
		}
		else if (Buffer.Cmp(STR_PATTERN_DATA) == 0)
		{
			if (AddedTrack->IsAudioTrack() == true)
				LoadPatternAudio(AddedTrack, &PatternInfo);
			else
				LoadPatternMIDI(AddedTrack, &PatternInfo);
			continue;
		}
		else
			continue;
		Read();
	}
}


void			WiredSessionXml::LoadPatternAudio(Track *AddedTrack, t_PatternXml *InfoPattern)
{
	wxString			Buffer;
	wxString			Value;
	t_AudioPatternXml		APatternInfo;
	AudioPattern			*Result;
	WaveFile			*AudioFile;
	bool				IsInData = false;

	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.Cmp(STR_PATTERN_DATA) == 0)
			break;
		if (Buffer.Cmp(STR_PATTERN_AUDIO_DATA) == 0)
		{
			IsInData = IsInData ? false : true;
			continue;
		}
		else if (Buffer.Cmp(STR_START_POS) == 0 && IsInData) 
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  Value.ToLong(&APatternInfo.StartWavePos);
		}
		else if (Buffer.Cmp(STR_END_POS) == 0 && IsInData)
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  Value.ToLong(&APatternInfo.EndWavePos);
		}
		else if (Buffer.Cmp(STR_FILENAME) == 0 && IsInData && !Value.IsEmpty())
		{
			Read();
			Value = GetNodeValue();
			if (!Value.IsEmpty())
			  APatternInfo.FileName = Value;
		}
		else
			continue;
		Read();
	}
	if (APatternInfo.FileName.size() > 0)
	{
		AudioFile = WaveCenter.AddWaveFile(APatternInfo.FileName);
		if (AudioFile != NULL)
		{
			Result = new AudioPattern(InfoPattern->Position, InfoPattern->EndPosition, InfoPattern->TrackNumber);
			Result->SetStartWavePos(APatternInfo.StartWavePos);
			Result->SetEndWavePos(APatternInfo.EndWavePos);
			Result->SetWave(AudioFile);
			Result->FileName = APatternInfo.FileName;
			AddedTrack->AddPattern(Result);
		}
		else
			cout << "[WIREDSESSION] Could not open file: " 
				 << APatternInfo.FileName << endl;
	}
	else
		cout << "[WIREDSESSION] Invalid FileName" << endl;
}

void			WiredSessionXml::LoadPatternMIDI(Track *AddedTrack, t_PatternXml *InfoPattern)
{
  wxString	Buffer;
  wxString	Value;

  while (42)
    {
      Read();
      Buffer = GetNodeName();
      if (Buffer.Cmp(STR_PATTERN) == 0)
	break;
      Read();
      Value = GetNodeValue();
    }
}

void			WiredSessionXml::Dumpfile(const wxString& FileName)
{
	if (FileName != wxT(""))
		_DocumentFileName = FileName;
	if (OpenDocument(_DocumentFileName) == true)
	{
		if (OpenDtd(DTD_FILENAME) == true)
		{
			if (ValidDocument() == true)
			{
				while (Read() == true)
				{
				  std::cout << "Node Name == {" << GetNodeName().mb_str() << "}" << std::endl;
					if (HasValue() == true)
					  std::cout << "Node Value == {" << GetNodeValue().mb_str() << "}" << std::endl;
				}
			}
			else
				std::cout << "[WIREDSESSION] File {" << _DocumentFileName.mb_str() 
					<< "} does not comply with DTD {" << DTD_FILENAME << "}" 
					<< std::endl;
		}
		else
		{
			std::cout << "[WIREDSESSION] Unable to load DTD file {" 
				<< DTD_FILENAME << "}" << std::endl << "error == " << strerror(errno) << std::endl;
		}
	}
	else
	{
		std::cout << "[WIREDSESSION] Unable to load file {" 
			<< _DocumentFileName.mb_str() << "}" << std::endl;
	}
}


wxString&	WiredSessionXml::GetAudioDir()
{
	return _WorkingDir;
}
