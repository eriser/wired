#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	"WiredLibFlac.h"

WiredLibFlac			WiredLibFlac::operator=(const WiredLibFlac& right)
{
	if (this != &right)
	{
		handle = right.handle;
	}
	return *this;
}

static bool die_(const char *msg)
{
  cout << "[WIRED_FLAC_CODEC] ERROR: " <<  msg << endl;
  return false;
}

static void init_metadata_blocks_()
{
  FLAC__metadata_get_streaminfo(flacfilename_, &streaminfo_);
}

static void free_metadata_blocks_()
{
  (void)streaminfo_;
}


::FLAC__StreamDecoderReadStatus DecoderCommon::common_read_callback_(FLAC__byte buffer[], unsigned *bytes)
{
	if(error_occurred_)
		return ::FLAC__STREAM_DECODER_READ_STATUS_ABORT;

	if(feof(file_))
		return ::FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	else if(*bytes > 0) {
		unsigned bytes_read = ::fread(buffer, 1, *bytes, file_);
		if(bytes_read == 0) {
			if(feof(file_))
				return ::FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
			else
				return ::FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
		}
		else {
			*bytes = bytes_read;
			return ::FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
		}
	}
	else
		return ::FLAC__STREAM_DECODER_READ_STATUS_ABORT; /* abort to avoid a deadlock */
}

::FLAC__StreamDecoderWriteStatus DecoderCommon::common_write_callback_(const ::FLAC__Frame *frame)
{
	if(error_occurred_)
		return ::FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	if(
		(frame->header.number_type == ::FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER && frame->header.number.frame_number == 0) ||
		(frame->header.number_type == ::FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER && frame->header.number.sample_number == 0)
 	) {
		printf("[WIRED_FLAC_CODEC] content... ");
		fflush(stdout);
	}
	
	return ::FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void DecoderCommon::common_metadata_callback_(const ::FLAC__StreamMetadata *metadata)
{
	if(error_occurred_)
		return;

	printf("%d... ", current_metadata_number_);
	fflush(stdout);

	
	if(current_metadata_number_ >= num_expected_) {
		(void)die_("[WIRED_FLAC_CODEC] got more metadata blocks than expected");
		error_occurred_ = true;
	}
	else {
		if(!::FLAC__metadata_object_is_equal(expected_metadata_sequence_[current_metadata_number_], metadata)) {
			(void)die_("[WIRED_FLAC_CODEC] metadata block mismatch");
			error_occurred_ = true;
		}
	}
	total_samples = metadata->data.stream_info.total_samples;
	current_metadata_number_++;
}

void DecoderCommon::common_error_callback_(::FLAC__StreamDecoderErrorStatus status)
{
	if(!ignore_errors_) {
		printf("[WIRED_FLAC_CODEC] ERROR: got error callback: err = %u (%s)\n", (unsigned)status, ::FLAC__StreamDecoderErrorStatusString[status]);
		error_occurred_ = true;
	}
}

::FLAC__StreamDecoderWriteStatus FileDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
  int k, i, j;

  if (pass == 0)
    {
      index_pcm = 0;
      pcm = new FLAC__int32[total_samples * get_channels()];
      pass = 1;
    }

  k = 0;
  for( j = 0; j < frame->header.blocksize; j++ )
    for( i = 0; i < get_channels(); i++ )
      pcm[index_pcm++] = buffer[i][j];

  return (FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE);
}

void FileDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata)
{
	common_metadata_callback_(metadata);
}

void FileDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
	common_error_callback_(status);
}

bool FileDecoder::die(const char *msg) const
{
  State state = get_state();
  
  if(msg)
    cout << "[WIRED_FLAC_CODEC] FAILED, " <<  msg << endl;
  else
    cout <<"[WIRED_FLAC_CODEC] FAILED";
  
  printf(", state = %u (%s)\n", (unsigned)((::FLAC__FileDecoderState)state), state.as_cstring());
  if(state == ::FLAC__FILE_DECODER_SEEKABLE_STREAM_DECODER_ERROR) {
    FLAC::Decoder::SeekableStream::State state_ = get_seekable_stream_decoder_state();
    printf("      seekable stream decoder state = %u (%s)\n", (unsigned)((::FLAC__SeekableStreamDecoderState)state_), state_.as_cstring());
    if(state_ == ::FLAC__SEEKABLE_STREAM_DECODER_STREAM_DECODER_ERROR) {
      FLAC::Decoder::Stream::State state__ = get_stream_decoder_state();
      printf("      stream decoder state = %u (%s)\n", (unsigned)((::FLAC__StreamDecoderState)state__), state__.as_cstring());
    }
  }

return false;
}


bool FileDecoder::test_respond()
{
  if(!set_filename(flacfilename_)) {
    cout << "[WIRED_FLAC_CODEC] FAILED at set_filename()" << endl;
    return false;
  }
  
  if(!set_md5_checking(true)) {
    cout << "[WIRED_FLAC_CODEC] FAILED at set_md5_checking()" << endl;
    return false;
  }
  
  cout << "[WIRED_FLAC_CODEC] init()... " << endl;
  if(init() != ::FLAC__FILE_DECODER_OK)
 	  return die();
  
  current_metadata_number_ = 0;
  
  cout << "[WIRED_FLAC_CODEC] process_until_end_of_file()... " ;
  
  if(!process_until_end_of_file()) {
    State state = get_state();
    cout << "[WIRED_FLAC_CODEC] FAILED, returned false, state = " << (unsigned)((::FLAC__FileDecoderState)state)  <<  "(" <<  state.as_cstring() << ")" << endl;
    return false;
  }
  cout << "" << endl;
  finish();
  return true;
}

void WiredLibFlac::init(list<s_LibInfo> &Info)
{
  t_LibInfo		LibInfo;
  int			temp;

  LibInfo.CodecMask = EXIST;
  LibInfo.Extension = "flac\tFree Lossless Audio Codec (*.flac)";
  LibInfo.Note = 5;
  handle = dlopen(SO_NAME, RTLD_LAZY);
  if (!handle)
    {
      Info.push_back(LibInfo);
      return ;
    }
  
  LibInfo.CodecMask = DECODE;
  Info.push_back(LibInfo);

  dlclose(handle);
}

bool WiredLibFlac::canDecode(const char* path)
{
	int		fd;
	char	*buf;

	if ((fd = open(path, O_RDONLY)) == -1)
    	return false;
    buf = new char((FLAC_FCC_LENGHT + 1) * sizeof(char));
    buf[FLAC_FCC_LENGHT] = 0;
    if (read(fd, buf, FLAC_FCC_LENGHT) == FLAC_FCC_LENGHT)
    {
    	if (strcmp(buf, FLAC_FCC_LABEL) == 0)
    	{
    		delete buf;
    		return true;	
    	}
    }
	delete buf;
	return false;
}

int WiredLibFlac::encode(float** pcm)
{
  return 1;
}


bool test_decoders(t_Pcm *OriginalPcm)
{ 
  FileDecoder *decoder;
  
  init_metadata_blocks_();
  num_expected_ = 0;
  expected_metadata_sequence_[num_expected_++] = &streaminfo_;
  
  cout << "[WIRED_FLAC_CODEC] allocating decoder instance... " << endl;
  decoder = new FileDecoder();
  if(0 == decoder) {
    cout << "[WIRED_FLAC_CODEC] FAILED, new returned NULL" << endl;
    return false;
  }
  if(!decoder->is_valid()) {
    cout << "[WIRED_FLAC_CODEC] FAILED, returned false" << endl;
    return false;
  }
  
  decoder->test_respond();

  OriginalPcm->pcm = (float*)decoder->pcm;
  OriginalPcm->PType = Float32;
  OriginalPcm->Channels = decoder->get_channels();
  OriginalPcm->TotalSample = decoder->total_samples;
  OriginalPcm->SampleRate = decoder->get_sample_rate();

  delete decoder;
  cout << "[WIRED_FLAC_CODEC] File decoded" << endl;
  
  free_metadata_blocks_();
  return true;
}

int WiredLibFlac::decode(const char *path, t_Pcm *pcm)
{
  flacfilename_ = path;
  if (!test_decoders(pcm))
    return 0;
  else       
    return 1;
}


extern "C" WiredApiCodec  *const WiredCodecConstruct()
{
  return ((WiredApiCodec*)new WiredLibFlac());
}
