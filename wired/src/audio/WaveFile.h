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

/**
 * Manages wave files.
 * With this, it's possible to easily manipulate wave file like read, write...
 */
class WaveFile
{
 public:

  /**
   * t_opening_mode manages all opening modes for the wave files.
   */
  typedef enum
    {
      read=0,
      write,
      rwrite,
      tmp
    } t_opening_mode;

 public:

  /**
   * This is the main WaveFile's constructor.
   */
  WaveFile();

  /**
   * This second constructor aims to instantiate a WaveFile object with
   * a filename, a loading mode, an opening mode and a channel.
   * \param filename a wxString, the file's name.
   * \param loadmem a bool, if the file will be mapped in the memory.
   * \param open_mode a t_opening_mode, the file's permissions.
   * By default wave files are opened in read mode.
   * \param channel an int, the channel's number. Currently it's not yet implemented.
   * By default the channel is 2.
   */
  WaveFile(wxString filename, bool loadmem = true, t_opening_mode open_mode = read, int channel = 2);


  /**
   * This third constructor aims to instantiate a WaveFile object with
   * a buffer, a size, a channel and a rate.
   * \param buffer a short pointer, the wave file's content.
   * \param size a unsigned int, the wave file's size.
   * \param channels an int, the channel's number. Currently it's not yet implemented.
   * \param rate a long, the wave file's rate.
   */
  WaveFile(short *buffer, unsigned int size, int channels, long rate);

  /**
   * WaveFile's destructor
   */
  ~WaveFile();

 private:

  /**
   * Initializes every attribute of this class.
   */
  void	InitVars();

  /**
   * Maps file in a memory.
   */
  void	InitBuffers();

  /**
   * Dumps wave file's informations.
   */
  void	DumpSf();

  /**
   * Opens a file with a path, a specific mode, a channel and a fd.
   * \param filename a wxString, the file's name.
   * \param open_mode a t_opening_mode, the file's permissions.
   * \param channel an int, the file's channel.
   * \param fd an int, the file descriptor of the wave file.
   * \return returns an int. It's the openning status.
   * Currently it always returns 0.
   */
  int Open(wxString filename, t_opening_mode open_mode = read, int channel = 2, int fd = -1);


 public:

  /**
   * Duplicates the static WaveFile object.
   * \return returns a new instantiated WaveFile object.
   */
  WaveFile *Clone()
    {
      cout << "ERROR ERROR Wired will fail miserably" << endl;

      WaveFile		*aNewWaveFile;

      aNewWaveFile = new WaveFile(*this);
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

  /**
   * Gets the channels' number.
   * \return returns a long. It's the channels' number.
   */
  long GetNumberOfChannels() { return sfinfo.channels; }

  /**
   * Gets the frames' number.
   * \return returns a long. It's the frames' number.
   */
  long GetNumberOfFrames()   { return NumberOfFrames; }

  /**
   * Reads nbr_of_frames from rw_buffer to the wave file.
   * \param rw_buffer a float pointer, the data content.
   * \param nbr_of_frames a int, the length.
   * By default it's setted to WAVE_TEMP_SIZE.
   * \return returns a sf_count_t.
   */
  sf_count_t ReadFloatF (float *rw_buffer, int nbr_of_frames = WAVE_TEMP_SIZE);

  /**
   * Writes nbr_of_frames from rw_buffer to the wave file.
   * \param rw_buffer a float pointer, the data content.
   * \param nbr_of_frames a int, the length.
   * \return returns a sf_count_t.
   */
  sf_count_t WriteFloatF (float *rw_buffer, int nbr_of_frames);

  /**
   * Sets a new cursor's position.
   * \param position an int. the new position.
   * By default it's setted to 0.
   * \return returns an int. It's the current position.
   */
  int SetCurrentPosition (int position = 0);

  /**
   * Gets the file's open mode.
   * \return returns an int, the file's open mode.
   */
  int GetOpenMode() const
  { return m_open_mode; };

  /**
   * Gets the channels' number.
   * \return returns an int, the channels' number.
   */
  int GetNumberOfChannels() const
  { return sfinfo.channels; };

  /**
   * Sets a new frames' number.
   * \param frames_nbr an int. the new frames' number.
   */
  void SetNumberOfFrames( int frames_nbr )
  {
    sfinfo.frames = frames_nbr;
    NumberOfFrames = sfinfo.frames;
  };

  /**
   * Gets the file's format.
   * \return returns an int, the file's format.
   */
  int GetFormat() const
  { return sfinfo.format; };

  /**
   * Gets the file's samplerate.
   * \return returns an int, the file's samplerate.
   */
  int GetSampleRate() const
  { return sfinfo.samplerate; };

  /**
   * Gets the file's pointer.
   * \return returns a SNDFILE pointer, the file's pointer.
   */
  SNDFILE * GetFilePtr() const
  { return sffile; };

  /**
   * Reads the file and get the content in the buf buffer.
   * \param buf a float pointer on pointer, the buffer which gets the content.
   * \param pos a long, the index.
   * \param size a long, the reading size.
   * \param delta a long.
   * By default it's setted to 0.
   * \param new_pos a long pointer, the new pointer's position.
   * By default it's setted to 0x0.
   * \return returns an unsigned long, the readed size.
   */
  unsigned long	Read(float **buf, long pos, long size,
		     long delta = 0, long *new_pos = 0x0);

  /**
   * Reads the file and fills the content to the buf pointer.
   * \param buf a float pointer, the readed buffer.
   * \param pos a long, the position to begin the reading.
   * \return returns a bool, false if it's failed otherwise true.
   */
  bool		Read(float *buf, long pos);

  /**
   * Sets the file's pitch.
   * \param p a float, the new pitch.
   */
  void		SetPitch(float p) { Pitch = p; }

  /**
   * Sets the invert status.
   * \param inv a bool, a new invert status.
   */
  void		SetInvert(bool inv) { Invert = inv; }

  /**
   * The data table.
   */
  float	   **Data;

  /**
   * The file's name.
   */
  wxString   Filename;

  /**
   * The flag to know if the file's content must be mapped in memory.
   */
  bool	   LoadedInMem;

  /**
   * The flag to know if there was errors.
   */
  bool	   Error;


 protected:

  /**
   * The file opening mode.
   */
  int m_open_mode;

  /**
   * The samplerate file.
   */
  SNDFILE *sffile;

  /**
   * file's informations.
   */
  SF_INFO  sfinfo;

  /**
   * The Frames' number.
   */
  long	   NumberOfFrames;

  /**
   * The tempory buffer.
   */
  float	  *TempBuf;

  /**
   * The pitch.
   */
  float	   Pitch;

  /**
   * Flag to know the way to read the file.
   */
  bool	   Invert;
};


/////////////////////////////////////////////
//  class cException
/////////////////////////////////////////////

/**
 * This class aims to manage wave files' exceptions
 */
class cException
{
 public:

  /**
   * The main cException constructor.
   * \param reason a wxString, the exception's message.
   */
  cException (wxString reason)
    { m_reason = reason; };

  /**
   * Gets exception's message.
   * \return returns the exception message.
   */
  wxString What() const
  { return m_reason; };

 private:

  /**
   * The exception's message.
   */
  wxString m_reason;
};


#endif
