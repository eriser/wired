// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "WriteWaveFile.h"
#include "../error.hh"

WriteWaveFile::WriteWaveFile(string filename, int sample_rate, int channels, 
			     unsigned long format, int type)
  : Filename(filename)
{
  memset (&sfinfo, 0, sizeof (sfinfo));

  sfinfo.samplerate = sample_rate;
  sfinfo.frames = 0; // dummy
  sfinfo.channels = channels;
  sfinfo.format = type | format;  

  if (!(sffile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo)))
    throw Error::File(filename, sf_strerror(0));
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

bool WriteWaveFile::WriteTitle(string s)
{
  return (sf_set_string (sffile, SF_STR_TITLE, s.c_str()));
}

bool WriteWaveFile::WriteCopyright(string s)
{
  return (sf_set_string (sffile, SF_STR_COPYRIGHT, s.c_str()));
}

bool WriteWaveFile::WriteSoftware(string s)
{
  return (sf_set_string (sffile, SF_STR_SOFTWARE, s.c_str()));
}

bool WriteWaveFile::WriteArtist(string s)
{
  return (sf_set_string (sffile, SF_STR_ARTIST, s.c_str()));
}

bool WriteWaveFile::WriteComment(string s)
{
  return (sf_set_string (sffile, SF_STR_COMMENT, s.c_str()));

}
bool WriteWaveFile::WriteDate(string s)
{
  return (sf_set_string (sffile, SF_STR_DATE, s.c_str()));
}
