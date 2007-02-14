// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WriteWaveFile.h"
#include "../error.hh"

WriteWaveFile::WriteWaveFile(wxString filename, int sample_rate, int channels,
			     unsigned long format, int type)
  : Filename(filename)
{
  memset (&sfinfo, 0, sizeof (sfinfo));

  sfinfo.samplerate = sample_rate;
  sfinfo.frames = 0; // dummy
  sfinfo.channels = channels;
  sfinfo.format = type | format;

  if (!(sffile = sf_open(filename.mb_str(*wxConvCurrent), SFM_WRITE, &sfinfo)))
    throw Error::File(filename, wxString(sf_strerror(0), *wxConvCurrent));
  //WriteSoftware("Wired");
}

WriteWaveFile::~WriteWaveFile()
{
  Close();
}

void WriteWaveFile::Close()
{
  if (sffile)
    {
      sf_close(sffile);
      sffile = 0x0;
    }
}

sf_count_t WriteWaveFile::WriteShort(short *ptr, sf_count_t items)
{
  return (sf_write_short(sffile, ptr, items));
}

sf_count_t WriteWaveFile::WriteInt(int *ptr, sf_count_t items)
{
  return (sf_write_int(sffile, ptr, items));
}

sf_count_t WriteWaveFile::WriteFloat(float *ptr, sf_count_t items)
{
  return (sf_write_float(sffile, ptr, items));
}

sf_count_t WriteWaveFile::WriteDouble(double *ptr, sf_count_t items)
{
  return (sf_write_double(sffile, ptr, items));
}

bool WriteWaveFile::WriteTitle(wxString s)
{
  return (sf_set_string(sffile, SF_STR_TITLE, s.mb_str(*wxConvCurrent)));
}

bool WriteWaveFile::WriteCopyright(wxString s)
{
  return (sf_set_string(sffile, SF_STR_COPYRIGHT, s.mb_str(*wxConvCurrent)));
}

bool WriteWaveFile::WriteSoftware(wxString s)
{
  return (sf_set_string(sffile, SF_STR_SOFTWARE, s.mb_str(*wxConvCurrent)));
}

bool WriteWaveFile::WriteArtist(wxString s)
{
  return (sf_set_string(sffile, SF_STR_ARTIST, s.mb_str(*wxConvCurrent)));
}

bool WriteWaveFile::WriteComment(wxString s)
{
  return (sf_set_string(sffile, SF_STR_COMMENT, s.mb_str(*wxConvCurrent)));

}
bool WriteWaveFile::WriteDate(wxString s)
{
  return (sf_set_string(sffile, SF_STR_DATE, s.mb_str(*wxConvCurrent)));
}
