// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __WAVEEDITOR__
#define __WAVEEDITOR__

#include "WaveView.h"

class WaveEditor : public WaveView
{
 public:
  WaveEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
	   bool fulldraw = false);
  ~WaveEditor();

  void			OnPaint(wxPaintEvent &event);
  void			OnSize(wxSizeEvent &event);
  void			OnMouseEvent(wxMouseEvent& event);

 private:  
  wxRect		mSelectedRegion;
  bool			mIsSelecting;
 
  DECLARE_EVENT_TABLE()
};

#endif
