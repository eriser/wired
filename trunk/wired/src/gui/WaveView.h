// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __WAVEVIEW__
#define __WAVEVIEW__

#include <wx/wx.h>
#include "../audio/WaveFile.h"
#include "WaveDrawer.h"

class WaveView : public wxWindow, public WaveDrawer
{
 protected:
  virtual void			SetSize(wxSize s);

 public:
  WaveView(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
	   bool fulldraw = false, bool use_settings = true);
  virtual ~WaveView();

//  void			SetDrawing();
//  void			SetWave(float **data, unsigned long frame_length, long channel_count);
//  void			SetWave(WaveFile *w);
  virtual void			OnPaint(wxPaintEvent &event);
  virtual void			OnSize(wxSizeEvent &event);
  virtual void			SetWave(float **data, unsigned long frame_length, long channel_count);
  virtual void			SetWave(WaveFile *w);
  void				SetDrawing();

  DECLARE_EVENT_TABLE()
};

#endif
