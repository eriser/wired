#ifndef _WIREDSAMPLERATE_H_
#define _WIREDSAMPLERATE_H_

#include <stdio.h>
#include <sndfile.h>
#include <samplerate.h>
#include <portaudio.h>
#include <string>
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
	WiredSampleRate(){}
	~WiredSampleRate(){}
	WiredSampleRate(const WiredSampleRate& copy);
	WiredSampleRate		operator=(const WiredSampleRate& right);

	void		Init(t_samplerate_info *Info);
	int			OpenFile(string& Path);					//return wxID_NO if not modified (or invalid), 	
														//else return wxID_YES r wxID_CANCEL if canceled
														// and set Path to the new FilePath
	int			SaveFile(string& Path);					//same return value as OpenFile()
	bool		IsSameFormat(int SndFileFormat, PaSampleFormat PaFormat);
	const char	*GetFormatName(int SndFileFormat);
	const char	*GetFormatName(PaSampleFormat PaFormat);
private:
	bool		Convert(SF_INFO *SrcInfo, string& SrcFile, SNDFILE *SrcData);
	int			GetConverterQuality();
	float		*ConvertSampleRate(SRC_STATE* Converter, float *Input, unsigned long FrameNb, double Ratio, unsigned long &ToWrite, bool End, int NbChannels, unsigned long &ReallyReaden);
	int			GetFileFormat(PaSampleFormat PaFormat);
	void		ChooseFileFormat(SF_INFO *DestInfo);
	t_samplerate_info	_ApplicationSettings;
};

#endif //_WIREDSAMPLERATE_H_
