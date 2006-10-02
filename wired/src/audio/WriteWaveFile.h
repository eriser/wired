// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WRITEWAVEFILE_H__
#define __WRITEWAVEFILE_H__

#include <iostream>
#include <wx/wx.h>

using namespace std;

#include <sndfile.h>

/**
 * Writes wave files.
 * It can write some stuff in wave files like the title, the artist ...
 */
class WriteWaveFile
{

 public:

  /**
   * Default constructor
   * \param filename a wxString. the file's path.
   * \param sample_rate an int, the file's samplerate.
   * \param channels an int, the channels' number of the file.
   * \param format an unsigned long format, the file's format.
   * \param type an int, the file's type.
   * By default it's SF_FORMAT_WAV.
   */
  WriteWaveFile(wxString filename, int sample_rate, int channels, unsigned long format,
		int type = SF_FORMAT_WAV);

  /**
   * The main destructor.
   */
  ~WriteWaveFile();

  /**
   * Closes the file.
   */
  void		Close();

  /**
   * Writes a short data in the file.
   * \param ptr a short pointer, the short to write.
   * \param items a sf_count_t, some information about the file.
   * \return returns a sf_count_t, the returned value from sf_write_short.
   */
  sf_count_t	WriteShort(short *ptr, sf_count_t items);

  /**
   * Writes a int data in the file.
   * \param ptr a int pointer, the int to write.
   * \param items a sf_count_t, some information about the file.
   * \return returns a sf_count_t, the returned value from sf_write_int.
   */
  sf_count_t	WriteInt(int *ptr, sf_count_t items);

  /**
   * Writes a float data in the file.
   * \param ptr a float pointer, the float to write.
   * \param items a sf_count_t, some information about the file.
   * \return returns a sf_count_t, the returned value from sf_write_float.
   */
  sf_count_t	WriteFloat(float *ptr, sf_count_t items);

  /**
   * Writes a double data in the file.
   * \param ptr a double pointer, the double to write.
   * \param items a sf_count_t, some information about the file.
   * \return returns a sf_count_t, the returned value from sf_write_double.
   */
  sf_count_t	WriteDouble(double *ptr, sf_count_t items);


  /**
   * Writes the title in the file.
   * \param s a wxString, the title to set.
   * \return returns a bool, if the writing did success it returns TRUE
   * else it returns FALSE.
   */
  bool		WriteTitle(wxString s);

  /**
   * Writes the copyright in the file.
   * \param s a wxString, the copyright to set.
   * \return returns a bool, if the writing did success it returns TRUE
   * else it returns FALSE.
   */
  bool		WriteCopyright(wxString s);

  /**
   * Writes the software's name in the file.
   * \param s a wxString, the software to set.
   * \return returns a bool, if the writing did success it returns TRUE
   * else it returns FALSE.
   */
  bool		WriteSoftware(wxString s);

  /**
   * Writes the artist's name in the file.
   * \param s a wxString, the artist to set.
   * \return returns a bool, if the writing did success it returns TRUE
   * else it returns FALSE.
   */
  bool		WriteArtist(wxString s);

  /**
   * Writes the comment in the file.
   * \param s a wxString, the comment to set.
   * \return returns a bool, if the writing did success it returns TRUE
   * else it returns FALSE.
   */
  bool		WriteComment(wxString s);

  /**
   * Writes the date in the file.
   * \param s a wxString, the date to set.
   * \return returns a bool, if the writing did success it returns TRUE
   * else it returns FALSE.
   */
  bool		WriteDate(wxString s);

  /**
   * The file's path.
   */
  wxString	Filename;



 protected:

  /**
   * The opened file.
   */
  SNDFILE	*sffile;

  /**
   * the file's informations.
   */
  SF_INFO	sfinfo;

};

#endif
