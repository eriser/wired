// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include "Colour.h"
#include "RulerLoop.h"

BEGIN_EVENT_TABLE(RulerLoop, wxWindow)
  EVT_PAINT(RulerLoop::OnPaint)
END_EVENT_TABLE()
  
RulerLoop::RulerLoop(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		     const wxSize &size)
  : wxWindow(parent, id, pos, size, wxNO_BORDER)
{
  MesCount = 4;
  Beats = 4;
}

RulerLoop::~RulerLoop()
{

}

void RulerLoop::SetBeats(int bars, int beats)
{
  MesCount = beats;
  Beats = bars;
  Refresh();
}

void		RulerLoop::OnPaint(wxPaintEvent &event)
{
  wxPaintDC	dc(this);
  wxSize	size;
  wxString	s;
  double	x;
  double	u;
  int		m;

  PrepareDC(dc);
  size = GetSize();
  dc.SetPen(wxPen(CL_RULER_BACKGROUND, 1, wxSOLID));
  dc.SetBrush(wxBrush(CL_RULER_BACKGROUND));
  dc.SetTextForeground(CL_RULER_PATTERNNUM);
  dc.DrawRectangle(0, 0, size.x, size.y);
  dc.SetPen(wxPen(CL_RULER_FOREGROUND, 1, wxSOLID));
  dc.SetFont(wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL));
  u = size.x /* * HoriZoomFactor */ / Beats /  MesCount;
  m = 0;
  for (x = 0; (long) floor(x) < size.x; x += u)
    {
      if (!(m % Beats))
	{
	  s.Printf("%d", (long) (m / Beats));
	  dc.DrawText(s, (int) floor(x) + 3, -2);
	}
      dc.DrawLine((int) floor(x), (m++ % Beats) ? RULER_HEIGHT - 4 : 0,
		  (int) floor(x), RULER_HEIGHT);
    }
}
