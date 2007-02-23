// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CLIPBOARD_H__
#define __CLIPBOARD_H__

#include "WaveFile.h"

/**
 * Manages the clipboard with wave files.
 * With this it's possible to copy, cut, paste and delete wave files.
 */
class cClipBoard : public WaveFile
{
 public:

  /**
   * Destructor
   */
  ~cClipBoard();

  /**
   * Static method to get the current object of this class.
   * \return returns the static ClipBoard pointer.
   */
  static cClipBoard& Global();

  /**
   * Copies frames in the clipboard.
   * \param wave a WaveFile, the wave file which must be edited.
   * \param from an int, where the copy must begin.
   * \param size_of_copy an int, the size of the copy.
   */
  void Copy (WaveFile& wave, int from, int size_of_copy);

  /**
   * Cuts frames in the clipboard and delete
   * the original wave file.
   * \param wave a WaveFile pointer, the wave file which must be edited.
   * \param from an int, where the copy must begin.
   * \param size_of_copy an int, the size of the copy.
   */
  void Cut (WaveFile& wave, int from, int size_of_copy);

  /**
   * Inserts frames in a wave file.
   * \param wave a WaveFile pointer, the wave file which must be edited.
   * \param to an int, where the file must be copied.
   */
  void Paste (WaveFile& wave, int to);

  /**
   * Deletes frames in a wave file.
   * \param wave a WaveFile, it's the wave file which must be edited.
   * \param from an int, where the copy must begin.
   * \param size_of_cut an int, the size of the delete part.
   */
  void Delete (WaveFile& wave, int from, int size_of_cut);

  /**
   * Keeps the offset of the copy.
   */
  long sizec;

 private:
  /**
   * spSingleton is the pointer "this" of this class.
   */
  static cClipBoard* spSingleton;


  /**
   * This is a private contructor with a filename,
   * a load flag and a file open mode.
   * \param filename a wxString, the file's path.
   * By default it's "/tmp/tmp.wav".
   * \param loadmem a bool, if the file will be mapped in the memory.
   * \param open_mode a t_opening_mode, the file's permissions.
   * By default wave files are opened in read and write mode.

   */
  cClipBoard(wxString filename = wxT("/tmp/tmp.wav"), bool loadmem = false,
	     t_opening_mode open_mode = WaveFile::rwrite);
};


#endif
