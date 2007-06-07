// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "RingError.h"
#ifndef WIN32
#include <unistd.h>
#endif

template<class C> inline RingBuffer<C>::RingBuffer( long num ) 
{
  bufferSize = num;
  buffer = 0x0;
  if( ((bufferSize-1) & bufferSize) != 0 ) 
    throw RingBufferError::NumBytesError();

  try 
    {
      buffer = new C[bufferSize];
    }
  catch (...)
    {
      throw;
    }
  
  Flush();
  bigMask = (bufferSize*2)-1;
  smallMask = (bufferSize)-1;
}

template<class C> inline RingBuffer<C>::~RingBuffer() 
{
  if (buffer)
    delete [] buffer;
}

/***************************************************************************
** Return number of bytes available for reading. */
template<class C> long RingBuffer<C>::GetReadAvailable( void )
{
  return ( (writeIndex - readIndex) & bigMask );
}
/***************************************************************************
** Return number of bytes available for writing. */
template<class C> long RingBuffer<C>::GetWriteAvailable( void ) 
{
  return ( bufferSize - GetReadAvailable());
}

/***************************************************************************
** Clear buffer. Should only be called when buffer is NOT being read. */
template<class C> void RingBuffer<C>::Flush( void )
{
  writeIndex = readIndex = 0;
}

/***************************************************************************
** Get address of region(s) to which we can write data.
** If the region is contiguous, size2 will be zero.
** If non-contiguous, size2 will be the size of second region.
** Returns room available to be written or numBytes, whichever is smaller.
*/
template<class C> long RingBuffer<C>::GetWriteRegions(long numBytes,
                                 C **dataPtr1, long *sizePtr1,
                                 C **dataPtr2, long *sizePtr2 )
{
  long   index;
  long   available = GetWriteAvailable();
  if( numBytes > available ) numBytes = available;
  
  /* Check to see if write is not contiguous. */
  index = writeIndex & smallMask;
  if( (index + numBytes) > bufferSize )
    {
      /* Write data in two blocks that wrap the buffer. */
      long   firstHalf = bufferSize - index;
      *dataPtr1 = &buffer[index];
      *sizePtr1 = firstHalf;
      *dataPtr2 = &buffer[0];
      *sizePtr2 = numBytes - firstHalf;
    }
  else
    {
      *dataPtr1 = &buffer[index];
      *sizePtr1 = numBytes;
      *dataPtr2 = NULL;
      *sizePtr2 = 0;
    }
  return numBytes;
}


/***************************************************************************
*/
template<class C> long RingBuffer<C>::AdvanceWriteIndex( long numBytes )
{
  //writeIndex = (writeIndex + numBytes) & bigMask;
  //  cout << "[RINGBUFFER WRITE] Write Index is: "<< writeIndex << endl;
  //cout << "[RINGBUFFER WRITE] Read Index is: "<< readIndex << endl;
  //return writeIndex;
  return writeIndex = (writeIndex + numBytes) & bigMask;
}

/***************************************************************************
** Get address of region(s) from which we can read data.
** If the region is contiguous, size2 will be zero.
** If non-contiguous, size2 will be the size of second region.
** Returns room available to be written or numBytes, whichever is smaller.
*/

template<class C> long RingBuffer<C>::GetReadRegions(long numBytes,
                                C **dataPtr1, long *sizePtr1,
                                C **dataPtr2, long *sizePtr2 )
{
  long   index;
  long   available = GetReadAvailable();
  
  if( numBytes > available ) numBytes = available;
  /* Check to see if read is not contiguous. */
  index = readIndex & smallMask;
  if( (index + numBytes) > bufferSize )
    {
      /* Write data in two blocks that wrap the buffer. */
      long firstHalf = bufferSize - index;
      *dataPtr1 = &buffer[index];
      *sizePtr1 = firstHalf;
      *dataPtr2 = &buffer[0];
      *sizePtr2 = numBytes - firstHalf;
    }
  else
    {
      *dataPtr1 = &buffer[index];
      *sizePtr1 = numBytes;
      *dataPtr2 = NULL;
      *sizePtr2 = 0;
    }
  return numBytes;
}
/***************************************************************************
 */
template<class C> long RingBuffer<C>::AdvanceReadIndex( long numBytes )
{
  return readIndex = (readIndex + numBytes) & bigMask;
}

/***************************************************************************
** Return bytes written. */
template<class C> long RingBuffer<C>::Write( C *data, long numBytes )
{
    long size1, size2, numWritten;
    C *data1, *data2;
    
    numWritten = GetWriteRegions( numBytes, &data1, &size1, &data2, &size2 );
    memcpy( data1, data, size1 * sizeof (C) );
    if( size2 > 0 )
      {
        data = ((C *)data) + size1;
        memcpy( data2, data, size2 * sizeof (C));
      }
    AdvanceWriteIndex(numWritten);
    return numWritten;
}

/***************************************************************************
** Return bytes read. */
template<class C> long RingBuffer<C>::Read( C *data, long numBytes )
{
    long size1, size2, numRead;
    C *data1, *data2;
    
    numRead = GetReadRegions( numBytes, &data1, &size1, &data2, &size2 );
    memcpy( data, data1, size1 * sizeof (C));
    if( size2 > 0 )
      {
        data = ((C *)data) + size1;
        memcpy( data, data2, size2 * sizeof (C));
      }
    AdvanceReadIndex( numRead );
    return numRead;
}
