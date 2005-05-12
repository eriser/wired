	#include "WiredDSSI.h"

WiredLADSPAInstance::WiredLADSPAInstance()
{
	_Handle = NULL;
	_Descriptor = NULL;
	_Type = 0;
	_Properties = 0;
}

WiredLADSPAInstance::~WiredLADSPAInstance()
{
	UnLoad();
	_Descriptor = NULL;
}

WiredLADSPAInstance::WiredLADSPAInstance(const WiredLADSPAInstance& copy) : Plugin()
{
	*this = copy;
}

WiredLADSPAInstance		WiredLADSPAInstance::operator=(const WiredLADSPAInstance& right)
{
	if (this != &right)
	{
		_InputAudioPluginsPorts = right._InputAudioPluginsPorts;
		_OutputAudioPluginsPorts = right._OutputAudioPluginsPorts;
		_InputDataPluginsPorts = right._InputDataPluginsPorts;
		_OutputDataPluginsPorts = right._OutputDataPluginsPorts;
		_Handle = right._Handle;
		_Descriptor = right._Descriptor;
		_Type = right._Type;
		_Properties = right._Properties;
		_GuiControls = right._GuiControls;
	}
	return *this;
}

bool					WiredLADSPAInstance::Init(const LADSPA_Descriptor *Descriptor)
{
	UnLoad();
	if (Descriptor)
		_Descriptor = (LADSPA_Descriptor *)Descriptor;
	else
		return false;
	return true;
}

bool					WiredLADSPAInstance::Load()
{
	if (_Descriptor)
	{
		UnLoad();
		if (_Descriptor->instantiate!= NULL)
			if ((_Handle = _Descriptor->instantiate(_Descriptor, Audio->OutputSampleFormat)))
			{
				_Properties = _Descriptor->Properties;
				Name = _Descriptor->Name;
				LoadPorts();
				return true;
			}
		cout << "Load Failed" << endl;
	}
	return false;
}

void					WiredLADSPAInstance::UnLoad()
{
	if (_Handle && _Descriptor)
	{
		cout << "Unloading Plugin " << _Descriptor->Name;
		ChangeActivateState(false);
		cout << ".";
		_Descriptor->cleanup(_Handle);
		cout << ".";
		_Handle = NULL;
		UnLoadPorts();
		cout << ".";
		cout << "done" << endl;
	}
}

void					WiredLADSPAInstance::UnloadGUIPorts()
{
	map<unsigned long, t_gui_port>::iterator	Iter;
	
	for (Iter = _GuiControls.begin(); Iter != _GuiControls.end(); Iter++)
		if (Iter->second.Data != NULL)
			delete Iter->second.Data;
}

bool					WiredLADSPAInstance::ChangeActivateState(bool Activate)
{
	if (_Handle && _Descriptor)
	{
		if (Activate && _Descriptor->activate != NULL)
			_Descriptor->activate(_Handle);
		else if (_Descriptor->deactivate != NULL)
			_Descriptor->deactivate(_Handle);
		return true;
	}
	return false;
}

void					WiredLADSPAInstance::UnLoadPorts()
{
	_InputAudioPluginsPorts.clear();
	_OutputAudioPluginsPorts.clear();
	_InputDataPluginsPorts.clear();
	_OutputDataPluginsPorts.clear();	
}

void					WiredLADSPAInstance::LoadPorts()
{
	unsigned long		pos;
	t_ladspa_port		CurrentPort;

	UnLoadPorts();	
	for (pos = 0, CurrentPort.Descriptor = 0, CurrentPort.Id = 0, CurrentPort.RangeHint.LowerBound = 0, 
		 CurrentPort.RangeHint.UpperBound = 0, CurrentPort.RangeHint.HintDescriptor = 0;
		 pos < _Descriptor->PortCount; 
	 	 pos ++, CurrentPort.Descriptor = 0, CurrentPort.Id = 0, CurrentPort.RangeHint.LowerBound = 0, 
		 CurrentPort.RangeHint.UpperBound = 0, CurrentPort.RangeHint.HintDescriptor = 0)
	{
		CurrentPort.Descriptor = _Descriptor->PortDescriptors[pos];
		CurrentPort.RangeHint = _Descriptor->PortRangeHints[pos];
		CurrentPort.Id = pos;
		CurrentPort.Name = _Descriptor->PortNames[pos];
		if (LADSPA_IS_PORT_INPUT(CurrentPort.Descriptor))
		{
			if (LADSPA_IS_PORT_CONTROL(CurrentPort.Descriptor))
			{
				_InputDataPluginsPorts.insert(_InputDataPluginsPorts.end(), CurrentPort);
				AddGuiControl(&CurrentPort);
			}
			else if (LADSPA_IS_PORT_AUDIO(CurrentPort.Descriptor))
			{
				_InputAudioPluginsPorts.insert(_InputAudioPluginsPorts.end(), CurrentPort);
				_Type |= TYPE_PLUGINS_EFFECT;
			}
		}
		else if (LADSPA_IS_PORT_OUTPUT(CurrentPort.Descriptor))
		{
			if (LADSPA_IS_PORT_CONTROL(CurrentPort.Descriptor))
				_OutputDataPluginsPorts.insert(_OutputDataPluginsPorts.end(), CurrentPort);
			else if (LADSPA_IS_PORT_AUDIO(CurrentPort.Descriptor))
				_OutputAudioPluginsPorts.insert(_OutputAudioPluginsPorts.end(), CurrentPort);
		}
	}
	if (!(_Type & TYPE_PLUGINS_EFFECT))
		_Type |= TYPE_PLUGINS_INSTR;
	DumpPorts();
}

void					WiredLADSPAInstance::AddGuiControl(t_ladspa_port *PortData)
{
	t_gui_port			NewGuiPort;

	NewGuiPort.LowerBound = PortData->RangeHint.LowerBound;
	NewGuiPort.UpperBound = PortData->RangeHint.UpperBound;
	NewGuiPort.Data = new LADSPA_Data;
	*(NewGuiPort.Data) = GetDefaultValue(&NewGuiPort, PortData->RangeHint.HintDescriptor);

	_GuiControls[PortData->Id] = NewGuiPort;
}

LADSPA_Data				WiredLADSPAInstance::GetDefaultValue(t_gui_port *GuiPort, LADSPA_PortRangeHintDescriptor Descriptor)
{
	if (LADSPA_IS_HINT_HAS_DEFAULT(Descriptor))
	{
		if(LADSPA_IS_HINT_DEFAULT_MINIMUM(Descriptor))
			return GuiPort->LowerBound;
		else if (LADSPA_IS_HINT_DEFAULT_LOW(Descriptor))
		{
			if (LADSPA_IS_HINT_LOGARITHMIC(Descriptor))
				return (LADSPA_Data) expf(logf(GuiPort->LowerBound) * 0.75 + logf(GuiPort->UpperBound) * 0.25);
			else
				return (LADSPA_Data) GuiPort->LowerBound * 0.75 + GuiPort->UpperBound * 0.25;
		}
		else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(Descriptor))
		{
			if (LADSPA_IS_HINT_LOGARITHMIC(Descriptor))
				return (LADSPA_Data) expf(logf(GuiPort->LowerBound) * 0.5 + logf(GuiPort->UpperBound) * 0.5);
			else
				return (LADSPA_Data) GuiPort->LowerBound * 0.5 + GuiPort->UpperBound * 0.5;
		}
		else if (LADSPA_IS_HINT_DEFAULT_HIGH(Descriptor))
		{
			if (LADSPA_IS_HINT_LOGARITHMIC(Descriptor))
				return (LADSPA_Data) expf(logf(GuiPort->LowerBound) * 0.25 + logf(GuiPort->UpperBound) * 0.75);
			else
				return (LADSPA_Data) GuiPort->LowerBound * 0.25 + GuiPort->UpperBound * 0.75;
		}
		else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(Descriptor))
			return GuiPort->UpperBound;
		else if (LADSPA_IS_HINT_DEFAULT_0(Descriptor))
			return 0.0f;
		else if (LADSPA_IS_HINT_DEFAULT_1(Descriptor))
			return 1.0f;
		else if (LADSPA_IS_HINT_DEFAULT_100(Descriptor))
			return 100.0f;
		else if (LADSPA_IS_HINT_DEFAULT_440(Descriptor))
			return 440.0f;
	}
	return GuiPort->LowerBound;
}

void					WiredLADSPAInstance::DumpPorts()
{
	list<t_ladspa_port>::iterator	Iter;
	
	cout << "############## Begin of Ports Dump ##############" << endl;
	cout << "Input(s) audio" << endl;
	for (Iter = _InputAudioPluginsPorts.begin(); Iter != _InputAudioPluginsPorts.end(); Iter++)
		cout << "Input Named {" << Iter->Name.c_str() << "}" << endl;
	cout << "Input(s) data" << endl;
	for (Iter = _InputDataPluginsPorts.begin(); Iter != _InputDataPluginsPorts.end(); Iter++)
		cout << "Input Named {" << Iter->Name.c_str() << "}" << endl;
	cout << "Output(s) audio" << endl;
	for (Iter = _OutputAudioPluginsPorts.begin(); Iter != _OutputAudioPluginsPorts.end(); Iter++)
		cout << "Output Named {" << Iter->Name.c_str() << "}" << endl;
	cout << "Output(s) data" << endl;
	for (Iter = _OutputDataPluginsPorts.begin(); Iter != _OutputDataPluginsPorts.end(); Iter++)
		cout << "Output Named {" << Iter->Name.c_str() << "}" << endl;
	cout << "############## End of Ports Dump ##############" << endl;
}

bool					WiredLADSPAInstance::IsLoaded()
{
	if (_Handle && _Descriptor)
		return true;
	return false;
}

void	 				WiredLADSPAInstance::Process(float **input, float **output, long sample_length)
{
	if (IsLoaded() == false)
		return;
	if (_InputAudioPluginsPorts.empty() || _OutputAudioPluginsPorts.empty())
		return;
	if (LADSPA_IS_INPLACE_BROKEN(_Properties) && *input == *output)
		return;
	ChangeActivateState();
	if (_InputAudioPluginsPorts.size() >= 2 && _OutputAudioPluginsPorts.size() >= 2)
		ProcessStereo(input, output, sample_length);
	else
		ProcessMono(input, output, sample_length);
}

void					WiredLADSPAInstance::ConnectMonoInput(float *input, unsigned long PortId)
{
	if (IsLoaded())
		_Descriptor->connect_port(_Handle, PortId, input);
}

void					WiredLADSPAInstance::ConnectMonoOutput(float *output, unsigned long PortId)
{
	if (IsLoaded())
		_Descriptor->connect_port(_Handle, PortId, output);
}

void					WiredLADSPAInstance::ProcessStereo(float **input, float **output, long sample_length)
{
	if (IsLoaded() && _InputAudioPluginsPorts.size() >= 2 && _OutputAudioPluginsPorts.size() >= 2)
	{
		ConnectMonoInput(input[0], GetPortId(_InputAudioPluginsPorts, 0));
		ConnectMonoOutput(output[0], GetPortId(_OutputAudioPluginsPorts, 0));
		_Descriptor->run(_Handle, (unsigned long) sample_length);
		ConnectMonoInput(input[1], GetPortId(_InputAudioPluginsPorts, 1));
		ConnectMonoOutput(output[1], GetPortId(_OutputAudioPluginsPorts, 1));
		_Descriptor->run(_Handle, (unsigned long) sample_length);
	}
}

void					WiredLADSPAInstance::ProcessMono(float **input, float **output, long sample_length)
{
	if (IsLoaded() && _InputAudioPluginsPorts.size() >= 1 && _OutputAudioPluginsPorts.size() >= 1)
	{
		ConnectMonoInput(input[0], GetPortId(_InputAudioPluginsPorts, 0));
		ConnectMonoOutput(output[0], GetPortId(_OutputAudioPluginsPorts, 0));
		_Descriptor->run(_Handle, (unsigned long) sample_length);
		ConnectMonoInput(input[1], GetPortId(_InputAudioPluginsPorts, 0));
		ConnectMonoOutput(output[1], GetPortId(_OutputAudioPluginsPorts, 0));
		_Descriptor->run(_Handle, (unsigned long) sample_length);
	}
}

void					WiredLADSPAInstance::Init()
{
	
}

void					WiredLADSPAInstance::Play()
{
	
}

void					WiredLADSPAInstance::Stop()
{
	
}

unsigned long			WiredLADSPAInstance::GetPortId(list<t_ladspa_port>& PortList, int index)
{
	list<t_ladspa_port>::iterator	Iter;
	unsigned long		Result = 0;
	int					pos;
	
	for (Iter = PortList.begin(), pos = 0; Iter != PortList.end(); Iter++, pos++)
		if (pos == index)
		{
			Result = Iter->Id;
			break;
		}
	return Result;
}

void					WiredLADSPAInstance::Load(WiredPluginData& Datas)
{
	string				Buffer;
	list<t_ladspa_port>::iterator	Iter;
	t_gui_port			NewData;
	
	for (Iter = _InputDataPluginsPorts.begin(); Iter != _InputDataPluginsPorts.end(); Iter++)
	{
		if (_GuiControls.find(Iter->Id) != _GuiControls.end())
		{
			Buffer = Datas.LoadValue(Iter->Name);
			if (Buffer.size() > 0 &&  NewData.Data != NULL)
				*(NewData.Data) = (LADSPA_Data)strtof(Buffer.c_str(), NULL);
		}
	}
}

void					WiredLADSPAInstance::Save(WiredPluginData& Datas)
{
	ostringstream					Buffer;
	list<t_ladspa_port>::iterator	Iter;
	
	for (Iter = _InputDataPluginsPorts.begin(); Iter != _InputDataPluginsPorts.end(); Iter++)
	{
		if (_GuiControls.find(Iter->Id) != _GuiControls.end())
		{
			Buffer << *(_GuiControls[Iter->Id].Data);
			Datas.SaveValue(Iter->Name, Buffer.str());
			Buffer.clear();	
		}
	}
}

void					WiredLADSPAInstance::SetBufferSize(long size)
{
	
}

void					WiredLADSPAInstance::SetSamplingRate(double rate)
{
	
}

void			  		WiredLADSPAInstance::SetBPM(float bpm)
{
	
}

void					WiredLADSPAInstance::SetSignature(int numerator, int denominator)
{
	
}
	
void					WiredLADSPAInstance::ProcessEvent(WiredEvent &event)
{
	
}

bool					WiredLADSPAInstance::HasView()
{
	
}

wxWindow*				WiredLADSPAInstance::CreateView(wxWindow *zone, wxPoint &pos, wxSize &size)
{
	
}

void					WiredLADSPAInstance::DestroyView()
{
	
}

bool					WiredLADSPAInstance::IsAudio()
{
	if (_Type & TYPE_PLUGINS_EFFECT)
		return true;
	return false;
}

bool					WiredLADSPAInstance::IsMidi()
{
	if (_Type & TYPE_PLUGINS_INSTR)
		return true;
	return false;
}

void					WiredLADSPAInstance::AskUpdate()
{
	
}

void					WiredLADSPAInstance::Update()
{
	
}

std::string				WiredLADSPAInstance::GetHelpString()
{
	return STR_DEFAULT_HELP;
}

void					WiredLADSPAInstance::SetHelpMode(bool On)
{
	
}

std::string				WiredLADSPAInstance::DefaultName()
{
	if (IsLoaded())
		return _Descriptor->Name;
	return STR_DEFAULT_NAME;
}

wxBitmap*				WiredLADSPAInstance::GetBitmap()
{
	
}

WiredDSSIPlugin::WiredDSSIPlugin()
{
	_Handle = NULL;
	_DSSIDescriptorFunction = NULL;
	_LADSPADescriptorFunction = NULL;
	_FileName = "";
}

WiredDSSIPlugin::WiredDSSIPlugin(const WiredDSSIPlugin& copy)
{
	*this= copy;
}

WiredDSSIPlugin::~WiredDSSIPlugin()
{
	//UnLoad();
}

WiredDSSIPlugin		WiredDSSIPlugin::operator=(const WiredDSSIPlugin& right)
{
	if (this != &right)
	{
		_Handle = right._Handle;
		_FileName = right._FileName;
		_DSSIDescriptorFunction = right._DSSIDescriptorFunction;
		_DSSIDescriptors = right._DSSIDescriptors;
		_LADSPADescriptorFunction = right._LADSPADescriptorFunction;
		_LADSPADescriptors = right._LADSPADescriptors;
		_PluginsInfo = right._PluginsInfo;
	}
	return *this;
}

bool				WiredDSSIPlugin::Load(const string& FileName, int& FirstIndex)
{
	bool			Found = false;
	
	_Handle = dlopen(FileName.c_str(), RTLD_NOW);
	if (_Handle != NULL)
	{
		_DSSIDescriptorFunction = (DSSI_Descriptor_Function) dlsym(_Handle, STR_DSSI_DESCRIPTOR_FUNCTION_NAME);
		if (_DSSIDescriptorFunction == NULL)
		{
			_LADSPADescriptorFunction = (LADSPA_Descriptor_Function) dlsym(_Handle, STR_LADSPA_DESCRIPTOR_FUNCTION_NAME);
			if (_LADSPADescriptorFunction == NULL)
				UnLoad();
			else
				Found = true;
		}
		else
			Found = true;
	}
	if (Found == true)
	{
		int		pos, PortPos, PluginInfo;
		LADSPA_PortDescriptor Pod;
		
		_FileName = FileName;
		if (_DSSIDescriptorFunction)
		{
			const DSSI_Descriptor		*CurrentDescriptor;
			
			for (pos = 0, PluginInfo = 0; (CurrentDescriptor = _DSSIDescriptorFunction(pos)); pos++, PluginInfo = 0)
			{
				cout << "Adding DSSI Plugin {" << CurrentDescriptor->LADSPA_Plugin->Name << "}" << endl;
				_DSSIDescriptors[FirstIndex] = CurrentDescriptor;
				PluginInfo |= TYPE_PLUGINS_DSSI;
				
				for (PortPos = 0; PortPos < CurrentDescriptor->LADSPA_Plugin->PortCount; PortPos++)
				{
					Pod = CurrentDescriptor->LADSPA_Plugin->PortDescriptors[PortPos];
					if (LADSPA_IS_PORT_AUDIO(Pod) && LADSPA_IS_PORT_INPUT(Pod))
					{
						PluginInfo |= TYPE_PLUGINS_EFFECT;
						break;
					}
				}
				if (!(PluginInfo & TYPE_PLUGINS_EFFECT))
					PluginInfo |= TYPE_PLUGINS_INSTR;
				_PluginsInfo[FirstIndex++] = PluginInfo;
			}
		}
		else if (_LADSPADescriptorFunction)
		{
			const LADSPA_Descriptor		*CurrentDescriptor;
			
			for (pos = 0, PluginInfo = 0; (CurrentDescriptor = _LADSPADescriptorFunction(pos)); pos++, PluginInfo = 0)
			{				
				cout << "Adding LADSPA Plugin {" << CurrentDescriptor->Name << "}" << endl;
				_LADSPADescriptors[FirstIndex] = CurrentDescriptor;
				PluginInfo |= TYPE_PLUGINS_LADSPA;
				for (PortPos = 0; PortPos < CurrentDescriptor->PortCount; PortPos++)
				{
					Pod = CurrentDescriptor->PortDescriptors[PortPos];;
					if (LADSPA_IS_PORT_AUDIO(Pod) && LADSPA_IS_PORT_INPUT(Pod))
					{
						PluginInfo |= TYPE_PLUGINS_EFFECT;
						break;
					}
				}
				if (!(PluginInfo & TYPE_PLUGINS_EFFECT))
					PluginInfo |= TYPE_PLUGINS_INSTR;
				_PluginsInfo[FirstIndex++] = PluginInfo;
			}
		}	
	}
	return Found;
}

map<int, string>	WiredDSSIPlugin::GetPluginsList()
{
	map<int, string>	Result;
	
	if (_LADSPADescriptorFunction)
	{
		map<int, const LADSPA_Descriptor*>::iterator	Iter;
		
		for (Iter = _LADSPADescriptors.begin(); Iter != _LADSPADescriptors.end(); Iter++)
			Result[Iter->first] = Iter->second->Name;
	}
	else if (_DSSIDescriptorFunction)
	{
		map<int, const DSSI_Descriptor*>::iterator	Iter;
		
		for (Iter = _DSSIDescriptors.begin(); Iter != _DSSIDescriptors.end(); Iter++)
			Result[Iter->first] = Iter->second->LADSPA_Plugin->Name;
	}
	return Result;
}

void				WiredDSSIPlugin::UnLoad()
{
	if (_Handle != NULL)
	{
		dlclose(_Handle);
		_Handle = NULL;
	}
}

int					WiredDSSIPlugin::GetPluginType(int PluginId)
{
	if (_PluginsInfo.find(PluginId) != _PluginsInfo.end())
		return _PluginsInfo[PluginId];
	return 0;
}

bool				WiredDSSIPlugin::Contains(int PluginId)
{
	if (_LADSPADescriptorFunction)
	{
		if (_LADSPADescriptors.find(PluginId) != _LADSPADescriptors.end())
			return true;
	}
	else if (_DSSIDescriptorFunction)
	{
		if (_DSSIDescriptors.find(PluginId) != _DSSIDescriptors.end())
			return true;
	}
	return false;	
}

bool				WiredDSSIPlugin::CreatePlugin(int PluginId, WiredLADSPAInstance* Plugin)
{	
	if (Contains(PluginId) == false || Plugin == NULL)
		return false;

	const LADSPA_Descriptor	*Descriptor;

	if (_LADSPADescriptorFunction)
		Descriptor = _LADSPADescriptors.find(PluginId)->second;
	else if (_DSSIDescriptorFunction)
		Descriptor = _DSSIDescriptors.find(PluginId)->second->LADSPA_Plugin;
	cout << "Creating Rack for Plugin " << Descriptor->Name << endl;
	Plugin->Init(Descriptor);
	cout << "Rack Created, loading" << endl;
	return Plugin->Load();
	return false;
}

bool				WiredDSSIPlugin::Contains(unsigned long PluginUniqueId)
{
	bool			Result = false;
	
	if (_DSSIDescriptorFunction)
	{
		map<int, const DSSI_Descriptor*>::iterator	Iter;

		for (Iter = _DSSIDescriptors.begin(); Iter != _DSSIDescriptors.end(); Iter++)
			if (Iter->second->LADSPA_Plugin->UniqueID == PluginUniqueId)
			{
				Result = true;
				break;
			}
	}
	else if (_LADSPADescriptorFunction)
	{
		map<int, const LADSPA_Descriptor*>::iterator	Iter;

		for (Iter = _LADSPADescriptors.begin(); Iter != _LADSPADescriptors.end(); Iter++)
			if (Iter->second->UniqueID == PluginUniqueId)
			{
				Result = true;
				break;
			}
	}
	return Result;
}

WiredDSSI::WiredDSSI()
{
	_CurrentPluginIndex = 0;
}

WiredDSSI::WiredDSSI(const WiredDSSI& copy)
{
	*this= copy;
}

WiredDSSI::~WiredDSSI()
{
	list<WiredLADSPAInstance*>::iterator	IterLoaded;
	WiredLADSPAInstance*					CurrentLoadedPlugin;

	for (IterLoaded = _LoadedPlugins.begin(); IterLoaded != _LoadedPlugins.end(); IterLoaded++)
	{
		CurrentLoadedPlugin = *IterLoaded;
		delete CurrentLoadedPlugin;
	}
	
	list<WiredDSSIPlugin*>::iterator	Iter;
	WiredDSSIPlugin*					CurrentPlugin;
	
	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		(*Iter)->UnLoad();
		CurrentPlugin = *Iter;
		delete CurrentPlugin;
	}
}

WiredDSSI		WiredDSSI::operator=(const WiredDSSI& right)
{
	if (this != &right)
	{
		_Plugins = right._Plugins;
		_CurrentPluginIndex = right._CurrentPluginIndex;
		_IdTable = right._IdTable;
		_LoadedPlugins = right._LoadedPlugins;
	}
	return *this;
}

void			WiredDSSI::LoadPLugins(int Type)
{
	char			*Dirs = NULL;
	
	if (Type & TYPE_PLUGINS_DSSI)
	{
		if ((Dirs = getenv(ENV_NAME_PLUGINS_DSSI)) != NULL)
			LoadPluginsFromPath(Dirs, TYPE_PLUGINS_DSSI);
		else
			LoadPluginsFromPath(DEFAULT_DSSI_PATH, TYPE_PLUGINS_DSSI);
	}
	if (Type & TYPE_PLUGINS_LADSPA)
	{
		if ((Dirs = getenv(ENV_NAME_PLUGINS_LADSPA)) != NULL)
			LoadPluginsFromPath(Dirs, TYPE_PLUGINS_LADSPA);
		else
			LoadPluginsFromPath(DEFAULT_LADSPA_PATH, TYPE_PLUGINS_LADSPA);
	}
}

void			WiredDSSI::LoadPluginsFromPath(const char *Dirs, int Type)
{
	list <string>::iterator 	Iter;
	DIR 						*CurrentDir = NULL;
	struct dirent 				*CurrentFile = NULL;
	string						DirsFromEnv = string(Dirs);
	list<string>				Paths = SplitPath(DirsFromEnv);
	
	for (Iter = Paths.begin(); Iter != Paths.end(); Iter++)
	{
		if ((CurrentDir = opendir((*Iter).c_str())) != NULL)
		{
			while ((CurrentFile = readdir(CurrentDir)))
			{
				//TODO bring back LADSPA / DSSI Load.
				if (Type == TYPE_PLUGINS_DSSI)
					LoadPlugins(string(*Iter) + string("/") + string(CurrentFile->d_name));
				else if (Type == TYPE_PLUGINS_LADSPA)
					LoadPlugins(string(*Iter) + string("/") + string(CurrentFile->d_name));
			}	
			closedir(CurrentDir);
		}
	}
}

void			WiredDSSI::LoadPlugins(const string& FileName)
{
	//cout << "Trying to open file named {" << FileName.c_str() << "}" << endl;
	WiredDSSIPlugin		*NewPlugin = new WiredDSSIPlugin();
	
	if (NewPlugin->Load(FileName, _CurrentPluginIndex))
		_Plugins.insert(_Plugins.end(), NewPlugin);
	//else
		//cout << "Could not open file" << endl;
}

list<string>	WiredDSSI::SplitPath(string& Path)
{
	istringstream f(Path.c_str());
	list<string> Result;
	string		Buffer;

	if (Path.find(ENV_PATH_SEPARATOR) == Path.npos)
		Result.insert(Result.end(), Path);
	else
		while (getline(f, Buffer, ENV_PATH_SEPARATOR))
			Result.insert(Result.end(), Buffer);
	return Result;
}

map<int, string>	WiredDSSI::GetPluginsList()
{
	map<int, string>					Result;
	list<WiredDSSIPlugin*>::iterator	Iter;
	map<int, string>::iterator			IterDescriptor;
	map<int, string>					CurrentPluginList;

	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		CurrentPluginList = (*Iter)->GetPluginsList();
		for (IterDescriptor = CurrentPluginList.begin(); IterDescriptor != CurrentPluginList.end(); IterDescriptor++)
			Result[IterDescriptor->first] = IterDescriptor->second;
	}
	return Result;
}

void				WiredDSSI::SetMenuItemId(int ModuleId, int MenuItemId)
{
	_IdTable[MenuItemId] = ModuleId;
}

int					WiredDSSI::GetPluginType(int PluginId)
{
	list<WiredDSSIPlugin*>::iterator	Iter;
	int									Result = 0;

	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		if ((Result = (*Iter)->GetPluginType(PluginId)))
			break;
	}
	return Result;
}

void				WiredDSSI::CreatePlugin(int MenuItemId)
{
	list<WiredDSSIPlugin*>::iterator	Iter;
	int									IdPlugin = 0;
	
	if (_IdTable.find(MenuItemId) == _IdTable.end())
		return;
	IdPlugin = _IdTable.find(MenuItemId)->second;
	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		if ((*Iter)->Contains(IdPlugin))
		{
			WiredLADSPAInstance		*NewPlugin = new WiredLADSPAInstance();
			
			if ((*Iter)->CreatePlugin(IdPlugin, NewPlugin))
			{
				cout << "Plugin successfully loaded" << endl;
				_LoadedPlugins.insert(_LoadedPlugins.end(), NewPlugin);
			}
			else
			{
				cout << "Cannot load the Plugin" << endl;
				delete NewPlugin;
			}
			break;
		}
	}	
}
