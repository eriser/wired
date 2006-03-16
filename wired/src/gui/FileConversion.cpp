#include <wx/filename.h>
#include <sndfile.h>
#include <strings.h>
#include <wx/progdlg.h>

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

bool				FileConversion::Init(t_samplerate_info *RateInit, wxString WorkingDir, unsigned long BufferSize, wxWindow *Parent)
{
	_WorkingDir = WorkingDir;
	_BufferSize = BufferSize;
	_Parent = Parent;
	cout << "[FILECONVERT] Loading Codec management..." << endl;
	//_CodecConverter.Init();
	//list<string>			CodecsListExtensions = _CodecConverter.GetExtension(DECODE);
	//list<string>::iterator	Iter;

//	for (Iter = CodecsListExtensions.begin(); Iter != CodecsListExtensions.end(); Iter++)
//  		_CodecsExtensions.insert(_CodecsExtensions.end(), *Iter);
  	cout << "...done" << endl << "[FILECONVERT] Loading samplerate management..." << endl;
	_SampleRateConverter.Init(RateInit);
	cout << "...done" << endl;
	_ShouldRun = true;
	//int res = Create();
	//if (res != wxTHREAD_NO_ERROR)
		//return false;
	return true;
}

vector<wxString>		*FileConversion::GetCodecsExtensions()
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

SNDFILE			*FileConversion::OpenDecodeFile(t_Pcm	&Data, const wxString &DestFileName, 
																						SF_INFO &Info, unsigned long *TotalReaden, int *sf_write_result)
{
  SNDFILE					*Result = NULL;

 	Info.samplerate = Data.SampleRate;
	Info.channels = Data.Channels;
	Info.format = 0;
	Info.format |= SF_FORMAT_WAV;	
	Info.format |= GetSndFFormat(Data.PType);
	if ((Result = sf_open(DestFileName.mb_str(*wxConvCurrent), SFM_WRITE, &Info)) == NULL)
	{
		cout << "[FILECONVERT] Codec 2 - Could not open file " << DestFileName.c_str();
		cout << "} because of error : " << sf_strerror(Result) << endl;
		*TotalReaden = 0;
		*sf_write_result = 0;
	}
	return Result;
}

bool				FileConversion::Decode(wxString *FileName)
{
    return false; // temp....
    bool        Res= true;
    
	if (_ShouldRun == false)
		return false;
	if (_CodecConverter.CanConvert(wxString(FileName->c_str()), DECODE) == true)
	{
		wxString						FileNameLocal(FileName->c_str());
		t_Pcm							Data;
    wxFileName				RelativeFileName;
    wxString						DestFileName;
	  SNDFILE						*Result = NULL;
		SF_INFO						Info;
		unsigned long			Readen = 0;
		unsigned long 		TotalReaden = 0;
		int								sf_write_result = 0;
		wxProgressDialog	ProgressDialog(_("Conversion"), _("Decoding file, please wait."), 
																		PROGRESS_DIALOG_UNIT, NULL, wxPD_SMOOTH | wxPD_ELAPSED_TIME |
																		wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_APP_MODAL);

		RelativeFileName = FileNameLocal.substr(FileNameLocal.find_last_of(wxT("/")));
		RelativeFileName.SetExt(wxString(wxT("wav"), *wxConvCurrent));
		DestFileName = _WorkingDir + wxString(wxT("/"), *wxConvCurrent) + RelativeFileName.GetFullName();
		Data.pcm = new float[_BufferSize];
		bzero(Data.pcm, _BufferSize);
		while ((Readen = _CodecConverter.Decode(FileNameLocal, &Data, _BufferSize)) > 0)
		{
			TotalReaden += Readen;
			if (Result == NULL)
				if ((Result = OpenDecodeFile(Data, DestFileName, Info, &TotalReaden, &sf_write_result)) == NULL) break;
			ProgressDialog.Update((int)(TotalReaden * PROGRESS_DIALOG_UNIT / Data.TotalSample));
			if (Data.PType == Float32)
				sf_write_result = sf_write_float(Result, (float *)Data.pcm,Readen);
			else
				sf_write_result = sf_writef_int(Result, (int *)Data.pcm, Readen);
			bzero(Data.pcm, _BufferSize);
		}
		
		_CodecConverter.EndDecode();
		delete[] (float *)Data.pcm;
		if (!TotalReaden || !sf_write_result)
		{
			cout << "[FILECONVERT] Codec 1 - Could not write decoded file {" << FileName->c_str();
			cout << "} because of error : " << sf_strerror(Result) << endl;
            Res = false;
		}
		else
			*FileName = DestFileName;
		ProgressDialog.Update(PROGRESS_DIALOG_UNIT);
		sf_close(Result);  
	}
    return Res;
}

bool				FileConversion::ConvertFromCodec(wxString *FileName)
{
	return Decode(FileName);
//	EnqueueAction(AImportWaveFile, FileName, NULL);
	//FileConversionMutex.Unlock();
}

void				FileConversion::ConvertToCodec(wxString *FileName)
{
	EnqueueAction(AExportWaveFile, FileName, NULL);
	FileConversionMutex.Unlock();
}

bool				FileConversion::ConvertSamplerate(wxString *FileName)
{
	bool HasChangedPath;
	return ConvertSamplerate(FileName, HasChangedPath);	
//	EnqueueAction(AConvertSampleRate, FileName, NULL);
//	FileConversionMutex.Unlock();
}

bool				FileConversion::ConvertSamplerate(wxString *FileName, bool &HasChangedPath)
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

void				FileConversion::ImportWaveFile(wxString *FileName)
{
	wxString file(FileName->c_str());
	SeqMutex.Lock();
	ImportWavePattern(FileName);
	SeqMutex.Unlock();
//	EnqueueAction(AImportFile, FileName, NULL);
	FileConversionMutex.Unlock();
}

void				FileConversion::EnqueueAction(FileConversionTypeAction ActionType, wxString *SrcFile, wxString *DstFile)
{
	FileConversionAction	*NewAction = new FileConversionAction;
	
	NewAction->TypeAction = ActionType;
	NewAction->SrcFileName = SrcFile == NULL ? NULL : new wxString(SrcFile->c_str());
	NewAction->DstFileName = DstFile == NULL ? NULL : new wxString(DstFile->c_str());
	_ActionsList.push_back(NewAction);
	FileConversionMutex.Unlock();
}

void				FileConversion::ImportWavePattern(wxString *FileName)
{
//	wxProgressDialog *Progress = new wxProgressDialog("Loading wave file", "Please wait...", 100, 
//														NULL, wxPD_AUTO_HIDE | wxPD_CAN_ABORT 
//														| wxPD_REMAINING_TIME);
//	Progress->Update(1);
    std::string temp(FileName->mb_str(*wxConvCurrent));
	cActionManager::Global().AddImportWaveAction(temp, true, true);
//	Progress->Update(80);
	//CreateUndoRedoMenus(EditMenu); // ??
//	Progress->Update(99);
//	delete Progress;
}

void				FileConversion::CopyToWorkingDir(wxString *FileName)
{

	wxMessageDialog msg(_Parent, _("Do you want to copy this file to your project directory ?"), wxT("Wired"), 
						wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE);
	int res = msg.ShowModal();
	if (res == wxID_OK)
	{
		wxFileName fn(FileName->c_str());
	  
		fn.SetPath(_WorkingDir);
		cout << "[FILECONVERT] Will copy File {" << FileName->c_str() << "} TO {" << fn.GetFullPath().c_str() << "}" << endl;
		if (!wxCopyFile(FileName->c_str(), fn.GetFullPath().c_str()))
		{
			wxMessageDialog copymsg(_Parent, _("Could not copy file"), wxT("Wired"), wxOK | wxICON_EXCLAMATION | wxCENTRE);
			copymsg.ShowModal();
		}
		else
			*FileName = fn.GetFullPath().c_str();
	}
}
