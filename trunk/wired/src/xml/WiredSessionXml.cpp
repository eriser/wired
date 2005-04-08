#include "WiredSessionXml.h"

//extern std::vector<PluginLoader *>	LoadedPluginsList;
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

bool			WiredSessionXml::Load(const std::string& FileName)
{
	//Dumpfile(FileName);
	//return true;
	if (FileName != "")
		_DocumentFileName = FileName;
	if (OpenDocument(_DocumentFileName) == true)
	{
		if (OpenDtd(std::string(DTD_FILENAME)) == true)
		{
			if (ValidDocument() == true)
				return ParseWiredSession();
			else
				std::cout << "[WIREDSESSION] File {" << _DocumentFileName.c_str() 
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
			<< _DocumentFileName.c_str() << "}" << std::endl;
	}
	return false;
}

bool			WiredSessionXml::Save()
{
	if (CreateFile() == true)
	{
		if (_DocumentWriter != NULL)
		{
			wxFileName 	session_dir(_DocumentWriterName.c_str());
			wxFileName 	f(_WorkingDir.c_str());
			TrackIter	Ts;

			f.MakeRelativeTo(session_dir.GetPath());
			StartElement(std::string(STR_ROOT_NODE_NAME));
			StartElement(std::string(STR_WORKING_DIR));
			WriteString((std::string)f.GetFullPath());
			EndElement();
			SaveSeq();
			for (Ts = Seq->Tracks.begin(); Ts != Seq->Tracks.end(); Ts++)
			{
				SaveTrack(*Ts);
			}
			EndElement();
			EndDocumentWriter(true, true);
		}
		return true;
	}
	std::cout << "[WIREDSESSION] Writing XML failed for file: " << _DocumentFileName.c_str() << std::endl;
	return false;
}

//<!ATTLIST Sequencer Loop (true,false) #REQUIRED 
//Click (true,false) #REQUIRED>
bool			WiredSessionXml::SaveSeq()
{
	int			Res = 0;
	char		Buffer[20];
	
	Res += this->StartElement(std::string(STR_SEQUENCEUR));
	Res += this->WriteAttribute(std::string(STR_LOOP), 
		Seq->Loop == true ? std::string(STR_TRUE) : std::string(STR_FALSE), false);
	Res += this->WriteAttribute(std::string(STR_CLICK), 
		Seq->Click == true ? std::string(STR_TRUE) : std::string(STR_FALSE));
	Res += this->StartElement(std::string(STR_BPM));
	sprintf(Buffer, "%.2f", Seq->BPM);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_SIG_NUM));
	sprintf(Buffer, "%d", Seq->SigNumerator);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_SIG_DEN));
	sprintf(Buffer, "%d", Seq->SigDenominator);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_CURRENT_POS));
	sprintf(Buffer, "%g", Seq->CurrentPos);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_END_POS));
	sprintf(Buffer, "%g", Seq->EndPos);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_BEGIN_LOOP));
	sprintf(Buffer, "%g", Seq->BeginLoopPos);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_END_LOOP));
	sprintf(Buffer, "%g", Seq->EndLoopPos);
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
	char		Buffer[20];
	PatternIter	Piter;

	Res += this->StartElement(std::string(STR_TRACK));
	Res += this->WriteAttribute(std::string(STR_TRACK_TYPE), 
		(TrackInfo->IsAudioTrack() == true ? std::string(STR_AUDIO) : std::string(STR_MIDI)), false);
	Res += this->WriteAttribute(std::string(STR_MUTED),
		TrackInfo->TrackOpt->Mute == true ? std::string(STR_TRUE) : std::string(STR_FALSE), false);
	Res += this->WriteAttribute(std::string(STR_RECORDING),
		 TrackInfo->TrackOpt->Record == true ? std::string(STR_TRUE) : std::string(STR_FALSE));
	Res += this->StartElement(std::string(STR_DEVIDE_ID));
	sprintf(Buffer, "%dl", TrackInfo->TrackOpt->DeviceId);
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_NAME));
	Res += this->WriteString((std::string)TrackInfo->TrackOpt->Text->GetValue());
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
	RackTrack	*Plugins = TrackInfo->TrackOpt->ConnectedRackTrack;
	
	if (Plugins == NULL)
		return true;
	for (IterPlugins = Plugins->Racks.begin(); IterPlugins != Plugins->Racks.end(); IterPlugins++)
		SavePlugin(*IterPlugins);
	return true;
}

bool			WiredSessionXml::SavePlugin(Plugin* PluginInfo)
{
	int					Res = 0;
	WiredPluginData		Params;
	PluginParamsIter	IterParam;
	char				Buffer[20];
	
	//TODO Replace with the Xml Version...
	//PluginInfo->Save(Params);

	Res += this->StartElement(std::string(STR_PLUGIN));
	Res += this->StartElement(std::string(STR_PLUGIN_ID));
	Res += this->WriteString(std::string(PluginInfo->InitInfo->UniqueId));
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_NAME));
	Res += this->WriteString(PluginInfo->Name);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_WIDTH));
	sprintf(Buffer, "%d", PluginInfo->InitInfo->UnitsX);
	Res += this->WriteString(std::string(Buffer));
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_HEIGHT));
	sprintf(Buffer, "%d", PluginInfo->InitInfo->UnitsY);
	Res += this->WriteString(std::string(Buffer));
	Res += this->EndElement();
	if (Params.GetParamsStack() != NULL)
		for (IterParam = Params.GetParamsStack()->begin(); IterParam != Params.GetParamsStack()->end(); IterParam++)
		{
			Res += this->StartElement(std::string(STR_PLUGIN_DATA));	
			Res += this->WriteElement(std::string(STR_PLUGIN_DATA_PARAM_NAME), IterParam->first);
			Res += this->StartElement(std::string(STR_PLUGIN_DATA_PARAM_VALUE));
			Res += this->WriteCDATA(IterParam->second);
			Res += this->EndElement();	
			Res += this->EndElement();
		}
	Res += this->EndElement();
	return Res == 0;
}

//<!ELEMENT Pattern (StartPos+,EndPos+,Name+,PatternData*)> 
bool			WiredSessionXml::SavePattern(Pattern* PatternInfo, bool AudioTrack)
{
	int			Res = 0;
	char		Buffer[20];

	Res += this->StartElement(std::string(STR_PATTERN));
	Res += this->StartElement(std::string(STR_START_POS));
	sprintf(Buffer, "%g", PatternInfo->GetPosition());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();	
	sprintf(Buffer, "%g", PatternInfo->GetEndPosition());
	Res += this->StartElement(std::string(STR_END_POS));
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_NAME));
	Res += this->WriteString((std::string)PatternInfo->GetName());
	Res += this->EndElement();
	if (AudioTrack == true)
		SavePatternAudioData((AudioPattern*)PatternInfo);
	else
		SavePatternMIDIData((MidiPattern*)PatternInfo);
	Res += this->EndElement();
	return Res == 0;
}

//<!ELEMENT PatternData (PatternAudio|PatternMIDIData)+>
//<!ELEMENT PatternAudioData (StartPos,EndPos,FileName)+>
bool			WiredSessionXml::SavePatternAudioData(AudioPattern* PatternInfo)
{
	int			Res = 0;
	char		Buffer[20];

	Res += this->StartElement(std::string(STR_PATTERN_DATA));
	Res += this->StartElement(std::string(STR_PATTERN_AUDIO_DATA));
	Res += this->StartElement(std::string(STR_START_POS));
	sprintf(Buffer, "%ld", PatternInfo->GetStartWavePos());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_END_POS));
	sprintf(Buffer, "%ld", PatternInfo->GetEndWavePos());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	Res += this->StartElement(std::string(STR_FILENAME));
	Res += this->WriteString(PatternInfo->FileName);
	Res += this->EndElement();
	Res += this->EndElement();
	Res += this->EndElement();
	return Res == 0;
}

//<!ELEMENT PatternData (PatternAudio|PatternMIDIData)+>
//<!ELEMENT PatternMIDIData (PPQN,StartPos,EndPos,MIDIMessage)+>
//<!ELEMENT FileName (#PCDATA)>
//<!ELEMENT PPQN (#PCDATA)>
//<!ELEMENT MIDIMessage (#PCDATA)>
bool				WiredSessionXml::SavePatternMIDIData(MidiPattern* PatternInfo)
{
	int				Res = 0;
	char			Buffer[20];
	MidiEventIter	MidiIter;

	Res += this->StartElement(std::string(STR_PATTERN_DATA));
	Res += this->StartElement(std::string(STR_PATTERN_MIDI_DATA));
	Res += this->StartElement(std::string(STR_PPQN));
	sprintf(Buffer, "%hd", PatternInfo->GetPPQN());
	Res += this->WriteString(Buffer);
	Res += this->EndElement();
	for (MidiIter = PatternInfo->Events.begin(); MidiIter != PatternInfo->Events.end(); MidiIter++)
	{
		Res += this->StartElement(std::string(STR_START_POS));
		sprintf(Buffer, "%g", (*MidiIter)->Position);
		Res += this->WriteString(Buffer);
		Res += this->EndElement();
		Res += this->StartElement(std::string(STR_END_POS));
		sprintf(Buffer, "%g", (*MidiIter)->EndPosition);
		Res += this->WriteString(Buffer);
		Res += this->EndElement();
		Res += this->StartElement(std::string(STR_MIDI_MESSAGE));
		sprintf(Buffer, "%d %d %d", (*MidiIter)->Msg[0], (*MidiIter)->Msg[1], (*MidiIter)->Msg[2]);
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

	fd = open(_DocumentFileName.c_str(), FLAGS_OPEN_CW);	
	if (fd != INVALID_FD)
	{
		close(fd);
		return CreateDocument(_DocumentFileName);
	}
	std::cout << "[WiredSessionXml] Could not open file : " << strerror(errno) << std::endl;
	return false;
}

bool			WiredSessionXml::ParseWiredSession()
{
	int			TrackNumber = 0;
	bool		IsInProject = false;
	bool		HasFoundProject = false;
	
	while (Read() == true)
	{
		std::string 	Name = GetNodeName();
		
		std::cout << "[ParesWiredSession] Node Name == " << Name.c_str() << std::endl;
		if (Name.compare(STR_ROOT_NODE_NAME) == 0)
		{
			HasFoundProject = true;
			IsInProject = IsInProject ? false : true;
			if (IsInProject == false)
				break;
		}
		if (Name.compare(STR_WORKING_DIR) == 0 && IsInProject)
			LoadWorkingDir();
		else if (Name.compare(STR_SEQUENCEUR) == 0 && IsInProject)
			LoadSeq();
		else if (Name.compare(STR_TRACK) == 0 && IsInProject)
			LoadTrack(TrackNumber++);
	}
	return HasFoundProject;
}

void			WiredSessionXml::LoadWorkingDir()
{
	Read();
	char	*Value = GetNodeValue();

	if (Value != NULL)
	{
		wxFileName 		session_dir(_DocumentFileName.c_str());
		wxFileName 		f(Value);

		f.MakeAbsolute(session_dir.GetPath());
		_WorkingDir = f.GetFullPath();
		cout << "[WIREDSESSIONXML] Audio dir: " << _WorkingDir.c_str() << endl;
	}	
	Read();	
}

void			WiredSessionXml::LoadSeq()
{
	std::string	Buffer;
	char		*Value;
	
	Buffer = GetAttribute(STR_LOOP);
	if (Buffer.c_str()  != NULL)
	{
		if (Buffer.compare(STR_TRUE) == 0)
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
	if (Buffer.c_str() != NULL)
	{
		if (Buffer.compare(STR_TRUE) == 0)
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
		if (Buffer.compare(STR_SEQUENCEUR) == 0)
			break;
		Read();
		Value = GetNodeValue();
		std::cout << "[LoadSeq] NodeName == {" << Buffer.c_str() << "} Value == {" << Value << "}" << std::endl;
		if (Buffer.compare(STR_BPM) == 0)
		{
			Seq->SetBPM(atof(Value));
			TransportPanel->SetBpm(atof(Value));
		}
		else if (Buffer.compare(STR_SIG_NUM) == 0)
		{
			Seq->SetSigNumerator(atoi(Value));
			//TODO See what's happening here (SetSigNumerator seems to hanging on for some obscure reason)
			TransportPanel->SetSigNumerator(atoi(Value));
		}
		else if (Buffer.compare(STR_SIG_DEN) == 0)
		{
			Seq->SetSigDenominator(atoi(Value));
			TransportPanel->SetSigDenominator(atoi(Value));
		}
		else if (Buffer.compare(STR_BEGIN_LOOP) == 0)
			Seq->BeginLoopPos = atof(Value);
		else if (Buffer.compare(STR_END_LOOP) == 0)
			Seq->EndLoopPos = atof(Value);
		else if(Buffer.compare(STR_CURRENT_POS) == 0)
			Seq->CurrentPos = atof(Value);
		else if (Buffer.compare(STR_END_POS) == 0)
			Seq->EndPos = atof(Value);
		else
			continue;
		Read();
	}
}

void			WiredSessionXml::LoadTrack(int Number)
{
	std::string	Buffer;
	char 		*Value;
	Track		*NewTrack;
	
	Buffer = GetAttribute(STR_TRACK_TYPE);	
	if (Buffer.c_str() != NULL)
	{
		if (Buffer.compare(STR_AUDIO) == 0)
			NewTrack = SeqPanel->AddTrack(true);
		else
			NewTrack = SeqPanel->AddTrack(false);
	}
	else
		NewTrack = SeqPanel->AddTrack();
	Buffer = GetAttribute(STR_MUTED);
	if (Buffer.c_str() != NULL)
	{
		if (Buffer.compare(STR_TRUE) == 0)
			NewTrack->TrackOpt->SetMute(true);
		else
			NewTrack->TrackOpt->SetMute(false);
	}
	else
		NewTrack->TrackOpt->SetMute(false);
	Buffer = GetAttribute(STR_RECORDING);	
	if (Buffer.c_str() != NULL)
	{
		if (Buffer.compare(STR_TRUE) == 0)
			NewTrack->TrackOpt->SetRecording(true);
		else
			NewTrack->TrackOpt->SetRecording(false);
	}
	else
		NewTrack->TrackOpt->SetRecording(false);
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.compare(STR_TRACK) == 0)
			break;
		if (Buffer.compare(STR_DEVIDE_ID) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				NewTrack->TrackOpt->SetDeviceId(atol(Value));
		}
		else if (Buffer.compare(STR_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				NewTrack->TrackOpt->SetName(wxString(Value));
		}
		else if (Buffer.compare(STR_PATTERN) == 0)
		{
			LoadPattern(NewTrack, Number);
			continue;
		}
		else if (Buffer.compare(STR_PLUGIN) == 0)
		{
			//TODO Implement multiple plugins stack
		}
		else
			continue;
		std::cout << "[LoadTrack] NodeName == {" << Buffer.c_str() << "} Value == {" << (Value != NULL ? Value : "NULL") << "}" << std::endl;
		Read();
	}
}

void			WiredSessionXml::LoadPlugin()
{
	std::string		Buffer;
	char 			*Value;
	t_PluginXml		Plugin;
	
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.compare(STR_PLUGIN) == 0)
			break;
		if (Buffer.compare(STR_PLUGIN_ID) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				Plugin.Id = Value;
		}
		else if (Buffer.compare(STR_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				Plugin.Name = Value;
		}
		else if (Buffer.compare(STR_WIDTH) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				Plugin.Width = atoi(Value);
		}
		else if (Buffer.compare(STR_HEIGHT) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				Plugin.Height = atoi(Value);
		}
		else if (Buffer.compare(STR_PLUGIN_DATA) == 0)
		{
			LoadPluginData(&Plugin);
		}
		else
			continue;
		std::cout << "[LoadPlugin] NodeName == {" << Buffer.c_str() << "} Value == {" << (Value != NULL ? Value : "NULL") << "}" << std::endl;
		Read();
	}
}

void			WiredSessionXml::LoadTrackPlugin(Track* TrackInfo, t_PluginXml *PluginInfo)
{
	RackTrack	*NewRack;
	Plugin 		*NewPlugin;
	
	NewRack = RackPanel->AddTrack();
	//NewPlugin = NewRack->AddRack(StartInfo, (*It));
	NewPlugin->Name = PluginInfo->Name;
	//TODO => Xml
//	NewPlugin->Load(PluginInfo->Data);
}

void			WiredSessionXml::LoadPluginData(t_PluginXml *Params)
{
	std::string		Buffer;
	char 			*Value;
	char			*ParamValue = NULL;
	std::string		ParamName;
	
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.compare(STR_PLUGIN_DATA) == 0)
			break;
		if (Buffer.compare(STR_PLUGIN_DATA_PARAM_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				ParamName = Value;
		}
		else if (Buffer.compare(STR_PLUGIN_DATA_PARAM_VALUE) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				ParamValue = strdup(Value);
		}
		else
			continue;
	}
	if (ParamValue != NULL && ParamName.size() > 0)
		Params->Data.SaveValue(ParamName, ParamValue);
}

void			WiredSessionXml::LoadPattern(Track *AddedTrack, int TrackNumber)
{
	std::string		Buffer;
	char 			*Value;
	t_PatternXml	PatternInfo;
	
	PatternInfo.TrackNumber = TrackNumber;
	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.compare(STR_PATTERN) == 0)
			break;
		if (Buffer.compare(STR_START_POS) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				PatternInfo.Position = atof(Value);
		}
		else if (Buffer.compare(STR_END_POS) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				PatternInfo.EndPosition = atof(Value);
		}
		else if (Buffer.compare(STR_NAME) == 0)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				PatternInfo.Name = Value;
		}
		else if (Buffer.compare(STR_PATTERN_DATA) == 0)
		{
			if (AddedTrack->IsAudioTrack() == true)
				LoadPatternAudio(AddedTrack, &PatternInfo);
			else
				LoadPatternMIDI(AddedTrack, &PatternInfo);
			continue;
		}
		else
			continue;
		std::cout << "[LoadPattern] NodeName == {" << Buffer.c_str() << "} Value == {" << (Value != NULL ? Value : "NULL") << "}" << std::endl;
		Read();
	}
}

void			WiredSessionXml::LoadPatternAudio(Track *AddedTrack, t_PatternXml *InfoPattern)
{
	std::string			Buffer;
	char 				*Value;
	t_AudioPatternXml	APatternInfo;
	AudioPattern		*Result;
	WaveFile			*AudioFile;
	bool				IsInData = false;

	while (Read() == true)
	{
		Buffer = GetNodeName();
		if (Buffer.compare(STR_PATTERN_DATA) == 0)
			break;
		if (Buffer.compare(STR_PATTERN_AUDIO_DATA) == 0)
		{
			IsInData = IsInData ? false : true;
			continue;
		}
		else if (Buffer.compare(STR_START_POS) == 0 && IsInData) 
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				APatternInfo.StartWavePos = atol(Value);
		}
		else if (Buffer.compare(STR_END_POS) == 0 && IsInData)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				APatternInfo.EndWavePos = atol(Value);
		}
		else if (Buffer.compare(STR_FILENAME) == 0 && IsInData && Value != NULL)
		{
			Read();
			Value = GetNodeValue();
			if (Value != NULL)
				APatternInfo.FileName = Value;
		}
		else
			continue;
		std::cout << "[LoadPatternAudio] NodeName == {" << Buffer.c_str() << "} Value == {" << (Value != NULL ? Value : "NULL") << "}" << std::endl;
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
			cout << "[WIREDSESSIONXML] Could not open file: " 
				 << APatternInfo.FileName << endl;
	}
	else
		cout << "[WIREDSESSIONXML] Invalid FileName" << endl;
}


void			WiredSessionXml::LoadPatternMIDI(Track *AddedTrack, t_PatternXml *InfoPattern)
{
	std::string	Buffer;
	char 		*Value;

	while (42)
	{
		Read();
		Buffer = GetNodeName();
		if (Buffer.compare(STR_PATTERN) == 0)
			break;
		Read();
		Value = GetNodeValue();		
		std::cout << "[LoadPatternMIDI] NodeName == {" << Buffer.c_str() << "} Value == {" << Value << "}" << std::endl;
	}
}

void			WiredSessionXml::Dumpfile(const std::string& FileName)
{
	if (FileName != "")
		_DocumentFileName = FileName;
	if (OpenDocument(_DocumentFileName) == true)
	{
		if (OpenDtd(std::string(DTD_FILENAME)) == true)
		{
			if (ValidDocument() == true)
			{
				while (Read() == true)
				{
					std::cout << "Node Name == {" << GetNodeName() << "}" << std::endl;
					if (HasValue() == true)
						std::cout << "Node Value == {" << GetNodeValue() << "}" << std::endl;
				}
			}
			else
				std::cout << "[WIREDSESSION] File {" << _DocumentFileName.c_str() 
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
			<< _DocumentFileName.c_str() << "}" << std::endl;
	}
}

WiredPluginData	WiredPluginData::operator=(const WiredPluginData& right)
{
	if (this != &right)
	{
		_Data = right._Data;
	}
	return *this;
}

bool			WiredPluginData::SaveValue(const std::string& Name, char *Value)
{
	if (_Data.find(Name) == _Data.end())
	{
		_Data[Name] = Value;
		return true;
	}
	return false;
}

char			*WiredPluginData::LoadValue(const std::string& Name)
{
	if (_Data.find(Name) != _Data.end())
		return _Data[Name];
	return NULL;
}

PluginParams	*WiredPluginData::GetParamsStack()
{
	if (_Data.empty() == false)
		return &_Data;
	return NULL;
}
