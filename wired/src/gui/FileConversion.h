// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _FILECONVERSION_H_
#define _FILECONVERSION_H_

#include <wx/thread.h>
#include <iostream>
#include <list>
#include <vector>
#include <deque>

#include "../samplerate/WiredSampleRate.h"
#include "../codec/WiredCodec.h"

#define		PROGRESS_DIALOG_UNIT	100

using namespace std;

typedef enum {
	AImportWaveFile = 0,
	AConvertSampleRate,
	AExportWaveFile,
	AImportFile
} FileConversionTypeAction;

typedef struct s_FileConversionAction
{
	FileConversionTypeAction	TypeAction;
	wxString			SrcFileName;
	wxString			DstFileName;
} FileConversionAction;

class	FileConversion //: public wxThread
{
public:
	FileConversion();
	~FileConversion();
	FileConversion(const FileConversion& copy){*this = copy;}
	FileConversion operator=(const FileConversion& right);
	
	virtual void		*Entry();
	bool				Init(t_samplerate_info *RateInit, wxString &WorkingDir, unsigned long BufferSize, wxWindow *Parent);
	vector<wxString>		*GetCodecsExtensions();
	bool				ConvertFromCodec(wxString& FileName);
	void				ConvertToCodec(wxString& FileName);
	bool				ConvertSamplerate(wxString& FileName);
	void				ImportWaveFile(wxString& FileName);
	void				Stop();
	void				SetBufferSize(unsigned long Size){_BufferSize = Size; _SampleRateConverter.SetBufferSize(Size);}
	void				SetSampleRate(unsigned long SampleRate){_SampleRateConverter.SetSampleRate(SampleRate);}
	void				SetFormat(PaSampleFormat Format){_SampleRateConverter.SetFormat(Format);}
private:
	void				CopyToWorkingDir(wxString& FileName);
	void				ImportWavePattern(wxString& FileName);
	bool				ConvertSamplerate(wxString& FileName, bool &HasChangedPath);
	int				GetSndFFormat(PcmType Type);
	bool				Decode(wxString& FileName);
	void				EnqueueAction(FileConversionTypeAction ActionType, wxString &SrcFile, wxString &DstFile);
	bool 				_ShouldRun;
	WiredSampleRate		_SampleRateConverter;
	WiredCodec			_CodecConverter;
	vector<wxString>		_CodecsExtensions;
	wxString				_WorkingDir;
	unsigned long		_BufferSize;
	wxWindow			*_Parent;
	deque<FileConversionAction *>	_ActionsList;
	
private:
	SNDFILE			*OpenDecodeFile(t_Pcm	&Data, const wxString &DestFileName, SF_INFO &Info, 
															unsigned long *TotalReaden, int *sf_write_result);
};

static wxMutex FileConversionMutex;

class	AskQuestion
{
public:
	AskQuestion(wxWindow *Parent){_Parent = Parent;}
	~AskQuestion(){};
	
	int					Ask(const wxString &Question, const wxString &Title)
	{
		wxMessageDialog	msg(_Parent, Question, Title, wxYES_NO | wxCANCEL  | wxICON_QUESTION | wxCENTRE);
		int res = msg.ShowModal();
		msg.Destroy();
		return res;
	}
private:
	wxWindow			*_Parent;
};

#endif /*FILECONVERSION_H_*/
