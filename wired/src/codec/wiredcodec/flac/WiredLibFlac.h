// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WiredLibFlac_H__
#define __WiredLibFlac_H__

#include	"WiredApiCodec.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <wx/file.h>

#include	<iostream>
#include	<dlfcn.h>

#include	<FLAC++/all.h>
#include	<FLAC/assert.h>

#define		SO_NAME				"libFLAC++.so"
#define		FLAC_FCC_LENGHT		4
#define		FLAC_FCC_LABEL		"fLaC"

#define		FLAC_FILE_DECODER_NEW				"FLAC__stream_decoder_new"
#define		FLAC_FILE_DECODER_DELETE			"FLAC__stream_decoder_delete"
#define		FLAC_FILE_DECODER_SET_MD5_CHECKING		"FLAC__stream_decoder_set_md5_checking"
#define		FLAC_FILE_DECODER_SET_FILENAME			"FLAC__stream_decoder_set_filename"
#define		FLAC_FILE_DECODER_SET_WRITE_CALLBACK		"FLAC__stream_decoder_set_write_callback"
#define		FLAC_FILE_DECODER_SET_METADATA_CALLBACK		"FLAC__stream_decoder_set_metadata_callback"
#define		FLAC_FILE_DECODER_SET_ERROR_CALLBACK		"FLAC__stream_decoder_set_error_callback"
#define		FLAC_FILE_DECODER_SET_CLIENT_DATA		"FLAC__stream_decoder_set_client_data"
#define		FLAC_FILE_DECODER_SET_METADATA_RESPOND_ALL	"FLAC__stream_decoder_set_metadata_respond_all"
#define		FLAC_FILE_DECODER_GET_STATE			"FLAC__stream_decoder_get_state"
#define		FLAC_FILE_DECODER_GET_RESOLVED_STATE_STRING	"FLAC__stream_decoder_get_resolved_state_string"
#define		FLAC_FILE_DECODER_GET_CHANNELS			"FLAC__stream_decoder_get_channels"
#define		FLAC_FILE_DECODER_GET_SAMPLE_RATE		"FLAC__stream_decoder_get_sample_rate"
#define		FLAC_FILE_DECODER_INIT				"FLAC__stream_decoder_init_file"
#define		FLAC_FILE_DECODER_FINISH			"FLAC__stream_decoder_finish"
#define		FLAC_FILE_DECODER_PROCESS_SINGLE		"FLAC__stream_decoder_process_single"
#define		FLAC_FILE_DECODER_GET_DECODE_POSITION		"FLAC__stream_decoder_get_decode_position"
#define		FLAC_FILE_DECODER_SEEK_ABSOLUTE			"FLAC__stream_decoder_seek_absolute"
#define		FLAC_FILE_DECODER_METADATA_CALLBACK		"FLAC__StreamDecoderMetadataCallback"
#define		FLAC_FILE_DECODER_WRITE_CALLBACK		"FLAC__StreamDecoderWriteCallback"

typedef FLAC__StreamDecoder* (*WiredFileDecoderNew)();
typedef void (*WiredFileDecoderDelete)(FLAC__StreamDecoder *);
typedef FLAC__bool (*WiredFileDecoderSetMd5Checking)(FLAC__StreamDecoder *, FLAC__bool);
typedef FLAC__bool (*WiredFileDecoderSetFilename)(FLAC__StreamDecoder *, const char *);
typedef FLAC__bool (*WiredFileDecoderSetWriteCallback)(FLAC__StreamDecoder *, FLAC__StreamDecoderWriteCallback);
typedef FLAC__bool (*WiredFileDecoderSetMetadataCallback)(FLAC__StreamDecoder *, FLAC__StreamDecoderMetadataCallback);
typedef FLAC__bool (*WiredFileDecoderSetErrorCallback)(FLAC__StreamDecoder *, FLAC__StreamDecoderErrorCallback);
typedef FLAC__bool (*WiredFileDecoderSetClientData)(FLAC__StreamDecoder *, void *);
typedef FLAC__bool (*WiredFileDecoderSetMetadataRespondAll)(FLAC__StreamDecoder *);
typedef	FLAC__StreamDecoderState (*WiredFileDecoderGetState)(const FLAC__StreamDecoder *);
typedef const char* (*WiredFileDecoderGetResolvedStateString)(const FLAC__StreamDecoder *);
typedef unsigned (*WiredFileDecoderGetChannels)(const FLAC__StreamDecoder *);
typedef unsigned (*WiredFileDecoderGetSampleRate)(const FLAC__StreamDecoder *);
typedef FLAC__StreamDecoderState (*WiredFileDecoderInit)(FLAC__StreamDecoder *);
typedef FLAC__bool (*WiredFileDecoderFinish)(FLAC__StreamDecoder *);
typedef FLAC__bool (*WiredFileDecoderProcessSingle)(FLAC__StreamDecoder *);
typedef FLAC__bool (*WiredFileDecoderGetDecodePosition)(const FLAC__StreamDecoder *, FLAC__uint64 *);
typedef FLAC__bool (*WiredFileDecoderSeekAbsolute)(FLAC__StreamDecoder *, FLAC__uint64);
typedef void (*WiredFileDecoderMetadataCallback)(const FLAC__StreamDecoder *, const FLAC__StreamMetadata *, void *);
typedef FLAC__StreamDecoderWriteStatus (*WiredFileDecoderWriteCallback)(const FLAC__StreamDecoder *, const FLAC__Frame *, const FLAC__int32 * const, void *);

const char *flacfilename_ = "";

bool test_decoders(t_Pcm *pcmOriginalPcm);

class   WiredLibFlac: public WiredApiCodec
{
 public:

  WiredLibFlac(){std::cout << "[WIRED_FLAC_CODEC] Child Flac created" << std::endl; handle = NULL;}
  ~WiredLibFlac(){if (handle) dlclose(handle);}
  WiredLibFlac(const WiredLibFlac& copy){*this = copy;};

  /* Inits codec */
  void				init(list<s_LibInfo> &Info);

  /* Encode and decode methodes */
  int				encode(float** pcm);
  int				decode(const char *path, t_Pcm *pcm, unsigned long length);
  int				EndDecode();

  /* Checks format */
  bool				CanConvert(const wxString& path, int Decode);

  /* Operators */
  WiredLibFlac		operator=(const WiredLibFlac& right);

 private:
  void						*handle;
  bool						LoadSymbol();
  FLAC__StreamDecoder				*decoder;
  void						*client_data;
/*   const FLAC__Frame				*frame;; */
/*   const FLAC__int32				*const buffer[]; */
   int						Pass;

  WiredFileDecoderNew				FileDecoderNew;
  WiredFileDecoderDelete			FileDecoderDelete;
  WiredFileDecoderSetMd5Checking		FileDecoderSetMd5Checking;
  WiredFileDecoderSetFilename			FileDecoderSetFilename;
  WiredFileDecoderSetWriteCallback		FileDecoderSetWriteCallback;
  WiredFileDecoderSetMetadataCallback		FileDecoderSetMetadataCallback;
  WiredFileDecoderSetErrorCallback		FileDecoderSetErrorCallback;
  WiredFileDecoderSetClientData			FileDecoderSetClientData;
  WiredFileDecoderSetMetadataRespondAll		FileDecoderSetMetadataRespondAll;
  WiredFileDecoderGetState			FileDecoderGetState;
  WiredFileDecoderGetResolvedStateString	FileDecoderGetResolvedStateString;
  WiredFileDecoderGetChannels			FileDecoderGetChannels;
  WiredFileDecoderGetSampleRate			FileDecoderGetSampleRate;
  WiredFileDecoderInit				FileDecoderInit;
  WiredFileDecoderFinish			FileDecoderFinish;
  WiredFileDecoderProcessSingle			FileDecoderProcessSingle;
  WiredFileDecoderGetDecodePosition		FileDecoderGetDecodePosition;
  WiredFileDecoderSeekAbsolute			FileDecoderSeekAbsolute;
  WiredFileDecoderMetadataCallback		FileDecoderMetadataCallback;
  WiredFileDecoderWriteCallback			FileDecoderWriteCallback;
};


extern "C" WiredApiCodec  *const WiredCodecConstruct();

#endif

