// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WiredExternalPluginLADSPA.h"

WiredLADSPAInstance::WiredLADSPAInstance(PlugStartInfo &info) : Plugin(info, NULL)
{
	_Handle = NULL;
	_Descriptor = NULL;
	_Type = 0;
	_Properties = 0;
	StartInfo = info;
	_Bypass = false;
    _IsPlaying = false;
}

WiredLADSPAInstance::~WiredLADSPAInstance()
{
	UnLoad();
}

WiredLADSPAInstance		WiredLADSPAInstance::operator=(const WiredLADSPAInstance& right)
{
	if (this != &right)
	{
		//this->::Plugin = right;
		Name= right.Name;
		StartInfo = right.StartInfo;
		InitInfo = right.InitInfo;
		_InputAudioPluginsPorts = right._InputAudioPluginsPorts;
		_OutputAudioPluginsPorts = right._OutputAudioPluginsPorts;
		_InputDataPluginsPorts = right._InputDataPluginsPorts;
		_OutputDataPluginsPorts = right._OutputDataPluginsPorts;
		_Handle = right._Handle;
		_Descriptor = right._Descriptor;
		_Type = right._Type;
		_Properties = right._Properties;
		_GuiControls = right._GuiControls;
		_IsPlaying = right._IsPlaying;
		_Bypass = right._Bypass;
	}
	return *this;
}
//
//bool					WiredLADSPAInstance::operator<(const WiredLADSPAInstance& right)
//{
//	return (this->Name.compare(right.Name)) <= 0;
//}

bool					WiredLADSPAInstance::Init(const LADSPA_Descriptor *Descriptor)
{
	UnLoad();
	if (Descriptor)
		_Descriptor = (LADSPA_Descriptor *)Descriptor;
	else
		return false;
	return true;
}

void					WiredLADSPAInstance::SetInfo(PlugInitInfo *Info)
{	
	Info->Name = Name;
	if (_InputAudioPluginsPorts.size() > 0)
		Info->Type = ePlugTypeEffect;
	else
		Info->Type = ePlugTypeInstrument;
	Info->UnitsX = _InputDataPluginsPorts.size() / 5 + ((_InputDataPluginsPorts.size() % 5) / 4) + 1;
	Info->UnitsY = 1;
	Info->UniqueExternalId = _Descriptor->UniqueID;
	InitInfo = Info;
}

void					WiredLADSPAInstance::SetInfo(PlugStartInfo *Info)
{
	StartInfo = *Info;
    Info->Size = wxSize(_InputDataPluginsPorts.size() / 5 + ((_InputDataPluginsPorts.size() % 5) / 4) + 1, 1);
}

bool					WiredLADSPAInstance::Load()
{
	if (_Descriptor)
	{
		UnLoad();
		if (_Descriptor->instantiate!= NULL)
			if ((_Handle = _Descriptor->instantiate(_Descriptor, (unsigned long) Audio->SampleRate)))
			{
				_Properties = _Descriptor->Properties;
				Name = wxString(_Descriptor->Name, *wxConvCurrent);
				LoadPorts();
				return true;
			}
		cout << "Load Failed" << endl;
	}
	return false;
}

void					WiredLADSPAInstance::UnLoad()
{
	if (_Handle && _Descriptor)
	{
		cout << "Unloading Plugin " << _Descriptor->Name;
		ChangeActivateState(false);
		cout << ".";
		_Descriptor->cleanup(_Handle);
		cout << ".";
		_Handle = NULL;
		UnLoadPorts();
		cout << ".";
		cout << "done" << endl;
	}
}

void					WiredLADSPAInstance::UnloadGUIPorts()
{
	map<unsigned long, t_gui_control>::iterator	Iter;
	
	for (Iter = _GuiControls.begin(); Iter != _GuiControls.end(); Iter++)
	{		
		if (Iter->second.Data.Data != NULL)
			delete Iter->second.Data.Data;
	}
}

bool					WiredLADSPAInstance::ChangeActivateState(bool Activate)
{
	if (_Handle && _Descriptor)
	{
		if (Activate && _Descriptor->activate != NULL)
			_Descriptor->activate(_Handle);
		else if (_Descriptor->deactivate != NULL)
			_Descriptor->deactivate(_Handle);
		return true;
	}
	return false;
}

void					WiredLADSPAInstance::UnLoadPorts()
{
	_InputAudioPluginsPorts.clear();
	_OutputAudioPluginsPorts.clear();
	_InputDataPluginsPorts.clear();
	_OutputDataPluginsPorts.clear();
}

void					WiredLADSPAInstance::LoadPorts()
{
	unsigned long		pos;
	t_ladspa_port		CurrentPort;

	UnLoadPorts();	
	for (pos = 0, CurrentPort.Descriptor = 0, CurrentPort.Id = 0, CurrentPort.RangeHint.LowerBound = 0, 
		 CurrentPort.RangeHint.UpperBound = 0, CurrentPort.RangeHint.HintDescriptor = 0;
		 pos < _Descriptor->PortCount; 
	 	 pos ++, CurrentPort.Descriptor = 0, CurrentPort.Id = 0, CurrentPort.RangeHint.LowerBound = 0, 
		 CurrentPort.RangeHint.UpperBound = 0, CurrentPort.RangeHint.HintDescriptor = 0)
	{
		CurrentPort.Descriptor = _Descriptor->PortDescriptors[pos];
		CurrentPort.RangeHint = _Descriptor->PortRangeHints[pos];
		CurrentPort.Id = pos;
		CurrentPort.Name = wxString(_Descriptor->PortNames[pos], *wxConvCurrent);
		if (LADSPA_IS_PORT_INPUT(CurrentPort.Descriptor))
		{
			if (LADSPA_IS_PORT_CONTROL(CurrentPort.Descriptor))
			{
				_InputDataPluginsPorts.insert(_InputDataPluginsPorts.end(), CurrentPort);
				AddGuiControl(&CurrentPort);
			}
			else if (LADSPA_IS_PORT_AUDIO(CurrentPort.Descriptor))
			{
				_InputAudioPluginsPorts.insert(_InputAudioPluginsPorts.end(), CurrentPort);
				_Type |= TYPE_PLUGINS_EFFECT;
			}
		}
		else if (LADSPA_IS_PORT_OUTPUT(CurrentPort.Descriptor))
		{
			if (LADSPA_IS_PORT_CONTROL(CurrentPort.Descriptor))
			{
				ConnectMonoOutput((float *) new LADSPA_Data, pos);
				_OutputDataPluginsPorts.insert(_OutputDataPluginsPorts.end(), CurrentPort);
			}
			else if (LADSPA_IS_PORT_AUDIO(CurrentPort.Descriptor))
				_OutputAudioPluginsPorts.insert(_OutputAudioPluginsPorts.end(), CurrentPort);
		}
	}
	if (!(_Type & TYPE_PLUGINS_EFFECT))
		_Type |= TYPE_PLUGINS_INSTR;
	//DumpPorts();
}

void					WiredLADSPAInstance::AddGuiControl(t_ladspa_port *PortData)
{
	t_gui_control			NewGuiPort;

	if (LADSPA_IS_HINT_BOUNDED_BELOW(PortData->RangeHint.HintDescriptor))
		NewGuiPort.Data.LowerBound = PortData->RangeHint.LowerBound;
	else
		NewGuiPort.Data.LowerBound = 0;
	if (LADSPA_IS_HINT_BOUNDED_ABOVE(PortData->RangeHint.HintDescriptor))
		NewGuiPort.Data.UpperBound = PortData->RangeHint.UpperBound;
	else
		NewGuiPort.Data.UpperBound = 100;
	NewGuiPort.Data.Data = new LADSPA_Data;
	*(NewGuiPort.Data.Data) = GetDefaultValue(&NewGuiPort.Data, PortData->RangeHint.HintDescriptor);
	NewGuiPort.Descriptor.Descriptor = PortData->Descriptor;
	NewGuiPort.Descriptor.RangeHint = PortData->RangeHint;
	NewGuiPort.Descriptor.Name = PortData->Name;
	NewGuiPort.Descriptor.Id = PortData->Id;
	_GuiControls[PortData->Id] = NewGuiPort;
	ConnectMonoInput((float *) NewGuiPort.Data.Data, PortData->Id);
}

LADSPA_Data				WiredLADSPAInstance::GetDefaultValue(t_gui_port *GuiPort, LADSPA_PortRangeHintDescriptor Descriptor)
{
	if (LADSPA_IS_HINT_HAS_DEFAULT(Descriptor))
	{
		if(LADSPA_IS_HINT_DEFAULT_MINIMUM(Descriptor))
			return GuiPort->LowerBound;
		else if (LADSPA_IS_HINT_DEFAULT_LOW(Descriptor))
		{
			if (LADSPA_IS_HINT_LOGARITHMIC(Descriptor))
				return (LADSPA_Data) expf(logf(GuiPort->LowerBound) * 0.75 + logf(GuiPort->UpperBound) * 0.25);
			else
				return (LADSPA_Data) GuiPort->LowerBound * 0.75 + GuiPort->UpperBound * 0.25;
		}
		else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(Descriptor))
		{
			if (LADSPA_IS_HINT_LOGARITHMIC(Descriptor))
				return (LADSPA_Data) expf(logf(GuiPort->LowerBound) * 0.5 + logf(GuiPort->UpperBound) * 0.5);
			else
				return (LADSPA_Data) GuiPort->LowerBound * 0.5 + GuiPort->UpperBound * 0.5;
		}
		else if (LADSPA_IS_HINT_DEFAULT_HIGH(Descriptor))
		{
			if (LADSPA_IS_HINT_LOGARITHMIC(Descriptor))
				return (LADSPA_Data) expf(logf(GuiPort->LowerBound) * 0.25 + logf(GuiPort->UpperBound) * 0.75);
			else
				return (LADSPA_Data) GuiPort->LowerBound * 0.25 + GuiPort->UpperBound * 0.75;
		}
		else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(Descriptor))
			return GuiPort->UpperBound;
		else if (LADSPA_IS_HINT_DEFAULT_0(Descriptor))
			return 0.0f;
		else if (LADSPA_IS_HINT_DEFAULT_1(Descriptor))
			return 1.0f;
		else if (LADSPA_IS_HINT_DEFAULT_100(Descriptor))
			return 100.0f;
		else if (LADSPA_IS_HINT_DEFAULT_440(Descriptor))
			return 440.0f;
	}
	return GuiPort->LowerBound;
}

void					WiredLADSPAInstance::DumpPorts()
{
	list<t_ladspa_port>::iterator	Iter;
	
	cout << "############## Begin of Ports Dump ##############" << endl;
	cout << "Input(s) audio" << endl;
	for (Iter = _InputAudioPluginsPorts.begin(); Iter != _InputAudioPluginsPorts.end(); Iter++)
		cout << "Input Named {" << Iter->Name.c_str() << "}" << " Id == " << Iter->Id << endl;
	cout << "Input(s) data" << endl;
	for (Iter = _InputDataPluginsPorts.begin(); Iter != _InputDataPluginsPorts.end(); Iter++)
	{
		cout << "Input Named {" << Iter->Name.c_str() << "}" << " Id == " << Iter->Id << endl;
//		cout << "Lower Bound == " << Iter->RangeHint.LowerBound << ", Upper Bound == " << Iter->RangeHint.UpperBound << endl;
	}
	cout << "Output(s) audio" << endl;
	for (Iter = _OutputAudioPluginsPorts.begin(); Iter != _OutputAudioPluginsPorts.end(); Iter++)
		cout << "Output Named {" << Iter->Name.c_str() << "}" << " Id == " << Iter->Id << endl;
	cout << "Output(s) data" << endl;
	for (Iter = _OutputDataPluginsPorts.begin(); Iter != _OutputDataPluginsPorts.end(); Iter++)
		cout << "Output Named {" << Iter->Name.c_str() << "}" << " Id == " << Iter->Id << endl;
	cout << "############## End of Ports Dump ##############" << endl;
}

bool					WiredLADSPAInstance::IsLoaded()
{
	if (_Handle && _Descriptor)
		return true;
	return false;
}

void					WiredLADSPAInstance::Bypass()
{
	_Bypass = _Bypass == true ? false : true;
}

void	 				WiredLADSPAInstance::Process(float **input, float **output, long sample_length)
{
	if (IsLoaded() == false || _InputAudioPluginsPorts.empty() || _OutputAudioPluginsPorts.empty() || 
		(LADSPA_IS_INPLACE_BROKEN(_Properties) && *input == *output) || _IsPlaying == false || _Bypass == true)
	{
		memcpy(output[0], input[0], sample_length * sizeof(float));
		memcpy(output[1], input[1], sample_length * sizeof(float));
	}
	else
	{
		if (_InputAudioPluginsPorts.size() >= 2 && _OutputAudioPluginsPorts.size() >= 2)
		{
			//memcpy(output[0], input[0], sample_length * sizeof(float));
			//memcpy(output[1], input[1], sample_length * sizeof(float));
			ProcessStereo(input, output, sample_length);
		}
		else
			ProcessMono(input, output, sample_length);
	}
}

void					WiredLADSPAInstance::ConnectMonoInput(float *input, unsigned long PortId)
{
	//if (IsLoaded())
		_Descriptor->connect_port(_Handle, PortId, input);
}

void					WiredLADSPAInstance::ConnectMonoOutput(float *output, unsigned long PortId)
{
	//if (IsLoaded())
		_Descriptor->connect_port(_Handle, PortId, output);
}

void					WiredLADSPAInstance::ProcessStereo(float **input, float **output, long sample_length)
{
	if (IsLoaded() && _InputAudioPluginsPorts.size() >= 2 && _OutputAudioPluginsPorts.size() >= 2)
	{
    	//	cout << "process Stereo 0, port == " << GetPortId(_InputAudioPluginsPorts, 0)<< " vers == " << GetPortId(_OutputAudioPluginsPorts, 0) << endl;
        if (input[0] && output[0])
        {
            ConnectMonoInput(input[0], GetPortId(_InputAudioPluginsPorts, 0));
            ConnectMonoOutput(output[0], GetPortId(_OutputAudioPluginsPorts, 0));
            if (input[1] && output[1])
            {
                ConnectMonoInput(input[1], GetPortId(_InputAudioPluginsPorts, 1));
                ConnectMonoOutput(output[1], GetPortId(_OutputAudioPluginsPorts, 1));
                _Descriptor->run(_Handle, (unsigned long) sample_length);
                return;
            }
        }
    }
     memcpy(output[0], input[0], sample_length * sizeof(float));
     memcpy(output[1], input[1], sample_length * sizeof(float));
}

void					WiredLADSPAInstance::ProcessMono(float **input, float **output, long sample_length)
{
	if (IsLoaded() && _InputAudioPluginsPorts.size() >= 1 && _OutputAudioPluginsPorts.size() >= 1)
	{
        if (input[0] && output[0])
        {
    		ConnectMonoInput(input[0], GetPortId(_InputAudioPluginsPorts, 0));
    		ConnectMonoOutput(output[0], GetPortId(_OutputAudioPluginsPorts, 0));
    		_Descriptor->run(_Handle, (unsigned long) sample_length);
        }
        if (input[1] && output[1])
        {
            ConnectMonoInput(input[1], GetPortId(_InputAudioPluginsPorts, 0));
        	ConnectMonoOutput(output[1], GetPortId(_OutputAudioPluginsPorts, 0));
        	_Descriptor->run(_Handle, (unsigned long) sample_length);
            return;
         }
      }      
     memcpy(output[0], input[0], sample_length * sizeof(float));
     memcpy(output[1], input[1], sample_length * sizeof(float));
}

void					WiredLADSPAInstance::Init()
{
	
}

void					WiredLADSPAInstance::Play()
{
	_IsPlaying = true;
	ChangeActivateState();
}

void					WiredLADSPAInstance::Stop()
{
	_IsPlaying = false;	
	ChangeActivateState(false);
}

unsigned long			WiredLADSPAInstance::GetPortId(list<t_ladspa_port>& PortList, int index)
{
	list<t_ladspa_port>::iterator	Iter;
	unsigned long		Result = 0;
	int					pos;
	
	for (Iter = PortList.begin(), pos = 0; Iter != PortList.end(); Iter++, pos++)
		if (pos == index)
		{
			Result = Iter->Id;
			break;
		}
	return Result;
}

void					WiredLADSPAInstance::Load(WiredPluginData& Datas)
{
	string				Buffer;
	list<t_ladspa_port>::iterator	Iter;
	t_gui_port			NewData;
	
	for (Iter = _InputDataPluginsPorts.begin(); Iter != _InputDataPluginsPorts.end(); Iter++)
	{
		if (_GuiControls.find(Iter->Id) != _GuiControls.end())
		{
			Buffer = Datas.LoadValue(Iter->Name);
			if (Buffer.size() > 0 &&  NewData.Data != NULL)
				*(NewData.Data) = (LADSPA_Data)strtof(Buffer.c_str(), NULL);
		}
	}
}

void					WiredLADSPAInstance::Save(WiredPluginData& Datas)
{
	ostringstream					Buffer;
	list<t_ladspa_port>::iterator	Iter;
	
	for (Iter = _InputDataPluginsPorts.begin(); Iter != _InputDataPluginsPorts.end(); Iter++)
	{
		if (_GuiControls.find(Iter->Id) != _GuiControls.end())
		{
			Buffer << *(_GuiControls[Iter->Id].Data.Data);
			Datas.SaveValue(Iter->Name, wxString(Buffer.str().c_str(), *wxConvCurrent));
			Buffer.clear();	
		}
	}
}

void					WiredLADSPAInstance::SetBufferSize(long size)
{
	
}

void					WiredLADSPAInstance::SetSamplingRate(double rate)
{
	
}

void			  		WiredLADSPAInstance::SetBPM(float bpm)
{
	
}

void					WiredLADSPAInstance::SetSignature(int numerator, int denominator)
{
	
}
	
void					WiredLADSPAInstance::ProcessEvent(WiredEvent &event)
{
	
}

bool					WiredLADSPAInstance::HasView()
{
	return true;
}

bool					WiredLADSPAInstance::IsAudio()
{
	if (_Type & TYPE_PLUGINS_EFFECT)
		return true;
	return false;
}

bool					WiredLADSPAInstance::IsMidi()
{
	if (_Type & TYPE_PLUGINS_INSTR)
		return true;
	return false;
}

wxString    			WiredLADSPAInstance::GetHelpString()
{
	if (IsLoaded())
	{
		wxString						HelpMsg;
		list<t_ladspa_port>::iterator	Iter;
		
		HelpMsg = _("This is a LADSPA or DSSI Plugin, named '");
		HelpMsg += wxString(_Descriptor->Name, *wxConvCurrent);
		HelpMsg += wxT("'");
		if (_Descriptor->Maker)
		{
			HelpMsg += _("<BR>Maker is : ");
			HelpMsg += wxString(_Descriptor->Maker, *wxConvCurrent);
		}
		if (_Descriptor->Copyright)
		{
			HelpMsg += _("<BR>Copyright : ");
			HelpMsg += wxString(_Descriptor->Copyright, *wxConvCurrent);
		}
		HelpMsg += _("<BR>This plugin is currently ");
		if (_Bypass == true)
			HelpMsg += _("disabled.<BR>");
		else
			HelpMsg += _("enabled.<BR>");
		if (_InputAudioPluginsPorts.size() > 0)
		{
			HelpMsg += _("Audio Inputs are : ");
			for (Iter = _InputAudioPluginsPorts.begin(); Iter != _InputAudioPluginsPorts.end(); Iter++)
			{
				HelpMsg += Iter->Name;
				HelpMsg += _("<BR>");
			}
		}
		if (_InputDataPluginsPorts.size() > 0)
		{
			HelpMsg += _("Data Inputs are : ");
			for (Iter = _InputDataPluginsPorts.begin(); Iter != _InputDataPluginsPorts.end(); Iter++)
			{
				HelpMsg += Iter->Name;
				HelpMsg += _("<BR>");
			}
		}
		if (_OutputAudioPluginsPorts.size() > 0)
		{
			HelpMsg += _("Audio Outputs are : ");
			for (Iter = _OutputAudioPluginsPorts.begin(); Iter != _OutputAudioPluginsPorts.end(); Iter++)
			{
				HelpMsg += Iter->Name;
				HelpMsg += wxT("<BR>");
			}
		}
		if (_OutputDataPluginsPorts.size() > 0)
		{			
			HelpMsg += _("Data Outputs are : ");
			for (Iter = _OutputDataPluginsPorts.begin(); Iter != _OutputDataPluginsPorts.end(); Iter++)
			{
				HelpMsg += Iter->Name;
				HelpMsg += wxT("<BR>");
			}
		}
		return (HelpMsg);
	}
	return STR_DEFAULT_HELP;
}

void					WiredLADSPAInstance::SetHelpMode(bool On)
{
	
}

wxString				WiredLADSPAInstance::DefaultName()
{
	if (IsLoaded())
		return wxString(_Descriptor->Name, *wxConvCurrent);
	return STR_DEFAULT_NAME;
}

unsigned long			WiredLADSPAInstance::GetUniqueId()
{
	if (IsLoaded())
		return _Descriptor->UniqueID;
	return 0;
}
