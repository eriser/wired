#ifndef __WIREDLIBMPEG_H__
#define __WIREDLIBMPEG_H__

#include	<iostream>
#include	<dlfcn.h>
#include	"libmpeg3.h"
#include	"../WiredApiCodec.h"

#define		MPEG3_EXTENTION			"mp3\tMPEG-1 layer III (*.mp3)"
#define		MPEG2_EXTENTION			"mp2\tMPEG-1 layer II (*.mp2)"
#define		AC3_EXTENTION			"ac3\tAC3 (*.ac3)"
#define		SO_NAME					"libmpeg3.so"

#define		MPEG3_CHECK_SIG			"mpeg3_check_sig"
#define		MPEG3_OPEN				"mpeg3_open"
#define		MPEG3_TOTAL_AS_STREAM	"mpeg3_total_astreams"
#define		MPEG3_AUDIO_CHANNELS	"mpeg3_audio_channels"
#define		MPEG3_AUDIO_SAMPLES		"mpeg3_audio_samples"
#define		MPEG3_SAMPLE_RATE		"mpeg3_sample_rate"
#define		MPEG3_READ_AUDIO		"mpeg3_read_audio"
#define		MPEG3_REREAD_AUDIO		"mpeg3_reread_audio"
#define		MPEG3_SEEK_BYTE			"mpeg3_seek_byte"
#define		MPEG3_CLOSE				"mpeg3_close"
#define		MPEG3_TELL_BYTE			"mpeg3_tell_byte"

typedef int			(*t_mpeg3_check_sig)		(char *path);
typedef mpeg3_t* 	(*t_mpeg3_open)				(char *path);
typedef int			(*t_mpeg3_total_astreams)	(mpeg3_t *file);
typedef int			(*t_mpeg3_audio_channels)	(mpeg3_t *file, int stream);
typedef long		(*t_mpeg3_audio_samples)	(mpeg3_t *file, int stream);
typedef int			(*t_mpeg3_sample_rate)		(mpeg3_t *file, int stream);
typedef int			(*t_mpeg3_read_audio)		(mpeg3_t *file, float *output_f, short *output_i, 
  												int channel, long samples, int stream);
typedef int			(*t_mpeg3_reread_audio)		(mpeg3_t *file, float *output_f, short *output_i, 
  												int channel, long samples, int stream);
typedef int			(*t_mpeg3_seek_byte)		(mpeg3_t *file, int64_t byte);
typedef int			(*t_mpeg3_close)			(mpeg3_t *file);
typedef int64_t		(*t_mpeg3_tell_byte)		(mpeg3_t *file);

class   WiredLibMpeg: public WiredApiCodec
{
 public:

  WiredLibMpeg(){std::cout << "[WIRED_MPEG_CODEC] Mpeg child created" << std::endl;file = NULL;}
  WiredLibMpeg(const WiredLibMpeg& copy){*this = copy;};
  ~WiredLibMpeg(){dlclose(handle);};

  void				init(list<t_LibInfo> &Info);				/* Inits codec */
  int				encode(float** pcm);						/* Encode methodes */
  int				decode(const char *path, t_Pcm *pcm, 
  							unsigned long length);				/* Decode methodes */
  bool				canDecode(const char* path);				/* Checks format */
  int				EndDecode();

  WiredLibMpeg		operator=(const WiredLibMpeg& right);
  
 private:
  void						*handle;
  mpeg3_t					*file;
  t_mpeg3_check_sig			mpeg3_check_sig_func;
  t_mpeg3_open				mpeg3_open_func;
  t_mpeg3_total_astreams	mpeg3_total_astreams_func;
  t_mpeg3_audio_channels	mpeg3_audio_channels_func;
  t_mpeg3_audio_samples		mpeg3_audio_samples_func;
  t_mpeg3_sample_rate		mpeg3_sample_rate_func;
  t_mpeg3_read_audio		mpeg3_read_audio_func;
  t_mpeg3_reread_audio		mpeg3_reread_audio_func;
  t_mpeg3_seek_byte			mpeg3_seek_byte_func;
  t_mpeg3_close				mpeg3_close_func;
  t_mpeg3_tell_byte			mpeg3_tell_byte_func;
  
  void				InitAccesLib();
  void				fillLibInfo(t_LibInfo& info, char *extension);
  void				mergeChannels(float* leftChan, float* rightChan, float* dst, int totalLen);
};

extern "C" WiredApiCodec  *const WiredCodecConstruct();

#endif

