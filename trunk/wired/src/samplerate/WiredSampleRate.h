#ifndef _WIREDSAMPLERATE_H_
#define _WIREDSAMPLERATE_H_

#include <wx/thread.h>
#include <stdio.h>
#include <sndfile.h>
#include <samplerate.h>
#include <portaudio.h>
#include <string>
#include <string.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/choicdlg.h>
#include <wx/file.h>
#include <wx/dialog.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <sstream>


#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

using namespace std;

#define STR_UNKNOWN_FORMAT "Unknown format"
#define	NB_SAMPLERATE_QUALITY 5

typedef struct s_format_types
{
	int				SndFileFormat;
	PaSampleFormat	PaFormat;
	const char		*FormatName;
};

extern struct s_format_types _FormatTypes[];

typedef struct s_samplerate_types
{
	unsigned long 	SampleRate;
	const char		*SampleRateName;
};

extern struct s_samplerate_types _SampleRateTypes[];

typedef struct s_samplerate_info
{
	string			WorkingDirectory;
	unsigned long	SampleRate;
	PaSampleFormat	Format;
	unsigned long	SamplesPerBuffer;
} t_samplerate_info;

class WiredSampleRate
{
public:
	WiredSampleRate();
	~WiredSampleRate();
	WiredSampleRate(const WiredSampleRate& copy);
	WiredSampleRate		operator=(const WiredSampleRate& right);

	void		Init(t_samplerate_info *Info);
	int			OpenFile(string& Path);					//return wxID_NO if not modified (or invalid), 	
														//else return wxID_YES r wxID_CANCEL if canceled
														// and set Path to the new FilePath
	bool		SaveFile(string& Path, unsigned int NbChannel, unsigned long NbSamples);					//return false if saving canceled
	void		EndSaveFile(unsigned int NbChannel);
	bool		IsSameFormat(int SndFileFormat, PaSampleFormat PaFormat);
	const char	*GetFormatName(int SndFileFormat);
	const char	*GetFormatName(PaSampleFormat PaFormat);
	void		WriteToFile(unsigned long NbSamples, float **Buffer, unsigned int NbChannel);
private:
	bool		Convert(SF_INFO *SrcInfo, string& SrcFile, SNDFILE *SrcData);
	int			GetConverterQuality();
	float		*ConvertSampleRate(SRC_STATE* Converter, float *Input, unsigned long FrameNb, double Ratio, unsigned long &ToWrite, bool End, int NbChannels, unsigned long &ReallyReaden);
	int			GetFileFormat(PaSampleFormat PaFormat);
	void		ChooseFileFormat(SF_INFO *DestInfo);
	float		*ConvertnChannels(float **Input, unsigned int NbChannels, SRC_STATE *Converter, unsigned long NbSamples, double Ratio, int End, unsigned long &ToWrite);
	t_samplerate_info	_ApplicationSettings;
	SNDFILE		*OpenedFile;
	SF_INFO		OpenedFileInfo;
	SRC_STATE	*StaticConverter;
	int			_Quality;
	int			_ConverterError;
	float		*_Buffer;
	float		**_ChannelBuffer;
};

extern wxMutex				SampleRateMutex;

#endif //_WIREDSAMPLERATE_H_
