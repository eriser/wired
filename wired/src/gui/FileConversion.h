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
	ImportWaveFile = 0,
	ConvertSampleRate,
	ExportWaveFile,
	ImportFile
} FileConversionTypeAction;

typedef struct s_FileConversionAction
{
	FileConversionTypeAction	TypeAction;
	string						SrcFileName;
	string						DstFileName;
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
	void				ConvertFromCodec(const string &FileName);
	void				ConvertToCodec(const string &FileName);
	void				ConvertSamplerate(const string &FileName);
	void				Stop();
private:
	bool				ConvertSamplerate(string& FileName, bool &HasChangedPath);
	int					GetSndFFormat(PcmType Type);
	void				Decode(string &FileName);
	void				EnqueueAction(FileConversionTypeAction ActionType, const string &SrcFile, const string &DstFile);
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
