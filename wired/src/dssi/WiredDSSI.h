#ifndef __WIREDDSSI_H__
#define __WIREDDSSI_H__

#include "dssi.h"
#include "ladspa.h"
#include "../engine/AudioEngine.h"
#include "../redist/Plugin.h"
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <math.h>

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

typedef struct s_gui_port
{
	LADSPA_Data		LowerBound;
	LADSPA_Data		UpperBound;
	LADSPA_Data		*Data;
} t_gui_port;

typedef struct s_ladspa_port
{
	LADSPA_PortDescriptor	Descriptor;
	LADSPA_PortRangeHint	RangeHint;
	unsigned long			Id;	
	string					Name;
} t_ladspa_port;

typedef struct s_gui_control
{
	t_gui_port		Data;
	t_ladspa_port	Descriptor;
} t_gui_control;

class	WiredLADSPAInstance : public Plugin
{
public:
	WiredLADSPAInstance();
	~WiredLADSPAInstance();
	WiredLADSPAInstance(const WiredLADSPAInstance& copy);
	WiredLADSPAInstance	operator=(const WiredLADSPAInstance& right);
	bool				Init(const LADSPA_Descriptor* Descriptor);
	bool				Load();
	void				SetInfo(PlugInitInfo *Info);
	bool				ChangeActivateState(bool Activate = true);

	//<Wired Plugin Implementation>
	void	 			Process(float **input, float **output, long sample_length);
	void				Init();
	void				Play();
	void				Stop();
	void				Load(WiredPluginData& Datas);
	void				Save(WiredPluginData& Datas);
	void				SetBufferSize(long size);
	void				SetSamplingRate(double rate);
	void			  	SetBPM(float bpm);
	void				SetSignature(int numerator, int denominator);
	void				ProcessEvent(WiredEvent &event);
	bool				HasView();
	wxWindow			*CreateView(wxWindow *zone, wxPoint &pos, wxSize &size){return NULL;}
	void				DestroyView();
	bool				IsAudio();
	bool				IsMidi();
	void				AskUpdate();
	void				Update();
	std::string			GetHelpString();
	void				SetHelpMode(bool On);
	std::string			DefaultName();
	wxBitmap			*GetBitmap();	
	//</Wired Plugin Implementation>
	
private:
	void				UnLoad();
	void				LoadPorts();
	void				UnLoadPorts();
	void				DumpPorts();
	bool				IsLoaded();
	void				ConnectMonoInput(float *input, unsigned long PortId);
	void				ConnectMonoOutput(float *output, unsigned long PortId);
	void				ProcessStereo(float **input, float **output, long sample_length);
	void				ProcessMono(float **input, float **output, long sample_length);
	unsigned long		GetPortId(list<t_ladspa_port>& PortList, int index);
	void				SetDataPortDefaultValue(t_ladspa_port *PortData);
	LADSPA_Data			GetDefaultValue(t_gui_port *GuiPort, LADSPA_PortRangeHintDescriptor Descriptor);
	void				AddGuiControl(t_ladspa_port *PortData);
	void				UnloadGUIPorts();

	int								_Type;
	LADSPA_Handle					_Handle;
	LADSPA_Descriptor				*_Descriptor;
	LADSPA_Properties				_Properties;
	list<t_ladspa_port>				_InputAudioPluginsPorts;
	list<t_ladspa_port>				_OutputAudioPluginsPorts;
	list<t_ladspa_port>				_InputDataPluginsPorts;
	list<t_ladspa_port>				_OutputDataPluginsPorts;
	map<unsigned long, t_gui_control>	_GuiControls;								//Key == PortId; Value == PortData
	bool							_IsPlaying;
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
	bool				Contains(unsigned long PluginUniqueId);

private:
	string								_FileName;
	void								*_Handle;
	DSSI_Descriptor_Function			_DSSIDescriptorFunction;
	map<int, const DSSI_Descriptor*>	_DSSIDescriptors;						//Key == PluginId; Value == PluginDescriptor
	LADSPA_Descriptor_Function			_LADSPADescriptorFunction;
	map<int, const LADSPA_Descriptor*>	_LADSPADescriptors;						//Key == PluginId; Value == PluginDescriptor
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
	WiredLADSPAInstance	*CreatePlugin(int MenuItemId);
	void				DestroyPlugin(WiredLADSPAInstance *Plug);
	
private:
	void			LoadPlugins(const string& FileName);
	list<string>	SplitPath(string& Path);
	void			LoadPluginsFromPath(const char *Dirs, int Type);

	list<WiredDSSIPlugin*>		_Plugins;
	list<WiredLADSPAInstance*>	_LoadedPlugins;
	map<int, int>			_IdTable;									//Key == MenuItemId; Value == PluginId (auto-increment)
	int						_CurrentPluginIndex;
};

#endif	//__WIREDDSSI_H__
