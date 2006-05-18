// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#include "ChoiceButton.h"

BEGIN_EVENT_TABLE(ChoiceButton, wxWindow)
  EVT_LEFT_DOWN(ChoiceButton::OnMouseEvent) 
  EVT_PAINT(ChoiceButton::OnPaint)
END_EVENT_TABLE()

ChoiceButton::ChoiceButton(wxWindow *parent, wxWindowID id, const wxPoint &pos,
			   const wxSize &size, wxString filename)
  : wxWindow(parent, id, pos, size)
{
  img = 0x0;
}

ChoiceButton::~ChoiceButton()
{

}

void ChoiceButton::SetImage(wxBitmap *bmp)
{
  img = bmp;
}

void ChoiceButton::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  wxBitmap  *tmp;
  wxMemoryDC memDC;
  wxSize s;

  s = GetSize();
  dc.SetPen(wxPen("BLACK", 1, wxSOLID)); 
  dc.DrawRectangle(0, 0, s.x, s.y);

  if (img)
    {
      tmp = img;
      memDC.SelectObject(*tmp);
      dc.Blit(0, 0, tmp->GetWidth(), tmp->GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
    }
}

void ChoiceButton::OnMouseEvent(wxMouseEvent &event)
{
  wxCommandEvent _event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  wxPostEvent(GetParent(), _event);
}




