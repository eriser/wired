#include "WiredSamplerate.h"

WiredSampleRate::WiredSampleRate(const WiredSampleRate& copy)
{
	*this = copy;
}

WiredSampleRate			WiredSampleRate::operator=(const WiredSampleRate& right)
{
	if (this != &right)
	{
		_ApplicationSettings = right._ApplicationSettings;
	}
	return *this;
}

void					WiredSampleRate::Init(t_samplerate_info *Info)
{
	_ApplicationSettings.WorkingDirectory = Info->WorkingDirectory;
	_ApplicationSettings.SampleRate = Info->SampleRate;
	_ApplicationSettings.Format = Info->Format;
}

bool					WiredSampleRate::OpenFile(string& Path)
{
	SNDFILE				*Result;
	SF_INFO				*Info;
	
	if ((Result = sf_open(Path.c_str(), SFM_READ, Info)))
	{
		if (Info->samplerate != _ApplicationSettings.SampleRate || Info->format != _ApplicationSettings.Format)
		{
			
		}
		sf_close(Result);
	}
	return false;
}
