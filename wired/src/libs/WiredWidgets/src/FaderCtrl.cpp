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

#include "FaderCtrl.h"

BEGIN_EVENT_TABLE(FaderCtrl, wxWindow)
  EVT_MOTION(FaderCtrl::OnMouseEvent) 
  EVT_PAINT(FaderCtrl::OnPaint)
  EVT_LEFT_DOWN(FaderCtrl::OnMouseEvent)
  EVT_KEY_DOWN(FaderCtrl::OnKeyDown)
  EVT_ENTER_WINDOW(FaderCtrl::OnEnterWindow)
END_EVENT_TABLE()

  
FaderCtrl::FaderCtrl(wxWindow *parent, wxWindowID id, 
		       wxImage *img_bg, wxImage  *img_fg,
		       long begin_value, long end_value, long val,
		       const wxPoint &pos, const wxSize &size)
    : wxWindow(parent, id, pos, 
	       wxSize(img_fg->GetWidth(), img_bg->GetHeight())),
      Value(val), BeginValue(begin_value), EndValue(end_value)
{ 
  coeff = (end_value - begin_value) / (double)(img_bg->GetHeight() - img_fg->GetHeight());
  bg = new wxBitmap(img_bg, -1);
  tmp_fg = new wxBitmap(img_fg, -1);
  tmp_fg->SetMask(new wxMask(*tmp_fg, *wxWHITE));
  fg = new StaticBitmap(this, -1,*tmp_fg,wxPoint(0,(int)(Value  / coeff)), wxSize(img_fg->GetWidth(), img_fg->GetHeight()));
  SetValue(val);
}

FaderCtrl::~FaderCtrl()
{
  delete tmp_fg;
  delete bg;
}

void FaderCtrl::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  wxMemoryDC memDC;
  
  memDC.SelectObject(*bg);
  dc.Blit(0, 0, bg->GetWidth(), bg->GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
}

void FaderCtrl::OnMouseEvent(wxMouseEvent &event)
{
  /*
    if (!event.LeftIsDown())
    return;
    wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
    e.SetEventObject(this);
    int pos;
    if (event.GetPosition().y - (fg->GetSize().y /2) >= 0 && 
      event.GetPosition().y + (fg->GetSize().y /2) <= GetSize().y)
    pos = event.GetPosition().y;
  else if (event.GetPosition().y - (fg->GetSize().y /2) < 0)
    pos = fg->GetSize().y /2;
  else if (event.GetPosition().y + (fg->GetSize().y /2) > GetSize().y)
    pos = GetSize().y - ( fg->GetSize().y / 2 );
  
  fg->Move(wxPoint(0,pos));
  Value = 
    (EndValue + BeginValue) - 
    static_cast<long>(pos * coeff);
  GetEventHandler()->ProcessEvent(e);
  */
  
  if(event.LeftIsDown() && ((event.GetPosition().y > (fg->GetSize().y / 2))) 
     && ((event.GetPosition().y < (((bg->GetHeight()) - fg->GetSize().y /2)))))
    {
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      fg->Move(wxPoint(0,event.GetPosition().y - (fg->GetSize().y /2)));
       Value = (EndValue + BeginValue) - (long)((event.GetPosition().y -(fg->GetSize().y /2)) * coeff) ;
       //      cout << "[FADER] value: " << Value << endl;
       GetEventHandler()->ProcessEvent(e);
    }
  else if (event.LeftIsDown())
    {
      if (event.GetPosition().y >= (GetSize().y - fg->GetSize().y /2 ) )
	{
	  wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
	  e.SetEventObject(this);
	  SetValue(BeginValue);
	  GetEventHandler()->ProcessEvent(e);
	}
      else if (event.GetPosition().y < fg->GetSize().y /2)
	{
	  wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
	  e.SetEventObject(this);
	  SetValue(EndValue);
	  GetEventHandler()->ProcessEvent(e);
	}
    }
}


int FaderCtrl::GetValue()
{
  return (Value);
}

void FaderCtrl::SetValue(int val)
{
  if (val < BeginValue)
    val = BeginValue;
  if (val > EndValue)
    val = EndValue;  
  if (val == BeginValue)
    fg->Move(wxPoint(0, bg->GetHeight() - fg->GetSize().y));
  else
    fg->Move(wxPoint(0, ((EndValue - BeginValue) - val) / coeff));
  Value = val;
  // cout << "coeff = " << int(coeff) << endl;
}

void FaderCtrl::OnKeyDown(wxKeyEvent& event)
{
  if(event.GetKeyCode() == WXK_UP)
    {
      Value++;
      SetValue(Value);
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      GetEventHandler()->ProcessEvent(e);
    }
  else if(event.GetKeyCode() == WXK_DOWN)
    {
      Value--;
      SetValue(Value);
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      GetEventHandler()->ProcessEvent(e);
    }
}

void FaderCtrl::OnEnterWindow(wxMouseEvent &event)
{
  wxPostEvent(GetParent(), event);
}

