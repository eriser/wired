#ifndef __WiredLibFlac_H__
#define __WiredLibFlac_H__

#include	"../WiredApiCodec.h"

#include	<iostream>
#include	<dlfcn.h>

#define		MPEG3_EXTENTION	"mp3\tMoving Picture Experts Group (*.mp3)"
#define		MPEG2_EXTENTION	"mp2\tMoving Picture Experts Group (*.mp2)"
#define		AC3_EXTENTION	"ac3\tAudio coding 3 (*.ac3)"
#define		SO_NAME			"libmpeg3.so"

//static const char *mpegFileName = "";

bool test_decoders(t_Pcm *pcmOriginalPcm);

class   WiredLibMpeg: public WiredApiCodec
{
 public:

  /*struct use for each codec to decode*/
  WiredLibMpeg(){std::cout << "[WIRED_MPEG_CODEC] Mpeg child created" << std::endl;}
  ~WiredLibMpeg(){}
 
  void*			codecstruct;

  /*init codec and looking for it formal*/
  void		init(list<s_LibInfo> &Info);

  /*encode and decode functions*/
  int		encode(float** pcm);
  int		decode(const string filename, t_Pcm *pcm);
 private:
  void			*handle;
};

extern "C" WiredApiCodec  *const WiredCodecConstruct();

#endif

