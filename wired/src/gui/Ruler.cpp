// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include "SequencerGui.h"
#include "Sequencer.h"
#include "Colour.h"
#include "Cursor.h"
#include "Ruler.h"
#include "ColoredLine.h"

Ruler::Ruler(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, size, wxRAISED_BORDER) 
{
//   T1 = new ColoredLine(this, -1, wxPoint(42, RULER_HEIGHT - RULER_XMARK_HEIGHT), wxSize(1, RULER_XMARK_HEIGHT    ), CL_RULER_XMARK);
//   T2 = new ColoredLine(this, -1, wxPoint(43, RULER_HEIGHT - RULER_XMARK_HEIGHT), wxSize(1, RULER_XMARK_HEIGHT - 1), CL_RULER_XMARK);
//   T3 = new ColoredLine(this, -1, wxPoint(44, RULER_HEIGHT - RULER_XMARK_HEIGHT), wxSize(1, RULER_XMARK_HEIGHT    ), CL_RULER_XMARK);
/*  T1 = new ColoredLine(this, -1, wxPoint(42, 0), wxSize(8, RULER_XMARK_HEIGHT    ), CL_RULER_XMARK);
  T2 = new ColoredLine(this, -1, wxPoint(43, 0), wxSize(16, RULER_XMARK_HEIGHT - 1), CL_RULER_XMARK);
  T3 = new ColoredLine(this, -1, wxPoint(44, 0), wxSize(24, RULER_XMARK_HEIGHT    ), CL_RULER_XMARK);
  T1->Show(true);
  T2->Show(true);
  T3->Show(true);*/
}

Ruler::~Ruler()
{
  /*  delete (T1);
  delete (T2);
  delete (T3);*/
}

/*
** Methode de dessin de la regle qui tient compte du scrolling
** et du facteur de zoom.
*/

void				Ruler::OnPaint(wxPaintEvent &event)
{
  wxPaintDC			dc(this);
  wxSize			size;
  wxString			s;
  double			x;
  double			u;
  long				m;
  long				h;

  PrepareDC(dc);
  size = GetClientSize();
  dc.SetPen(wxPen(CL_RULER_BACKGROUND, 1, wxSOLID));
  dc.SetBrush(wxBrush(CL_RULER_BACKGROUND));
  dc.SetTextForeground(CL_RULER_PATTERNNUM);
  dc.SetFont(wxFont(RULER_NUMBER_HEIGHT, wxDEFAULT, wxNORMAL, wxNORMAL));
  dc.DrawRectangle(0, 0, size.x, size.y);
  dc.SetPen(wxPen(CL_RULER_FOREGROUND, 1, wxSOLID));
  u = MEASURE_WIDTH * SeqPanel->HoriZoomFactor / Seq->SigNumerator;
  m = (int) ceil(XScroll / u);
  h = GetClientSize().y;
  for (x = u * m - XScroll; (long) floor(x) < size.x; x += u)
    {
      if (!(m % Seq->SigNumerator))
	{
	  s.Printf("%d", 1 + (long) (m / Seq->SigNumerator));
	  dc.DrawText(s, (int) floor(x) + RULER_NUMBER_XOFFSET, RULER_NUMBER_YOFFSET);
	}
      dc.DrawLine((int) floor(x), (m++ % Seq->SigNumerator) ? h - RULER_MEASURE_DIV_HEIGHT : 0,
		  (int) floor(x), h);
    }
}

void				Ruler::OnMouseEvent(wxMouseEvent &e)
{
  SeqPanel->SetCurrentPos(((double) (XScroll + e.m_x)) / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor));
  Seq->SetCurrentPos(SeqPanel->PlayCursor->GetPos());
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  wxPostEvent(GetParent(), event);
}

void				Ruler::SetXScroll(long x, long range, long seqwidth)
{
  
  if (!range || ((XScroll = (long) floor(((Seq->EndPos * MEASURE_WIDTH * SeqPanel->HoriZoomFactor
				- seqwidth) * x) / range)) < 0))
    XScroll = 0;
}

void				Ruler::SetXScrollValue(long X)
{
  if (X >= 0)
    XScroll = X;
}


long				Ruler::GetXScroll()
{
  return (XScroll);
}

int				Ruler::GetXPos(double pos)
{
  return ((int) (MEASURE_WIDTH * SeqPanel->HoriZoomFactor * pos));
}

void				Ruler::MoveXMark(long x)
{
  //printf("youpi ? %d\n", x);
  T1->SetSize((int)     x, (int)     RULER_HEIGHT - RULER_XMARK_HEIGHT, (int) -1, -1, wxSIZE_USE_EXISTING);
  T2->SetSize((int) 1 + x, (int) 1 + RULER_HEIGHT - RULER_XMARK_HEIGHT, (int) -1, -1, wxSIZE_USE_EXISTING);
  T3->SetSize((int) 2 + x, (int)     RULER_HEIGHT - RULER_XMARK_HEIGHT, (int) -1, -1, wxSIZE_USE_EXISTING);
  T1->Show(true);
  T2->Show(true);
  T3->Show(true);
}

BEGIN_EVENT_TABLE(Ruler, wxWindow)
  EVT_LEFT_DOWN(Ruler::OnMouseEvent) 
  EVT_PAINT(Ruler::OnPaint)
END_EVENT_TABLE()
