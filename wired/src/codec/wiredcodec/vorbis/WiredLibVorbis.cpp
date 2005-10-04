#include	"WiredLibVorbis.h"
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>

WiredLibVorbis			WiredLibVorbis::operator=(const WiredLibVorbis& right)
{
  if (this != &right)
    {
      handle = right.handle;
      vf = right.vf;
      OvOpen= right.OvOpen;
      OvReadFloat = right.OvReadFloat;
      OvInfo = right.OvInfo;
      OvClear = right.OvClear;
      Pass = right.Pass;
      TotalRead = right.TotalRead;
      pcmout = right.pcmout;
      current_section = right.current_section;
      FileToOpen = right.FileToOpen;
      vi = right.vi;
    }
  return *this;
}

bool WiredLibVorbis::LoadSymbol()
{
  OvOpen = (WiredOvOpen) dlsym(handle, OV_OPEN);
  if (!OvOpen)
    {
      cout << "Can t load symbol:" << OV_OPEN << endl;
      return false;
    }
  OvReadFloat = (WiredOvReadFloat) dlsym(handle, OV_READ_FLOAT);
  if (!OvReadFloat)
    {
      cout << "Can t load symbol:" << OV_READ_FLOAT << endl;
      return false;
    }
  OvInfo = (WiredOvInfo) dlsym(handle, OV_INFO);
  if (!OvInfo)
    {
      cout << "Can t load symbol:" << OV_INFO << endl;
      return false;
    }
  OvClear = (WiredOvClear) dlsym(handle, OV_CLEAR);
  if (!OvClear)
    {
      cout << "Can t load symbol:" << OV_CLEAR << endl;
      return false;
    }
  OvPcmTotal = (WiredOvPcmTotal) dlsym(handle, OV_PCM_TOTAL);
  if (!OvPcmTotal)
    {
      cout << "Can t load symbol:" << OV_PCM_TOTAL << endl;
      return false;
    }
  return true;
}

void WiredLibVorbis::init(list<s_LibInfo> &Info)
{
  t_LibInfo		LibInfoVorbis;

  LibInfoVorbis.Extension = VORBIS_EXTENTION;
  LibInfoVorbis.Note = 5;
  LibInfoVorbis.CodecMask = EXIST;
  handle = dlopen(SO_NAME, RTLD_LAZY);
  if (!handle)
    {
      Info.push_back(LibInfoVorbis);
      cout << "[WIRED_OGG_CODEC] Can't open " << SO_NAME << endl; 
      dlclose(handle);
      return ;
    }
  if (LoadSymbol() == false)
    {
      Info.push_back(LibInfoVorbis);
      dlclose(handle);
    }
  LibInfoVorbis.CodecMask = DECODE;
  Info.push_back(LibInfoVorbis);
  return ;
}

int WiredLibVorbis::encode(float** pcm)
{
  return 1;
}

bool	WiredLibVorbis::canDecode(const char* path)
{
  int		fd;
  char		*buf;
  
  if ((fd = open(path, O_RDONLY)) == -1)
    return false;
  buf = new char((VORBIS_FCC_LENGHT + 1) * sizeof(char));
  buf[VORBIS_FCC_LENGHT] = 0;
  if (read(fd, buf, VORBIS_FCC_LENGHT) == VORBIS_FCC_LENGHT)
    {
      if (strcmp(buf, VORBIS_FCC_LABEL) == 0)
    	{
	  delete buf;
	  return true;	
    	}
    }
  delete buf;
  return false;
}

int	WiredLibVorbis::EndDecode()
{
  OvClear(&vf);
  dlclose(handle);
  Pass = 0;
  TotalRead = 0;
}

int WiredLibVorbis::decode(const char *path, t_Pcm *pcm, unsigned long length)
{
  long		ret;
  int		i;
  int k = 0;
  int j;

  cout << "Debut-OGG" << endl;
  if (Pass != 1)
    {
      if (!(FileToOpen = fopen(path, "r")))
	{
	  cout << "[WIRED_OGG_CODEC] Can t open file:" << path << endl;
	  return 0;
	}
      if(OvOpen(FileToOpen, &vf, NULL, 0) < 0)
	{
	  cout << "[WIRED_OGG_CODEC] Input does not appear to be an Ogg bitstream" << endl;
	  fclose(FileToOpen);
	  return 0;
	}
      vi = OvInfo(&vf,-1);
      pcm->TotalSample = OvPcmTotal(&vf,-1);
      pcm->SampleRate = vi->rate;
      pcm->Channels = vi->channels;
      pcm->PType = Float32;
      Pass = 1;
      TotalRead = 0;
    }
  //ca je suis oblige de faire une allocation .. enfin y a pas d info la dessus ...
  pcmout = new float*[length * sizeof(float*)];
  if (TotalRead >= pcm->TotalSample)
    return 0;
  ret = OvReadFloat(&vf, &pcmout,length , &current_section);
  for (j = 0; j < length & j < pcm->TotalSample; j++)
    for (i = 0; i < vi->channels; i++)
      ((float*)pcm->pcm)[k++] = pcmout[i][j];
  TotalRead += ret;
  cout << "read:" << TotalRead << " / " << pcm->TotalSample << endl;
  // ici ca plante que sur certains fichiers le delete
  delete[] pcmout;
  cout << "read:" << TotalRead << " / " << pcm->TotalSample << endl;
  cout << "Fin-OGG" << endl;
  return ret;
}


extern "C" WiredApiCodec  *const WiredCodecConstruct()
{
  return ((WiredApiCodec*)new WiredLibVorbis());
}
