// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WiredSampleRate.h"

struct s_format_types	_FormatTypes[] =
{
	{SF_FORMAT_PCM_U8, paUInt8, _("Unsigned 8 bits")},
	{SF_FORMAT_PCM_S8, paInt8, _("8 bits")},
	{SF_FORMAT_PCM_16, paInt16, _("16 bits")},
	{SF_FORMAT_PCM_24, paInt24, _("24 bits")},
	{SF_FORMAT_PCM_32, paFloat32, _("Floating 32 bits")},
	{0, 0, NULL}
};

struct s_samplerate_types	_SampleRateTypes[]=
{
	{8000, wxT("8000 Hz")},
	{11025, wxT("11025 Hz")},
	{16000, wxT("16000 Hz")},
	{22050, wxT("22050 Hz")},
	{32000, wxT("32000 Hz")},
	{44100, wxT("44100 Hz")},
	{48000, wxT("48000 Hz")},
	{88200, wxT("88200 Hz")},
	{96000, wxT("96000 Hz")},
	{192000, wxT("192000 Hz")},
	{0, 0}
};
