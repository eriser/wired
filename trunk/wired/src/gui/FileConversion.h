#ifndef _FILECONVERSION_H_
#define _FILECONVERSION_H_

#include <wx/thread.h>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <deque>

#include "../samplerate/WiredSampleRate.h"
#include "../codec/WiredCodec.h"

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
	string						*SrcFileName;
	string						*DstFileName;
} FileConversionAction;

class	FileConversion : public wxThreadHelper
{
public:
	FileConversion();
	~FileConversion();
	FileConversion(const FileConversion& copy){*this = copy;}
	FileConversion operator=(const FileConversion& right);
	
	virtual void		*Entry();
	bool				Init(t_samplerate_info *RateInit, string WorkingDir, unsigned long BufferSize);
	vector<string>		*GetCodecsExtensions();
	void				ConvertFromCodec(string *FileName);
	void				ConvertToCodec(string *FileName);
	void				ConvertSamplerate(string *FileName);
	void				ImportWaveFile(string *FileName);
	void				Stop();
private:
	void				CopyToWorkingDir(string *FileName);
	void				ImportWavePattern(string *FileName);
	bool				ConvertSamplerate(string *FileName, bool &HasChangedPath);
	int					GetSndFFormat(PcmType Type);
	void				Decode(string *FileName);
	void				EnqueueAction(FileConversionTypeAction ActionType, string *SrcFile, string *DstFile);
	bool 				_ShouldRun;
	WiredSampleRate		_SampleRateConverter;
	WiredCodec			_CodecConverter;
	vector<string>		_CodecsExtensions;
	string				_WorkingDir;
	unsigned long		_BufferSize;
	deque<FileConversionAction *>	_ActionsList;
};

static wxMutex FileConversionMutex;

#endif /*FILECONVERSION_H_*/
