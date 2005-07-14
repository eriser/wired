#include "WiredSampleRate.h"

struct s_format_types	_FormatTypes[] =
{
	{SF_FORMAT_PCM_U8, paUInt8, "Unsigned 8 bits"},
	{SF_FORMAT_PCM_S8, paInt8, "8 bits"},
	{SF_FORMAT_PCM_16, paInt16, "16 bits"},
	{SF_FORMAT_PCM_24, paInt24, "24 bits"},
	{SF_FORMAT_PCM_32, paFloat32, "Floating 32 bits"},
	{0, 0}
	
};

struct s_samplerate_types	_SampleRateTypes[]=
{
	{8000, "8000 Hz"},
	{11025, "11025 Hz"},
	{16000, "16000 Hz"},
	{22050, "22050 Hz"},
	{32000, "32000 Hz"},
	{44100, "44100 Hz"},
	{48000, "48000 Hz"},
	{88200, "88200 Hz"},
	{96000, "96000 Hz"},
	{192000, "192000 Hz"},
	{0, 0}
};
