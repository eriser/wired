#ifndef __MIXER_ERROR_H__
#define __MIXER_ERROR_H__

#include <string>

namespace MixerError
{
  class MixerError {};
  class InputBuffersFull : public MixerError {};
  class Failure : public MixerError
    {
      std::string msg;
    public:
      Failure(std::string s) : msg(s) { };
      std::string getMsg() { return msg; }
    };
};

#endif//__MIXER_ERROR_H__
