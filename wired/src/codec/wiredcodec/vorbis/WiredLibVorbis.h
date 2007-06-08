// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WiredLibVorbis_H__
#define __WiredLibVorbis_H__

#include	"WiredApiCodec.h"

#include	<iostream>
#include	<dlfcn.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	"vorbisfile.h"

#define		VORBIS_EXTENTION	_("ogg\tVorbis Ogg (*.ogg)")
#define		SO_NAME			"libvorbisfile.so"
#define		VORBIS_FCC_LENGHT	3
#define		VORBIS_FCC_LABEL	"Ogg"
#define		OV_OPEN			"ov_open"
#define		OV_READ_FLOAT		"ov_read_float"
#define		OV_INFO			"ov_info"
#define		OV_CLEAR		"ov_clear"
#define		OV_PCM_TOTAL		"ov_pcm_total"

typedef int (*WiredOvOpen)(FILE *, OggVorbis_File *, char *, long);
typedef long (*WiredOvReadFloat)(OggVorbis_File *, float ***, int, int *bitstream);
typedef vorbis_info* (*WiredOvInfo)(OggVorbis_File *,int);
typedef int (*WiredOvClear)(OggVorbis_File *);
typedef ogg_int64_t (*WiredOvPcmTotal)(OggVorbis_File *, int);

class   WiredLibVorbis: public WiredApiCodec
{
 public:

  WiredLibVorbis(){std::cout << "[WIRED_OGG_CODEC] Vorbis ogg child created" << std::endl;Pass = 0; handle = NULL;}
  ~WiredLibVorbis(){if (handle) dlclose(handle);}
  WiredLibVorbis(const WiredLibVorbis& copy){*this = copy;};

  /* Inits codec */
  void				init(list<s_LibInfo> &Info);

  /* Encode and decode methodes */
  int				encode(float** pcm);
  // int				decode(const char *path, t_Pcm *pcm);
  int				decode(const char *path, t_Pcm *pcm, unsigned long length);				/* Decode methodes */
  /* Checks format */
  bool				CanConvert(const wxString& path, int Decode);

  int				EndDecode();

  /* Operators */
  WiredLibVorbis		operator=(const WiredLibVorbis& right);

 private:
  void			*handle;
  OggVorbis_File	vf;
  WiredOvOpen		OvOpen;
  WiredOvReadFloat	OvReadFloat;
  WiredOvInfo		OvInfo;
  WiredOvClear		OvClear;
  WiredOvPcmTotal	OvPcmTotal;

  int			Pass;
  int			TotalRead;
  float			**pcmout;
  int			current_section;
  FILE			*FileToOpen;

  vorbis_info		*vi;

  bool			LoadSymbol();
};

extern "C" WiredApiCodec  *const WiredCodecConstruct();

#endif

