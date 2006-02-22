#ifndef _WIREDEXTERNALPLUGINMGR_H_
#define _WIREDEXTERNALPLUGINMGR_H_

#include <stdlib.h>
#include <sys/types.h>
//#include <dirent.h>
#include <wx/dir.h>

#include "WiredExternalPluginGui.h"
#include "WiredExternalPlugin.h"

#include <list>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

#define	TYPE_PLUGINS_DSSI 1
#define	TYPE_PLUGINS_LADSPA 2

#define	TYPE_PLUGINS_INSTR 4
#define	TYPE_PLUGINS_EFFECT 8

#define ENV_NAME_PLUGINS_DSSI "DSSI_PATH"
#define ENV_NAME_PLUGINS_LADSPA "LADSPA_PATH"
#define DEFAULT_DSSI_PATH "/usr/lib/dssi:/usr/local/lib/dssi"
#define DEFAULT_LADSPA_PATH "/usr/lib/ladspa:/usr/local/lib/ladspa"
#define ENV_PATH_SEPARATOR ':'
#define STR_DSSI_DESCRIPTOR_FUNCTION_NAME "dssi_descriptor"
#define STR_LADSPA_DESCRIPTOR_FUNCTION_NAME "ladspa_descriptor"
#define STR_DEFAULT_NAME "LADSPA Plugin"
#define STR_DEFAULT_HELP "No help provided by this plugin"


class 	WiredExternalPluginMgr
{
public:
	WiredExternalPluginMgr();
	~WiredExternalPluginMgr();
	WiredExternalPluginMgr(const WiredExternalPluginMgr& copy);
	WiredExternalPluginMgr		operator=(const WiredExternalPluginMgr& right);
	
	void				LoadPLugins(int Type);
	map<int, string>	GetPluginsList();
	list<string>		GetSortedPluginsList(const string& Separator = "#");
	void				SetMenuItemId(int ModuleId, int MenuItemId);
	int					GetPluginType(int PluginId);
	WiredDSSIGui		*CreatePlugin(int MenuItemId, PlugStartInfo &info);
	WiredDSSIGui		*CreatePlugin(unsigned long UniqueId);
	void				DestroyPlugin(WiredDSSIGui *Plug);
	void				SetStartInfo(PlugStartInfo &Info);
	
private:
	void			LoadPlugins(const string& FileName);
	list<string>	SplitPath(string& Path);
	void			LoadPluginsFromPath(const char *Dirs, int Type);

	list<WiredDSSIPlugin*>		_Plugins;
	list<WiredDSSIGui*>	_LoadedPlugins;
	map<int, int>			_IdTable;									//Key == MenuItemId; Value == PluginId (auto-increment)
	int						_CurrentPluginIndex;
	map<unsigned long, int>			_UniqueIdTable;								//Key == Plugin unique ID; Value == PluginId
	PlugStartInfo			_StartInfo;
};



#endif //_WIREDEXTERNALPLUGINMGR_H_
