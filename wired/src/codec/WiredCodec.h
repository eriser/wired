#ifndef __WiredCodec_H__
#define __WiredCodec_H__

#include <wx/thread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include "./wiredcodec/WiredApiCodec.h"

using namespace std;

#define CODEC_PATH	"/usr/local/lib"
#define	WLIBCONSTRUCT	"WiredCodecConstruct"

class			WiredCodec
{
 public:
  WiredCodec();
  WiredCodec (const WiredCodec& copy){*this = copy;};
  ~WiredCodec();
  
  void			      		Init();														/* Init and loads codecs */
  unsigned long				Decode(const string &filename, t_Pcm *pcm, 
  									unsigned long length);								/* Decode file */
  int			       		Encode(float **pcm, string OutExtension);					/* Encode stream */
  int			       		EndDecode();												/* Close file */

  list<string>				GetExtension(int Decode);												/* Returns a list of extensions readable by all codecs */
  bool					CanConvert(const string &filename, int Decode);							/* Proper codec installed ? */

 private:
  list<t_WLib>				_WLib;														/* Instance of codec found */
  string       				_WiredPath;													/* path of codecs */
  list<string>				_WiredSo;													/* list of codec.so */
  std::map<string, int>			codecToUse;													/* Codec to use :) */
  unsigned long				_CurrentUniqueID;											/* */
  char					*path;
  list<string>				_DecodeExtList;
  list<string>				_EncodeExtList;

  void					FeelExtension(list<t_LibInfo> Info);
  void	       				InitWLib();													/* look for .so */
  void	       				WLoadLib();													/* load all .so */
  void	       				WLibLoader(const string& filename);							/* load .so */
  int	       				CheckExtension(const string& str, const list<string>& ExtList);	/* check same occurence extensions */
  t_WLib       				FindBestCodec(string extension);							/* Not implemented yet */
  void	       				DumpCodec();
};

typedef WiredApiCodec* (*WiredCodecConstruct)();

extern wxMutex				WiredCodecMutex;

#endif
