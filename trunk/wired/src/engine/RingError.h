// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __RING_ERROR_H__
#define __RING_ERROR_H__

#include <wx/string.h>
namespace RingBufferError
{
  class RingBufferError {};
  class NumBytesError : public RingBufferError {};
  class Failure : public RingBufferError 
    {
      wxString msg;
    public:
      Failure(wxString s) : msg(s) { };
      wxString getMsg() { return msg; }
    };
};
#endif//__RING_ERROR_H__

