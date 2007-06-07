// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WiredCodec_H__
#define __WiredCodec_H__

#include <list>
#include <map>
#include <wx/thread.h>
#include <wx/string.h>
#ifndef WIN32
#include <dlfcn.h>
#endif
#include "./wiredcodec/WiredApiCodec.h"

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

  std::list<wxString>				GetExtension(int Decode);												/* Returns a list of extensions readable by all codecs */
  bool						CanConvert(const wxString &filename, int Decode);							/* Proper codec installed ? */

 private:
	 std::list<t_WLib>				_WLib;														/* Instance of codec found */
  wxString       				_WiredPath;													/* path of codecs */
  std::list<wxString>				_WiredSo;													/* list of codec.so */
  std::map<wxString, int>			codecToUse;													/* Codec to use :) */
  unsigned long				_CurrentUniqueID;											/* */
  char					*path;
  std::list<wxString>				_DecodeExtList;
  std::list<wxString>				_EncodeExtList;

  void					FeelExtension(std::list<t_LibInfo> Info);
  void	       				InitWLib();													/* look for .so */
  void	       				WLoadLib();													/* load all .so */
  void	       				WLibLoader(const wxString& filename);							/* load .so */
  int	       				CheckExtension(const wxString& str, const std::list<wxString>& ExtList);	/* check same occurence extensions */
  t_WLib       				FindBestCodec(wxString extension);							/* Not implemented yet */
  void	       				DumpCodec();
};

typedef WiredApiCodec* (*WiredCodecConstruct)();

static wxMutex		WiredCodecMutex;
//extern wxMutex				WiredCodecMutex;

#endif
