// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
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
  wxWindow::SetBackgroundStyle(wxBG_STYLE_CUSTOM);
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
  dc.DrawLine(0, 0, size.x - 1, size.y - 1);
  dc.DrawPoint(size.x - 1, size.y - 1);
}

void			ColoredLine::SetColor(wxColour c)
{
  c0loR = c;
  Refresh();
}

void			ColoredLine::SetVSize(unsigned long h)
{
  SetSize(-1, -1, -1, (int) h, wxSIZE_USE_EXISTING);
  Refresh();
}

void			ColoredLine::SetHSize(unsigned long w)
{
  SetSize(-1, -1, (int) w, -1, wxSIZE_USE_EXISTING);
  Refresh();
}

void			ColoredLine::ReSize(unsigned long w, unsigned long h)
{
  SetSize(-1, -1, (int) w, (int) h, wxSIZE_USE_EXISTING);
  Refresh();
}

