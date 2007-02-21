// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WiredAPICodec_H__
#define __WiredAPICodec_H__

#include <stdlib.h>
#include <list>
#include <wx/wx.h>
#include <wx/string.h>

#define	EXIST	0x1
#define	DECODE	0x2
#define	ENCODE	0x4

#define	CODEC_MASK	0x0

#define USE_FILENAME	1
#define USE_PCM		2

using namespace std;

typedef struct		s_LibInfo
{
  int  		CodecMask;					/* Codec capabilities */
  wxString	Extension;					/* Hanfled extension */
  short		Note;						/* Note to determine if that codec is the best suited for this extension */
}			t_LibInfo;

enum PcmType
  {
	UInt8 = 0,
	Int8,
	Int16,
	Int24,
	Float32
  };

typedef struct  s_Pcm
{
  void			*pcm;						/* audio streams */
  PcmType		PType;						/* Encoded format */
  int			TotalSample;				/* Total number of samples */
  int			SampleRate;					/* Samplerate */
  int			Channels;					/* Number of channels */
}		 		t_Pcm;

class   WiredApiCodec
{
 public:
  WiredApiCodec(){};
  ~WiredApiCodec(){};

  virtual void		init(list<s_LibInfo> &Info) = 0;			/* Inits codec and fills infos */
  virtual int		encode(float** pcm) = 0;					/* Encode function */
  virtual int		decode(const char *path, t_Pcm *pcm, 
  							unsigned long lenght) = 0;			/* Decode function */
  virtual int		EndDecode() = 0;							/* Closes file */
  virtual bool		CanConvert(const wxString& path, int Decode) = 0;			/* True if codec can decode file */
  void			SetuniqueId(unsigned long Id)				/* Sets unique Id */
  					{_UniqueId = Id;}
  unsigned long		GetUniqueId()								/* Returns unique Id */
  					{return _UniqueId;}
  					
 private:
   unsigned long		_UniqueId;								/* Used to identify codec */
};

typedef struct			s_WLib
{
	list<t_LibInfo>     Info;
	WiredApiCodec       *Codec;
	void				*handle;
}						t_WLib;

#endif
