// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef ENGINE_ERROR_H_
# define ENGINE_ERROR_H_

#include <string>

namespace Error
{
  class AudioEngineError {};
  class NoDevice : public AudioEngineError {};
  class InvalidDeviceSettings : public AudioEngineError {};
  class ChannelsNotSet : public AudioEngineError {};
  class StreamNotOpen : public AudioEngineError {};
  class InitFailure : public AudioEngineError 
    {
      wxString msg;
    public:
      InitFailure(wxString s) : msg(s) { };
      wxString getMsg() { return msg; }
    };
};

#endif /* ENGINE_ERROR_H_ */
