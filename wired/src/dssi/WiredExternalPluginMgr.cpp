#include "WiredExternalPluginMgr.h"

WiredExternalPluginMgr::WiredExternalPluginMgr()
{
	_CurrentPluginIndex = 0;
}

WiredExternalPluginMgr::WiredExternalPluginMgr(const WiredExternalPluginMgr& copy)
{
	*this= copy;
}

WiredExternalPluginMgr::~WiredExternalPluginMgr()
{
	list<WiredDSSIGui*>::iterator	IterLoaded;
	WiredLADSPAInstance*					CurrentLoadedPlugin;

	for (IterLoaded = _LoadedPlugins.begin(); IterLoaded != _LoadedPlugins.end(); IterLoaded++)
	{
		cout << "before  delete  LADSPA" << endl;
		CurrentLoadedPlugin = *IterLoaded;
		delete CurrentLoadedPlugin;
		cout << "after  delete  LADSPA" << endl;
	}
	
	list<WiredDSSIPlugin*>::iterator	Iter;
	WiredDSSIPlugin*					CurrentPlugin;
	cout << "before  delete  DSSI" << endl;
	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		(*Iter)->UnLoad();
		CurrentPlugin = *Iter;
		delete CurrentPlugin;
	}
}

WiredExternalPluginMgr		WiredExternalPluginMgr::operator=(const WiredExternalPluginMgr& right)
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

void			WiredExternalPluginMgr::LoadPLugins(int Type)
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

void			WiredExternalPluginMgr::LoadPluginsFromPath(const char *Dirs, int Type)
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

void			WiredExternalPluginMgr::LoadPlugins(const string& FileName)
{
	WiredDSSIPlugin		*NewPlugin = new WiredDSSIPlugin();
	
	if (NewPlugin->Load(FileName, _CurrentPluginIndex))
		_Plugins.insert(_Plugins.end(), NewPlugin);
	else
		delete NewPlugin;
}

list<string>	WiredExternalPluginMgr::SplitPath(string& Path)
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

map<int, string>	WiredExternalPluginMgr::GetPluginsList()
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

void				WiredExternalPluginMgr::SetMenuItemId(int ModuleId, int MenuItemId)
{
	_IdTable[MenuItemId] = ModuleId;
}

int					WiredExternalPluginMgr::GetPluginType(int PluginId)
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

WiredDSSIGui		*WiredExternalPluginMgr::CreatePlugin(int MenuItemId)
{
	list<WiredDSSIPlugin*>::iterator	Iter;
	int									IdPlugin = 0;
	
	if (_IdTable.find(MenuItemId) == _IdTable.end())
		return NULL;
	IdPlugin = _IdTable.find(MenuItemId)->second;
	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		if ((*Iter)->Contains(IdPlugin))
		{
			WiredDSSIGui		*NewPlugin = new WiredDSSIGui();
			
			if ((*Iter)->CreatePlugin(IdPlugin, NewPlugin))
			{
				NewPlugin->Load();
				cout << "Plugin successfully loaded" << endl;
				_LoadedPlugins.insert(_LoadedPlugins.end(), NewPlugin);
				return NewPlugin;
			}
			else
			{
				cout << "Cannot load the Plugin" << endl;
				delete NewPlugin;
			}
			break;
		}
	}	
	return NULL;
}

void				WiredExternalPluginMgr::DestroyPlugin(WiredDSSIGui *Plug)
{
	list<WiredDSSIGui*>::iterator	Iter;
	
	for (Iter = _LoadedPlugins.begin(); Iter != _LoadedPlugins.end(); Iter++)
	{
		if (*Iter == Plug)
		{
			_LoadedPlugins.remove(Plug);
			delete Plug;
			break;	
		}
	}
	cout << "Can't find plugin to unload" << endl;
}
