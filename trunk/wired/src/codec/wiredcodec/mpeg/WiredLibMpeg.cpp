// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include	"WiredLibMpeg.h"

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>

void WiredLibMpeg::init(list<t_LibInfo> &Info)
{
	t_LibInfo		LibInfoMp3;
	t_LibInfo		LibInfoMp2;
	t_LibInfo		LibInfoAc3;
	int				temp;

	fillLibInfo(LibInfoMp3, MPEG3_EXTENTION);
	fillLibInfo(LibInfoMp2, MPEG2_EXTENTION);
	fillLibInfo(LibInfoAc3, AC3_EXTENTION);
	Info.push_back(LibInfoMp3);
	Info.push_back(LibInfoMp2);
	Info.push_back(LibInfoAc3);
	handle = dlopen(SO_NAME, RTLD_LAZY);
	if (!handle)
	{
    	cout << "[WIRED_MPEG_CODEC] Can't open " << SO_NAME << endl;
	    return ;
    }
	InitAccesLib();
}

void	WiredLibMpeg::fillLibInfo(t_LibInfo& info, char *extension)
{
	info.Extension = wxString(extension, *wxConvCurrent);
	info.Note = 5;
	info.CodecMask = EXIST | DECODE;
}

void	WiredLibMpeg::InitAccesLib()
{
  if ((mpeg3_check_sig_func = (t_mpeg3_check_sig) dlsym(handle, MPEG3_CHECK_SIG)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_check_sig" << endl;
	return ;
  }
  if ((mpeg3_open_func = (t_mpeg3_open) dlsym(handle, MPEG3_OPEN)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_open" << endl;
	return ;
  }
  if ((mpeg3_total_astreams_func = (t_mpeg3_total_astreams) dlsym(handle, MPEG3_TOTAL_AS_STREAM)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_total_astreams" << endl;
	return ;
  }
  if ((mpeg3_audio_channels_func = (t_mpeg3_audio_channels) dlsym(handle, MPEG3_AUDIO_CHANNELS)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_audio_channels" << endl;
	return ;
  }
  if ((mpeg3_audio_samples_func = (t_mpeg3_audio_samples) dlsym(handle, MPEG3_AUDIO_SAMPLES)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_audio_samples" << endl;
	return ;
  }
  if ((mpeg3_sample_rate_func = (t_mpeg3_sample_rate) dlsym(handle, MPEG3_SAMPLE_RATE)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_sample_rate" << endl;
	return ;
  }
  if ((mpeg3_read_audio_func = (t_mpeg3_read_audio) dlsym(handle, MPEG3_READ_AUDIO)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_read_audio" << endl;
	return ;
  }
  if ((mpeg3_reread_audio_func = (t_mpeg3_reread_audio) dlsym(handle, MPEG3_REREAD_AUDIO)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_reread_audio" << endl;
	return ;
  }
  if ((mpeg3_seek_byte_func = (t_mpeg3_seek_byte) dlsym(handle, MPEG3_SEEK_BYTE)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_seek_byte" << endl;
	return ;
  }
  if ((mpeg3_close_func = (t_mpeg3_close) dlsym(handle, MPEG3_CLOSE)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_close" << endl;
	return ;
  }
  if ((mpeg3_tell_byte_func = (t_mpeg3_tell_byte) dlsym(handle, MPEG3_TELL_BYTE)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_tell_byte" << endl;
	return ;
  }
  if ((mpeg3_set_sample_func = (t_mpeg3_set_sample) dlsym(handle, MPEG3_SET_SAMPLE)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_set_sample" << endl;
	return ;
  }
  if ((mpeg3_get_sample_func = (t_mpeg3_get_sample) dlsym(handle, MPEG3_GET_SAMPLE)) == NULL)
  {
  	cout << "[WIRED_MPEG_CODEC] Can't resolve symbol mpeg3_get_sample" << endl;
	return ;
  }
}

int		WiredLibMpeg::encode(float** pcm)
{
	return 1;
}

bool	WiredLibMpeg::CanConvert(const wxString& path, int Decode)
{
	if (Decode & ENCODE)
		return false;
	if (!mpeg3_check_sig_func)
		return false;
	if (mpeg3_check_sig_func((char*)(const char*)path.mb_str(*wxConvCurrent)) != 1)
		return false;
	return true;
}

void	WiredLibMpeg::mergeChannels(float* leftChan, float* rightChan, float* dst, int totalLen, unsigned int NbChannels)
{
	int cpt;

	for (cpt = 0; cpt < totalLen; cpt++)
	{
		dst[cpt] = leftChan[cpt / NbChannels];
	}
}

int	WiredLibMpeg::EndDecode()
{
	if (file == NULL || mpeg3_close_func == NULL)
		return 0;
	return mpeg3_close_func(file);
}

int WiredLibMpeg::decode(const char *path, t_Pcm *pcm, unsigned long length)
{
	int				stream = 0;
	static long			filePosInSample = 0;

	if (file == NULL)
	{
	  cout << path<< endl;
		file = mpeg3_open_func((char*)path);
		if (file == NULL)
		{
			cout << "[WIREDLIBMPEG] Can't open file" << endl;
			return 0;
		}
		stream = mpeg3_total_astreams_func(file);
		pcm->PType = Float32;
		pcm->Channels = mpeg3_audio_channels_func(file, 0);
		pcm->TotalSample = mpeg3_audio_samples_func(file, 0);
		pcm->SampleRate = mpeg3_sample_rate_func(file, 0);
		cout << "[WIREDLIBMPEG] " << pcm->Channels << " | " << pcm->TotalSample << " | " << pcm->SampleRate << endl;
	}
	if (filePosInSample > pcm->TotalSample)
		return 0;
	if (pcm->Channels > 0)
	{
		float* leftChan = new float[length / pcm->Channels];
		float* rightChan = new float[length / pcm->Channels];
		memset(leftChan, 0, length / pcm->Channels);
		memset(rightChan, 0, length / pcm->Channels);

		if (filePosInSample < 0)
		{
			cout << "[WIREDLIBMPEG] Can't tell position" << endl;
			delete[] leftChan;
			delete[] rightChan;
			return 0;
		}
//				mpeg3_read_audio_func(file, leftChan, NULL, 0, length, 0);
//				mpeg3_set_sample_func(file, filePosInSample, 0);
//				mpeg3_read_audio_func(file, rightChan, NULL, 1, length, 0);
//				mpeg3_reread_audio_func(file, rightChan, NULL, 1, length, 0);

		for (int cpt = 0; cpt < pcm->Channels && cpt != 2; cpt++)
		{
			if (cpt == 0)
				mpeg3_read_audio_func(file, leftChan, NULL, 0, length / pcm->Channels, 0);
			else
				mpeg3_reread_audio_func(file, rightChan, NULL, 0, length / pcm->Channels, 0);
		}
		mergeChannels(leftChan, rightChan, (float*)pcm->pcm, length, pcm->Channels);
		delete[] leftChan;
		delete[] rightChan;
	}
	else
	{
		cout << "[WIREDLIBMPEG] No channel found" << endl;
		return 0;
	}
	filePosInSample += (length / pcm->Channels);
	if (filePosInSample > pcm->TotalSample)
	{
		cout << "[WIREDLIBMPEG] decoding done: " << filePosInSample << " / " << pcm->TotalSample << endl;
		return (filePosInSample % pcm->TotalSample);
	}
//	cout << "[WIREDLIBMPEG] during: " << filePosInSample << " / " << pcm->TotalSample << endl;
	return length;
}

WiredLibMpeg			WiredLibMpeg::operator=(const WiredLibMpeg& right)
{
	if (this != &right)
	{
		handle = right.handle;
		file = right.file;
        mpeg3_check_sig_func = right.mpeg3_check_sig_func;
        mpeg3_open_func = right.mpeg3_open_func;
        mpeg3_total_astreams_func = right.mpeg3_total_astreams_func;
        mpeg3_audio_channels_func = right.mpeg3_audio_channels_func;
        mpeg3_audio_samples_func = right.mpeg3_audio_samples_func;
        mpeg3_sample_rate_func = right.mpeg3_sample_rate_func;
        mpeg3_read_audio_func = right.mpeg3_read_audio_func;
        mpeg3_reread_audio_func = right.mpeg3_reread_audio_func;
        mpeg3_seek_byte_func = right.mpeg3_seek_byte_func;
        mpeg3_close_func = right.mpeg3_close_func;
        mpeg3_tell_byte_func = right.mpeg3_tell_byte_func;
        mpeg3_set_sample_func = right.mpeg3_set_sample_func;
        mpeg3_get_sample_func = right.mpeg3_get_sample_func;
	}
	return *this;
}

extern "C" WiredApiCodec  *const WiredCodecConstruct()
{
  return ((WiredApiCodec*)new WiredLibMpeg());
}
