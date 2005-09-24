#include	"WiredLibMpeg.h"
#include	<libmpeg3.h>

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>

void	mergeChannels(float* leftChan, float* rightChan, float* dst, int totalLen);

void WiredLibMpeg::init(list<s_LibInfo> &Info)
{
	t_LibInfo		LibInfoMp3;
	t_LibInfo		LibInfoMp2;
	t_LibInfo		LibInfoAc3;
	int				temp;

	cout << "WiredLibMpeg - Init" << endl;
	LibInfoMp3.Extension = MPEG3_EXTENTION;
	LibInfoMp3.Note = 5;
	LibInfoMp3.CodecMask = DECODE;
	LibInfoMp2.Extension = MPEG2_EXTENTION;
	LibInfoMp2.Note = 5;
	LibInfoMp2.CodecMask = DECODE;
	LibInfoAc3.Extension = AC3_EXTENTION;
	LibInfoAc3.Note = 5;
	LibInfoAc3.CodecMask = DECODE;
	handle = dlopen(SO_NAME, RTLD_LAZY);
	if (!handle)
	{
    	cout << "[WIREDLIBMPEG] Can't open " << SO_NAME << endl;
		Info.push_back(LibInfoMp3);
		Info.push_back(LibInfoMp2);
		Info.push_back(LibInfoAc3);
	    return ;
    }
    Info.push_back(LibInfoMp3);
	Info.push_back(LibInfoMp2);
	Info.push_back(LibInfoAc3);
	dlclose(handle);
}

int WiredLibMpeg::encode(float** pcm)
{
	return 1;
}

void	mergeChannels(float* leftChan, float* rightChan, float* dst, int totalLen)
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

int WiredLibMpeg::decode(const string filename, t_Pcm *pcm)
{
	mpeg3_t		*file;
	int			stream = 0;
	float		*output_f;
	char		*path;
	
	cout << "[WIREDLIBMPEG] decoding" << endl;
	path = (char*)malloc(filename.size() * sizeof(char));
	strcpy(path, filename.c_str());
	if (mpeg3_check_sig(path) != 1)
	{
		cout << "[WIREDLIBMPEG] Bad file format" << endl;
		return 0;
	}
	file = mpeg3_open(path);
	if (file == NULL)
	{
		cout << "[WIREDLIBMPEG] Can't open file" << endl;
		return 0;
	}
	stream = mpeg3_total_astreams(file);
	pcm->PType = Float32;
	pcm->Channels = mpeg3_audio_channels(file, 0);
	pcm->TotalSample = mpeg3_audio_samples(file, 0);
	pcm->SampleRate = mpeg3_sample_rate(file, 0);
	if (pcm->Channels > 0)
	{
		pcm->pcm = new float[pcm->TotalSample * 2];
		float* leftChan = new float[pcm->TotalSample];
		float* rightChan = new float[pcm->TotalSample];
		for (int cpt = 0; cpt < pcm->Channels; cpt++)
		{
			if (cpt == 0)
			{
				mpeg3_read_audio(file, leftChan, NULL, cpt, pcm->TotalSample, 0);
			}
			else
			{
				mpeg3_seek_byte(file, 0);
				mpeg3_read_audio(file, rightChan, NULL, cpt, pcm->TotalSample, 0);
			}
		}
		if (pcm->Channels == 2)
			mergeChannels(leftChan, rightChan, (float*)pcm->pcm, pcm->TotalSample * pcm->Channels);
		if (pcm->Channels == 1)
			mergeChannels(leftChan, leftChan, (float*)pcm->pcm, pcm->TotalSample * 2);
		delete leftChan;
		delete rightChan;
	}
	else
	{
		cout << "[WIREDLIBMPEG] No channel found" << endl;
		return 0;
	}
	cout << "[WIREDLIBMPEG] done" << endl;
	return mpeg3_close(file);
}


extern "C" WiredApiCodec  *const WiredCodecConstruct()
{
  return ((WiredApiCodec*)new WiredLibMpeg());
}
