#include <wx/filename.h>
#include <sndfile.h>
#include <strings.h>

#include "FileConversion.h"
#include "../undo/cAddTrackAction.h"
#include "../undo/cImportMidiAction.h"

wxMutex FileConversionMutex;

FileConversion::FileConversion() : wxThreadHelper()
{
	_ShouldRun = false;
}

FileConversion::~FileConversion()
{
	GetThread()->Delete();
}

FileConversion		FileConversion::operator=(const FileConversion& right)
{
	if (this != &right)
	{
		_ShouldRun = right._ShouldRun;
		_CodecConverter = right._CodecConverter;
		_SampleRateConverter = right._SampleRateConverter;
	}
	return *this;
}

bool				FileConversion::Init(t_samplerate_info *RateInit, string WorkingDir, unsigned long BufferSize)
{
	_WorkingDir = WorkingDir;
	_BufferSize = BufferSize;
	cout << "[FILECONVERT] Loading Codec management...";
	_CodecConverter.Init();
	list<string>			CodecsListExtensions = _CodecConverter.GetExtension();
	list<string>::iterator	Iter;

	for (Iter = CodecsListExtensions.begin(); Iter != CodecsListExtensions.end(); Iter++)
  		_CodecsExtensions.insert(_CodecsExtensions.end(), *Iter);
  	cout << "...done" << endl << "[FILECONVERT] Loading samplerate management...";
	_SampleRateConverter.Init(RateInit);
	cout << "...done" << endl;
	_ShouldRun = true;
	if (Create() != wxTHREAD_NO_ERROR)
		return false;	
	FileConversionMutex.Lock();
	return true;
}

vector<string>		*FileConversion::GetCodecsExtensions()
{
	return &_CodecsExtensions;
}

void				*FileConversion::Entry()
{	
	while(_ShouldRun)
	{
		FileConversionMutex.Lock();
		if (_ActionsList.empty() == false)
		{
			FileConversionAction	*Action = _ActionsList.front();
			_ActionsList.pop_front();
			switch (Action->TypeAction)
			{
				case ImportWaveFile :
					Decode(Action->SrcFileName);					
					EnqueueAction(ConvertSampleRate, Action->SrcFileName, "");
					break;
				case ExportWaveFile :
					cout << "[FILECONVERT] Encoding is not supported" << endl;
					break;
				case ConvertSampleRate :
					ConvertSamplerate(Action->SrcFileName);
					break;
				case ImportFile :
					
					break;
				default :
					break;
			}
			delete Action;
		}
		if (GetThread()->TestDestroy() == true)
			break;
		FileConversionMutex.Lock();
	}
	return (GetThread()->Wait());
}

int						FileConversion::GetSndFFormat(PcmType Type)
{
	switch (Type)
	{
		case UInt8:
			return SF_FORMAT_PCM_U8;
		case Int8:
			return SF_FORMAT_PCM_S8;
		case Int16:
			return SF_FORMAT_PCM_16;
		case Int24:
			return SF_FORMAT_PCM_24;
		case Float32:
			return SF_FORMAT_FLOAT;
		default: 
			return SF_FORMAT_PCM_16;
	}
}

void				FileConversion::Decode(string &FileName)
{
	if (_ShouldRun == false)
		return;
	if (_CodecConverter.CanDecode(FileName) == true)
	{
		t_Pcm			Data;
      	wxFileName		RelativeFileName;
      	string			DestFileName;
	  	SNDFILE			*Result = NULL;
		SF_INFO			Info;
		unsigned long	Readen = 0;
		unsigned long 	TotalReaden = 0;
		int				sf_write_result = 0;

		RelativeFileName = FileName.substr(FileName.find_last_of("/"));
		RelativeFileName.SetExt(wxString("wav"));
		DestFileName = _WorkingDir + string("/") + RelativeFileName.GetFullName();
		cout << "file {" << DestFileName.c_str() << "}" << endl;
		
		Data.pcm = new float[2 * _BufferSize];
		bzero(Data.pcm, _BufferSize * 2);
		while ((Readen = _CodecConverter.Decode(FileName, &Data, _BufferSize)) > 0)
		{
			TotalReaden += Readen;
			if (Result == NULL)
			{
		      	Info.samplerate = Data.SampleRate;
				Info.channels = Data.Channels;
				Info.format = 0;
				Info.format |= SF_FORMAT_WAV;	
				Info.format |= GetSndFFormat(Data.PType);
				cout << "samplerate == " << Info.samplerate << ", Channels == " << Info.channels
					<< ", Enum == " << GetSndFFormat(Data.PType) << endl;
				if ((Result = sf_open(DestFileName.c_str(), SFM_WRITE, &Info)) == NULL)
				{
					cout << "[FILECONVERT] Codec 2 - Could not open file " << DestFileName.c_str();
					cout << "} because of error : " << sf_strerror(Result) << endl;
					TotalReaden = 0;
					sf_write_result = 0;
					break;
				}
			}
			if (Data.PType == Float32)
				sf_write_result = sf_writef_float(Result, (float *)Data.pcm,Readen * Info.channels);
			else
				sf_write_result = sf_writef_int(Result, (int *)Data.pcm, Readen * Info.channels);
			bzero(Data.pcm, _BufferSize * 2);				
		}
		_CodecConverter.EndDecode();
		cout << "Total Readen == " << TotalReaden << endl;
		delete (float *)Data.pcm;
		if (!TotalReaden || !sf_write_result)
		{
			cout << "[FILECONVERT] Codec 1 - Could not write decoded file {" << FileName.c_str();
			cout << "} because of error : " << sf_strerror(Result) << endl;
		}
		else
			FileName = DestFileName;
		sf_close(Result);  
	}

}

void				FileConversion::ConvertFromCodec(const string &FileName)
{
	EnqueueAction(ImportWaveFile, FileName, "");
	FileConversionMutex.Unlock();
}

void				FileConversion::ConvertToCodec(const string &FileName)
{
	EnqueueAction(ExportWaveFile, FileName, "");
	FileConversionMutex.Unlock();
}

void				FileConversion::ConvertSamplerate(const string &FileName)
{
	EnqueueAction(ConvertSampleRate, FileName, "");
	FileConversionMutex.Unlock();
}

bool				FileConversion::ConvertSamplerate(string& FileName, bool &HasChangedPath)
{
	int					HasConvertedFile;
    t_samplerate_info	Info;

    HasConvertedFile = _SampleRateConverter.OpenFile(FileName, NULL);
	if (HasConvertedFile == wxID_CANCEL)
		return false;
	else if (HasConvertedFile == wxID_NO)
		return true;
	else if (HasConvertedFile == wxID_YES)
	{
		HasChangedPath = true;
		return true;
	}
	return true;
}

void				FileConversion::EnqueueAction(FileConversionTypeAction ActionType, const string &SrcFile, const string &DstFile)
{
	FileConversionAction	*NewAction = new FileConversionAction;
	
	NewAction->TypeAction = ActionType;
	NewAction->SrcFileName = SrcFile;
	NewAction->DstFileName = DstFile;
	_ActionsList.push_back(NewAction);
}
