#ifndef __WIREDLIBMPEG_H__
#define __WIREDLIBMPEG_H__

#include	"../WiredApiCodec.h"

#include	<iostream>
#include	<dlfcn.h>

#define		MPEG3_EXTENTION	"mp3\tMPEG-1 layer III (*.mp3)"
#define		MPEG2_EXTENTION	"mp2\tMPEG-1 layer II (*.mp2)"
#define		AC3_EXTENTION	"ac3\tAC3 (*.ac3)"
#define		SO_NAME			"libmpeg3.so"


bool test_decoders(t_Pcm *pcmOriginalPcm);

class   WiredLibMpeg: public WiredApiCodec
{
 public:

  WiredLibMpeg(){std::cout << "[WIRED_MPEG_CODEC] Mpeg child created" << std::endl;}
  ~WiredLibMpeg(){}
  WiredLibMpeg(const WiredLibMpeg& copy){*this = copy;};

  /* Inits codec */
  void				init(list<s_LibInfo> &Info);

  /* Encode and decode methodes */
  int				encode(float** pcm);
  int				decode(const char *path, t_Pcm *pcm);
  
  /* Checks format */
  bool				canDecode(const char* path);

  /* Operators */
  WiredLibMpeg		operator=(const WiredLibMpeg& right);
  
 private:
  void			*handle;
};

extern "C" WiredApiCodec  *const WiredCodecConstruct();

#endif

