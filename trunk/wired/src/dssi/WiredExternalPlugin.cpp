// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WiredExternalPlugin.h"

WiredDSSIPlugin::WiredDSSIPlugin()
{
  _DSSIDescriptorFunction = NULL;
  _LADSPADescriptorFunction = NULL;
  _FileName = wxT("");
}

WiredDSSIPlugin::WiredDSSIPlugin(const WiredDSSIPlugin& copy)
{
	*this= copy;
}

WiredDSSIPlugin::~WiredDSSIPlugin()
{
  UnLoad();
}

WiredDSSIPlugin		WiredDSSIPlugin::operator=(const WiredDSSIPlugin& right)
{
	if (this != &right)
	{
		_FileName = right._FileName;
		_DSSIDescriptorFunction = right._DSSIDescriptorFunction;
		_DSSIDescriptors = right._DSSIDescriptors;
		_LADSPADescriptorFunction = right._LADSPADescriptorFunction;
		_LADSPADescriptors = right._LADSPADescriptors;
		_PluginsInfo = right._PluginsInfo;
	}
	return *this;
}

bool			WiredDSSIPlugin::Load(const wxString& FileName, int& FirstIndex)
{
  bool			Found = false;
	
  if (_Handle.Load(FileName, wxDL_NOW))
    {
      _DSSIDescriptorFunction = (DSSI_Descriptor_Function) _Handle.GetSymbol(STR_DSSI_DESCRIPTOR_FUNCTION_NAME);
      if (_DSSIDescriptorFunction == NULL)
	{
	  _LADSPADescriptorFunction = (LADSPA_Descriptor_Function) _Handle.GetSymbol(STR_LADSPA_DESCRIPTOR_FUNCTION_NAME);
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
      int		pos, PluginInfo;
      unsigned long PortPos;
      LADSPA_PortDescriptor Pod;
		
      _FileName = FileName;
      if (_DSSIDescriptorFunction)
	{
	  const DSSI_Descriptor		*CurrentDescriptor;
			
	  for (pos = 0, PluginInfo = 0; (CurrentDescriptor = _DSSIDescriptorFunction(pos)); pos++, PluginInfo = 0)
	    {
#ifdef __DEBUG__
	      cout << "[DSSIPLUG] Adding DSSI Plugin {" << CurrentDescriptor->LADSPA_Plugin->Name << "}" << endl;
#endif
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
#ifdef __DEBUG__
	      cout << "[DSSIPLUG] Adding LADSPA Plugin {" << CurrentDescriptor->Name << "}" << endl;
#endif
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

map<int, wxString>	WiredDSSIPlugin::GetPluginsList()
{
	map<int, wxString>	Result;
	
	if (_LADSPADescriptorFunction)
	{
		map<int, const LADSPA_Descriptor*>::iterator	Iter;

		for (Iter = _LADSPADescriptors.begin(); Iter != _LADSPADescriptors.end(); Iter++)
			Result[Iter->first] = wxString(Iter->second->Name, *wxConvCurrent);
	}
	else if (_DSSIDescriptorFunction)
	{
		map<int, const DSSI_Descriptor*>::iterator	Iter;
		
		for (Iter = _DSSIDescriptors.begin(); Iter != _DSSIDescriptors.end(); Iter++)
			Result[Iter->first] = wxString(Iter->second->LADSPA_Plugin->Name, *wxConvCurrent);
	}
	return Result;
}

map<int, unsigned long>		WiredDSSIPlugin::GetPluginsListUniqueId()
{
	map<int, unsigned long>	Result;
	
	if (_LADSPADescriptorFunction)
	{
		map<int, const LADSPA_Descriptor*>::iterator	Iter;

		for (Iter = _LADSPADescriptors.begin(); Iter != _LADSPADescriptors.end(); Iter++)
			Result[Iter->first] = Iter->second->UniqueID;
	}
	else if (_DSSIDescriptorFunction)
	{
		map<int, const DSSI_Descriptor*>::iterator	Iter;
		
		for (Iter = _DSSIDescriptors.begin(); Iter != _DSSIDescriptors.end(); Iter++)
			Result[Iter->first] = Iter->second->LADSPA_Plugin->UniqueID;
	}
	return Result;
}

void				WiredDSSIPlugin::UnLoad()
{
  if (_Handle.IsLoaded())
  {
    _Handle.Unload();
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

	const LADSPA_Descriptor	*Descriptor = NULL;

	if (_LADSPADescriptorFunction)
		Descriptor = _LADSPADescriptors.find(PluginId)->second;
	else if (_DSSIDescriptorFunction)
		Descriptor = _DSSIDescriptors.find(PluginId)->second->LADSPA_Plugin;
	cout << "[DSSIPLUG] Creating Rack for Plugin " << Descriptor->Name << endl;
	Plugin->Init(Descriptor);
	cout << "[DSSIPLUG] Rack Created, loading" << endl;
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
