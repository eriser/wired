// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	"WiredLibFlac.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

WiredLibFlac			WiredLibFlac::operator=(const WiredLibFlac& right)
{
  if (this != &right)
    {
      handle = right.handle;
      decoder = right.decoder;
      client_data = right.client_data;
      Pass = right.Pass;
      FileDecoderNew = right.FileDecoderNew;
      FileDecoderDelete = right.FileDecoderDelete;
      FileDecoderSetMd5Checking = right.FileDecoderSetMd5Checking;
      FileDecoderSetFilename = right.FileDecoderSetFilename;
      FileDecoderSetWriteCallback = right.FileDecoderSetWriteCallback;
      FileDecoderSetMetadataCallback = right.FileDecoderSetMetadataCallback;
      FileDecoderSetErrorCallback = right.FileDecoderSetErrorCallback;
      FileDecoderSetClientData = right.FileDecoderSetClientData;
      FileDecoderSetMetadataRespondAll = right.FileDecoderSetMetadataRespondAll;
      FileDecoderGetState = right.FileDecoderGetState;
      FileDecoderGetResolvedStateString = right.FileDecoderGetResolvedStateString;
      FileDecoderGetChannels = right.FileDecoderGetChannels;
      FileDecoderGetSampleRate = right.FileDecoderGetSampleRate;
      FileDecoderInit = right.FileDecoderInit;
      FileDecoderFinish = right.FileDecoderFinish;
      FileDecoderProcessSingle = right.FileDecoderProcessSingle;
      FileDecoderGetDecodePosition = right.FileDecoderGetDecodePosition;
      FileDecoderSeekAbsolute = right.FileDecoderSeekAbsolute;
      FileDecoderMetadataCallback = right.FileDecoderMetadataCallback;
      FileDecoderWriteCallback = right.FileDecoderWriteCallback;
    }
  return *this;
}

void WiredLibFlac::init(list<s_LibInfo> &Info)
{
  t_LibInfo		LibInfo;
  int			temp;

  LibInfo.CodecMask = EXIST;
  LibInfo.Extension = "flac\tFree Lossless Audio Codec (*.flac)";
  LibInfo.Note = 5;
  handle = dlopen(SO_NAME, RTLD_LAZY);
  if (!handle)
    {
      Info.push_back(LibInfo);
      return ;
    }
  if (LoadSymbol() == false)
    {
      Info.push_back(LibInfo);
      return;
    }  
  LibInfo.CodecMask = DECODE;
  Info.push_back(LibInfo);
}

bool WiredLibFlac::CanConvert(const char* path, int Decode)
{
  int		fd;
  char		*buf;
  
  if (Decode & ENCODE)
    return false;
  if (wxFile::Exists(path) == false)
    return false;
  buf = new char((FLAC_FCC_LENGHT + 1) * sizeof(char));
  buf[FLAC_FCC_LENGHT] = 0;
  if (read(fd, buf, FLAC_FCC_LENGHT) == FLAC_FCC_LENGHT)
    {
      if (strcmp(buf, FLAC_FCC_LABEL) == 0)
    	{
	  delete buf;
	  return true;	
    	}
    }
  delete buf;
  return false;
}

int WiredLibFlac::encode(float** pcm)
{
  return 1;
}

// void	write_out(FLAC__int32* pcm, const char* file, int size)
// {
//   int	fd;
//   int	i;

//   cout << "--WRITE--" << endl;
//   fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
//   if (fd > 0)
//     {
//       cout << "fd:" << fd <<  " size:" << size << endl;

//       if (write(fd, (void*)pcm,  size) <= 0)
// 	cout << "rien" << endl;
      
//     }
//   close(fd);
//   cout << "--AND WRITE--" << endl;
// }


bool test_decoders(t_Pcm *OriginalPcm)
{ 
}

bool WiredLibFlac::LoadSymbol()
{
  FileDecoderNew = (WiredFileDecoderNew) dlsym(handle, FLAC_FILE_DECODER_NEW);
  if (!FileDecoderNew)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_NEW << endl;
      return false;
    }
  FileDecoderDelete = (WiredFileDecoderDelete) dlsym(handle, FLAC_FILE_DECODER_DELETE);
  if (!FileDecoderDelete)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_DELETE << endl;
      return false;
    }
  FileDecoderSetMd5Checking = (WiredFileDecoderSetMd5Checking) dlsym(handle, FLAC_FILE_DECODER_SET_MD5_CHECKING);
  if (!FileDecoderSetMd5Checking)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SET_MD5_CHECKING << endl;
      return false;
    }
  FileDecoderSetFilename = (WiredFileDecoderSetFilename) dlsym(handle, FLAC_FILE_DECODER_SET_FILENAME);
  if (!FileDecoderSetFilename)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SET_FILENAME << endl;
      return false;
    }
  FileDecoderSetWriteCallback = (WiredFileDecoderSetWriteCallback) dlsym(handle, FLAC_FILE_DECODER_SET_WRITE_CALLBACK);
  if (!FileDecoderSetWriteCallback)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SET_WRITE_CALLBACK << endl;
      return false;
    }
  FileDecoderSetMetadataCallback = (WiredFileDecoderSetMetadataCallback) dlsym(handle, FLAC_FILE_DECODER_SET_METADATA_CALLBACK);
  if (!FileDecoderSetMetadataCallback)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SET_METADATA_CALLBACK << endl;
      return false;
    }
  FileDecoderSetErrorCallback = (WiredFileDecoderSetErrorCallback) dlsym(handle, FLAC_FILE_DECODER_SET_ERROR_CALLBACK);
  if (!FileDecoderSetErrorCallback)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SET_ERROR_CALLBACK << endl;
      return false;
    }
  FileDecoderSetClientData = (WiredFileDecoderSetClientData) dlsym(handle, FLAC_FILE_DECODER_SET_CLIENT_DATA);
  if (!FileDecoderSetClientData)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SET_CLIENT_DATA << endl;
      return false;
    }
  FileDecoderSetMetadataRespondAll = (WiredFileDecoderSetMetadataRespondAll) dlsym(handle, FLAC_FILE_DECODER_SET_METADATA_RESPOND_ALL);
  if (!FileDecoderSetMetadataRespondAll)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SET_METADATA_RESPOND_ALL << endl;
      return false;
    }
  FileDecoderGetState = (WiredFileDecoderGetState) dlsym(handle, FLAC_FILE_DECODER_GET_STATE);
  if (!FileDecoderGetState)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_GET_STATE << endl;
      return false;
    }
  FileDecoderGetResolvedStateString = (WiredFileDecoderGetResolvedStateString) dlsym(handle, FLAC_FILE_DECODER_GET_RESOLVED_STATE_STRING);
  if (!FileDecoderGetResolvedStateString)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_GET_RESOLVED_STATE_STRING << endl;
      return false;
    }
  FileDecoderGetChannels = (WiredFileDecoderGetChannels) dlsym(handle, FLAC_FILE_DECODER_GET_CHANNELS);
  if (!FileDecoderGetChannels)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_GET_CHANNELS << endl;
      return false;
    }
  FileDecoderGetSampleRate = (WiredFileDecoderGetSampleRate) dlsym(handle, FLAC_FILE_DECODER_GET_SAMPLE_RATE);
  if (!FileDecoderGetSampleRate)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_GET_SAMPLE_RATE << endl;
      return false;
    }
  FileDecoderInit = (WiredFileDecoderInit) dlsym(handle, FLAC_FILE_DECODER_INIT);
  if (!FileDecoderInit)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_INIT << endl;
      return false;
    }
  FileDecoderFinish = (WiredFileDecoderFinish) dlsym(handle, FLAC_FILE_DECODER_FINISH);
  if (!FileDecoderFinish)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_FINISH << endl;
      return false;
    }
  FileDecoderProcessSingle = (WiredFileDecoderProcessSingle) dlsym(handle, FLAC_FILE_DECODER_PROCESS_SINGLE);
  if (!FileDecoderProcessSingle)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_PROCESS_SINGLE << endl;
      return false;
    }
  FileDecoderGetDecodePosition = (WiredFileDecoderGetDecodePosition) dlsym(handle, FLAC_FILE_DECODER_GET_DECODE_POSITION);
  if (!FileDecoderGetDecodePosition)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_GET_DECODE_POSITION << endl;
      return false;
    }
  FileDecoderSeekAbsolute = (WiredFileDecoderSeekAbsolute) dlsym(handle, FLAC_FILE_DECODER_SEEK_ABSOLUTE);
  if (!FileDecoderSeekAbsolute)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_SEEK_ABSOLUTE << endl;
      return false;
    }
  FileDecoderMetadataCallback = (WiredFileDecoderMetadataCallback) dlsym(handle, FLAC_FILE_DECODER_METADATA_CALLBACK);
  if (!FileDecoderMetadataCallback)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_METADATA_CALLBACK << endl;
      return false;
    }
  FileDecoderWriteCallback = (WiredFileDecoderWriteCallback) dlsym(handle, FLAC_FILE_DECODER_WRITE_CALLBACK);
  if (!FileDecoderWriteCallback)
    {
      cout << "Can t load symbol:" << FLAC_FILE_DECODER_WRITE_CALLBACK << endl;
      return false;
    }
  return true;
}

int	WiredLibFlac::EndDecode()
{
  
}

int WiredLibFlac::decode(const char *path, t_Pcm *pcm, unsigned long length)
{

  if (Pass != 1)
    {
      flacfilename_ = path;
      
      decoder = FileDecoderNew();
      
      if (FileDecoderSetMd5Checking(decoder, false) == false)
	{
	  cout << "[WIRED_FLAC_CODEC] md5 doesn t match" <<endl;
	  return 0;
	}
      if (FileDecoderSetFilename(decoder, path) == false)
	{
	  cout << "[WIRED_FLAC_CODEC] Can t set decoder filename" <<endl;
	  return 0;
	}
      if (FileDecoderSetClientData(decoder, client_data) == false)
	{
	  cout << "[WIRED_FLAC_CODEC] Can t set decoder client data" <<endl;
	  return 0;
	}  
//       if (FileDecoderSetWriteCallback(decoder, frame, buffer, client_data))
// 	{
// 	  cout << "[WIRED_FLAC_CODEC] Can t set decoder write callback" <<endl;
// 	  return 0;
// 	}
      Pass = 1;
    }


//   if (FLAC__file_decoder_set_write_callback(decoder, NULL) == false)
//     {
//       cout << "[WIRED_FLAC_CODEC] Can t set decoder write callback" <<endl;
//       return 0;
//     }
//   if (FLAC__file_decoder_set_metadata_callback(decoder, NULL) == false)
//     {
//       cout << "[WIRED_FLAC_CODEC] Can t set decoder metadata callback" <<endl;
//       return 0;
//     }

  if (FileDecoderInit(decoder)  != FLAC__FILE_DECODER_OK)
    {
      cout << "[WIRED_FLAC_CODEC] Can t init decoder" <<endl;
      return 0;
    }


  if (FileDecoderFinish(decoder) != FLAC__FILE_DECODER_UNINITIALIZED)
    {
      cout << "[WIRED_FLAC_CODEC] can t uninit decoder" <<endl;
      return 0;
    }
  FileDecoderDelete(decoder);
  return 0;
}


extern "C" WiredApiCodec  *const WiredCodecConstruct()
{
  return ((WiredApiCodec*)new WiredLibFlac());
}
