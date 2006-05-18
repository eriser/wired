// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WiredSampleRate.h"

static wxMutex				SampleRateMutex;

WiredSampleRate::WiredSampleRate()
{
	OpenedFile = NULL;
	StaticConverter = NULL;	
}

WiredSampleRate::~WiredSampleRate()
{
	if (OpenedFile)
		EndSaveFile(2);
}

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
		OpenedFile = right.OpenedFile;
		OpenedFileInfo = right.OpenedFileInfo;
		StaticConverter = right.StaticConverter;
		_Quality = right._Quality;
		_ConverterError = right._ConverterError;
		_Buffer = right._Buffer;
        _RateBuffer = right._RateBuffer;
	}
	return *this;
}

void					WiredSampleRate::Init(t_samplerate_info *Info)
{
	_ApplicationSettings.WorkingDirectory = Info->WorkingDirectory;
	_ApplicationSettings.SampleRate = Info->SampleRate;
	_ApplicationSettings.Format = Info->Format;
	_ApplicationSettings.SamplesPerBuffer = Info->SamplesPerBuffer;
	StaticConverter = NULL;
		
}

int						WiredSampleRate::OpenFile(wxString *Path, wxWindow* parent)
{
	SNDFILE				*Result;
	SF_INFO				Info;
	int					Res = wxID_NO;
	bool				SameSampleRate, SameFormat;
	
	Info.format = 0;
	if ((Result = sf_open(Path->mb_str(*wxConvCurrent), SFM_READ, &Info)) != NULL)
	{
		SameFormat = IsSameFormat(Info.format, _ApplicationSettings.Format);
		SameSampleRate = (int)Info.samplerate == (int)_ApplicationSettings.SampleRate ? true : false;
		if (!SameFormat || !SameSampleRate)
		{
			wxString			strFormats(_("Would you like to convert your file from "));
			wxChar				buf[1024];
			int					res;
            
			if (!SameFormat)
			{
				strFormats += GetFormatName(Info.format);
				strFormats += _(" to ");
				strFormats += GetFormatName(_ApplicationSettings.Format);
			}
			if (!SameSampleRate)
			{
				if (!SameFormat)
					strFormats += _(" and  ");
				strFormats += _("samplerate ");
				wxSnprintf(buf, 1024, _("%d Hz to %ld Hz ?"), Info.samplerate, _ApplicationSettings.SampleRate);
			}
			strFormats += buf;

			wxMessageDialog msg(parent, strFormats, _("File format mismatch"), 
								wxYES_NO | wxCANCEL  | wxICON_QUESTION | wxCENTRE);
			res = msg.ShowModal();
        	if (res == wxID_YES)
        	{
        		if (Convert(&Info, *Path, Result))
	        		Res = wxID_YES;
	        	else
	        		Res = wxID_NO;
        	}
        	else if (res  == wxID_CANCEL)
        		Res = wxID_CANCEL;
		}
		sf_close(Result);
	}
	return Res;
}

int						WiredSampleRate::GetConverterQuality()
{
	wxArrayString			Choices;
	int						Result;
	wxString				Msg(_("Please Choose conversion quality (default is better)"));
	wxString				Title(_("Conversion quality"));
	
	for (int pos = 0; pos < NB_SAMPLERATE_QUALITY; pos ++)
	{
		Choices.Add(wxString(src_get_name(pos), *wxConvCurrent));
	}

	wxSingleChoiceDialog	Dlg(NULL, Msg, Title, Choices, NULL);
	if (Dlg.ShowModal() == wxID_OK)
		Result = Dlg.GetSelection();
	else 
		Result = 0;
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
		cout << "[FILECONVERT] An error occured while trying to convert file : " << src_strerror(res) << endl;
		return NULL;
	}
	else
	{
		ToWrite = Data.output_frames_gen;
		ReallyReaden = Data.input_frames_used;
		return Data.data_out;
	}
}

bool					WiredSampleRate::Convert(SF_INFO *SrcInfo, wxString& SrcFile, SNDFILE *SrcData)
{
	
	SNDFILE				*Result;	
	SF_INFO				Info;	
	wxString				DestFileName;
	int					ConversionQuality;
	
	if (SrcFile.find(wxT("/")) != SrcFile.npos)
	{
		DestFileName = _ApplicationSettings.WorkingDirectory + SrcFile.substr(SrcFile.find_last_of(wxT("/")));
		Info.samplerate = _ApplicationSettings.SampleRate;
		Info.channels = SrcInfo->channels;
		Info.format = SrcInfo->format;
		Info.format |= GetFileFormat(_ApplicationSettings.Format);
		if ((Result = sf_open(DestFileName.mb_str(*wxConvCurrent), SFM_WRITE, &Info)))
		{
			ConversionQuality = GetConverterQuality();
			wxProgressDialog ProgressBar(_("Converting wave file"), _("Please wait..."), 
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
				
				ProgressBar.Update(NbLoop, wxT(""), &HasFailed);
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
							cout << "[FILECONVERT] An error occured while writing to file " << DestFileName.c_str() << endl;
							HasFailed = true;
						}
						delete Output;
						if (Readen > ReallyReaden)
							sf_seek(SrcData, ReallyReaden - Readen, SEEK_CUR);
					}
					NbLoop += ReallyReaden;
					if (HasFailed || ProgressBar.Update(NbLoop) == false)
					{
						sf_close(Result);
						delete[] Buffer;
						return false;
					}
				}
				if (sf_error(SrcData) != SF_ERR_NO_ERROR)
				{
					cout << "[FILECONVERT] Sndfile error {" << sf_strerror(SrcData) << "}" << endl;
					sf_close(Result);
					delete[] Buffer;
					src_delete(Converter);
					return false;
				}
				src_delete(Converter);
				delete[] Buffer;
			
			}
			sf_close(Result);
		}
		else
		{
			cout << "[FILECONVERT] Error while loading file !" << endl;
			return false;
		}
		chmod(DestFileName.mb_str(*wxConvCurrent), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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

const wxChar*				WiredSampleRate::GetFormatName(int SndFileFormat)
{
	int		pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0; pos++)
		if (_FormatTypes[pos].SndFileFormat & SndFileFormat)
			return (const wxChar *)_FormatTypes[pos].FormatName;
	return STR_UNKNOWN_FORMAT;
}

const wxChar*				WiredSampleRate::GetFormatName(PaSampleFormat PaFormat)
{	
	int		pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0; pos++)
		if (_FormatTypes[pos].PaFormat & PaFormat)
			return (const wxChar *)_FormatTypes[pos].FormatName;
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
	wxDialog 			Dialog(NULL, -1, _("File format selection"), wxDefaultPosition, DialogSize, 
								wxCENTRE | wxCAPTION | wxTHICK_FRAME | wxSTAY_ON_TOP);	
	wxStaticText		DialogText(&Dialog, -1, wxString(_("Please select the options that\nyou want for the downmix.\n(Defaults are current project's configuration)")),
									wxPoint(13, 10), wxSize(-1, -1), wxALIGN_CENTER);
	wxButton			DialogButton(&Dialog, wxID_OK, wxT(""), wxPoint(135, 110));
	wxComboBox			CBFormat(&Dialog, -1, wxT(""), ComboPos, ComboSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	int					pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0;pos++)
		CBFormat.Append(wxString(_FormatTypes[pos].FormatName, *wxConvCurrent));
	ComboPos.x = 200;
	ComboSize.x = 100;
	wxComboBox			CBSampleRate(&Dialog, -1, wxT(""), ComboPos, ComboSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	for (pos = 0; _SampleRateTypes[pos].SampleRate != 0;pos++)
		CBSampleRate.Append(wxString(_SampleRateTypes[pos].SampleRateName, *wxConvCurrent));
	Dialog.SetReturnCode(wxID_OK);
	if (Dialog.ShowModal() == wxID_OK)
	{
		for (pos = 0; _FormatTypes[pos].SndFileFormat != 0;pos++)
			if (CBFormat.GetValue().compare(wxString(_FormatTypes[pos].FormatName, *wxConvCurrent)) == 0)
			{
				DestInfo->format = _FormatTypes[pos].SndFileFormat;
				break;
			}
		for (pos = 0; _SampleRateTypes[pos].SampleRate != 0;pos++)
			if (CBSampleRate.GetValue().compare(wxString(_SampleRateTypes[pos].SampleRateName, *wxConvCurrent)) == 0)
			{
				DestInfo->samplerate = _SampleRateTypes[pos].SampleRate;
				break;
			}
	}
}

bool					WiredSampleRate::SaveFile(wxString& Path, unsigned int NbChannel, unsigned long NbSamples, bool interleaved)
{
	wxFile				File;
	
	ChooseFileFormat(&OpenedFileInfo);
	OpenedFileInfo.channels = NbChannel;
	OpenedFileInfo.format |= SF_FORMAT_WAV;
	if (sf_format_check(&OpenedFileInfo) == false)
		cout << "[FILECONVERT] An error occured while trying to open a file, bad parameters)" << endl;
	if ((OpenedFile = sf_open(Path.mb_str(*wxConvCurrent), SFM_WRITE, &OpenedFileInfo)) == NULL)
	{
		cout << "[FILECONVERT] An error occured while trying to open file (details :" << sf_strerror(OpenedFile) << ")" << endl;
		return false;
	}
    sf_command (OpenedFile, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE);
	_Quality = GetConverterQuality();
	StaticConverter = src_new(_Quality, (interleaved == true ? NbChannel : 1), &_ConverterError);
	_Buffer = new float[NbChannel * NbSamples];
    _RateBuffer = new float[NbChannel * NbSamples];
	return true;
}

//non interleaved
float					*WiredSampleRate::ConvertnChannels(float **Input, unsigned int NbChannels, SRC_STATE *Converter, unsigned long NbSamples, double Ratio, int End, unsigned long &ToWrite)
{
	SRC_DATA			Data;
	unsigned int		CurrentChannel;
	int					res = 0;

	for (CurrentChannel = 0; CurrentChannel < NbChannels; CurrentChannel++)
	{
		Data.data_in = Input[CurrentChannel];
		Data.input_frames = NbSamples;
		Data.output_frames = NbSamples;
		Data.src_ratio = Ratio;
		Data.end_of_input = End;
		if ((res = src_process(Converter, &Data)))
		{
			cout << "[FILECONVERT] An error occured while trying to convert file (details :" << src_strerror(res) << ")" << endl;
			return NULL;
		}
		ToWrite = Data.output_frames_gen;
	}

	unsigned long		CurrentSample, CurrentResSample;
	
	bzero(_Buffer, NbChannels * ToWrite);	
	ToWrite *= NbChannels;
	return _Buffer;
}
// interleaved
float					*WiredSampleRate::ConvertnChannels(float *Input, unsigned int NbChannels, SRC_STATE *Converter, unsigned long NbSamples, double Ratio, int End, unsigned long &ToWrite)
{
	SRC_DATA	Data;
	unsigned int	CurrentPos = 0;
	int				res = 0;

    while (1)
    {
    	bzero(_RateBuffer, NbChannels * NbSamples);
    	Data.data_in = Input + (CurrentPos * NbChannels);
    	Data.input_frames = NbSamples - CurrentPos;
    	Data.output_frames = NbSamples;
    	Data.data_out = _RateBuffer;
    	Data.src_ratio = Ratio;
    	Data.end_of_input = End;
    	if ((res = src_process(Converter, &Data)))
    	{
    		cout << "[FILECONVERT] An error occured while trying to convert file (details :" << src_strerror(res) << ")" << endl;
    		return NULL;
    	}    
        else
        {
            if (sf_writef_float(OpenedFile, _RateBuffer, Data.output_frames_gen) != Data.output_frames_gen)
            {
                cout << "[FILECONVERT] An error occured while writing to file " << endl;
                return NULL;
            }
                
        }
        if (!Data.output_frames_gen)
            break;
        else
            CurrentPos += Data.input_frames_used;
    }
	return _RateBuffer;
}

//non interleaved, used in Seqencer to export Wave
void					WiredSampleRate::WriteToFile(unsigned long NbSamples, float **Buffer, unsigned int NbChannel)
{
	float		*Output = NULL;
	double 		Ratio = (double)  OpenedFileInfo.samplerate / _ApplicationSettings.SampleRate;
	unsigned long ToWrite;
	unsigned int Chans = 0;

	unsigned long		CurrentSample;
	bzero(_Buffer, NbChannel * NbSamples);
	for (CurrentSample = 0, ToWrite = 0; CurrentSample < NbSamples; CurrentSample++)
		for (Chans = 0; Chans < NbChannel; Chans++)
			_Buffer[ToWrite++] = Buffer[Chans][CurrentSample];
	Output = ConvertnChannels(_Buffer, NbChannel, StaticConverter, NbSamples, Ratio, (NbSamples < _ApplicationSettings.SamplesPerBuffer ? 1 : 0), ToWrite);	
	if (sf_error(OpenedFile) != SF_ERR_NO_ERROR)
		cout << "[FILECONVERT] sndfile error {" << sf_strerror(OpenedFile) << "}" << endl;
}

//interleaved
void					WiredSampleRate::WriteToFile(unsigned long NbSamples, float *Buffer, unsigned int NbChannel)
{
	float		*Output = NULL;
	double 		Ratio = (double)  _ApplicationSettings.SampleRate / OpenedFileInfo.samplerate;
	unsigned long ToWrite;

	Output = ConvertnChannels(Buffer, NbChannel, StaticConverter, NbSamples, Ratio, (NbSamples < _ApplicationSettings.SamplesPerBuffer ? 1 : 0), ToWrite);
	if (Output)
	{
		if (sf_writef_float(OpenedFile, Output, ToWrite) != ToWrite)
			cout << "[FILECONVERT] An error occured while writing to file " << endl;
	}
	if (sf_error(OpenedFile) != SF_ERR_NO_ERROR)
		cout << "[FILECONVERT] sndfile error {" << sf_strerror(OpenedFile) << "}" << endl;
}

void					WiredSampleRate::EndSaveFile(unsigned int NbChannel)
{
	if(OpenedFile)
	{
		sf_close(OpenedFile);
		src_delete(StaticConverter);
		OpenedFile = NULL;
		delete[] _Buffer;
        delete[] _RateBuffer;
	}
}

void					WiredSampleRate::SetBufferSize(unsigned long Size)
{
	_ApplicationSettings.SamplesPerBuffer = Size;
}

void					WiredSampleRate::SetSampleRate(unsigned long SampleRate)
{
	_ApplicationSettings.SampleRate = SampleRate;
}

void					WiredSampleRate::SetFormat(PaSampleFormat Format)
{
	_ApplicationSettings.Format = Format;
}
