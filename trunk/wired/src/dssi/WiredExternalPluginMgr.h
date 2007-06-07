// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _WIREDEXTERNALPLUGINMGR_H_
#define _WIREDEXTERNALPLUGINMGR_H_

#include <list>
#include <map>
#include <wx/string.h>
#include <wx/dir.h>
#include <wx/log.h>
#include "WiredExternalPluginGui.h"
#include "WiredExternalPlugin.h"

#define	TYPE_PLUGINS_DSSI 1
#define	TYPE_PLUGINS_LADSPA 2

#define	TYPE_PLUGINS_INSTR 4
#define	TYPE_PLUGINS_EFFECT 8

#define ENV_NAME_PLUGINS_DSSI (wxT("DSSI_PATH"))
#define ENV_NAME_PLUGINS_LADSPA (wxT("LADSPA_PATH"))
#define DEFAULT_DSSI_PATH (wxT("/usr/lib/dssi:/usr/local/lib/dssi"))
#define DEFAULT_LADSPA_PATH (wxT("/usr/lib/ladspa:/usr/local/lib/ladspa"))
#define ENV_PATH_SEPARATOR (':')

class 	WiredExternalPluginMgr
{
public:
	WiredExternalPluginMgr();
	~WiredExternalPluginMgr();
	WiredExternalPluginMgr(const WiredExternalPluginMgr& copy);
	WiredExternalPluginMgr		operator=(const WiredExternalPluginMgr& right);
	
	void			LoadPLugins(int Type);
	std::map<int, wxString>	GetPluginsList();
	std::list<wxString>		GetSortedPluginsList(const wxString& Separator = wxT("#"));
	void			SetMenuItemId(int ModuleId, int MenuItemId);
	int			GetPluginType(int PluginId);
	WiredDSSIGui		*CreatePluginFromMenu(int MenuItemId, PlugStartInfo &info);
	WiredDSSIGui		*CreatePluginFromUniqueId(unsigned long UniqueId);
	WiredDSSIGui		*CreatePluginFromIdPlugin(unsigned long IdPlugin, PlugStartInfo &info);
	WiredDSSIGui*		CreatePlugin(unsigned long IdPlugin, PlugStartInfo& info);

	void			DestroyPlugin(WiredDSSIGui *Plug);
	void			SetStartInfo(PlugStartInfo &Info);
	
private:
	void			LoadPlugins(const wxString& FileName);
	std::list<wxString>		SplitPath(const wxString& Path);
	void			LoadPluginsFromPath(const wxString& Dirs, int Type);

	std::list<WiredDSSIPlugin*>	_Plugins;
	std::list<WiredDSSIGui*>	_LoadedPlugins;
	std::map<int, int>		_IdTable;									//Key == MenuItemId; Value == PluginId (auto-increment)
	int			_CurrentPluginIndex;
	std::map<unsigned long, int>	_UniqueIdTable;								//Key == Plugin unique ID; Value == PluginId
	PlugStartInfo		_StartInfo;
};



#endif //_WIREDEXTERNALPLUGINMGR_H_
