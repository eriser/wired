// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


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
		     float begin_val, float end_val, float *val, bool is_int,
		     const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, 
	     wxSize(img_fg->GetWidth(), img_bg->GetHeight())),
    Value(val), BeginValue(begin_val), EndValue(end_val)
{ 
  Ord = end_val;
  Coeff = (end_val - begin_val) / 
    (double)(img_fg->GetHeight() - img_bg->GetHeight());
  IsInteger = is_int;
  Label = 0x0;

  bg = new wxBitmap(*img_bg);
  tmp_fg = new wxBitmap(*img_fg);
  tmp_fg->SetMask(new wxMask(*tmp_fg, *wxWHITE));
  fg = new StaticBitmap(this, -1, *tmp_fg, wxPoint(0, 0), 
			wxSize(img_fg->GetWidth(), img_fg->GetHeight()));
  SetValue(val);
  
//   cout << "img->GetHeight = " << img_bg->GetHeight() << endl;
//   cout << "begin_value - end_value = " << begin_val - end_val << endl;
//   cout << "Coeff = " << Coeff << "\t\t Ord = " << Ord << "\t\t" << begin_val << " -> " << end_val << endl;
//   for (int i = begin_value; i <= end_value; i += (begin_value > end_value) ? -1 : 1)
//     cout << "f(" << i << ") = " << Coeff * i + Ord << endl;
}

FaderCtrl::FaderCtrl(wxWindow *parent, wxWindowID id, 
		     wxImage *img_bg, wxImage *img_fg,
		     float begin_val, float end_val, float *val, bool is_int,
		     const wxPoint &pos, const wxSize &size,
		     wxWindow* hintparent, const wxPoint &hintpos)
  : wxWindow(parent, id, pos, wxSize(img_fg->GetWidth(), img_bg->GetHeight())),
    Value(val), BeginValue(begin_val), EndValue(end_val)
{ 
  IsInteger = is_int;
  Ord = end_val;
  Coeff = (end_val - begin_val) / (double) (img_fg->GetHeight() - img_bg->GetHeight());

  bg = new wxBitmap(*img_bg);
  tmp_fg = new wxBitmap(*img_fg);
  tmp_fg->SetMask(new wxMask(*tmp_fg, *wxWHITE));
  fg = new StaticBitmap(this, -1, *tmp_fg, wxPoint(0, 0), 
			wxSize(img_fg->GetWidth(), img_fg->GetHeight()));
  SetValue(val);

  wxString s;
  if (IsInteger)
    s.Printf(wxT("%d"), (int)GetValue());
  else
    s.Printf(wxT("%.2f"), GetValue());
  Label = new Hint(hintparent, -1, s,
		   wxPoint(GetPosition().x, GetPosition().y + GetSize().y), 
		   wxDefaultSize, *wxWHITE, *wxBLACK);
  Label->Show(false);

 //  cout << "img->GetHeight = " << img_bg->GetHeight() << endl;
//   cout << "begin_value - end_value = " << begin_val - end_val << endl;
//   cout << "Coeff = " << Coeff << "\t\t Ord = " << Ord << "\t\t" << begin_val << " -> " << end_val << endl;
// 
//   for (int i = begin_value; i <= end_value; i += (begin_value > end_value) ? -1 : 1)
//     cout << "f(" << i << ") = " << Coeff * i + Ord << endl;
}


FaderCtrl::~FaderCtrl()
{
//  if (tmp_fg)
//    delete tmp_fg;
//  if (bg)
//    delete bg;
//  if (fg)
//    delete fg;
//  if (Label)
//    delete Label;
}

void		FaderCtrl::OnPaint(wxPaintEvent &event)
{
  wxPaintDC	dc(this);
  wxMemoryDC	memDC;
  
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
	s.Printf(wxT("%d"), lrintf(GetValue()));
      else
	s.Printf(wxT("%.2f"), GetValue());
      Label->SetLabel(s);
    }
  OnMouseEvent(event);
}

void		FaderCtrl::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
  if (Label)
    Label->Show(false);
}

//
// Moves the fader when the user hold the mouse's left button
// and sets the value according to the position
// Follows up the event to the parent if the mouse's right button is pressed
//

void		FaderCtrl::OnMouseEvent(wxMouseEvent &event)
{
  if (event.LeftIsDown())
    {
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      if (event.GetPosition().y >= (GetSize().y - fg->GetSize().y / 2))
	SetValue(&BeginValue);
      else if (event.GetPosition().y < fg->GetSize().y / 2)
	SetValue(&EndValue);
      else
	SetValue(Ord + Coeff * 
		 (event.GetPosition().y - fg->GetSize().y / 2));
      GetEventHandler()->ProcessEvent(e);
    }
  else if (event.RightDown())
    wxPostEvent(GetParent(), event);
  if (Label)
    {
      wxString s;
      if (IsInteger)
	s.Printf(wxT("%d"), lrintf(GetValue()));
      else
	s.Printf(wxT("%.2f"), GetValue());
      Label->SetLabel(s);
      Label->Show((event.LeftIsDown()));
    }
}

//
// Returns the fader's value (obvious)
//

float		FaderCtrl::GetValue()
{
  return (*Value);
}

//
// Sets the fader's value at *val and moves the fader
// to the matching position
// the value is bounded by the ones specified in the
// constructor to avoid graphical bugs
//

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
      fg->Move(wxPoint(0, (*Value - Ord) / Coeff));
    }
}

//
// calls the method SetValue(float *)
//

void		FaderCtrl::SetValue(float val)
{
  SetValue(&val);
}

//
// hide the popup when the key pressed is released
//

void		FaderCtrl::OnKeyUp(wxKeyEvent& event)
{
  if (Label)
    Label->Show(false);
}

//
// When arrow keys (up & down) pressed faders moved by one pixel
// When pageup/pagedown pressed faders moved 1/10 of range
//

void		FaderCtrl::OnKeyDown(wxKeyEvent& event)
{
  float		step = 0;

  if (event.GetKeyCode() == WXK_UP)
    step = -Coeff;
  else if (event.GetKeyCode() == WXK_DOWN)
    step = Coeff;
  else if (event.GetKeyCode() == WXK_NEXT)
    step = -Ord / 10;
  else if (event.GetKeyCode() == WXK_PRIOR)
    step = Ord / 10;
  if (step)
    {
      SetValue(*Value + step);
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      GetEventHandler()->ProcessEvent(e);
    }
  if (Label)
    {
      Label->Show(true);
      wxString s;
      if (IsInteger)
	s.Printf(wxT("%d"), lrintf(GetValue()));
      else
	s.Printf(wxT("%.2f"), GetValue());
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
