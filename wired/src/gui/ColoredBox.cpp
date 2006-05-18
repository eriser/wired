// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include <wx/window.h>
#include <wx/colour.h>
#include "ColoredBox.h"

ColoredBox::ColoredBox(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,
		       		       const wxColour color, const wxColour bordercolor)
  : wxWindow(parent, id, pos, size, wxNO_BORDER)
{
  Color = color;
  BorderColor = bordercolor;
}

ColoredBox::~ColoredBox()
{

}

void					ColoredBox::SetColors(wxColour color, wxColour bordercolor)
{
  Color = color;
  BorderColor = bordercolor;
}

void					ColoredBox::OnPaint(wxPaintEvent &e)
{
  wxPaintDC				dc(this);

  dc.SetPen(wxPen(BorderColor, 1, wxSOLID)); 
  dc.SetBrush(wxBrush(Color, wxSOLID)); 
  dc.DrawRectangle(0, 0, GetSize().x, GetSize().y);
}


void					ColoredBox::OnClick(wxMouseEvent &e)
{
  wxScrollEvent				ev(wxEVT_SCROLL_TOP, GetId());
  ev.SetEventObject(this);
  GetEventHandler()->ProcessEvent(ev);
}


BEGIN_EVENT_TABLE(ColoredBox, wxWindow)
  EVT_PAINT(ColoredBox::OnPaint)
  EVT_LEFT_UP(ColoredBox::OnClick)
END_EVENT_TABLE()

