#include "WiredSampleRate.h"

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
		if (Info->samplerate != _ApplicationSettings.SampleRate || 
			IsSameFormat(Info->format, _ApplicationSettings.Format) == false)
		{
			
		}
		sf_close(Result);
	}
	return false;
}

bool					WiredSampleRate::IsSameFormat(int SndFileFormat, PaSampleFormat PaFormat)
{
	int		pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0; pos++)
	{
		if (_FormatTypes[pos].PaFormat == PaFormat)
		{
			if (_FormatTypes[pos].SndFileFormat & SndFileFormat)
				return true;
			else
				return false;
		}
	}
	return true;
}

const char*				WiredSampleRate::GetFormatName(int SndFileFormat)
{
	int		pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0; pos++)
		if (_FormatTypes[pos].SndFileFormat & SndFileFormat)
			return _FormatTypes[pos].FormatName;
	return STR_UNKNOWN_FORMAT;	
}

const char*				WiredSampleRate::GetFormatName(PaSampleFormat PaFormat)
{	
	int		pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0; pos++)
		if (_FormatTypes[pos].PaFormat == PaFormat)
			return _FormatTypes[pos].FormatName;
	return STR_UNKNOWN_FORMAT;
}
