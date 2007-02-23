// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WAVEENV_H__
#define __WAVEENV_H__

#include "WaveView.h"

class WaveEnv : public WaveView
{
 public:
  WaveEnv(wxWindow *parent, wxWindowID id,
	  const wxPoint& pos, const wxSize& size);
  ~WaveEnv();
 protected:
  void OnSize(wxSizeEvent &event);
  void OnPaint(wxPaintEvent &event);
  void OnClick(wxMouseEvent &event);

};

#endif//__WAVEENV_H__
