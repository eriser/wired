// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <iostream>
#include <cmath>
#include "WaveFile.h"
#include "../error.hh"

using namespace std;

WaveFile::WaveFile()
{

  InitVars();

  // This file must be opened in read/write mode.
  // Instializing its sf_info with good datas.
  // These infos can be updated with the SFC_UPDATE_HEADER_NOW commands.
  sfinfo.channels   = 1;
  sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
  sfinfo.samplerate = 1;

  // Decides what's the file's open mode.
  // TODO: sf_open must be replaced by sf_open_fd in order to manage temp files.
  sffile = sf_open_fd (fileno(tmpfile()), SFM_RDWR, &sfinfo, true);

  if (sffile == NULL)
  {
    Error = true;
    cout << "[WAVEFILE] Unable to create temporary file in read/write mode " << endl;
    throw Error::File(wxString(wxT("")), wxString(sf_strerror(0), wxConvCurrent));
  }
}


WaveFile::WaveFile(wxString filename, bool loadmem, t_opening_mode open_mode, int channel)
{
  InitVars();
  Filename = filename;
  m_open_mode = open_mode;
  LoadedInMem = loadmem;
  Open(filename, open_mode);

  InitBuffers();
}

WaveFile::WaveFile(short *buffer, unsigned int size, int channels, long rate)
{
  //  int	channelsCompt;
  //  int	framesCompt;

  InitVars();
  LoadedInMem = true;

  if (!channels)
  {
    Error = true;
    throw Error::NoChannels();
  }
  NumberOfFrames = size / channels;
  sfinfo.channels = channels;
  // cout << "[WAVEFILE] Channels :" << channels << "; size: "  << size << "; NumberOfFrames: "  << NumberOfFrames << endl;
  Data = new float *[channels];
  for (int channelsCompt = 0; channelsCompt < channels; channelsCompt++)
  {
    Data[channelsCompt] = new float[NumberOfFrames];
    for (int framesCompt = 0; framesCompt < NumberOfFrames; framesCompt++)
      Data[channelsCompt][framesCompt] = (float)buffer[channelsCompt * NumberOfFrames + framesCompt] / 32767.f;
  }
}

WaveFile::~WaveFile()
{
  // We must reset all vars.
  memset(&sfinfo, 0, sizeof(sfinfo));
  if (sffile)
    sf_close(sffile);
  sffile = NULL;

  if (Data)
    {
      for (int channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
       if(Data[channelsCompt])
         delete [] Data[channelsCompt];
      delete [] Data;
    }
  Data = NULL;

  if (TempBuf)
    delete [] TempBuf;
  TempBuf = NULL;
}

void           WaveFile::InitVars()
{
  associated_pattern = 1;
  Channel_to_read = 0;
  Data = NULL;
  LoadedInMem = false;
  Error = false;
  sffile = NULL;
  m_open_mode = read;

  // Initializing header files.
  memset(&sfinfo, 0, sizeof(sfinfo));
  NumberOfFrames = 0;
  TempBuf = NULL;
  Pitch = 1.f;
  Invert = false;
}

void           WaveFile::InitBuffers()
{
  long         size = sfinfo.channels * WAVE_TEMP_SIZE;

  // Load full wav into memory.
  if (LoadedInMem)
    {
      float		*tmp;
      unsigned long	channelsCompt;
      unsigned long	framesCompt;
      unsigned long	tmpCompt;
      long		readedFrames;

      tmp = new float[size];
      Data = new float *[sfinfo.channels];
      for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
       Data[channelsCompt] = new float [sfinfo.frames];
      NumberOfFrames = 0;

      do
       {
         readedFrames = sf_readf_float(sffile, tmp, WAVE_TEMP_SIZE);

         for (framesCompt = NumberOfFrames, tmpCompt = 0; framesCompt < (NumberOfFrames + readedFrames); framesCompt++)
           for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++, tmpCompt++)
             Data[channelsCompt][framesCompt] = tmp[tmpCompt];
         NumberOfFrames += readedFrames;
       }
      while ((NumberOfFrames < sfinfo.frames) && (readedFrames > 0));

      delete [] tmp;
    }
  else // Malloc a temporary buffer for reading file in the future
    {
      NumberOfFrames = sfinfo.frames;
      TempBuf = new float[size];
      memset(TempBuf, 0, sizeof(*TempBuf) * size);
    }
}

void           WaveFile::DumpSf()
{
  cout << this << "-----START-----" << endl;

  cout << "Sffile ptr : " << sffile << endl;
  cout << "Sfinfo : " << sfinfo.frames << endl;
  cout << "Sfinfo : " << sfinfo.samplerate << endl;
  cout << "Sfinfo : " << sfinfo.channels << endl;
  cout << "Sfinfo : " << sfinfo.format << endl;
  cout << "Sfinfo : " << sfinfo.sections << endl;
  cout << "Sfinfo : " << sfinfo.seekable << endl;
  cout << "-----ENDING------" << endl;

}

int             WaveFile::Open(wxString filename, t_opening_mode open_mode, int channel, int fd)
{
  // What's the file's open mode (write or read/write)
  // Initializing its sf_info with good datas.
  // These infos can be updated with SFC_UPDATE_HEADER_NOW commands.
  if ((m_open_mode == rwrite) || (m_open_mode == write) || (m_open_mode == tmp))
    {
      if (channel == 1)
        sfinfo.channels   = 1;
      else
        sfinfo.channels   = 2;
      sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

      sfinfo.samplerate = 44100;
    }

  // What's the file's open mode
  switch (open_mode)
    {
    case read :
      sffile = sf_open(Filename.mb_str(), SFM_READ, &sfinfo);
      break;
    case write :
      sffile = sf_open(Filename.mb_str(), SFM_WRITE, &sfinfo);
      break;
    case rwrite :
      sffile = sf_open(Filename.mb_str(), SFM_RDWR, &sfinfo);
      cout << "[WAVEFILE] Temporary file created " << Filename.mb_str() << endl;
      break;
    case tmp :
      if (fd != -1)
        sffile = sf_open_fd(fd, SFM_RDWR, &sfinfo, TRUE);
      break;
    default:
      throw cException(wxT("[WAVEFILE] : Unknow opening mode!!"));
    }

  if (sffile == NULL)
    {
      Error = true;
      cout << "[WAVEFILE] " << open_mode << " " << sf_strerror(0) << " : " <<
        filename.mb_str() << endl;

      // We retry with read only
      if (open_mode == rwrite)
        {
          cout << "[WAVEFILE] Could not open file for writing, trying read-only..." << endl;
          m_open_mode = read;
          sf_close(sffile);
          sffile = sf_open(Filename.mb_str(), SFM_READ, &sfinfo);
        }
      if (sffile == NULL)
        {
          wxString      errmsg = wxString(sf_strerror(sffile), *wxConvCurrent);

          // we must close for free'ing resources
          sf_close(sffile);
          throw Error::File(filename, errmsg);
        }
    }
  return (0);
}

unsigned long WaveFile::Read(float **buf, long pos, long size,
    long delta, long *new_pos)
{
  long		ret;
  long		framesCompt;
  long		channelsCompt;
  float		returnedCompt;

  if (!LoadedInMem)
    {
      // this condition is very bad and useless, need to rewrite a bufferclass to avoid it
      if (WAVE_TEMP_SIZE < size)
	{
	  if(TempBuf)
	    delete [] TempBuf;
	  TempBuf = new float[sfinfo.channels * size];
	}

      if (Invert)
	ret = sf_seek(sffile, NumberOfFrames - pos - size, SEEK_SET);
      else
	ret = sf_seek(sffile, pos, SEEK_SET);

      if (ret == -1)
	{
	  // if seek failed, we re-open file
	  cout << "[WAVEFILE] " << sf_strerror(sffile) << endl;

	  // we must close file
	  sf_close(sffile);
	  if (Open(Filename, read))
	    {
	      if (Invert)
		ret = sf_seek(sffile, NumberOfFrames - pos - size, SEEK_SET);
	      else
		ret = sf_seek(sffile, pos, SEEK_SET);
	      if (ret)
		cout << "[WAVEFILE] Second : " << sf_strerror(sffile) << endl;
	    }
	}

    // read from file
    ret = sf_readf_float(sffile, TempBuf, size);

    // fill the buffer if we can't read enough from file
    if (ret < size)
      for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
        for (framesCompt = ret; framesCompt < size; framesCompt++)
          buf[channelsCompt][delta + framesCompt] = 0.f;

    ret *= sfinfo.channels;
    if (Invert)
    {
      for (framesCompt = 0, returnedCompt = ret; ((int)returnedCompt >= 0) && (framesCompt < ret); framesCompt++, returnedCompt -= Pitch)
      {
        for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
          buf[channelsCompt][delta + framesCompt] = TempBuf[(int)returnedCompt];
        returnedCompt -= channelsCompt - 1;
      }
    }
    else
    {
      for (framesCompt = 0, returnedCompt = 0.f + Channel_to_read; ((int)returnedCompt < ret) && (framesCompt < ret); framesCompt++, returnedCompt += Pitch)
      {
        for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
          buf[channelsCompt][delta + framesCompt] = TempBuf[(int)returnedCompt];
        returnedCompt +=  channelsCompt - 1;
      }
    }
    if (sfinfo.channels == 1)
      memcpy(buf[1], buf[0] , ret * sizeof(float));
    if (new_pos)
      *new_pos = pos + (int)ceilf(returnedCompt);
    return (ret);
  }

  // Loading in memory
  if (sfinfo.channels > 1)
  {
    if ((pos + size) > NumberOfFrames)
    {
      ret = size;
      size = NumberOfFrames - pos;
      for (framesCompt = size; framesCompt < ret; framesCompt++)
      {
        for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
          buf[channelsCompt][framesCompt + delta] = 0.f;
      }
    }
    if (Pitch == 1.f)
    {
      if (Invert)
      {
        ret = NumberOfFrames - pos;
        for (framesCompt = 0; framesCompt < size; framesCompt++)
          for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
            buf[channelsCompt][framesCompt + delta] = Data[channelsCompt][ret - framesCompt];
      }
      else
      {
        memcpy(buf[0] + delta, Data[0] + pos, size * sizeof(float));
        memcpy(buf[1] + delta, Data[1] + pos, size * sizeof(float));
      }
      if (new_pos)
        *new_pos = pos + size;
    }
    else
    {
      if (Invert)
      {
        double	dataCompt;

        ret = NumberOfFrames - pos - 1;
        for (returnedCompt = ret, framesCompt = 0, dataCompt = 0; (returnedCompt >= 0.0) && (framesCompt < size); returnedCompt -= Pitch, framesCompt++, dataCompt += Pitch)
        {
          for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
            buf[channelsCompt][delta + framesCompt] = Data[channelsCompt][(int)returnedCompt];
        }
        returnedCompt = dataCompt;
      }
      else
      {
        for (returnedCompt = 0.f, framesCompt = 0; (pos + (int)returnedCompt < NumberOfFrames) && (framesCompt < size);
            returnedCompt += Pitch, framesCompt++)
        {
          for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
            buf[channelsCompt][delta + framesCompt] = Data[channelsCompt][pos + (int)returnedCompt];
        }
        /*if (framesCompt < size)
          {
          for (; framesCompt < size; framesCompt++)
          for (channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
          buf[channelsCompt][delta + framesCompt] = 0.f;
          }	  */
      }
      if (new_pos)
        *new_pos = pos + (int)ceilf(returnedCompt);
    }

  }
  else // Mono
  {
    /*      memcpy(buf[0] + delta, Data[0] + pos , size * sizeof(float));
            memcpy(buf[1] + delta, buf[0], size * sizeof(float));
            if (new_pos)
     *new_pos = pos + size;*/

    if (Pitch == 1.f)
    {
      if (Invert)
      {
        ret = NumberOfFrames - pos;
        for (framesCompt = 0; framesCompt < size; framesCompt++)
          buf[0][framesCompt + delta] = Data[0][ret - framesCompt];
      }
      else
        memcpy(buf[0] + delta, Data[0] + pos, size * sizeof(float));
      if (new_pos)
        *new_pos = pos + size;
    }
    else
    {
      if (Invert)
      {
        double dataCompt;

        ret = NumberOfFrames - pos - 1;
        for (returnedCompt = ret, framesCompt = 0, dataCompt = 0; (returnedCompt >= 0.0) && (framesCompt < size); returnedCompt -= Pitch, framesCompt++, dataCompt += Pitch)
          buf[0][delta + framesCompt] = Data[0][(int)returnedCompt];
        returnedCompt = dataCompt;
      }
      else
      {
        for (returnedCompt = 0.f, framesCompt = 0; (pos + (int)returnedCompt < NumberOfFrames) && (framesCompt < size);
            returnedCompt += Pitch, framesCompt++)
          buf[0][delta + framesCompt] = Data[0][pos + (int)returnedCompt];
      }
      if (new_pos)
        *new_pos = pos + (int)ceilf(returnedCompt);
    }
    memcpy(buf[1] + delta, buf[0] + delta, size * sizeof(float));
  }
  return (size);
}

bool WaveFile::Read(float *buf, long pos)
{
  sf_seek(sffile, pos, SEEK_SET);
  return (sf_readf_float(sffile, buf, 1));
}


int WaveFile::SetCurrentPosition (int position)
{
  if (position > sfinfo.frames)
    throw cException (wxT("Seek error : index overflow"));
  return sf_seek (sffile, position, SEEK_SET);
};


sf_count_t WaveFile::ReadFloatF (float *rw_buffer, int nbr_of_frames)
{
  sf_count_t read_frames = sf_readf_float(sffile, rw_buffer, nbr_of_frames);

  return read_frames;
}


sf_count_t WaveFile::WriteFloatF (float *rw_buffer, int nbr_of_frames)
{
  sf_count_t read_frames = sf_writef_float(sffile, rw_buffer, nbr_of_frames);

  return read_frames;
}

void		WaveFile::SetChannelToRead(long channel)
{
  Channel_to_read = channel;
}

long		WaveFile::GetChannelToRead()
{
  return Channel_to_read;
}

WaveFile *WaveFile::Clone()
{
	cout << "ERROR ERROR Wired will fail miserably" << endl;
	WaveFile *aNewWaveFile = new WaveFile(*this);
	if (LoadedInMem)
	{
		aNewWaveFile->Data = new float *[sfinfo.channels];
		for (int channelsCompt = 0; channelsCompt < sfinfo.channels; channelsCompt++)
		{
			aNewWaveFile->Data[channelsCompt] = new float[NumberOfFrames];
			for (int framesCompt = 0; framesCompt < NumberOfFrames; framesCompt++)
				aNewWaveFile->Data[channelsCompt][framesCompt] = Data[channelsCompt][framesCompt];
		}
		return aNewWaveFile;
	}
}
