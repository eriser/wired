// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __RING_H__
#define __RING_H__

#include <iostream>
using namespace std;
#include <unistd.h>

template<class C> class RingBuffer
{
public:
  RingBuffer(long num);
  ~RingBuffer();
  
  //bool	Init();
  void Flush();
  
  long GetWriteAvailable();
  long GetReadAvailable();
  
  long Write( C *data, long numBytes );
  long Read( C *data, long numBytes );
  
  long AdvanceWriteIndex( long numBytes );
  long AdvanceReadIndex( long numBytes );
  
  long GetReadRegions(  long numBytes,
			C **dataPtr1, long *sizePtr1,
			C **dataPtr2, long *sizePtr2 );
  long GetWriteRegions( long numBytes,
			C **dataPtr1, long *sizePtr1,
			C **dataPtr2, long *sizePtr2 );
  
  long  bufferSize; 
  /* Number of bytes in FIFO. Power of 2. Set by RingBuffer_Init. */
  long  writeIndex;
  /* Index of next writable byte. Set by RingBuffer_AdvanceWriteIndex. */
  long  readIndex; 
  /* Index of next readable byte. Set by RingBuffer_AdvanceReadIndex. */
  long	bigMask;
  /* Used for wrapping indices with extra bit to distinguish full/empty. */
  long	smallMask;  
  /* Used for fitting indices to buffer. */
  C	*buffer;
};
#include "RingBuffer.hxx"
#endif//__RING_H__
