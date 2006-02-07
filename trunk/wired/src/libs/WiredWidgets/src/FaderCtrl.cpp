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
  EVT_LEFT_DOWN(FaderCtrl::OnLeftDown)
  EVT_LEFT_UP(FaderCtrl::OnLeftUp)
  EVT_RIGHT_DOWN(FaderCtrl::OnMouseEvent)
  EVT_KEY_DOWN(FaderCtrl::OnKeyDown)
  EVT_KEY_UP(FaderCtrl::OnKeyUp)
  EVT_ENTER_WINDOW(FaderCtrl::OnEnterWindow)
  EVT_LEAVE_WINDOW(FaderCtrl::OnLeaveWindow)
END_EVENT_TABLE()

  
FaderCtrl::FaderCtrl(wxWindow *parent, wxWindowID id, 
		     wxImage *img_bg, wxImage  *img_fg,
		     float begin_value, float end_value, float *val, bool is_int,
		     const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, 
	     wxSize(img_fg->GetWidth(), img_bg->GetHeight())),
    Value(val), BeginValue(begin_value), EndValue(end_value)
{ 
  //Coeff = (end_value - begin_value) / (double)(img_bg->GetHeight() - img_fg->GetHeight());
  Coeff = (begin_value - end_value) / (double)(img_bg->GetHeight() - img_fg->GetHeight());
  bg = new wxBitmap(img_bg, -1);
  tmp_fg = new wxBitmap(img_fg, -1);
  tmp_fg->SetMask(new wxMask(*tmp_fg, *wxWHITE));
  fg = new StaticBitmap(this, -1,*tmp_fg,wxPoint(0,(int)(*Value / Coeff)), wxSize(img_fg->GetWidth(), img_fg->GetHeight()));
  SetValue(val);
  IsInteger = is_int;
  Label = 0x0;
}

FaderCtrl::FaderCtrl(wxWindow *parent, wxWindowID id, 
		     wxImage *img_bg, wxImage  *img_fg,
		     float begin_value, float end_value, float *val, bool is_int,
		     const wxPoint &pos, const wxSize &size,
		     wxWindow* hintparent, const wxPoint &hintpos)
  : wxWindow(parent, id, pos, wxSize(img_fg->GetWidth(), img_bg->GetHeight())),
    Value(val), BeginValue(begin_value), EndValue(end_value)
{ 
  //Coeff = (end_value - begin_value) / (double)(img_bg->GetHeight() - img_fg->GetHeight());
  Coeff = (float) (img_bg->GetHeight() - img_fg->GetHeight()) / (float) (begin_value - end_value);
  Ord = -Coeff * end_value;
  bg = new wxBitmap(img_bg, -1);
  tmp_fg = new wxBitmap(img_fg, -1);
  tmp_fg->SetMask(new wxMask(*tmp_fg, *wxWHITE));
  fg = new StaticBitmap(this, -1, *tmp_fg, wxPoint(0, (int)(Coeff * *Value + Ord)), 
			wxSize(img_fg->GetWidth(), img_fg->GetHeight()));
  IsInteger = is_int;
  SetValue(val);
  wxString s;
  if (IsInteger)
    s.Printf("%d", (int)GetValue());
  else
    s.Printf("%.2f", GetValue());
  Label = new Hint(hintparent, -1, s,
		   wxPoint(GetPosition().x, GetPosition().y + GetSize().y), 
		   wxDefaultSize, *wxWHITE, *wxBLACK);
  Label->Show(false);
  //cout << "img->GetHeight = " << img_bg->GetHeight() << endl;
  //cout << "begin_value - end_value = " << begin_value - end_value << endl;
  cout << "Coeff = " << Coeff << "\t\t Ord = " << Ord << "\t\t" << begin_value << " -> " << end_value << endl;
//   for (int i = begin_value; i <= end_value; i += (begin_value > end_value) ? -1 : 1)
//     cout << "f(" << i << ") = " << Coeff * i + Ord << endl;
}


FaderCtrl::~FaderCtrl()
{
  if (tmp_fg)
    delete tmp_fg;
  if (bg)
    delete bg;
  if (fg)
    delete fg;
  if (Label)
    delete Label;
}

void		FaderCtrl::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  wxMemoryDC memDC;
  
  memDC.SelectObject(*bg);
  dc.Blit(0, 0, bg->GetWidth(), bg->GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
}

void		FaderCtrl::OnLeftDown(wxMouseEvent& event)
{
  if (Label)
    {
      Label->Show(true);
      wxString s;
      if (IsInteger)
	s.Printf("%d", (int)GetValue());
      else
	s.Printf("%.2f", GetValue());
      Label->SetLabel(s);
    }
  OnMouseEvent(event);
}

void		FaderCtrl::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
  if (Label)
    Label->Show(false);
}

void		FaderCtrl::OnMouseEvent(wxMouseEvent &event)
{
  if(event.LeftIsDown() && ((event.GetPosition().y > (fg->GetSize().y / 2))) 
     && ((event.GetPosition().y < (((bg->GetHeight()) - fg->GetSize().y /2)))))
    {
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      //fg->Move(wxPoint(0,(int)(event.GetPosition().y - (fg->GetSize().y /2))));
      //SetValue((EndValue + BeginValue) - (long)((event.GetPosition().y - (fg->GetSize().y / 2)) * Coeff));
      *Value = (float) ((event.GetPosition().y - (fg->GetSize().y / 2) - Ord) / Coeff);
      cout << "Value = " << *Value << endl;
      cout << "cursor pos = " << event.GetPosition().y << endl;
      SetValue(Value);
      GetEventHandler()->ProcessEvent(e);
    }
  else if (event.LeftIsDown())
    {
      if (event.GetPosition().y >= (GetSize().y - fg->GetSize().y /2 ) )
	{
	  wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
	  e.SetEventObject(this);
	  SetValue(&BeginValue);
	  GetEventHandler()->ProcessEvent(e);
	}
      else if (event.GetPosition().y < fg->GetSize().y /2)
	{
	  wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
	  e.SetEventObject(this);
	  SetValue(&EndValue);
	  GetEventHandler()->ProcessEvent(e);
	}
    }
  else if (event.RightDown())
    wxPostEvent(GetParent(), event);
  if (Label)
    {
      wxString s;
      if (IsInteger)
	s.Printf("%d", (int)GetValue());
      else
	s.Printf("%.2f", GetValue());
      Label->SetLabel(s);
      Label->Show((event.LeftIsDown()));
    }

  cout << BeginValue << " < "<< "Value : " << *Value << " < " << EndValue << endl;
}

float		FaderCtrl::GetValue()
{
  return (*Value);
}

void		FaderCtrl::SetValue(float *val)
{
  if (!val)
    return ;
  if (*val <= BeginValue)
    {
      *Value = BeginValue;
      fg->Move(wxPoint(0, bg->GetHeight() - fg->GetSize().y));
    }
  else if (*val >= EndValue)
    {
      *Value = EndValue;
      fg->Move(wxPoint(0, 0));
    }
  else
    {
      *Value = *val;
      fg->Move(wxPoint(0, (int) (Coeff * *Value + Ord)));
    }
  cout << "val = " << (float) *Value << endl;
  cout << "moveto : " << (int) (Coeff * *Value + Ord) << endl;
}

void		FaderCtrl::SetValue(float val)
{
  SetValue(&val);
}

// void		FaderCtrl::SetValue(int val)
// {
//   SetValue((float)val);
// }

void		FaderCtrl::OnKeyUp(wxKeyEvent& event)
{
  if (Label)
    {
      Label->Show(false);
    }
}

void		FaderCtrl::OnKeyDown(wxKeyEvent& event)
{
  if(event.GetKeyCode() == WXK_UP)
    {
      //(*Value)++;
      cout << "Value = " << *Value << endl;
      SetValue(*Value + 1);
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      GetEventHandler()->ProcessEvent(e);
    }
  else if(event.GetKeyCode() == WXK_DOWN)
    {
      //      (*Value)--;
      cout << "Value = " << *Value << endl;
      SetValue(*Value - 1);
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      GetEventHandler()->ProcessEvent(e);
    }
  if (Label)
    {
      Label->Show(true);
      wxString s;
      if (IsInteger)
	s.Printf("%d", (int)GetValue());
      else
	s.Printf("%.2f", GetValue());
      Label->SetLabel(s);
    }
}

void		FaderCtrl::OnEnterWindow(wxMouseEvent &event)
{
  wxPostEvent(GetParent(), event);
}

void		FaderCtrl::OnLeaveWindow(wxMouseEvent &event)
{
  if (Label)
    Label->Show(false);
  wxPostEvent(GetParent(), event);
}
