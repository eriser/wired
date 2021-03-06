// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

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
  wxString		Dirs;
  wxLogNull		NoLog;

  if (Type & TYPE_PLUGINS_DSSI)
    {
      if (wxGetEnv(ENV_NAME_PLUGINS_DSSI, &Dirs))
	LoadPluginsFromPath(Dirs, TYPE_PLUGINS_DSSI);
      else
	LoadPluginsFromPath(DEFAULT_DSSI_PATH, TYPE_PLUGINS_DSSI);
    }
  if (Type & TYPE_PLUGINS_LADSPA)
    {
      if (wxGetEnv(ENV_NAME_PLUGINS_LADSPA, &Dirs))
	LoadPluginsFromPath(Dirs, TYPE_PLUGINS_LADSPA);
      else
	LoadPluginsFromPath(DEFAULT_LADSPA_PATH, TYPE_PLUGINS_LADSPA);
    }
}

void			WiredExternalPluginMgr::LoadPluginsFromPath(const wxString& Dirs, int Type)
{
	list<wxString>::iterator 	Iter;
	list<wxString>			Paths = SplitPath(Dirs);
	
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

list<wxString>	WiredExternalPluginMgr::SplitPath(const wxString& Path)
{
  int idx;
  list<wxString> Result;
  wxString		BufferB, BufferE;

  if (Path.Find(ENV_PATH_SEPARATOR) == -1)
    Result.insert(Result.end(), Path);
  else
    {
      BufferE = Path;
      idx = BufferE.Find(ENV_PATH_SEPARATOR);
      
      while(idx != -1)
	{
	  BufferB = BufferE.Mid(0, idx);
	  BufferE = BufferE.Mid(idx + 1);
	  Result.insert(Result.end(), BufferB);
	  idx = BufferE.find(ENV_PATH_SEPARATOR);
	}
      Result.insert(Result.end(), BufferE);
    }
  
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

WiredDSSIGui		*WiredExternalPluginMgr::CreatePluginFromMenu(int MenuItemId, PlugStartInfo &info)
{
  if (_IdTable.find(MenuItemId) == _IdTable.end())
    {
      cout << "[DSSI] Menu entry not found" << endl;
      return NULL;
    }
  return (CreatePlugin( _IdTable.find(MenuItemId)->second, info));
}

WiredDSSIGui		*WiredExternalPluginMgr::CreatePluginFromUniqueId(unsigned long UniqueId)
{
  if (_UniqueIdTable.find(UniqueId) == _UniqueIdTable.end())
    {
      cout << "[DSSI] Unique Id not found" << endl;
      return NULL;
    }
  return (CreatePlugin( _UniqueIdTable.find(UniqueId)->second, _StartInfo));
}

WiredDSSIGui*		WiredExternalPluginMgr::CreatePlugin(unsigned long IdPlugin, PlugStartInfo &info)
{
  list<WiredDSSIPlugin*>::iterator	Iter;
  int					intIdPlugin = (int)IdPlugin;

  for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
   {
      if ((*Iter)->Contains(intIdPlugin))
	{
	  WiredDSSIGui*	NewPlugin = new WiredDSSIGui(info);

	  if ((*Iter)->CreatePlugin(intIdPlugin, NewPlugin))
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
	  return NULL;
	}
    }
  return NULL;
}

void				WiredExternalPluginMgr::DestroyPlugin(WiredDSSIGui *Plug)
{
	list<WiredDSSIGui*>::iterator	Iter;
	
	if (!Plug)
	{
	  cerr << "[DSSI] DestroyPlugin called for no reason" << endl;
	  return;
	}
	if (_LoadedPlugins.empty())
	{
	  cerr << "[DSSI] DestroyPlugin : no plugins !!" << endl;
	  return;
	}
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
