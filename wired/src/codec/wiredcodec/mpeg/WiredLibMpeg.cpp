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
	info.Extension = extension;
	info.Note = 5;
	info.CodecMask = DECODE;
}

void	WiredLibMpeg::InitAccesLib()
{
  mpeg3_check_sig_func = (t_mpeg3_check_sig) dlsym(handle, MPEG3_CHECK_SIG);
  mpeg3_open_func = (t_mpeg3_open) dlsym(handle, MPEG3_OPEN);
  mpeg3_total_astreams_func = (t_mpeg3_total_astreams) dlsym(handle, MPEG3_TOTAL_AS_STREAM);
  mpeg3_audio_channels_func = (t_mpeg3_audio_channels) dlsym(handle, MPEG3_AUDIO_CHANNELS);
  mpeg3_audio_samples_func = (t_mpeg3_audio_samples) dlsym(handle, MPEG3_AUDIO_SAMPLES);
  mpeg3_sample_rate_func = (t_mpeg3_sample_rate) dlsym(handle, MPEG3_SAMPLE_RATE);
  mpeg3_read_audio_func = (t_mpeg3_read_audio) dlsym(handle, MPEG3_READ_AUDIO);
  mpeg3_seek_byte_func = (t_mpeg3_seek_byte) dlsym(handle, MPEG3_SEEK_BYTE);
  mpeg3_close_func = (t_mpeg3_close) dlsym(handle, MPEG3_CLOSE);
  mpeg3_tell_byte_func = (t_mpeg3_tell_byte) dlsym(handle, MPEG3_TELL_BYTE);
}

int		WiredLibMpeg::encode(float** pcm)
{
	return 1;
}

bool	WiredLibMpeg::canDecode(const char* path)
{
	if (mpeg3_check_sig_func((char*)path) != 1)
		return false;
	return true;
}

void	WiredLibMpeg::mergeChannels(float* leftChan, float* rightChan, float* dst, int totalLen)
{
	int cpt;
	
	for (cpt = 0; cpt < totalLen; cpt++)
	{
		if ((cpt % 2) == 0)
			dst[cpt] = leftChan[cpt / 2];
		else
			dst[cpt] = rightChan[cpt / 2];
	}
}

int	WiredLibMpeg::EndDecode()
{
	if (file == NULL)
		return 0;
	return mpeg3_close_func(file);
}

int WiredLibMpeg::decode(const char *path, t_Pcm *pcm, unsigned long length)
{
	int				stream = 0;
	float			*output_f;
	unsigned long	retLength = 0;
	static unsigned long	lengthRead = 0;
	
	cout << "[WIREDLIBMPEG] decoding: " << length << endl;
	if (file == NULL)
		file = mpeg3_open_func((char*)path);
	if (file == NULL)
	{
		cout << "[WIREDLIBMPEG] Can't open file" << endl;
		return retLength;
	}
	stream = mpeg3_total_astreams_func(file);
	pcm->PType = Float32;
	pcm->Channels = mpeg3_audio_channels_func(file, 0);
	pcm->TotalSample = mpeg3_audio_samples_func(file, 0);
	pcm->SampleRate = mpeg3_sample_rate_func(file, 0);
	if (length >= (pcm->TotalSample * pcm->Channels))
	{
		cout << "[WIREDLIBMPEG] Not enought samples" << endl;
		return retLength;	
	}
	if (pcm->Channels > 0)
	{
		float* leftChan = new float[length / 2];
		float* rightChan = new float[length / 2];
		int64_t filePosition = mpeg3_tell_byte_func(file);
		
		if (filePosition < 0)
		{
			cout << "[WIREDLIBMPEG] Can't tel position" << endl;
			return retLength;
		}
		for (int cpt = 0; cpt < pcm->Channels && cpt != 2; cpt++)
		{
			if (cpt == 0)
			{
				retLength = (unsigned long) mpeg3_read_audio_func(file, leftChan, NULL, cpt, (length / 2), 0);
			}
			else
			{
				mpeg3_seek_byte_func(file, filePosition);
				retLength = (unsigned long) mpeg3_read_audio_func(file, rightChan, NULL, cpt, (length / 2), 0);
			}
		}
		if (pcm->Channels == 2)
		{
			pcm->pcm = new float[pcm->TotalSample * 2];
			mergeChannels(leftChan, rightChan, (float*)pcm->pcm, length);
			delete leftChan;
		}
//		if (pcm->Channels == 1)
//			pcm->pcm = (void*)leftChan;
		delete rightChan;
	}
	else
	{
		cout << "[WIREDLIBMPEG] No channel found" << endl;
		return 0;
	}
	lengthRead += length;
	if (lengthRead > pcm->TotalSample * pcm->Channels)
	{
		cout << "[WIREDLIBMPEG] done: " << 0 << endl;
		return 0;
	}
	cout << "[WIREDLIBMPEG] done: " << 1 << endl;
	return length;
}

WiredLibMpeg			WiredLibMpeg::operator=(const WiredLibMpeg& right)
{
	if (this != &right)
	{
		handle = right.handle;
	}
	return *this;
}

extern "C" WiredApiCodec  *const WiredCodecConstruct()
{
  return ((WiredApiCodec*)new WiredLibMpeg());
}
