#include "WiredDSSI.h"

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

bool				WiredDSSIPlugin::CreatePlugin(int PluginId)
{	
	if (Contains(PluginId) == false)
		return false;
	if (_LADSPADescriptorFunction)
	{
		map<int, const LADSPA_Descriptor*>::iterator	Iter = _LADSPADescriptors.find(PluginId);
		
		cout << "Creating Rack for Plugin " << Iter->second->Name << endl;
	}
	else if (_DSSIDescriptorFunction)
	{
		map<int, const DSSI_Descriptor*>::iterator	Iter = _DSSIDescriptors.find(PluginId);
		
		cout << "Creating Rack for Plugin " << Iter->second->LADSPA_Plugin->Name << endl;
	}
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
	}
	return *this;
}

void			WiredDSSI::LoadPLugins(int Type)
{
	char			*Dirs = NULL;
	
	if (Type & TYPE_PLUGINS_DSSI)
		if ((Dirs = getenv(ENV_NAME_PLUGINS_DSSI)) != NULL)
			LoadPluginsFromPath(Dirs, TYPE_PLUGINS_DSSI);
	if (Type & TYPE_PLUGINS_LADSPA)
		if ((Dirs = getenv(ENV_NAME_PLUGINS_LADSPA)) != NULL)
			LoadPluginsFromPath(Dirs, TYPE_PLUGINS_LADSPA);
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
	ifstream f(Path.c_str());
	list<string> Result;
	string		Buffer;

	if (Path.find(ENV_PATH_SEPARATOR) == Path.npos)
		Result.insert(Result.end(), Path);
	else
		while (getline(f, Buffer, ENV_PATH_SEPARATOR))
			Result.insert(Result.end(), Buffer);
	f.close();
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
			(*Iter)->CreatePlugin(IdPlugin);
			break;
		}
	}	
}
