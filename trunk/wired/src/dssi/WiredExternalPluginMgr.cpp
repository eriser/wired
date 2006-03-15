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

	cout << "[DSSI] Deleting Loaded plugins ...";
	for (IterLoaded = _LoadedPlugins.begin(); IterLoaded != _LoadedPlugins.end(); IterLoaded++)
	{
		CurrentLoadedPlugin = *IterLoaded;
		delete CurrentLoadedPlugin;
	}
	cout << "...done" << endl;
	list<WiredDSSIPlugin*>::iterator	Iter;
	WiredDSSIPlugin*					CurrentPlugin;
	cout << "[DSSI] Deleting found plugins ...";
	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		(*Iter)->UnLoad();
		CurrentPlugin = *Iter;
		delete CurrentPlugin;
	}
	cout << "...done" << endl;
}

WiredExternalPluginMgr		WiredExternalPluginMgr::operator=(const WiredExternalPluginMgr& right)
{
	if (this != &right)
	{
		_Plugins = right._Plugins;
		_CurrentPluginIndex = right._CurrentPluginIndex;
		_IdTable = right._IdTable;
		_LoadedPlugins = right._LoadedPlugins;
		_UniqueIdTable = right._UniqueIdTable;
		_StartInfo = right._StartInfo;
	}
	return *this;
}

void			WiredExternalPluginMgr::LoadPLugins(int Type)
{
	char			*Dirs = NULL;
	wxLogNull		NoLog;
	
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
	list <wxString>::iterator 	Iter;
//	DIR 						*CurrentDir = NULL;
//	struct dirent 				*CurrentFile = NULL;
	wxString					DirsFromEnv = wxString(Dirs, *wxConvCurrent);
	list<wxString>				Paths = SplitPath(DirsFromEnv);
	
	for (Iter = Paths.begin(); Iter != Paths.end(); Iter++)
	{
		wxDir 					CurrentDir((*Iter));
		if (CurrentDir.IsOpened())
		{
			wxString filename;
		    bool cont = CurrentDir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
		    while (cont)
		    {
				//TODO bring back LADSPA / DSSI Load.
				if (Type == TYPE_PLUGINS_DSSI)
					LoadPlugins(wxString(*Iter) + wxString(wxT("/")) + (wxString)filename);
				else if (Type == TYPE_PLUGINS_LADSPA)
					LoadPlugins(wxString(*Iter) + wxString(wxT("/")) + (wxString)filename);
		        cont = CurrentDir.GetNext(&filename);
		    }
		}
	}
	//_Plugins.sort(SortByName<WiredLADSPAInstance>());
	//std::sort(_Plugins.begin(), _Plugins.end(), SortPluginsByName());
}

void			WiredExternalPluginMgr::LoadPlugins(const wxString& FileName)
{
	WiredDSSIPlugin		*NewPlugin = new WiredDSSIPlugin();
	
	if (NewPlugin->Load(FileName, _CurrentPluginIndex))
	{
		_Plugins.insert(_Plugins.end(), NewPlugin);
		map<int, unsigned long>::iterator		Iter;
		map<int, unsigned long>					ListUniqueID = NewPlugin->GetPluginsListUniqueId();

		for (Iter = ListUniqueID.begin(); Iter != ListUniqueID.end(); Iter++)
			_UniqueIdTable[Iter->second] = Iter->first;
	}
	else
		delete NewPlugin;
}

list<wxString>	WiredExternalPluginMgr::SplitPath(wxString& Path)
{
	istringstream f((const char *)Path.mb_str(*wxConvCurrent));
	list<wxString> Result;
	std::string		Buffer;

	if (Path.find(ENV_PATH_SEPARATOR) == Path.npos)
		Result.insert(Result.end(), Path);
	else
		while (getline(f, Buffer, ENV_PATH_SEPARATOR))
			Result.insert(Result.end(), wxString(Buffer.c_str(), *wxConvCurrent));
	return Result;
}

map<int, wxString>	WiredExternalPluginMgr::GetPluginsList()
{
	map<int, wxString>					Result;
	list<WiredDSSIPlugin*>::iterator	Iter;
	map<int, wxString>::iterator			IterDescriptor;
	map<int, wxString>					CurrentPluginList;

	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		CurrentPluginList = (*Iter)->GetPluginsList();
		for (IterDescriptor = CurrentPluginList.begin(); IterDescriptor != CurrentPluginList.end(); IterDescriptor++)
			Result[IterDescriptor->first] = IterDescriptor->second;
	}	
	return Result;
}

list<wxString>		WiredExternalPluginMgr::GetSortedPluginsList(const wxString& Separator)
{
	list<wxString>						Result;
	list<WiredDSSIPlugin*>::iterator	Iter;
	map<int, wxString>::iterator			IterDescriptor;
	map<int, wxString>					CurrentPluginList;
	wxChar								buf[1024];
	wxString								StrResult;

	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		CurrentPluginList = (*Iter)->GetPluginsList();
		for (IterDescriptor = CurrentPluginList.begin(); IterDescriptor != CurrentPluginList.end(); IterDescriptor++)
		{
			StrResult = wxString(IterDescriptor->second + Separator);
			wxSnprintf(buf, 1024, wxT("%d"), IterDescriptor->first);
			Result.insert(Result.end(), StrResult + wxString(buf, *wxConvCurrent));
		}
	}
	Result.sort();
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

WiredDSSIGui		*WiredExternalPluginMgr::CreatePlugin(int MenuItemId, PlugStartInfo &info)
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
			WiredDSSIGui		*NewPlugin = new WiredDSSIGui(info);
			
			if ((*Iter)->CreatePlugin(IdPlugin, NewPlugin))
			{
				NewPlugin->Load();
				cout << "[DSSI] Plugin successfully loaded" << endl;
				_LoadedPlugins.insert(_LoadedPlugins.end(), NewPlugin);
				return NewPlugin;
			}
			else
			{
				cout << "[DSSI] Cannot load the Plugin" << endl;
				delete NewPlugin;
			}
			break;
		}
	}	
	return NULL;
}

WiredDSSIGui		*WiredExternalPluginMgr::CreatePlugin(unsigned long UniqueId)
{
	list<WiredDSSIPlugin*>::iterator	Iter;
	int									IdPlugin = 0;
	
	if (_UniqueIdTable.find(UniqueId) == _UniqueIdTable.end())
		return NULL;
	IdPlugin = _UniqueIdTable.find(UniqueId)->second;
	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		if ((*Iter)->Contains(IdPlugin))
		{
			WiredDSSIGui		*NewPlugin = new WiredDSSIGui(_StartInfo);
			
			if ((*Iter)->CreatePlugin(IdPlugin, NewPlugin))
			{
				NewPlugin->Load();
				cout << "[DSSI] Plugin successfully loaded" << endl;
				_LoadedPlugins.insert(_LoadedPlugins.end(), NewPlugin);
				return NewPlugin;
			}
			else
			{
				cout << "[DSSI] Cannot load the Plugin" << endl;
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
	cout << "[DSSI] Can't find plugin to unload" << endl;
}

void				WiredExternalPluginMgr::SetStartInfo(PlugStartInfo &Info)
{
	_StartInfo = Info;
}
