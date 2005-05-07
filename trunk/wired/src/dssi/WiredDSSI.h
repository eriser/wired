#ifndef __WIREDDSSI_H__
#define __WIREDDSSI_H__

#include "dssi.h"
#include "ladspa.h"
#include "../engine/AudioEngine.h"
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>

#include <list>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#define	TYPE_PLUGINS_DSSI 1
#define	TYPE_PLUGINS_LADSPA 2

#define	TYPE_PLUGINS_INSTR 4
#define	TYPE_PLUGINS_EFFECT 8

#define ENV_NAME_PLUGINS_DSSI "DSSI_PATH"
#define ENV_NAME_PLUGINS_LADSPA "LADSPA_PATH"
#define ENV_PATH_SEPARATOR ':'
#define STR_DSSI_DESCRIPTOR_FUNCTION_NAME "dssi_descriptor"
#define STR_LADSPA_DESCRIPTOR_FUNCTION_NAME "ladspa_descriptor"

class	WiredLADSPAInstance
{
public:
	WiredLADSPAInstance();
	~WiredLADSPAInstance();
	WiredLADSPAInstance(const WiredLADSPAInstance& copy);
	WiredLADSPAInstance	operator=(const WiredLADSPAInstance& right);
	bool				Init(const LADSPA_Descriptor* Descriptor);
	bool				Load();
	bool				ChangeActivateState(bool Activate = true);
private:
	void				UnLoad();
	
	LADSPA_Handle		_Handle;
	LADSPA_Descriptor	*_Descriptor;
};

class	WiredDSSIPlugin
{
public:
	WiredDSSIPlugin();
	~WiredDSSIPlugin();
	WiredDSSIPlugin(const WiredDSSIPlugin& copy);
	WiredDSSIPlugin		operator=(const WiredDSSIPlugin& right);
	bool				Load(const string& FileName, int& FirstIndex);
	void				UnLoad();
	map<int, string>	GetPluginsList();
	int					GetPluginType(int PluginId);
	bool				Contains(int PluginId);
	bool				CreatePlugin(int PluginId, WiredLADSPAInstance *Plugin);

private:
	string								_FileName;
	void								*_Handle;
	DSSI_Descriptor_Function			_DSSIDescriptorFunction;
	map<int, const DSSI_Descriptor*>	_DSSIDescriptors;
	LADSPA_Descriptor_Function			_LADSPADescriptorFunction;
	map<int, const LADSPA_Descriptor*>	_LADSPADescriptors;
	map<int, int>						_PluginsInfo;
};

class 	WiredDSSI
{
public:
	WiredDSSI();
	~WiredDSSI();
	WiredDSSI(const WiredDSSI& copy);
	WiredDSSI		operator=(const WiredDSSI& right);
	
	void				LoadPLugins(int Type);
	map<int, string>	GetPluginsList();
	void				SetMenuItemId(int ModuleId, int MenuItemId);
	int					GetPluginType(int PluginId);
	void				CreatePlugin(int MenuItemId);
	
private:
	void			LoadPlugins(const string& FileName);
	list<string>	SplitPath(string& Path);
	void			LoadPluginsFromPath(const char *Dirs, int Type);

	list<WiredDSSIPlugin*>		_Plugins;
	list<WiredLADSPAInstance*>	_LoadedPlugins;
	map<int, int>			_IdTable;
	int						_CurrentPluginIndex;
};

#endif	//__WIREDDSSI_H__
