// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <wx/window.h>
#include "Colour.h"
#include "ColoredLine.h"

BEGIN_EVENT_TABLE(ColoredLine, wxWindow)
  /*  EVT_LEFT_DOWN(ColoredLine::OnMouseEvent)*/
  EVT_PAINT(ColoredLine::OnPaint)
END_EVENT_TABLE()


ColoredLine::ColoredLine(wxWindow *parent, wxWindowID id, const wxPoint &pos,
			 const wxSize &size, const wxColour c)
  : wxWindow(parent, id, pos, size, wxNO_BORDER)
{
  SetColor(c);
}

ColoredLine::~ColoredLine()
{
  
}

void			ColoredLine::OnPaint(wxPaintEvent &event)
{
  wxPaintDC		dc(this);
  wxSize		size;

  PrepareDC(dc);
  size = GetSize();
  dc.SetPen(wxPen(c0loR, 1, wxSOLID));
  dc.DrawLine(0, 0, size.x - 1, size.y);
}

void			ColoredLine::SetColor(wxColour c)
{
  c0loR = c;
  Refresh();
}

void			ColoredLine::ReSize(unsigned long h)
{
  SetSize(-1, (int) h);
  Refresh();
}
