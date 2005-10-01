#ifndef __WiredAPICodec_H__
#define __WiredAPICodec_H__

#include <stdlib.h>
#include <list>
#include <string>

#define	EXIST	0x0
#define	DECODE	0x1
#define	ENCODE	0x2

#define	CODEC_MASK	0x0

#define USE_FILENAME	1
#define USE_PCM		2

using namespace std;

typedef struct		s_LibInfo
{
  /*what can do the lib*/
  int			CodecMask;
  /*extension readable by the lib*/
  string	       	Extension;
  /*Note to determine if that codec is the best for this extension*/
  short			Note;
  /*where it has to read the start of the extension format*/
  int			fccStartPos;
  /*string format's lenght*/
  int			fccLenght;
  /*format's name*/
  string		fccLabel;

}			t_LibInfo;

enum PcmType
  {
	UInt8 = 0,
	Int8,
	Int16,
	Int24,
	Float32
  };


typedef struct   s_Pcm
{
  void			*pcm;
  PcmType		PType;
  int			TotalSample;
  int			SampleRate;
  int			Channels;
}		 t_Pcm;



class   WiredApiCodec
{
 public:
  WiredApiCodec(){};
  ~WiredApiCodec(){};

  /*struct use for each codec to decode*/
 // void*			codecstruct;

  /*init codec and looking for it formal*/
  virtual void		init(list<s_LibInfo> &Info) = 0;

  /*encode and decode functions*/
  virtual int		encode(float** pcm) = 0;
//  virtual int		decode(const string &filename, t_Pcm *pcm) = 0;
  virtual int		decode(char *filename, t_Pcm *pcm) = 0;

  void			SetuniqueId(unsigned long Id){_UniqueId = Id;}
  unsigned long		GetUniqueId(){return _UniqueId;}
 private:
   unsigned long		_UniqueId;
  /*Return 1 if the codec can decode this file*/
};


typedef struct		s_WLib
{
  list<t_LibInfo>      	Info;
  WiredApiCodec*       	Codec;
//  t_Pcm			*pcm;
}			t_WLib;

#endif

