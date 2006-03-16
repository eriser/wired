#include "HintedKnob.h"


BEGIN_EVENT_TABLE(HintedKnob, wxWindow)
  EVT_LEAVE_WINDOW(HintedKnob::OnLeave)
  EVT_ENTER_WINDOW(KnobCtrl::OnEnterWindow)
  EVT_MOTION(HintedKnob::OnMouseEvent) 
  EVT_PAINT(KnobCtrl::OnPaint)
  EVT_LEFT_DOWN(HintedKnob::OnMouseEvent)
  EVT_LEFT_UP(HintedKnob::OnMouseEvent)
  EVT_KEY_DOWN(HintedKnob::OnKeyDown)
END_EVENT_TABLE()

HintedKnob::HintedKnob(wxWindow *parent, wxWindowID id, wxWindow* hintparent,
		       wxImage *img_bg, wxImage *img_fg,
		       int begin_value, int end_value, int init_val, int pas,
		       const wxPoint &pos, const wxSize &size, 
		       const wxPoint& hintpos)
  : KnobCtrl(parent, id, img_bg, img_fg, begin_value, end_value, init_val, pas,
	     pos, size)
{
  wxString s;
  s.Printf(wxT("%d"), init_val);
  Label = new Hint(hintparent, -1, s, 
		   wxPoint( hintpos.x + GetSize().x, hintpos.y + GetSize().y ),
		   wxDefaultSize, *wxWHITE, *wxBLACK);
  Label->Show(false);
}


HintedKnob::~HintedKnob()
{
	if (Label)
		delete Label;
}

void HintedKnob::OnMouseEvent(wxMouseEvent &event)
{
  if (event.LeftIsDown())
    Label->Show(true);
  KnobCtrl::OnMouseEvent(event);
  wxString s;
  s.Printf(wxT("%d"), GetValue());
  Label->SetLabel(s);
  if (!event.LeftIsDown())
    Label->Show(false);
}

void HintedKnob::OnKeyDown(wxKeyEvent& event)
{
  Label->Show(true);
  KnobCtrl::OnKeyDown(event);
  wxString s;
  s.Printf(wxT("%d"), GetValue());
  Label->SetLabel(s);
}  

void HintedKnob::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
  if (Label)
    Label->Show(false);
}

void HintedKnob::OnKeyUp(wxKeyEvent& WXUNUSED(event))
{
  if (Label)
    Label->Show(false);
}

void HintedKnob::OnLeave(wxMouseEvent &event)
{
  if (Label)
    Label->Show(false);
  wxPostEvent(GetParent(), event);
}
