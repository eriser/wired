#include "KnobCtrl.h"
#include <math.h>

BEGIN_EVENT_TABLE(KnobCtrl, wxWindow)
  EVT_MOTION(KnobCtrl::OnMouseEvent) 
  EVT_PAINT(KnobCtrl::OnPaint)
  EVT_LEFT_DOWN(KnobCtrl::OnMouseEvent)
  EVT_RIGHT_DOWN(KnobCtrl::OnMouseEvent)
  EVT_LEFT_UP(KnobCtrl::OnLeftUp)
  EVT_ENTER_WINDOW(KnobCtrl::OnEnterWindow)
  EVT_LEAVE_WINDOW(KnobCtrl::OnLeave)
  EVT_KEY_DOWN(KnobCtrl::OnKeyDown)
  EVT_KEY_UP(KnobCtrl::OnKeyUp)
END_EVENT_TABLE()

namespace Limits
{
  namespace Knob
  {
    static unsigned Higher = 128;
  }
}

KnobCtrl::KnobCtrl(wxWindow *parent, wxWindowID id, wxImage *img_bg, 
		   wxImage *img_fg, int begin_value, int end_value, 
		   int init_val, int pas, 
		   const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, wxSize(img_bg->GetWidth(), img_bg->GetHeight())),
    knob_fg(img_fg), knob_bg(img_bg), Size(size), Value(0), OldValue(0), 
    BeginValue(begin_value), EndValue(end_value), Pas(pas), oldy(0)
{
  assert(Limits::Knob::Higher);
  if (Pas == 0)
    throw Pas; // FIXME throw something serious
  fg_tmp = new wxBitmap(knob_fg, -1); 
  fg_mask = new wxMask(*fg_tmp, *wxBLACK);
  fg_tmp->SetMask(fg_mask);
  fg = new wxStaticBitmap(this, -1, *fg_tmp, wxPoint(0,0), wxSize(knob_fg->GetWidth(), knob_fg->GetHeight()));
  SetValue(init_val);
  Label = 0x0;
}

KnobCtrl::KnobCtrl(wxWindow *parent, wxWindowID id,
		   wxImage *img_bg, wxImage *img_fg,
		   int begin_value, int end_value, int init_val, int pas,
		   const wxPoint &pos, const wxSize &size, 
		   wxWindow* hintparent, const wxPoint& hintpos)
  : wxWindow(parent, id, pos, wxSize(img_bg->GetWidth(), img_bg->GetHeight())),
    knob_fg(img_fg), knob_bg(img_bg), Size(size), Value(0), OldValue(0), 
    BeginValue(begin_value), EndValue(end_value), Pas(pas), oldy(0)
{
  assert(Limits::Knob::Higher);
  if (Pas == 0)
    throw Pas; // FIXME throw something serious
  fg_tmp = new wxBitmap(knob_fg, -1); 
  fg_mask = new wxMask(*fg_tmp, *wxBLACK);
  fg_tmp->SetMask(fg_mask);
  fg = new wxStaticBitmap(this, -1, *fg_tmp, 
			  wxPoint(0,0), wxSize(knob_fg->GetWidth(), 
					       knob_fg->GetHeight()));
  SetValue(init_val);
  
  wxString s;
  s.Printf("%d", init_val);
  Label = new Hint(hintparent, -1, s, 
		   wxPoint( hintpos.x + pos.x + GetSize().x, 
			    hintpos.y + pos.y + GetSize().y ),
		   wxDefaultSize, *wxWHITE, *wxBLACK);
  Label->Show(false);
}


KnobCtrl::~KnobCtrl()
{
  delete fg_mask;
}

void KnobCtrl::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  wxMemoryDC memDC;
  memDC.SelectObject(*knob_bg);
  dc.Blit(0, 0, knob_bg->GetWidth(), knob_bg->GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
}

void KnobCtrl::OnLeftUp(wxMouseEvent& event)
{
  if (Label)
    Label->Show(false);
  OnMouseEvent(event);
}

void KnobCtrl::OnMouseEvent(wxMouseEvent &event)
{
  
  static bool first = true;
  wxClientDC dc (this);
  wxPoint offset;
  if (event.LeftIsDown())
    {
      if (first)
	first = false;
      else
	{
	  wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
	  int tmp = event.GetPosition().y/2 - (Size.y / 2);
	  SetValue_(Value - (tmp - oldy));
	  GetEventHandler()->ProcessEvent(e);
	  //      cout << "delta : " << tmp - oldy << " value : "<< Value << endl;
	  oldy = tmp;
	}
    }
  if (event.LeftUp())
    {
      oldy = 0;
      first = true;
    }
  if (event.RightDown())
    {
      wxPostEvent(GetParent(), event);
    }
  if (event.LeftIsDown() && Label)
    {
      Label->Show(true);
      wxString s;
      s.Printf("%d", GetValue());
      Label->SetLabel(s);
    }
  else if (!event.LeftIsDown() && Label)
    Label->Show(false);

}

void KnobCtrl::OnKeyDown(wxKeyEvent& event)
{
  int value = GetValue();
  if(event.GetKeyCode() == WXK_UP)
    {
      value++;
      if (value > EndValue)
	value = EndValue;
      SetValue(value + 1); // need a fix, SetValue(x) <==> x-1 = GetValue()
      wxScrollEvent e(wxEVT_SCROLL_TOP, GetId());
      e.SetEventObject(this);
      GetEventHandler()->ProcessEvent(e);
    }
  else if(event.GetKeyCode() == WXK_DOWN)
    {
      value--;
      if (value < BeginValue)
	value = BeginValue;
      SetValue(value);
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

void KnobCtrl::OnKeyUp(wxKeyEvent& WXUNUSED(event))
{
  if (Label)
    Label->Show(false);
}

int KnobCtrl::GetValue()
{ 
  return Value * (EndValue - BeginValue) / Limits::Knob::Higher / Pas * Pas + BeginValue;
}

void KnobCtrl::SetValue(int val)
{
  // FIXME verify result is > 0 and < Limits::Knob::Higher
  SetValue_((val - BeginValue) * Limits::Knob::Higher / (EndValue - BeginValue));
}

void KnobCtrl::SetValue_(unsigned val)
{ 
  Value = val; 
  if (Value < 0)
    Value = 0;
  if (Value > Limits::Knob::Higher)
    Value = Limits::Knob::Higher;
  if (abs(Value - OldValue) >= Pas)
    {
      float angle = M_PI - (2* M_PI / ((EndValue - BeginValue)) * GetValue());
      //      float angle = M_PI - (2* M_PI / Limits::Knob::Higher * Value);
      int x = static_cast<int>(round((Size.x /2) - sin(angle) * (Size.x /2 - knob_fg->GetWidth()/2) - knob_fg->GetWidth()/2));
      int y = static_cast<int>(round((Size.y /2) - cos(angle) * (Size.y /2 - knob_fg->GetHeight()/2) - knob_fg->GetHeight()/2));
      //      cout << "pas : " << Pas << " Value : " << Value << " delta : " << abs(Value - OldValue) << endl;
      fg->Move(wxPoint(x,y));
      OldValue = Value;
    }
}

void KnobCtrl::OnEnterWindow(wxMouseEvent &event)
{
  wxPostEvent(GetParent(), event);
}

void KnobCtrl::OnLeave(wxMouseEvent &event)
{
  if (Label)
    Label->Show(false);
  wxPostEvent(GetParent(), event);
}
