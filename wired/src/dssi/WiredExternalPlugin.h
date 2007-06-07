// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _WIREDEXTERNALPLUGIN_H_
#define _WIREDEXTERNALPLUGIN_H_

#include <wx/dynlib.h>
#include <map>
#include "dssi.h"
#include "WiredExternalPluginLADSPA.h"
#include "../engine/AudioEngine.h"


#define STR_DSSI_DESCRIPTOR_FUNCTION_NAME wxT("dssi_descriptor")
#define STR_LADSPA_DESCRIPTOR_FUNCTION_NAME wxT("ladspa_descriptor")

class	WiredDSSIPlugin
{
public:
	WiredDSSIPlugin();
	~WiredDSSIPlugin();
	WiredDSSIPlugin(const WiredDSSIPlugin& copy);
	WiredDSSIPlugin		operator=(const WiredDSSIPlugin& right);
	bool				Load(const wxString& FileName, int& FirstIndex);
	void				UnLoad();
	std::map<int, wxString>	GetPluginsList();
	int					GetPluginType(int PluginId);
	bool				Contains(int PluginId);
	bool				CreatePlugin(int PluginId, WiredLADSPAInstance *Plugin);
	bool				Contains(unsigned long PluginUniqueId);
	std::map<int, unsigned long>	GetPluginsListUniqueId();

private:
	wxString								_FileName;
	wxDynamicLibrary							_Handle;
	DSSI_Descriptor_Function			_DSSIDescriptorFunction;
	std::map<int, const DSSI_Descriptor*>	_DSSIDescriptors;						//Key == PluginId; Value == PluginDescriptor
	LADSPA_Descriptor_Function			_LADSPADescriptorFunction;
	std::map<int, const LADSPA_Descriptor*>	_LADSPADescriptors;						//Key == PluginId; Value == PluginDescriptor
	std::map<int, int>						_PluginsInfo;
};


#endif //_WIREDEXTERNALPLUGIN_H_
