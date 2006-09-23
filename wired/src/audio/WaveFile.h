// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WAVEFILE_H__
#define __WAVEFILE_H__

#include <iostream>

using namespace std;

#include        <sndfile.h>
#include        <wx/toolbar.h>
#include        <wx/wx.h>

#define		WAVE_TEMP_SIZE		4096


/////////////////////////////////////////////
//  class WaveFile
/////////////////////////////////////////////


class WaveFile
{
 public:
  typedef enum 
    {
      read=0,
      write,
      rwrite,
      tmp
    } t_opening_mode;
  
 public:
  WaveFile(); 
  WaveFile(wxString filename, bool loadmem = true, t_opening_mode open_mode = read, int channel = 2);
  WaveFile(short *buffer, unsigned int size, int channels, long rate);
  ~WaveFile();
 
 private:
  void	InitVars();
  void	InitBuffers();
  void	DumpSf();
  int	Open(wxString filename, t_opening_mode open_mode = read, int channel = 2, int fd = -1);

 public:

  WaveFile *Clone()
    {
      cout << "ERROR ERROR Wired will fail miserably" << endl;
      WaveFile *w;
      w = new WaveFile(*this);
      if (LoadedInMem)
	{
	  w->Data = new float *[sfinfo.channels];
	  for (int i = 0; i < sfinfo.channels; i++)
	    {
	      w->Data[i] = new float[NumberOfFrames];
	      for (int j = 0; j < NumberOfFrames; j++)
		w->Data[i][j] = Data[i][j];
	    }
	  return w;
	}
    }

 public:
  long GetNumberOfChannels() { return sfinfo.channels; }
  long GetNumberOfFrames()   { return NumberOfFrames; }
  
  sf_count_t ReadFloatF (float *rw_buffer, int nbr_of_frames = WAVE_TEMP_SIZE);

  // Write nbr_of_frames from rw_buffer to the wave file
  sf_count_t WriteFloatF (float *rw_buffer, int nbr_of_frames);

  // Positionne l'index de lectur ou d'ecriture
  int SetCurrentPosition (int position = 0);

  int GetOpenMode() const
  { return m_open_mode; };

  int GetNumberOfChannels() const
  { return sfinfo.channels; };

  void SetNumberOfFrames( int frames_nbr )
  { 
    sfinfo.frames = frames_nbr; 
    NumberOfFrames = sfinfo.frames;
  };
  
  int GetFormat() const
  { return sfinfo.format; };

  int GetSampleRate() const
  { return sfinfo.samplerate; };

  SNDFILE * GetFilePtr() const
  { return sffile; };
  
  unsigned long	Read(float **buf, long pos, long size, 
		     long delta = 0, long *new_pos = 0x0);
  bool		Read(float *buf, long pos);

  void		SetPitch(float p) { Pitch = p; }
  void		SetInvert(bool inv) { Invert = inv; }

  float	   **Data;
  wxString   Filename;
  bool	   LoadedInMem;
  bool	   Error;

 protected:
  int m_open_mode; 
  SNDFILE *sffile;
  SF_INFO  sfinfo;
  long	   NumberOfFrames;
  float	  *TempBuf;
  float	   Pitch;
  bool	   Invert;
};


/////////////////////////////////////////////
//  class cException
/////////////////////////////////////////////

class cException
{
public:
	cException (wxString reason )
	{ m_reason = reason; };

	wxString What() const
	{ return m_reason; };

private:
	wxString m_reason;
};


#endif
