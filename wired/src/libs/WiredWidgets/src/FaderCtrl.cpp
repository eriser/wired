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
  Label = 0x0;
}

FaderCtrl::FaderCtrl(wxWindow *parent, wxWindowID id, 
		     wxImage *img_bg, wxImage  *img_fg,
		     long begin_value, long end_value, long val,
		     const wxPoint &pos, const wxSize &size,
		     wxWindow* hintparent, const wxPoint &hintpos)
  : wxWindow(parent, id, pos, 
	     wxSize(img_fg->GetWidth(), img_bg->GetHeight())),
    Value(val), BeginValue(begin_value), EndValue(end_value)
{ 
  coeff = (end_value - begin_value) / (double)(img_bg->GetHeight() - img_fg->GetHeight());
  bg = new wxBitmap(img_bg, -1);
  tmp_fg = new wxBitmap(img_fg, -1);
  tmp_fg->SetMask(new wxMask(*tmp_fg, *wxWHITE));
  fg = new StaticBitmap(this, -1,*tmp_fg,wxPoint(0,(int)(Value  / coeff)), wxSize(img_fg->GetWidth(), img_fg->GetHeight()));
  cout << "Setvalue : " << val << endl;
  SetValue(val);
  
  wxString s;
  s.Printf("%f", val);
  Label = new Hint(hintparent, -1, s, 
		   wxPoint( hintpos.x + GetSize().x, hintpos.y + GetSize().y ),
		   wxDefaultSize, *wxWHITE, *wxBLACK);
  Label->Show(false);
  
}

FaderCtrl::~FaderCtrl()
{
//	wxMask*Mask = tmp_fg->GetMask();
//	if  (Mask)
//		delete Mask;
	if (tmp_fg)
	  delete tmp_fg;
	if (bg)
	  delete bg;
	if (Label)
	delete Label;
}

void FaderCtrl::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  wxMemoryDC memDC;
  
  memDC.SelectObject(*bg);
  dc.Blit(0, 0, bg->GetWidth(), bg->GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
}

void FaderCtrl::OnLeftDown(wxMouseEvent& event)
{
  if (Label)
    {
      Label->Show(true);
      wxString s;
      s.Printf("%d", GetValue());
      Label->SetLabel(s);
    }
  OnMouseEvent(event);
}

void FaderCtrl::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
  if (Label)
    {
      Label->Show(false);
      /*wxString s;
      s.Printf("%d", GetValue());
      Label->SetLabel(s);*/
    }
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
      fg->Move(wxPoint(0,(int)(event.GetPosition().y - (fg->GetSize().y /2))));
       Value = (EndValue + BeginValue) - (long)((event.GetPosition().y -(fg->GetSize().y /2)) * coeff) ;
       //      cout << "[FADER] value: " << Value << endl;
       GetEventHandler()->ProcessEvent(e);
    }
  else if (event.LeftIsDown())
    {
      if (event.GetPosition().y >= (GetSize().y - fg->GetSize().y /2 ) )
	{
	  //  cout << "event get pos = " << event.GetPosition().y << "GetSize().y " << GetSize().y << "fg->GetSize().y " << fg->GetSize().y<< endl;
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
  else if (event.RightDown())
    wxPostEvent(GetParent(), event);
  if (Label)
    {
      wxString s;
      s.Printf("%d", GetValue());
      Label->SetLabel(s);
    }
}


int FaderCtrl::GetValue()
{
  return (Value);
}

void		FaderCtrl::SetValue(int val)
{
  int		range = EndValue - BeginValue;

  if (val <= BeginValue)
    {
      val = BeginValue;
      fg->Move(wxPoint(0, bg->GetHeight() - fg->GetSize().y));
    }
  else if (val >= EndValue)
    {
      val = EndValue;
      fg->Move(wxPoint(0, 0));
    }
  else
    //fg->Move(wxPoint(0, (int)(((EndValue /*- BeginValue*/) - val) / coeff))); // bug
    //fg->Move(wxPoint(0, (range - val + BeginValue) / coeff));
    fg->Move(wxPoint(0, (EndValue - val) / coeff));
  Value = val;
  
  cout << "[] Value = " << Value << endl; 
  //cout << "y pos = " << range - val + BeginValue << endl;
  //cout << "moveto = " << (int)((EndValue /*- BeginValue*/) - val) / coeff << endl;
  //cout << "coeff = " << coeff << endl;
}


void FaderCtrl::OnKeyUp(wxKeyEvent& event)
{
  if (Label)
    {
      Label->Show(false);
    }
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
  if (Label)
    {
      Label->Show(true);
      wxString s;
      s.Printf("%d", GetValue());
      Label->SetLabel(s);
    }
}

void FaderCtrl::OnEnterWindow(wxMouseEvent &event)
{
  wxPostEvent(GetParent(), event);
}

void FaderCtrl::OnLeaveWindow(wxMouseEvent &event)
{
  if (Label)
    Label->Show(false);
  wxPostEvent(GetParent(), event);
}

