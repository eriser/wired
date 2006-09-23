// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WaveFile.h"
#include <math.h>
#include "../error.hh"

#include <iostream>

using namespace std;

WaveFile::WaveFile()
{
  InitVars();

  // Ce fichier doit etre ouvert en mode read/write
  // on initialise son sf_info avec des donnees valides
  // Ces infos peuvent etre mis a jour avec la commande SFC_UPDATE_HEADER_NOW 
  sfinfo.channels   = 1;
  sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
  sfinfo.samplerate = 1;

  // Determine le mode d'ouverture du fichier
  // TODO : sf_open doit etre remplace par sf_open_fd pour gerer 
  // correctement les fichiers temporaires
  sffile = sf_open_fd (fileno( tmpfile() ), SFM_RDWR, &sfinfo, true);

  if (sffile == NULL )
  {
    Error = true;
    cout << "[WAVEFILE] Unable to create temporary file in read/write mode " << endl;
    throw Error::File(wxString(wxT("")), wxString(sf_strerror(0), wxConvCurrent));
  }    
}


WaveFile::WaveFile(wxString filename, bool loadmem, t_opening_mode open_mode, int channel) 
{
  unsigned long i;
  long k;

  InitVars();
  Filename = filename;
  m_open_mode = open_mode;
  LoadedInMem = loadmem;
  Open(filename, open_mode);

  InitBuffers();
}

WaveFile::WaveFile(short *buffer, unsigned int size, int channels, long rate)
{
  int	i;
  int	j;

  InitVars();
  LoadedInMem = true;

  if (!channels)
  {
    Error = true;
    throw Error::NoChannels();
  }
  NumberOfFrames = size / channels;
  sfinfo.channels = channels;
  //cout << "[WAVEFILE] Channels :" << channels << "; size: "  << size << "; NumberOfFrames: "  << NumberOfFrames << endl;    
  Data = new float *[channels];
  for (i = 0; i < channels; i++)
  {
    Data[i] = new float[NumberOfFrames];
    for (j = 0; j < NumberOfFrames; j++)
      Data[i][j] = (float)buffer[i * NumberOfFrames + j] / 32767.f;
  }
}

WaveFile::~WaveFile()
{
  // we must reset all vars
  memset(&sfinfo, 0, sizeof(sfinfo));
  if (sffile)
    sf_close(sffile);
  sffile = NULL;

  if (Data)
    {
      for (int i = 0; i < sfinfo.channels; i++)
       if(Data[i])
         delete [] Data[i];
      delete [] Data;
    }
  Data = NULL;

  if (TempBuf)
    delete [] TempBuf;
  TempBuf = NULL;
}

void           WaveFile::InitVars()
{
  Data = NULL;
  LoadedInMem = false;
  Error = false;
  sffile = NULL;
  m_open_mode = read;

  // Initialise le header de fichier
  memset(&sfinfo, 0, sizeof(sfinfo));
  NumberOfFrames = 0;
  TempBuf = NULL;
  Pitch = 1.f;
  Invert = false;
}

void           WaveFile::InitBuffers()
{
  long         size = sfinfo.channels * WAVE_TEMP_SIZE;

  // load full wav into memory
  if (LoadedInMem)
    {
      float            *tmp;
      unsigned long    i;
      unsigned long    j, l;
      long             k;

      tmp = new float[size];
      Data = new float *[sfinfo.channels];
      for (i = 0; i < sfinfo.channels; i++)
       Data[i] = new float [sfinfo.frames];
      NumberOfFrames = 0;

      do
       {
         k = sf_readf_float(sffile, tmp, WAVE_TEMP_SIZE);

         for (j = NumberOfFrames, l = 0; j < (NumberOfFrames + k); j++)
           for (i = 0; i < sfinfo.channels; i++, l++)
             Data[i][j] = tmp[l];
         NumberOfFrames += k;    
       }
      while ((NumberOfFrames < sfinfo.frames) && (k > 0));

      delete [] tmp;
    }
  else // malloc a temporary buffer for reading file in the future
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
  // Si le fichier doit etre ouvert en mode write ou read/write
  // on initialise son sf_info avec des donnees valides
  // Ces infos peuvent etre mis a jour avec la commande SFC_UPDATE_HEADER_NOW 
  if ((m_open_mode == rwrite) || (m_open_mode == write) || (m_open_mode == tmp))
    {
      if (channel == 1)
        sfinfo.channels   = 1;
      else
        sfinfo.channels   = 2;
      sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

      sfinfo.samplerate = 44100;
    }

  // Determine le mode d'ouverture du fichier
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
  long		ret, i, j;
  float		k;
  double	kk;

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
      for (j = 0; j < sfinfo.channels; j++)
        for (i = ret; i < size; i++)
          buf[j][delta + i] = 0.f;      

    ret *= sfinfo.channels;
    if (Invert)
    {	
      for (i = 0, k = ret; ((int)k >= 0) && (i < ret); i++, k -= Pitch)
      {
        for (j = 0; j < sfinfo.channels; j++)
          buf[j][delta + i] = TempBuf[(int)k];
        k -= j - 1;
      }

    }
    else
    {
      for (i = 0, k = 0.f; ((int)k < ret) && (i < ret); i++, k += Pitch)
      {
        for (j = 0; j < sfinfo.channels; j++)
          buf[j][delta + i] = TempBuf[(int)k];
        k += j - 1;
      }
    }
    if (sfinfo.channels == 1)	
      memcpy(buf[1], buf[0] , ret * sizeof(float));
    if (new_pos)
      *new_pos = pos + (int)ceilf(k);
    return (ret);      
  }
  // Loading en mem

  if (sfinfo.channels > 1)
  {       
    if ((pos + size) > NumberOfFrames)
    {
      ret = size;
      size = NumberOfFrames - pos;
      for (i = size; i < ret; i++)
      {
        for (j = 0; j < sfinfo.channels; j++)
          buf[j][i + delta] = 0.f;
      }
    }
    if (Pitch == 1.f)
    {
      if (Invert)
      {
        ret = NumberOfFrames - pos;
        for (i = 0; i < size; i++)
          for (j = 0; j < sfinfo.channels; j++)
            buf[j][i + delta] = Data[j][ret - i];
      }
      else
      {
        memcpy(buf[0] + delta, Data[0] + pos, size * sizeof(float));
        memcpy(buf[1] + delta, Data[1] + pos, size * sizeof(float));
      }	  
      if (new_pos)
      {
        *new_pos = pos + size;
      }
    }
    else
    {
      if (Invert)
      {
        double kk;
        ret = NumberOfFrames - pos - 1;
        for (k = ret, i = 0, kk = 0; (k >= 0.0) && (i < size); k -= Pitch, i++, kk += Pitch)
        {
          for (j = 0; j < sfinfo.channels; j++)
            buf[j][delta + i] = Data[j][(int)k];
        }
        k = kk;
      }
      else
      {
        for (k = 0.f, i = 0; (pos + (int)k < NumberOfFrames) && (i < size); 
            k += Pitch, i++)
        {
          for (j = 0; j < sfinfo.channels; j++)
            buf[j][delta + i] = Data[j][pos + (int)k];
        }
        /*if (i < size)
          {
          for (; i < size; i++)
          for (j = 0; j < sfinfo.channels; j++)
          buf[j][delta + i] = 0.f;
          }	  */
      }
      if (new_pos)
        *new_pos = pos + (int)ceilf(k);
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
        for (i = 0; i < size; i++)
          buf[0][i + delta] = Data[0][ret - i];
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
        double kk;
        ret = NumberOfFrames - pos - 1;
        for (k = ret, i = 0, kk = 0; (k >= 0.0) && (i < size); k -= Pitch, i++, kk += Pitch)
          buf[0][delta + i] = Data[0][(int)k];
        k = kk;
      }
      else
      {
        for (k = 0.f, i = 0; (pos + (int)k < NumberOfFrames) && (i < size); 
            k += Pitch, i++)
          buf[0][delta + i] = Data[0][pos + (int)k];
      }
      if (new_pos)
        *new_pos = pos + (int)ceilf(k);
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
  if ( position > sfinfo.frames )
    throw cException (wxT("Seek error : index overflow"));
  return sf_seek (sffile, position, SEEK_SET); 
};


sf_count_t WaveFile::ReadFloatF (float *rw_buffer, int nbr_of_frames)
{
  sf_count_t read_frames = sf_readf_float (sffile, rw_buffer, nbr_of_frames) ;
  return read_frames;
}


sf_count_t WaveFile::WriteFloatF (float *rw_buffer, int nbr_of_frames)
{
  sf_count_t read_frames = sf_writef_float (sffile, rw_buffer, nbr_of_frames) ;
  return read_frames;
}
