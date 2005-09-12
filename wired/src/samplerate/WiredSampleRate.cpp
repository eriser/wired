#include "WiredSampleRate.h"

WiredSampleRate::WiredSampleRate(const WiredSampleRate& copy)
{
	*this = copy;
}

WiredSampleRate			WiredSampleRate::operator=(const WiredSampleRate& right)
{
	if (this != &right)
	{
		_ApplicationSettings.WorkingDirectory = right._ApplicationSettings.WorkingDirectory;
		_ApplicationSettings.SampleRate = right._ApplicationSettings.SampleRate;
		_ApplicationSettings.Format = right._ApplicationSettings.Format;
		_ApplicationSettings.SamplesPerBuffer = right._ApplicationSettings.SamplesPerBuffer;
	}
	return *this;
}

void					WiredSampleRate::Init(t_samplerate_info *Info)
{
	_ApplicationSettings.WorkingDirectory = Info->WorkingDirectory;
	_ApplicationSettings.SampleRate = Info->SampleRate;
	_ApplicationSettings.Format = Info->Format;
	_ApplicationSettings.SamplesPerBuffer = Info->SamplesPerBuffer;
}

int						WiredSampleRate::OpenFile(string& Path)
{
	SNDFILE				*Result;
	SF_INFO				Info;
	int					Res = wxID_NO;
	bool				SameSampleRate, SameFormat;
	
	SaveFile(Path);
	return wxID_CANCEL;
	Info.format = 0;
	if ((Result = sf_open(Path.c_str(), SFM_READ, &Info)) != NULL)
	{
//		cout << "succesfully opened file" << Path.c_str() << endl;
		SameFormat = IsSameFormat(Info.format, _ApplicationSettings.Format);
		SameSampleRate = (int)Info.samplerate == (int)_ApplicationSettings.SampleRate ? true : false;
		if (!SameFormat || !SameSampleRate)
		{
//			cout << "Successfully opend file, info : format == " << Info.format << ", samplerate == " << Info.samplerate 
//					<< ", NbFrames == " << Info.frames << "sections == " << Info.sections << ", Seekable == " << Info.seekable << endl;
			string			strFormats;
			ostringstream	oss;
			
			strFormats = "Would you like to convert your file from ";
			if (!SameFormat)
			{
				strFormats += GetFormatName(Info.format);
				strFormats += " to ";
				strFormats += GetFormatName(_ApplicationSettings.Format);
			}
			if (!SameSampleRate)
			{
				if (!SameFormat)
					strFormats += " and  ";
				strFormats += "samplerate from ";
				oss << Info.samplerate << string(" Hz to ");
				oss << _ApplicationSettings.SampleRate ;
				oss << string(" Hz");
			}
			strFormats += oss.str();
			strFormats += " ?";
			wxMessageDialog msg(NULL, strFormats, "File format mismatch", 
								wxYES_NO | wxCANCEL  | wxICON_QUESTION | wxCENTRE);
			int res = msg.ShowModal();
			
        	if (res == wxID_YES)
        	{
        		if (Convert(&Info, Path, Result))
	        		Res = wxID_YES;
	        	else
	        		Res = wxID_NO;
	        		//Res = wxID_CANCEL;
        	}
        	else if (res  == wxID_CANCEL)
        		Res = wxID_CANCEL;
			msg.Destroy();
		}
		sf_close(Result);
	}
	return Res;
}

int						WiredSampleRate::GetConverterQuality()
{
	wxSingleChoiceDialog	*Dlg;
	wxArrayString			Choices;
	int						Values[NB_SAMPLERATE_QUALITY];
	int						Result;
	wxString				Msg("Please Choose conversion quality (default is better)");
	wxString				Title("Conversion quality");
	
	for (int pos = 0; pos < NB_SAMPLERATE_QUALITY; pos ++)
	{
		Values[pos] = pos;
		Choices.Add(src_get_name(pos));
	}
	
	Dlg = new wxSingleChoiceDialog(NULL, Msg, Title, Choices, (char **)&Values);
	if (Dlg->ShowModal() == wxID_OK)
		Result = (int) Dlg->GetSelectionClientData();
	else 
		Result = 0;
	delete Dlg;
	return Result;
}

float					*WiredSampleRate::ConvertSampleRate(SRC_STATE* Converter, float *Input, unsigned long FrameNb, double Ratio, unsigned long &ToWrite, bool End, int NbChannels, unsigned long &ReallyReaden)
{
	int					res = 0;
	SRC_DATA			Data;
	
	Data.data_in = Input;
	Data.input_frames = (long) FrameNb;
	Data.output_frames = (long) FrameNb;
	Data.data_out = new float[FrameNb * NbChannels];
	Data.src_ratio = Ratio;
	if (End == false)
		Data.end_of_input = 0;
	else
		Data.end_of_input = 1;
	res = src_process(Converter, &Data);
	if (res)
	{
		cout << "An error occured while trying to convert file : " << src_strerror(res) << endl;
		return NULL;
	}
	else
	{
		ToWrite = Data.output_frames_gen;
		ReallyReaden = Data.input_frames_used;
		return Data.data_out;
	}
}

bool					WiredSampleRate::Convert(SF_INFO *SrcInfo, string& SrcFile, SNDFILE *SrcData)
{
	SNDFILE				*Result;
	SF_INFO				Info;	
	string				DestFileName;
	int					ConversionQuality;
	
	if (SrcFile.find("/") != SrcFile.npos)
	{
		DestFileName = _ApplicationSettings.WorkingDirectory + SrcFile.substr(SrcFile.find_last_of("/"));
		Info.samplerate = _ApplicationSettings.SampleRate;
		Info.channels = SrcInfo->channels;
		Info.format = SrcInfo->format;
		Info.format |= GetFileFormat(_ApplicationSettings.Format);
		if ((Result = sf_open(DestFileName.c_str(), SFM_WRITE, &Info)))
		{
			ConversionQuality = GetConverterQuality();
			wxProgressDialog *ProgressBar = new wxProgressDialog("Converting wave file", "Please wait...", 
											SrcInfo->frames , NULL, wxPD_SMOOTH | wxPD_ELAPSED_TIME |
											wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_APP_MODAL);
			if (sf_seek(SrcData, 0, SEEK_SET) != -1)
			{
				float		*Buffer, *Output;
				sf_count_t	Readen = 0;
				int			NbLoop = 0;
				double 		Ratio = (double)  _ApplicationSettings.SampleRate / SrcInfo->samplerate;
				bool		HasFailed = false;
				unsigned long ToWrite, ReallyReaden;
				int			ConverterError;
				SRC_STATE*	Converter = src_new(ConversionQuality, SrcInfo->channels, &ConverterError);
				
				ProgressBar->Update(NbLoop, "", &HasFailed);
				Buffer = new float[_ApplicationSettings.SamplesPerBuffer * Info.channels];
				while ((Readen = sf_readf_float(SrcData, Buffer, _ApplicationSettings.SamplesPerBuffer)) > 0)
				{
					if (Ratio != 1)
					{
						Output = ConvertSampleRate(Converter, Buffer, (unsigned long) Readen, Ratio, ToWrite, (Readen < _ApplicationSettings.SamplesPerBuffer ? true : false) , SrcInfo->channels, ReallyReaden);
						if (!Output)
							HasFailed = true;
					}
					else
						Output = Buffer;
					if (!HasFailed)
					{
						if (sf_writef_float(Result, Output, ToWrite) != ToWrite)
						{
							cout << "An error occured while writing to file " << DestFileName.c_str() << endl;
							HasFailed = true;
						}
						delete Output;
						if (Readen > ReallyReaden)
							sf_seek(SrcData, ReallyReaden - Readen, SEEK_CUR);
					}
					NbLoop += ReallyReaden;
					if (HasFailed || ProgressBar->Update(NbLoop) == false)
					{
						delete ProgressBar;
						sf_close(Result);
						delete Buffer;
						return false;
					}
				}
				if (sf_error(SrcData) != SF_ERR_NO_ERROR)
				{
					delete ProgressBar;
					sf_close(Result);
					delete Buffer;
					return false;
				}
//				cout << "sndfile error {" << sf_strerror(SrcData) << "}" << endl;
//				src_delete(Converter);
//				delete Buffer;
			
			}
			delete ProgressBar;
			sf_close(Result);
		}
		else
		{
			cout << "Error while loading file !" << endl;
			return false;
		}
		chmod(DestFileName.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		SrcFile = DestFileName;
		return true;
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
		if (_FormatTypes[pos].PaFormat & PaFormat)
			return _FormatTypes[pos].FormatName;
	return STR_UNKNOWN_FORMAT;
}

int						WiredSampleRate::GetFileFormat(PaSampleFormat PaFormat)
{
	int		pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0; pos++)
		if (_FormatTypes[pos].PaFormat & PaFormat)
			return _FormatTypes[pos].SndFileFormat;
	return SF_FORMAT_PCM_32; // default value is float 32 bits
}

void					WiredSampleRate::ChooseFileFormat(SF_INFO *DestInfo)
{
	wxSize				DialogSize(350, 150);
	wxSize				ComboSize(140, 20);
	wxPoint				ComboPos(40, 70);
	wxDialog 			Dialog(NULL, -1, "File format selection", wxDefaultPosition, DialogSize, 
								wxCENTRE | wxCAPTION | wxTHICK_FRAME | wxSTAY_ON_TOP);
	wxStaticText		*DialogText;
	wxButton			*DialogButton;
	int					pos;
	
	DialogButton = new wxButton(&Dialog, wxID_OK, "", wxPoint(135, 110));
	DialogText = new wxStaticText(&Dialog, -1, wxString("Please select the options that\nyou want for the downmix.\n(Defaults are current project's configuration)"),
									wxPoint(13, 10), wxSize(-1, -1), wxALIGN_CENTER);
	wxComboBox			*CBFormat = new wxComboBox(&Dialog, -1, "", ComboPos, ComboSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0;pos++)
		CBFormat->Append(wxString(_FormatTypes[pos].FormatName));
	ComboPos.x = 200;
	ComboSize.x = 100;
	wxComboBox			*CBSampleRate = new wxComboBox(&Dialog, -1, "", ComboPos, ComboSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	for (pos = 0; _SampleRateTypes[pos].SampleRate != 0;pos++)
		CBSampleRate->Append(wxString(_SampleRateTypes[pos].SampleRateName));
	Dialog.SetReturnCode(wxID_OK);
	if (Dialog.ShowModal() == wxID_OK)
	{
		for (pos = 0; _FormatTypes[pos].SndFileFormat != 0;pos++)
			if (CBFormat->GetValue().compare(_FormatTypes[pos].FormatName) == 0)
			{
				DestInfo->format = _FormatTypes[pos].SndFileFormat;
				break;
			}
		for (pos = 0; _SampleRateTypes[pos].SampleRate != 0;pos++)
			if (CBSampleRate->GetValue().compare(_SampleRateTypes[pos].SampleRateName) == 0)
			{
				DestInfo->samplerate = _SampleRateTypes[pos].SampleRate;
				break;
			}
	}
	delete CBFormat;
	delete CBSampleRate;
	delete DialogText;
	delete DialogButton;
}

int						WiredSampleRate::SaveFile(string& Path)
{
	wxFile				File;
	int					Res = wxID_CANCEL;
	
	if (File.Exists(Path.c_str()))
	{
		wxMessageDialog msg(NULL, string("The file ") + Path + " already exists on the filesystem.\n Do you want to overwrite it ?" , 
							"File already exists", wxYES | wxICON_QUESTION | wxCENTRE);
		if (msg.ShowModal() ==  wxID_YES)
		{
			SF_INFO		FileInfo;
			SNDFILE		*FileData;
			
			ChooseFileFormat(&FileInfo);
			FileInfo.channels = 2;
			if ((FileData = sf_open(Path.c_str(), SFM_WRITE, &FileInfo)) != NULL)
			{
				
			}
		}
	}
	return Res;
}
