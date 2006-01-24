#ifndef _WIREDEXTERNALPLUGIN_H_
#define _WIREDEXTERNALPLUGIN_H_

#include "dssi.h"
#include "../engine/AudioEngine.h"
#include "WiredExternalPluginLADSPA.h"

#include <map>
using namespace std;

#define STR_DSSI_DESCRIPTOR_FUNCTION_NAME "dssi_descriptor"
#define STR_LADSPA_DESCRIPTOR_FUNCTION_NAME "ladspa_descriptor"

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
	bool				Contains(unsigned long PluginUniqueId);
	map<int, unsigned long>	GetPluginsListUniqueId();

private:
	string								_FileName;
	void								*_Handle;
	DSSI_Descriptor_Function			_DSSIDescriptorFunction;
	map<int, const DSSI_Descriptor*>	_DSSIDescriptors;						//Key == PluginId; Value == PluginDescriptor
	LADSPA_Descriptor_Function			_LADSPADescriptorFunction;
	map<int, const LADSPA_Descriptor*>	_LADSPADescriptors;						//Key == PluginId; Value == PluginDescriptor
	map<int, int>						_PluginsInfo;
};


#endif //_WIREDEXTERNALPLUGIN_H_
