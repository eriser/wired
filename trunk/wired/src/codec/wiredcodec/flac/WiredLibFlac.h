#ifndef __WiredLibFlac_H__
#define __WiredLibFlac_H__

#include	"../WiredApiCodec.h"

#include	<iostream>
#include	<dlfcn.h>

#include	<FLAC++/all.h>
#include	<FLAC/assert.h>

#define		SO_NAME				"libFLAC++.so"
#define		FLAC_FCC_LENGHT		4
#define		FLAC_FCC_LABEL		"fLaC"


#ifdef _MSC_VER
// warning C4800: 'int' : forcing to bool 'true' or 'false' (performance warning)
#pragma warning ( disable : 4800 )
#endif

static ::FLAC__StreamMetadata streaminfo_, padding_, seektable_, application1_, application2_, vorbiscomment_, cuesheet_, unknown_;
static ::FLAC__StreamMetadata *expected_metadata_sequence_[8];
static unsigned num_expected_;
static const char *flacfilename_ = "";
static unsigned flacfilesize_;
static int pass = 0;

bool test_decoders(t_Pcm *pcmOriginalPcm);

class   WiredLibFlac: public WiredApiCodec
{
 public:

  WiredLibFlac(){std::cout << "[WIRED_FLAC_CODEC] Child Flac created" << std::endl;}
  ~WiredLibFlac(){}
  WiredLibFlac(const WiredLibFlac& copy){*this = copy;};

  /* Inits codec */
  void				init(list<s_LibInfo> &Info);

  /* Encode and decode methodes */
  int				encode(float** pcm);
  int				decode(const char *path, t_Pcm *pcm);
  
  /* Checks format */
  bool				canDecode(const char* path);

  /* Operators */
  WiredLibFlac		operator=(const WiredLibFlac& right);
  
 private:
  void			*handle;
};

class DecoderCommon {
public:
	FILE *file_;
	unsigned current_metadata_number_;
	bool ignore_errors_;
	bool error_occurred_;

	DecoderCommon(): file_(0), current_metadata_number_(0), ignore_errors_(false), error_occurred_(false) { }
	::FLAC__StreamDecoderReadStatus common_read_callback_(FLAC__byte buffer[], unsigned *bytes);
	::FLAC__StreamDecoderWriteStatus common_write_callback_(const ::FLAC__Frame *frame);
	void common_metadata_callback_(const ::FLAC__StreamMetadata *metadata);
	void common_error_callback_(::FLAC__StreamDecoderErrorStatus status);
	int total_samples;
};


class FileDecoder : public FLAC::Decoder::File, public DecoderCommon {
public:
	FileDecoder(): FLAC::Decoder::File(), DecoderCommon() { }
	~FileDecoder() {pcm = 0; pass = 0;}

	// from FLAC::Decoder::File
	::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	void error_callback(::FLAC__StreamDecoderErrorStatus status);

	bool die(const char *msg = 0) const;
	int	index_pcm;
	FLAC__int32	*pcm;
	bool test_respond();
};


extern "C" WiredApiCodec  *const WiredCodecConstruct();

#endif

