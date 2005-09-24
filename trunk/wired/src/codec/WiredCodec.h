#ifndef __WiredCodec_H__
#define __WiredCodec_H__

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include <dlfcn.h>

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "./wiredcodec/WiredApiCodec.h"

using namespace std;

#define CODEC_PATH	"/usr/local/lib"

#define	WLIBCONSTRUCT	"WiredCodecConstruct"

#define BUF_SIZE	4049

class			WiredCodec
{
 public:
  WiredCodec();
  WiredCodec(const string& DataPath);
  ~WiredCodec();

  void			Init();
  t_Pcm      		Decode(const string &filename);
  int			Encode(float **pcm, string OutExtension);

  /*Return a list of extension readable by all codecs*/
  list<string>		GetExtension();

 private:

  /*look for .so*/
  void			InitWLib();

  /*load all .so*/
  void			WLoadLib();

  /*load .so*/
  void			WLibLoader(const string& filename);

  /*check extension to don t have same occurence two time*/
  int			CheckExtension(const string& str);

  /*look if it can decode the file*/
  bool			CanDecode(const string &filename){return false;}

  /*list of extension*/
  list<string>	_ExtList;

  /*Instance of codec found*/
  list<t_WLib>		_WLib;

  /*path of codecs*/
  string		_WiredPath;

  /*list of codec.so*/
  vector<string>	_WiredSo;

  t_WLib WiredCodec::FindBestCodec(string extension);
};

typedef WiredApiCodec* (*WiredCodecConstruct)();

#endif
