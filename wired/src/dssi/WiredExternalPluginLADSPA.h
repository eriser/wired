#ifndef _WIREDEXTERNALPLUGINLADSPA_H_
#define _WIREDEXTERNALPLUGINLADSPA_H_

#include "ladspa.h"
#include "../engine/AudioEngine.h"
#include "../redist/Plugin.h"
#include <stdlib.h>
#include <dlfcn.h>
#include <math.h>

#include <list>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#define	TYPE_PLUGINS_DSSI 1
#define	TYPE_PLUGINS_LADSPA 2

#define	TYPE_PLUGINS_INSTR 4
#define	TYPE_PLUGINS_EFFECT 8

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
	void				SetInfo(PlugStartInfo *Info);
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
	void				DestroyView(){}
	bool				IsAudio();
	bool				IsMidi();
	void				AskUpdate(){}
	void				Update() {}
	std::string			GetHelpString();
	void				SetHelpMode(bool On);
	std::string			DefaultName();
	wxBitmap			*GetBitmap(){return NULL;}	
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
	bool							_IsPlaying;
protected:
	map<unsigned long, t_gui_control>	_GuiControls;								//Key == PortId; Value == PortData
};


#endif //_WIREDEXTERNALPLUGINLADSPA_H_