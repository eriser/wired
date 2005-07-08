#include	"WiredLibFlac.h"


static bool die_(const char *msg)
{
  cout << "[FLAC] ERROR: " <<  msg << endl;
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
		printf("content... ");
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
		(void)die_("got more metadata blocks than expected");
		error_occurred_ = true;
	}
	else {
		if(!::FLAC__metadata_object_is_equal(expected_metadata_sequence_[current_metadata_number_], metadata)) {
			(void)die_("metadata block mismatch");
			error_occurred_ = true;
		}
	}
	total_samples = metadata->data.stream_info.total_samples;
	current_metadata_number_++;
}

void DecoderCommon::common_error_callback_(::FLAC__StreamDecoderErrorStatus status)
{
	if(!ignore_errors_) {
		printf("ERROR: got error callback: err = %u (%s)\n", (unsigned)status, ::FLAC__StreamDecoderErrorStatusString[status]);
		error_occurred_ = true;
	}
}

::FLAC__StreamDecoderWriteStatus FileDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
  int k, i, j;

  if (pass == 0)
    {
      index_pcm = 0;
      //      cout << index_pcm << " et " << total_samples << " * " << get_channels() << " = " << (total_samples * get_channels())<< endl;
      pcm = new FLAC__int32[total_samples * get_channels() +1];
      pass = 1;
    }

  k = 0;
  for( j = 0; j < frame->header.blocksize; j++ )
    for( i = 0; i < get_channels(); i++ )
      {
	FLAC__int32 sample;

	sample = buffer[i][j];
	pcm[index_pcm++] = sample;
      }
  
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
    cout << "[FLAC] FAILED, " <<  msg << endl;
  else
    cout <<"[FLAC] FAILED";
  
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
    cout << "[FLAC] FAILED at set_filename()" << endl;
    return false;
  }
  
  if(!set_md5_checking(true)) {
    cout << "{FLAC} FAILED at set_md5_checking()" << endl;
    return false;
  }
  
  cout << "[FLAC] init()... " << endl;
  if(init() != ::FLAC__FILE_DECODER_OK)
 	  return die();
  
  current_metadata_number_ = 0;
  
  cout << "[FLAC] process_until_end_of_file()... " ;
  
  if(!process_until_end_of_file()) {
    State state = get_state();
    cout << "[FLAC] FAILED, returned false, state = " << (unsigned)((::FLAC__FileDecoderState)state)  <<  "(" <<  state.as_cstring() << ")" << endl;
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
  
  cout << "[FLAC] allocating decoder instance... " << endl;
  decoder = new FileDecoder();
  if(0 == decoder) {
    cout << "[FLAC] FAILED, new returned NULL" << endl;
    return false;
  }
  if(!decoder->is_valid()) {
    cout << "[FLAC] FAILED, returned false" << endl;
    return false;
  }
  
  decoder->test_respond();

  OriginalPcm->pcm = decoder->pcm;
  OriginalPcm->PType = Int16;
  OriginalPcm->Channels = decoder->get_channels();
  OriginalPcm->TotalSample = decoder->total_samples;
  OriginalPcm->SampleRate = decoder->get_sample_rate();

  delete decoder;
  cout << "[FLAC] File decoded" << endl;
  
  free_metadata_blocks_();
  return true;
}

int WiredLibFlac::decode(const string filename, t_Pcm *pcm)
{
  if (!test_decoders(pcm))
    return 0;
  else       
    return 1;
}


extern "C" WiredApiCodec  *const WiredCodecConstruct()
{
  return ((WiredApiCodec*)new WiredLibFlac());
}
