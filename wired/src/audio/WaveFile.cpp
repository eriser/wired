// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "WaveFile.h"
#include <math.h>
#include "../error.hh"

#include <iostream>

using namespace std;

WaveFile::WaveFile()
{
  LoadedInMem = false;
  Data = NULL;
  TempBuf = NULL;
  // Initialise le header de fichier
  memset (&sfinfo, 0, sizeof(sfinfo) );

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
: Filename(filename), LoadedInMem(loadmem), TempBuf(0), Pitch(1), 
  Invert(false), Data(0), Error(false),   m_open_mode (open_mode)
{
  unsigned long i;
  long k;

  // Initialise le header de fichier
  memset (&sfinfo, 0, sizeof(sfinfo) );

  //cout << "[WaveFile ] ----  LoadedInMem = " << LoadedInMem << endl;
  // Si le fichier doit etre ouvert en mode write ou read/write
  // on initialise son sf_info avec des donnees valides
  // Ces infos peuvent etre mis a jour avec la commande SFC_UPDATE_HEADER_NOW 
  if ( (m_open_mode == rwrite) || (m_open_mode == write) )
  {
	if (channel == 1)
	  sfinfo.channels   = 1;
	else
	  sfinfo.channels   = 2;
    sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    sfinfo.samplerate = 44100;
  }

  // Determine le mode d'ouverture du fichier
  // TODO : sf_open doit etre remplace par sf_open_fd pour gerer 
  // correctement les fichiers temporaires
  switch (open_mode)
  {
    case read : 
      sffile = sf_open (Filename.mb_str(*wxConvCurrent), SFM_READ, &sfinfo);
      break;
    case write : 
      sffile = sf_open (Filename.mb_str(*wxConvCurrent), SFM_WRITE, &sfinfo);
      break;
    case rwrite :
      sffile = sf_open (Filename.mb_str(*wxConvCurrent), SFM_RDWR, &sfinfo);
      cout << "[WAVEFILE] Temporary file created" << endl;
      break;
    case tmp :
      //cout << "Trying to creat a new temporary file " << endl;
      //sfinfo.format = SF_ENDIAN_CPU | SF_FORMAT_AU | SF_FORMAT_FLOAT;
      sffile = sf_open_fd (fileno(tmpfile()), SFM_RDWR, &sfinfo, TRUE) ;
      //cout << "Temporary file created" << endl;
      break;
    default: 
      throw cException (wxT("[WAVEFILE] : Unknow opening mode!!"));
  }

  if (sffile == NULL )
  {
    Error = true;
    cout << "[WAVEFILE] Error opening file for << " << open_mode << "  : " << filename
      << "; with error : " << sf_strerror(0) << endl;

    // We retry with read only
    if (open_mode == rwrite)
    {
      cout << "[WAVEFILE] Could not open file for writing, trying read-only..." << endl;
      sffile = sf_open (Filename.mb_str(*wxConvCurrent), SFM_READ, &sfinfo);
      m_open_mode =0;
      if (sffile == NULL)
        throw Error::File(filename, wxString(sf_strerror(0), *wxConvCurrent));
    }
    else
      throw Error::File(filename, wxString(sf_strerror(0), *wxConvCurrent));
  }    
  /*  sf_close(sffile);

      if (!(sffile = sf_open(filename.c_str(), SFM_RDWR, &sfinfo)))
      {
      Error = true;
      cout << "Error opening file for read/write: " << sf_strerror(0) << endl;
      throw Error::File(filename, sf_strerror(0));
      }
      */
  long  size = sfinfo.channels * WAVE_TEMP_SIZE;
  unsigned long j, l;
  float tmp[size];

  if (loadmem)
  {
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
      /*	      for (i = 0; i < sfinfo.channels; i++)
                {		  
                for (j = NumberOfFrames, l = NumberOfFrames  i; j < k; j++, l += j + i)
                Data[i][j] = tmp[l];
                }*/
      NumberOfFrames += k;	  
    }
    while ((NumberOfFrames < sfinfo.frames) && (k > 0));
  }
  else
  {
    NumberOfFrames = sfinfo.frames;
    TempBuf = new float[sfinfo.channels * WAVE_TEMP_SIZE];
  }
 // cout << "[WAVEFILE] read: " << NumberOfFrames << "; sf: " << sfinfo.frames << endl;    
 cout << "[WAVEFILE] End" << endl;
}

WaveFile::WaveFile(short *buffer, unsigned int size, int channels, long rate)
  : Filename(wxT("")), LoadedInMem(true), sffile(0),  TempBuf(0), Pitch(1), Invert(false), 
  Error(false)
{
  memset (&sfinfo, 0, sizeof (sfinfo));
  if (!channels)
  {
    Error = true;
    throw Error::NoChannels();
  }
  NumberOfFrames = size / channels;
  sfinfo.channels = channels;
  //cout << "[WAVEFILE] Channels :" << channels << "; size: "  << size << "; NumberOfFrames: "  << NumberOfFrames << endl;    
  Data = new float *[channels];
  for (int i = 0; i < channels; i++)
  {
    Data[i] = new float[NumberOfFrames];
    for (int j = 0; j < NumberOfFrames; j++)
      Data[i][j] = (float)buffer[i * NumberOfFrames + j] / 32767.f;
  }
}

WaveFile::~WaveFile()
{
  if (sffile)
    sf_close(sffile);

  if (LoadedInMem)
  {
  	if (Data)
  	{
	    for (int i = 0; i < sfinfo.channels; i++)
	    	if(Data[i])
		      delete[] Data[i];
	    delete[] Data;
  	}
  }
  else if (TempBuf)
   delete[] TempBuf;
}

unsigned long WaveFile::Read(float **buf, long pos, long size, 
    long delta, long *new_pos)
{
  long ret, i, j;
  float		k;

  if (!LoadedInMem)
  {
    if (WAVE_TEMP_SIZE < size)
    {
    	if(TempBuf)
	      delete TempBuf;
      TempBuf = new float[sfinfo.channels * size];
    }
    if (Invert)	
      sf_seek(sffile, NumberOfFrames - pos - size, SEEK_SET);      
    else
      sf_seek(sffile, pos, SEEK_SET);      

    ret = sf_readf_float(sffile, TempBuf, size);      

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

