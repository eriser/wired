// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include "Colour.h"
#include "RulerMidi.h"
#include "Clavier.h"
#include "MidiPart.h"

RulerMidi::RulerMidi(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		     const wxSize &size, EditMidi *editmidi) :
  wxPanel(parent, id, pos, size, wxTHICK_FRAME)
{
  em = editmidi;
  ZoomX = 1;
}

void				RulerMidi::OnPaint(wxPaintEvent &event)
{
  wxPaintDC			dc(this);
  wxSize			size;
  wxString			s;
  double			x;
  long				m;

  PrepareDC(dc);
  size = GetSize();
  dc.SetPen(wxPen(CL_RULER_BACKGROUND, 1, wxSOLID));
  dc.SetBrush(wxBrush(CL_RULER_BACKGROUND));
  dc.SetTextForeground(CL_RULER_PATTERNNUM);
  dc.DrawRectangle(0, 0, size.x, size.y);
  dc.SetPen(wxPen(CL_RULER_FOREGROUND, 1, wxSOLID));
  m = 0;
  for (x = 0; (long) floor(x) < size.x; x += ROW_WIDTH * ZoomX)
  { 
    if (!(m % 4))
    {
      s.Printf(wxT("%lu"), (long) (m / 4));
      dc.DrawText(s, (int) floor(x) + 2, 0);
    }
    dc.DrawLine((int) floor(x), (m++ % 4) ? MIDI_RULER_HEIGHT - 8 : 0,
	        (int) floor(x), MIDI_RULER_HEIGHT);
  }
}

void				RulerMidi::SetZoomX(double pZoomX)
{
  ZoomX = pZoomX;
}

BEGIN_EVENT_TABLE(RulerMidi, wxWindow)
  EVT_PAINT(RulerMidi::OnPaint)
END_EVENT_TABLE()
