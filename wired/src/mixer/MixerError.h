// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIXER_ERROR_H__
#define __MIXER_ERROR_H__

#include <wx/string.h>

namespace MixerError
{
  class MixerError {};
  class InputBuffersFull : public MixerError {};
  class Failure : public MixerError
    {
      wxString msg;
    public:
      Failure(wxString s) : msg(s) { };
      wxString getMsg() { return msg; }
    };
};

#endif//__MIXER_ERROR_H__
