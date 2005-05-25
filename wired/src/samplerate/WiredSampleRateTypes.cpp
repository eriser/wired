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
