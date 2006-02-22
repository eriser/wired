#include "WiredSampleRate.h"

static wxMutex				SampleRateMutex;

WiredSampleRate::WiredSampleRate()
{
	OpenedFile = NULL;
	StaticConverter = NULL;	
}

WiredSampleRate::~WiredSampleRate()
{
	//SampleRateMutex.Lock();
	if (OpenedFile)
		EndSaveFile(2);
	//SampleRateMutex.Unlock();
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
		_ChannelBuffer = right._ChannelBuffer;
	}
	return *this;
}

void					WiredSampleRate::Init(t_samplerate_info *Info)
{
	//SampleRateMutex.Lock();
	_ApplicationSettings.WorkingDirectory = Info->WorkingDirectory;
	_ApplicationSettings.SampleRate = Info->SampleRate;
	_ApplicationSettings.Format = Info->Format;
	_ApplicationSettings.SamplesPerBuffer = Info->SamplesPerBuffer;
	StaticConverter = NULL;
	//SampleRateMutex.Unlock();
		
}

int						WiredSampleRate::OpenFile(string *Path, wxWindow* parent)
{
	SNDFILE				*Result;
	SF_INFO				Info;
	int					Res = wxID_NO;
	bool				SameSampleRate, SameFormat;
	
	Info.format = 0;
	if ((Result = sf_open(Path->c_str(), SFM_READ, &Info)) != NULL)
	{
//		cout << "succesfully opened file" << Path->c_str() << endl;
		SameFormat = IsSameFormat(Info.format, _ApplicationSettings.Format);
		SameSampleRate = (int)Info.samplerate == (int)_ApplicationSettings.SampleRate ? true : false;
		if (!SameFormat || !SameSampleRate)
		{
//			cout << "Successfully opend file, info : format == " << Info.format << ", samplerate == " << Info.samplerate 
//					<< ", NbFrames == " << Info.frames << ", sections == " << Info.sections << ", Seekable == " << Info.seekable << endl;
			wxString			strFormats(_("Would you like to convert your file from "));
			char				buf[1024];
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
        		//SampleRateMutex.Lock();
        		if (Convert(&Info, *Path, Result))
	        		Res = wxID_YES;
	        	else
	        		Res = wxID_NO;
	        		//SampleRateMutex.Unlock();
        	}
        	else if (res  == wxID_CANCEL)
        		Res = wxID_CANCEL;
		}
		sf_close(Result);
	}
	//SampleRateMutex.Unlock();
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
		Choices.Add(src_get_name(pos));
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
				
				ProgressBar.Update(NbLoop, "", &HasFailed);
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
	wxDialog 			Dialog(NULL, -1, _("File format selection"), wxDefaultPosition, DialogSize, 
								wxCENTRE | wxCAPTION | wxTHICK_FRAME | wxSTAY_ON_TOP);	
	wxStaticText		DialogText(&Dialog, -1, wxString(_("Please select the options that\nyou want for the downmix.\n(Defaults are current project's configuration)")),
									wxPoint(13, 10), wxSize(-1, -1), wxALIGN_CENTER);
	wxButton			DialogButton(&Dialog, wxID_OK, "", wxPoint(135, 110));
	wxComboBox			CBFormat(&Dialog, -1, "", ComboPos, ComboSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	int					pos;
	
	for (pos = 0; _FormatTypes[pos].SndFileFormat != 0;pos++)
		CBFormat.Append(wxString(_FormatTypes[pos].FormatName));
	ComboPos.x = 200;
	ComboSize.x = 100;
	wxComboBox			CBSampleRate(&Dialog, -1, "", ComboPos, ComboSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	for (pos = 0; _SampleRateTypes[pos].SampleRate != 0;pos++)
		CBSampleRate.Append(wxString(_SampleRateTypes[pos].SampleRateName));
	Dialog.SetReturnCode(wxID_OK);
	if (Dialog.ShowModal() == wxID_OK)
	{
		for (pos = 0; _FormatTypes[pos].SndFileFormat != 0;pos++)
			if (CBFormat.GetValue().compare(_FormatTypes[pos].FormatName) == 0)
			{
				DestInfo->format = _FormatTypes[pos].SndFileFormat;
				break;
			}
		for (pos = 0; _SampleRateTypes[pos].SampleRate != 0;pos++)
			if (CBSampleRate.GetValue().compare(_SampleRateTypes[pos].SampleRateName) == 0)
			{
				DestInfo->samplerate = _SampleRateTypes[pos].SampleRate;
				break;
			}
	}
}

bool					WiredSampleRate::SaveFile(string& Path, unsigned int NbChannel, unsigned long NbSamples, bool interleaved)
{
	wxFile				File;
	
	ChooseFileFormat(&OpenedFileInfo);
	OpenedFileInfo.channels = NbChannel;
	OpenedFileInfo.format |= SF_FORMAT_WAV;
	if (sf_format_check(&OpenedFileInfo) == false)
		cout << "[FILECONVERT] An error occured while trying to open a file, bad parameters)" << endl;
	if ((OpenedFile = sf_open(Path.c_str(), SFM_WRITE, &OpenedFileInfo)) == NULL)
	{
		cout << "[FILECONVERT] An error occured while trying to open file (details :" << sf_strerror(OpenedFile) << ")" << endl;
		//SampleRateMutex.Unlock();
		return false;
	}
	_Quality = GetConverterQuality();
	StaticConverter = src_new(_Quality, (interleaved == true ? NbChannel : 1), &_ConverterError);
	_Buffer = new float[NbChannel * NbSamples];
	_ChannelBuffer = new float *[NbChannel];
	//TO FIX bad allocation ....
	for (unsigned int CurrentChannel = 0; CurrentChannel < NbChannel; CurrentChannel++)
	{
		_ChannelBuffer[CurrentChannel] = new float[NbSamples];
		bzero(_ChannelBuffer[CurrentChannel], NbSamples * sizeof(float));
	}
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
		bzero(_ChannelBuffer[CurrentChannel], NbSamples);
		Data.data_in = Input[CurrentChannel];
		Data.input_frames = NbSamples;
		Data.output_frames = NbSamples;
		Data.data_out = _ChannelBuffer[CurrentChannel];
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
	
	for (CurrentSample = 0, CurrentResSample = 0; CurrentSample < ToWrite; CurrentSample++)
	{
		_Buffer[CurrentResSample++] = _ChannelBuffer[0][CurrentSample];
		_Buffer[CurrentResSample++] = _ChannelBuffer[1][CurrentSample];		
	}
	ToWrite *= NbChannels;
	return _Buffer;
}
// interleaved
float					*WiredSampleRate::ConvertnChannels(float *Input, unsigned int NbChannels, SRC_STATE *Converter, unsigned long NbSamples, double Ratio, int End, unsigned long &ToWrite)
{
	SRC_DATA			Data;
	unsigned int		CurrentChannel;
	int					res = 0;

	bzero(_Buffer, NbChannels * NbSamples);
	for (CurrentChannel = 0; CurrentChannel < NbChannels; CurrentChannel++)
	{
		bzero(_ChannelBuffer[CurrentChannel], NbSamples);
		Data.data_in = Input;
		Data.input_frames = NbSamples * NbChannels;
		Data.output_frames = NbSamples * NbChannels;
		Data.data_out = _Buffer;
		Data.src_ratio = Ratio;
		Data.end_of_input = End;
		if ((res = src_process(Converter, &Data)))
		{
			cout << "[FILECONVERT] An error occured while trying to convert file (details :" << src_strerror(res) << ")" << endl;
			return NULL;
		}
		ToWrite = Data.output_frames_gen;
	}
	return _Buffer;
}

//non interleaved, used in Seqencer to export Wave
void					WiredSampleRate::WriteToFile(unsigned long NbSamples, float **Buffer, unsigned int NbChannel)
{
//	SampleRateMutex.Lock();
	float		*Output = NULL;
	double 		Ratio = (double)  _ApplicationSettings.SampleRate / OpenedFileInfo.samplerate;
	unsigned long ToWrite;
	unsigned int Chans = 0;

	cout <<  "Exporting non interleaved for " << NbSamples * NbChannel << " samples" << endl;
	unsigned long		CurrentSample;
	bzero(_Buffer, NbChannel * NbSamples);
	for (CurrentSample = 0, ToWrite = 0; CurrentSample < NbSamples; CurrentSample++)
		for (Chans = 0; Chans < NbChannel; Chans++)
			_Buffer[ToWrite++] = Buffer[Chans][CurrentSample];
	Output = _Buffer;
	//Output = ConvertnChannels(Buffer, NbChannel, StaticConverter, NbSamples, Ratio, (NbSamples < _ApplicationSettings.SamplesPerBuffer ? 1 : 0), ToWrite);	
	if (Output)
	{
		if (sf_writef_float(OpenedFile, Output, ToWrite) != ToWrite)
			cout << "[FILECONVERT] An error occured while writing to file " << endl;
	}
	if (sf_error(OpenedFile) != SF_ERR_NO_ERROR)
		cout << "[FILECONVERT] sndfile error {" << sf_strerror(OpenedFile) << "}" << endl;
	//SampleRateMutex.Unlock();
}

//interleaved
void					WiredSampleRate::WriteToFile(unsigned long NbSamples, float *Buffer, unsigned int NbChannel)
{
	float		*Output = NULL;
	double 		Ratio = (double)  _ApplicationSettings.SampleRate / OpenedFileInfo.samplerate;
	unsigned long ToWrite;

	//SampleRateMutex.Lock();
	Output = ConvertnChannels(Buffer, NbChannel, StaticConverter, NbSamples, Ratio, (NbSamples < _ApplicationSettings.SamplesPerBuffer ? 1 : 0), ToWrite);
	if (Output)
	{
		if (sf_writef_float(OpenedFile, Output, ToWrite) != ToWrite)
			cout << "[FILECONVERT] An error occured while writing to file " << endl;
	}
	if (sf_error(OpenedFile) != SF_ERR_NO_ERROR)
		cout << "[FILECONVERT] sndfile error {" << sf_strerror(OpenedFile) << "}" << endl;
	//SampleRateMutex.Unlock();
}

void					WiredSampleRate::EndSaveFile(unsigned int NbChannel)
{	
	cout << "Closed File exported 0" << endl;
	if(OpenedFile)
	{
		if (!OpenedFile)
			return;
		
		cout << "Closed File exported 1" << endl;
		sf_close(OpenedFile);
		cout << "Closed File exported 2" << endl;
		src_delete(StaticConverter);
		cout << "Closed File exported 3" << endl;
		OpenedFile = NULL;
//		for (unsigned int CurrentChannel = 0; CurrentChannel < NbChannel; CurrentChannel++)
//			delete _ChannelBuffer[CurrentChannel];

		//delete [] _ChannelBuffer;
		//delete _Buffer;
		cout << "Closed File exported" << endl;
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
