// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <vector>
#include <iostream>
#include "AudioEngine.h"
#include "WiredDocument.h"

#define  NUM_BUFFERS  1

/**
 * The Channel class manages mono/stereo buffers,
 * their associated volumes and allow some controls
 * on them.
*/

class Channel : public WiredDocument
{
 public:

/**
 * Ctor setting volumes for each channel
*/
  Channel(bool stereo, bool visible = true, WiredDocument* docParent = NULL);

/**
 * Copy Ctor
*/

//  Channel(const Channel& copy) {*this = copy;}
//  Channel	operator=(const Channel& right);

/**
 * DCtor cleaning all channels
*/


/**
 * DCtor cleaning all channels
*/

  ~Channel();

/**
 * Dump all values
 * \return void
*/
  void		Dump();

/**
 * True for Stereo, false for mono
*/

  bool		Stereo;

/**
 * Float volume for the left channel
*/

  float		VolumeLeft;

/**
 * Float volume for the right channel
*/

  float		VolumeRight;

/**
 * Variable is set but currently unused
*/

  long		InputNum;

/**
 * Stereo buffer filling
 * \param input a float*
 * \param bytes a long
 * \return void
*/

  void		PushBuffer(float* input, long bytes);

/**
 * Mono Buffer filling
 * \param a float **
 * \return void
*/

  void		PushBuffer(float **);

/**
 * Mono buffer filling
 * \param a float *
 * \return void
*/

  void		PushBuffer(float *);

/**
 * Allocates memory for a given numbers of buffers
 * \param unsigned int
 * \return void
*/

  void		AddBuffers(unsigned int);

/**
 * Returns a buffer at the given position int i
 * \param i an int
 * \return a buffer as a float*
*/

  float		*PopBuffer(int i);

/**
 * Removes the first buffer of the vector
 * \return void
*/

  void		RemoveFirstBuffer();

/**
 * Delete all values in the buffer's vector
 * \param renew a bool, if true, call AddBuffers(NUM_BUFFERS)
 * after the full vector's buffers removal (default is true)
 * \return void
*/

  void		ClearAllBuffers(bool renew = true);

/**
 * Variable is set but currently unused
*/

  wxString	Label;

/**
 * If Visible is true, buffers will be fully loaded
 * else will be only allocated
*/

  bool		Visible;

/**
 * float for left RMS
*/

  float		Lrms;

/**
 * float for right RMS
*/

  float		Rrms;

/**
 * Where mono buffers are stored
*/

  std::vector<float*> MonoBuffers;

/**
 * Where stereo buffers are stored
*/

  std::vector<float**> StereoBuffers;

/**
 * Represent a position in the MonoBuffers vector
*/

  int		CurBuf;

/**
 * Variable is set but currently unused
*/

  bool		Filled;

/**
 * Mute setter for left channel
*/

  inline void	SetMuteLeft(bool muted)
    { MuteLeft = muted; };

/**
 * Mute setter for right channel
*/

  inline void	SetMuteRight(bool muted)
  { MuteRight = muted; };

  /**
   * Mute getter for left channel
   */

  inline bool	GetMuteLeft()
  { return MuteLeft; };

  /**
   * Mute getter for right channel
   */

  inline bool	GetMuteRight()
  { return MuteRight; };


  /**
   * WiredDocument implementation
   */
  void		Save();

  /**
   * WiredDocument implementation
   */
  void		Load(SaveElementArray data);


  /**
   * WiredDocument implementation
   */
  void		CleanChildren();



 private:

/**
 * True if the left channel is muted, else false
 *
*/

  bool		MuteLeft;

/**
 * True if the right channel is muted, else false
 *
*/

  bool		MuteRight;
};

#endif //__CHANNEL_H__


