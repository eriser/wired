// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __RULERMIDI_H__
#define __RULERMIDI_H__

#include <wx/wx.h>
#include "EditMidi.h"

#define MIDI_RULER_HEIGHT	20

class RulerMidi : public wxPanel
{
  virtual void	OnPaint(wxPaintEvent &event);

 public:
  RulerMidi(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	const wxSize &size, class EditMidi *editmidi);
  void SetZoomX(double pZoomX);
  EditMidi	*em;
  double ZoomX;
  DECLARE_EVENT_TABLE()
};

#endif/* __RULERMIDI_H__*/

