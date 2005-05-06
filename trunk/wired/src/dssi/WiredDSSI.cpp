#include "WiredDSSI.h"

WiredDSSIPlugin::WiredDSSIPlugin()
{
	_Handle = NULL;
	_DSSIDescriptorFunction = NULL;
	_LADSPADescriptorFunction = NULL;
	_FileName = "";
}

WiredDSSIPlugin::WiredDSSIPlugin(const WiredDSSIPlugin& copy)
{
	*this= copy;
}

WiredDSSIPlugin::~WiredDSSIPlugin()
{
	//UnLoad();
}

WiredDSSIPlugin		WiredDSSIPlugin::operator=(const WiredDSSIPlugin& right)
{
	if (this != &right)
	{
		_Handle = right._Handle;
		_FileName = right._FileName;
		_DSSIDescriptorFunction = right._DSSIDescriptorFunction;
		_DSSIDescriptors = right._DSSIDescriptors;
		_LADSPADescriptorFunction = right._LADSPADescriptorFunction;
		_LADSPADescriptors = right._LADSPADescriptors;
	}
	return *this;
}

bool				WiredDSSIPlugin::Load(const string& FileName)
{
	bool			Found = false;
	
	_Handle = dlopen(FileName.c_str(), RTLD_NOW);
	if (_Handle != NULL)
	{
		_DSSIDescriptorFunction = (DSSI_Descriptor_Function) dlsym(_Handle, STR_DSSI_DESCRIPTOR_FUNCTION_NAME);
		if (_DSSIDescriptorFunction == NULL)
		{
			_LADSPADescriptorFunction = (LADSPA_Descriptor_Function) dlsym(_Handle, STR_LADSPA_DESCRIPTOR_FUNCTION_NAME);
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
		int		pos;
		
		_FileName = FileName;
		if (_DSSIDescriptorFunction)
		{
			const DSSI_Descriptor		*CurrentDescriptor;
			
			for (pos = 0; (CurrentDescriptor = _DSSIDescriptorFunction(pos)); pos++)
			{
				cout << "Adding DSSI Plugin {" << CurrentDescriptor->LADSPA_Plugin->Name << "}" << endl;
				_DSSIDescriptors.insert(_DSSIDescriptors.end(), CurrentDescriptor);
			}
		}
		else if (_LADSPADescriptorFunction)
		{
			const LADSPA_Descriptor		*CurrentDescriptor;
			
			for (pos = 0; (CurrentDescriptor = _LADSPADescriptorFunction(pos)); pos++)
			{
				cout << "Adding LADSPA Plugin {" << CurrentDescriptor->Name << "}" << endl;
				_LADSPADescriptors.insert(_LADSPADescriptors.end(), CurrentDescriptor);
			}
		}	
	}
	return Found;
}

void				WiredDSSIPlugin::UnLoad()
{
	if (_Handle != NULL)
	{
		dlclose(_Handle);
		_Handle = NULL;
	}
}

WiredDSSI::WiredDSSI()
{
	;
}

WiredDSSI::WiredDSSI(const WiredDSSI& copy)
{
	*this= copy;
}

WiredDSSI::~WiredDSSI()
{
	list<WiredDSSIPlugin*>::iterator	Iter;
	WiredDSSIPlugin*					CurrentPlugin;
	
	for (Iter = _Plugins.begin(); Iter != _Plugins.end(); Iter++)
	{
		(*Iter)->UnLoad();
		CurrentPlugin = *Iter;
		delete CurrentPlugin;
	}
}

WiredDSSI		WiredDSSI::operator=(const WiredDSSI& right)
{
	if (this != &right)
	{
		_Plugins = right._Plugins;
	}
	return *this;
}

void			WiredDSSI::LoadPLugins(int Type)
{
	char			*Dirs = NULL;
	
	if (Type & TYPE_PLUGINS_DSSI)
		if ((Dirs = getenv(ENV_NAME_PLUGINS_DSSI)) != NULL)
			LoadPluginsFromPath(Dirs, TYPE_PLUGINS_DSSI);
	if (Type & TYPE_PLUGINS_LADSPA)
		if ((Dirs = getenv(ENV_NAME_PLUGINS_LADSPA)) != NULL)
			LoadPluginsFromPath(Dirs, TYPE_PLUGINS_LADSPA);
}

void			WiredDSSI::LoadPluginsFromPath(const char *Dirs, int Type)
{
	list <string>::iterator 	Iter;
	DIR 						*CurrentDir = NULL;
	struct dirent 				*CurrentFile = NULL;
	string						DirsFromEnv = string(Dirs);
	list<string>				Paths = SplitPath(DirsFromEnv);
	
	for (Iter = Paths.begin(); Iter != Paths.end(); Iter++)
	{
		if ((CurrentDir = opendir((*Iter).c_str())) != NULL)
		{
			while ((CurrentFile = readdir(CurrentDir)))
			{
				//TODO bring back LADSPA / DSSI Load.
				if (Type == TYPE_PLUGINS_DSSI)
					LoadPlugins(string(*Iter) + string("/") + string(CurrentFile->d_name));
				else if (Type == TYPE_PLUGINS_LADSPA)
					LoadPlugins(string(*Iter) + string("/") + string(CurrentFile->d_name));
			}	
			closedir(CurrentDir);
		}
	}
}

void			WiredDSSI::LoadPlugins(const string& FileName)
{
	//cout << "Trying to open file named {" << FileName.c_str() << "}" << endl;
	WiredDSSIPlugin		*NewPlugin = new WiredDSSIPlugin();
	
	if (NewPlugin->Load(FileName))
		_Plugins.insert(_Plugins.end(), NewPlugin);
	//else
		//cout << "Could not open file" << endl;
}

list<string>	WiredDSSI::SplitPath(string& Path)
{
	ifstream f(Path.c_str());
	list<string> Result;
	string		Buffer;

	if (Path.find(ENV_PATH_SEPARATOR) == Path.npos)
		Result.insert(Result.end(), Path);
	else
		while (getline(f, Buffer, ENV_PATH_SEPARATOR))
			Result.insert(Result.end(), Buffer);
	f.close();
	return Result;
}
