#include <wx/layout.h>
#include "HintedFader.h"


BEGIN_EVENT_TABLE(HintedFader, wxWindow)
  EVT_LEAVE_WINDOW(HintedFader::OnLeave)
  EVT_ENTER_WINDOW(HintedFader::OnEnterWindow)
  EVT_LEFT_DOWN(HintedFader::OnLeftDown)
  
  //EVT_LEFT_DOWN(HintedFader::OnMouseEvent)
  EVT_LEFT_UP(HintedFader::OnLeftUp)
  EVT_MOTION(HintedFader::OnMouseEvent)
  EVT_PAINT(FaderCtrl::OnPaint)
  EVT_KEY_DOWN(HintedFader::OnKeyDown)
  EVT_KEY_UP(HintedFader::OnKeyUp)
END_EVENT_TABLE()


HintedFader::HintedFader(wxWindow *parent, wxWindowID id, wxWindow* hintparent,
			 wxImage *img_bg, wxImage  *img_fg,
			 long begin_value, long end_value, long val,
			 const wxPoint &pos, const wxSize &size, 
			 const wxPoint &hintpos)//, const wxSize &size, )
  : FaderCtrl(parent, id, img_bg, img_fg, begin_value, end_value, val, 
	      pos, size)
{

  wxString s;
  s.Printf("%d", val);
  HintParent = hintparent;
  Label = new Hint(hintparent, -1, s, 
		   //wxPoint( hintpos.x + GetSize().x, hintpos.y + GetSize().y ),
		   //wxPoint(parent->GetPosition().x + this->GetPosition().x, parent->GetPosition().y + this->GetPosition().y + 75),
		   wxPoint(this->GetPosition().x, this->GetPosition().y + this->GetSize().y),
		   wxDefaultSize, *wxWHITE, *wxBLACK);
  Label->Show(false);
  
}

HintedFader::~HintedFader()
{
  if (Label)
    delete Label;
}

 void HintedFader::OnLeftDown(wxMouseEvent& WXUNUSED(event))
{
  //Label->SetPosition(GetPosition());
  
  Label->Show(true);
}

void HintedFader::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
  //if (!event.LeftIsDown())
  Label->Show(false);
}

void HintedFader::OnKeyDown(wxKeyEvent& event)
{
  Label->Show(true);
  FaderCtrl::OnKeyDown(event);
  wxString s;
  s.Printf("%d", GetValue());
  Label->SetLabel(s);
}

void HintedFader::OnKeyUp(wxKeyEvent& event)
{
  Label->Show(false);
}

void HintedFader::OnMouseEvent(wxMouseEvent &event)
{
  if (event.LeftIsDown())
    Label->Show(true);
  FaderCtrl::OnMouseEvent(event);
  wxString s;
  s.Printf("%d", GetValue());
  cout << "GetValue : " << GetValue() << endl;
  Label->SetLabel(s);
  if (!event.LeftIsDown())
    Label->Show(false);
}


void HintedFader::OnEnterWindow(wxMouseEvent &event)
{
  wxPostEvent(GetParent(), event);
}

void HintedFader::OnLeave(wxMouseEvent &event)
{
  Label->Show(false);
  wxPostEvent(GetParent(), event);
}
