#ifndef __RING_ERROR_H__
#define __RING_ERROR_H__

#include <string>
namespace RingBufferError
{
  class RingBufferError {};
  class NumBytesError : public RingBufferError {};
  class Failure : public RingBufferError 
    {
      std::string msg;
    public:
      Failure(std::string s) : msg(s) { };
      std::string getMsg() { return msg; }
    };
};
#endif//__RING_ERROR_H__

