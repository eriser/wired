#include <wx/filename.h>
#include <sndfile.h>
#include <strings.h>

#include "FileConversion.h"
#include "../undo/cAddTrackAction.h"
#include "../undo/cImportMidiAction.h"
#include "../sequencer/Sequencer.h"

FileConversion::FileConversion()// : wxThread()
{
	_ShouldRun = false;
}

FileConversion::~FileConversion()
{
	//Delete();
}

FileConversion		FileConversion::operator=(const FileConversion& right)
{
	if (this != &right)
	{
		_ShouldRun = right._ShouldRun;
		_CodecConverter = right._CodecConverter;
		_SampleRateConverter = right._SampleRateConverter;
		_CodecsExtensions = right._CodecsExtensions;
		_WorkingDir = right._WorkingDir;
		_BufferSize = right._BufferSize;
		_ActionsList = right._ActionsList;
		_Parent = right._Parent;
	}
	return *this;
}

bool				FileConversion::Init(t_samplerate_info *RateInit, string WorkingDir, unsigned long BufferSize, wxWindow *Parent)
{
	_WorkingDir = WorkingDir;
	_BufferSize = BufferSize;
	_Parent = Parent;
	cout << "[FILECONVERT] Loading Codec management..." << endl;
	_CodecConverter.Init();
	list<string>			CodecsListExtensions = _CodecConverter.GetExtension(DECODE);
	list<string>::iterator	Iter;

	for (Iter = CodecsListExtensions.begin(); Iter != CodecsListExtensions.end(); Iter++)
  		_CodecsExtensions.insert(_CodecsExtensions.end(), *Iter);
  	cout << "...done" << endl << "[FILECONVERT] Loading samplerate management..." << endl;
	_SampleRateConverter.Init(RateInit);
	cout << "...done" << endl;
	_ShouldRun = true;
	//int res = Create();
	//if (res != wxTHREAD_NO_ERROR)
		//return false;
	return true;
}

vector<string>		*FileConversion::GetCodecsExtensions()
{
	return &_CodecsExtensions;
}

void				FileConversion::Stop()
{
	FileConversionMutex.Unlock();
	//Delete();
}

void				*FileConversion::Entry()
{
	cout << "[FILECONVERT] Thread started !" << endl;
	bool HasChangedPath;
	
	while(_ShouldRun)
	{
		FileConversionMutex.Lock();
		cout << "[FILECONVERT] Thread working !" << endl;
		if (_ActionsList.empty() == false)
		{
			FileConversionAction	*Action = _ActionsList.front();
			_ActionsList.pop_front();
			cout << "[FILECONVERT] Thread working ! 1" << endl;
			switch (Action->TypeAction)
			{
				case AImportWaveFile :
					Decode(Action->SrcFileName);
					break;
				case AExportWaveFile :
					cout << "[FILECONVERT] Encoding is not supported" << endl;
					break;
				case AConvertSampleRate :
					cout << "[FILECONVERT] Thread working ! Samplerate" << endl;
					HasChangedPath = false;
					ConvertSamplerate(Action->SrcFileName, HasChangedPath);					
					cout << "[FILECONVERT] Thread working ! Samplerate ended" << endl;
					break;
				case AImportFile :				
					cout << "[FILECONVERT] Thread working ! import file" << endl;
					SeqMutex.Lock();
					ImportWavePattern(Action->SrcFileName);					
					SeqMutex.Unlock();
					cout << "[FILECONVERT] Thread working ! import file ended" << endl;
					break;
				default :
//					FileConversionMutex.Lock();
					break;
			}
			cout << "[FILECONVERT] Thread working ! 2" << endl;
			if (Action->SrcFileName)
				delete Action->SrcFileName;
			if (Action->DstFileName)
				delete Action->DstFileName;
			delete Action;
			FileConversionMutex.Unlock();
			cout << "[FILECONVERT] Thread has finished his work !" << endl;
		}
		else
		{
			cout << "[FILECONVERT] Thread has no work !" << endl;
			//FileConversionMutex.Lock();
		}
//		if (TestDestroy() == true)
//			break;
	}
	cout << "[FILECONVERT] Thread killed !" << endl;
// return (Wait());
	return NULL;
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

void				FileConversion::Decode(string *FileName)
{
	if (_ShouldRun == false)
		return;
	if (_CodecConverter.CanConvert(string(FileName->c_str()), DECODE) == true)
	{
		string			FileNameLocal(FileName->c_str());
		t_Pcm			Data;
      	wxFileName		RelativeFileName;
      	string			DestFileName;
	  	SNDFILE			*Result = NULL;
		SF_INFO			Info;
		unsigned long	Readen = 0;
		unsigned long 	TotalReaden = 0;
		int				sf_write_result = 0;

		RelativeFileName = FileNameLocal.substr(FileNameLocal.find_last_of("/"));
		RelativeFileName.SetExt(wxString("wav"));
		DestFileName = _WorkingDir + string("/") + RelativeFileName.GetFullName();
//		cout << "file {" << DestFileName.c_str() << "}" << endl;
		
		Data.pcm = new float[2 * _BufferSize];
		bzero(Data.pcm, _BufferSize * 2);
		while ((Readen = _CodecConverter.Decode(FileNameLocal, &Data, _BufferSize)) > 0)
		{
//			cout << "000 file {" << DestFileName.c_str() << "}" << endl;
			TotalReaden += Readen;
			if (Result == NULL)
			{
		      	Info.samplerate = Data.SampleRate;
				Info.channels = Data.Channels;
				Info.format = 0;
				Info.format |= SF_FORMAT_WAV;	
				Info.format |= GetSndFFormat(Data.PType);
//				cout << "samplerate == " << Info.samplerate << ", Channels == " << Info.channels
//					<< ", Enum == " << GetSndFFormat(Data.PType) << endl;
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
		
//		cout << "01 file {" << DestFileName.c_str() << "}" << endl;
		_CodecConverter.EndDecode();
//		cout << "Total Readen == " << TotalReaden << endl;
		delete[] (float *)Data.pcm;
		if (!TotalReaden || !sf_write_result)
		{
			cout << "[FILECONVERT] Codec 1 - Could not write decoded file {" << FileName->c_str();
			cout << "} because of error : " << sf_strerror(Result) << endl;
		}
		else
			*FileName = DestFileName;
		sf_close(Result);  
	}

}

void				FileConversion::ConvertFromCodec(string *FileName)
{
	Decode(FileName);
//	EnqueueAction(AImportWaveFile, FileName, NULL);
	FileConversionMutex.Unlock();
}

void				FileConversion::ConvertToCodec(string *FileName)
{
	EnqueueAction(AExportWaveFile, FileName, NULL);
	FileConversionMutex.Unlock();
}

void				FileConversion::ConvertSamplerate(string *FileName)
{
	bool HasChangedPath;
	ConvertSamplerate(FileName, HasChangedPath);	
//	EnqueueAction(AConvertSampleRate, FileName, NULL);
	FileConversionMutex.Unlock();
}

bool				FileConversion::ConvertSamplerate(string *FileName, bool &HasChangedPath)
{
	int					HasConvertedFile;

    HasConvertedFile = _SampleRateConverter.OpenFile(FileName, _Parent);
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

void				FileConversion::ImportWaveFile(string *FileName)
{
	string file(FileName->c_str());
	SeqMutex.Lock();
	ImportWavePattern(FileName);
	SeqMutex.Unlock();
//	EnqueueAction(AImportFile, FileName, NULL);
	FileConversionMutex.Unlock();
}

void				FileConversion::EnqueueAction(FileConversionTypeAction ActionType, string *SrcFile, string *DstFile)
{
	FileConversionAction	*NewAction = new FileConversionAction;
	
	NewAction->TypeAction = ActionType;
	NewAction->SrcFileName = SrcFile == NULL ? NULL : new string(SrcFile->c_str());
	NewAction->DstFileName = DstFile == NULL ? NULL : new string(DstFile->c_str());
	_ActionsList.push_back(NewAction);
	FileConversionMutex.Unlock();
}

void				FileConversion::ImportWavePattern(string *FileName)
{
//	wxProgressDialog *Progress = new wxProgressDialog("Loading wave file", "Please wait...", 100, 
//														NULL, wxPD_AUTO_HIDE | wxPD_CAN_ABORT 
//														| wxPD_REMAINING_TIME);
//	Progress->Update(1);
	string filename(FileName->c_str());
	cActionManager::Global().AddImportWaveAction(filename, true, true);
//	Progress->Update(80);
	//CreateUndoRedoMenus(EditMenu); // ??
//	Progress->Update(99);
//	delete Progress;
}

void				FileConversion::CopyToWorkingDir(string *FileName)
{

	wxMessageDialog msg(_Parent, _("Do you want to copy this file to your project directory ?"), "Wired", 
						wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE);
	int res = msg.ShowModal();
	if (res == wxID_OK)
	{
		wxFileName fn(FileName->c_str());
	  
		fn.SetPath(_WorkingDir);
		cout << "[FILECONVERT] Will copy File {" << FileName->c_str() << "} TO {" << fn.GetFullPath().c_str() << "}" << endl;
		if (!wxCopyFile(FileName->c_str(), fn.GetFullPath().c_str()))
		{
			wxMessageDialog copymsg(_Parent, _("Could not copy file"), "Wired", wxOK | wxICON_EXCLAMATION | wxCENTRE);
			copymsg.ShowModal();
		}
		else
			*FileName = fn.GetFullPath().c_str();
	}
}
