// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WiredCodec_H__
#define __WiredCodec_H__

#include <wx/thread.h>
#include <wx/string.h>
#include <dlfcn.h>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include "./wiredcodec/WiredApiCodec.h"

using namespace std;

#define	WLIBCONSTRUCT	"WiredCodecConstruct"

class			WiredCodec
{
 public:
  WiredCodec();
  WiredCodec (const WiredCodec& copy){*this = copy;};
  WiredCodec	operator=(const WiredCodec& right);
  ~WiredCodec();
  
  void			      		Init();														/* Init and loads codecs */
  unsigned long				Decode(const wxString &filename, t_Pcm *pcm, 
  									unsigned long length);								/* Decode file */
  int			       		Encode(float **pcm, wxString OutExtension);					/* Encode stream */
  int			       		EndDecode();												/* Close file */

  list<wxString>				GetExtension(int Decode);												/* Returns a list of extensions readable by all codecs */
  bool						CanConvert(const wxString &filename, int Decode);							/* Proper codec installed ? */

 private:
  list<t_WLib>				_WLib;														/* Instance of codec found */
  wxString       				_WiredPath;													/* path of codecs */
  list<wxString>				_WiredSo;													/* list of codec.so */
  std::map<wxString, int>			codecToUse;													/* Codec to use :) */
  unsigned long				_CurrentUniqueID;											/* */
  char					*path;
  list<wxString>				_DecodeExtList;
  list<wxString>				_EncodeExtList;

  void					FeelExtension(list<t_LibInfo> Info);
  void	       				InitWLib();													/* look for .so */
  void	       				WLoadLib();													/* load all .so */
  void	       				WLibLoader(const wxString& filename);							/* load .so */
  int	       				CheckExtension(const wxString& str, const list<wxString>& ExtList);	/* check same occurence extensions */
  t_WLib       				FindBestCodec(wxString extension);							/* Not implemented yet */
  void	       				DumpCodec();
};

typedef WiredApiCodec* (*WiredCodecConstruct)();

static wxMutex		WiredCodecMutex;
//extern wxMutex				WiredCodecMutex;

#endif
