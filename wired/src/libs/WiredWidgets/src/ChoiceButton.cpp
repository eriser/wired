/*
** Copyright (C) 2004 by Wired Team
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

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




