// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _WIREDSAMPLERATE_H_
#define _WIREDSAMPLERATE_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <wx/thread.h>
#include <stdio.h>
#include <sndfile.h>
#include <samplerate.h>
#include "portaudio.h"
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/choicdlg.h>
#include <wx/file.h>
#include <wx/dialog.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/intl.h>


#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

using namespace std;

#define STR_UNKNOWN_FORMAT _("Unknown format")
#define	NB_SAMPLERATE_QUALITY 5



struct s_format_types
{
	int				SndFileFormat;
	PaSampleFormat	PaFormat;
	const wxChar		*FormatName;
};

extern struct s_format_types _FormatTypes[];

struct s_samplerate_types
{
	unsigned long 	SampleRate;
	const wxChar		*SampleRateName;
};

extern struct s_samplerate_types _SampleRateTypes[];

typedef struct s_samplerate_info
{
	wxString		WorkingDirectory;
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
	int		OpenFile(wxString& Path, wxWindow* parent);					//return wxID_NO if not modified (or invalid), 	
	int		OpenFileNoGraph(wxString& Path, wxWindow* parent);													//else return wxID_YES r wxID_CANCEL if canceled
														// and set Path to the new FilePath
	bool		SaveFile(wxString& Path, unsigned int NbChannel, unsigned long NbSamples, bool interleaved);					//return false if saving canceled
	void		EndSaveFile(unsigned int NbChannel);
	bool		IsSameFormat(int SndFileFormat, PaSampleFormat PaFormat);
	const wxChar	*GetFormatName(int SndFileFormat);
	const wxChar	*GetFormatName(PaSampleFormat PaFormat);
	void		WriteToFile(unsigned long NbSamples, float **Buffer, unsigned int NbChannel); // non interleaved
	void		WriteToFile(unsigned long NbSamples, float *Buffer, unsigned int NbChannel);  //interleaved
	void		SetSampleRate(unsigned long SampleRate);
	void		SetFormat(PaSampleFormat Format);
	void		SetBufferSize(unsigned long Size);
	t_samplerate_info Get_Apllication_Setting(){return _ApplicationSettings;};
	bool		Convert(SF_INFO *SrcInfo, wxString& SrcFile, SNDFILE *SrcData, bool isgraph);
private:

	int			GetConverterQuality();
	float		*ConvertSampleRate(SRC_STATE* Converter, float *Input, unsigned long FrameNb, double Ratio, unsigned long &ToWrite, bool End, int NbChannels, unsigned long &ReallyReaden);
	int			GetFileFormat(PaSampleFormat PaFormat);
	void		ChooseFileFormat(SF_INFO *DestInfo);
	float		*ConvertnChannels(float **Input, unsigned int NbChannels, SRC_STATE *Converter, unsigned long NbSamples, double Ratio, int End, unsigned long &ToWrite); // non interleaved
	float		*ConvertnChannels(float *Input, unsigned int NbChannels, SRC_STATE *Converter, unsigned long NbSamples, double Ratio, int End, unsigned long &ToWrite);  // interleaved
	t_samplerate_info	_ApplicationSettings;
	SNDFILE		*OpenedFile;
	SF_INFO		OpenedFileInfo;
	SRC_STATE	*StaticConverter;
	int			_Quality;
	int			_ConverterError;
	float		*_Buffer;
    float      *_RateBuffer;
};

#endif //_WIREDSAMPLERATE_H_
