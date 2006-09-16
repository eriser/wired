// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef ENGINE_ERROR_H_
# define ENGINE_ERROR_H_


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
