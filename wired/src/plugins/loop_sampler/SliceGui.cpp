#include "SliceGui.h"
#include <iostream>
//#include <wx/wx.h>

using namespace std;

#define SIZE_R	9

BEGIN_EVENT_TABLE(SliceBtn, wxWindow)
  EVT_MOUSE_EVENTS(SliceBtn::OnMouseEvent)
END_EVENT_TABLE()

SliceBtn::SliceBtn(SliceGui *s, wxWindow *parent, wxWindowID id, const wxPoint &pos, 
		   const wxSize &size, int style)
  : wxWindow(parent, id, pos, size, style), MySlice(s)
{
  SetBackgroundColour(wxColour(130, 206,  95));   
}

SliceBtn::~SliceBtn()
{

}

void SliceBtn::OnMouseEvent(wxMouseEvent &event)
{
  if (event.LeftDown())
    {
      wxCommandEvent ev(TYPE_SLICE_SELECTED, GetId());
      ev.SetClientData(MySlice);
      wxPostEvent(GetParent(), ev);
    }
  if (event.Dragging())
    {
      wxCommandEvent ev(TYPE_SLICE_MOVE, GetId());
      ev.SetClientData(MySlice);
      ev.SetInt(event.m_x);
      wxPostEvent(GetParent(), ev);      
    }
}


SliceGui::SliceGui(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
  : Selected(false)
{
  l = new wxStaticLine(parent, -1, pos, size, wxLI_VERTICAL);
  l->SetBackgroundColour(*wxBLACK);
  
  r = new SliceBtn(this, parent, id, wxPoint(pos.x - SIZE_R + 1, size.y - SIZE_R), wxSize(SIZE_R, SIZE_R),
		   wxSIMPLE_BORDER);

  /*  r->Connect(31, wxEVT_LEFT_DOWN, 
	     (wxObjectEventFunction)(wxEventFunction) 
	     (wxMouseEventFunction)&SliceGui::OnMouseEvent);
  r->Connect(31, wxEVT_LEFT_UP, 
	     (wxObjectEventFunction)(wxEventFunction) 
	     (wxMouseEventFunction)&SliceGui::OnMouseEvent);
  r->Connect(31, wxEVT_MOTION, 
	     (wxObjectEventFunction)(wxEventFunction) 
	     (wxMouseEventFunction)&SliceGui::OnMouseEvent);
  */
}

SliceGui::~SliceGui()
{

}

void SliceGui::SetPosition(const wxPoint &pos)
{
  l->SetPosition(pos);
  r->SetPosition(wxPoint(pos.x - SIZE_R + 1, l->GetSize().y - SIZE_R));
}

void SliceGui::SetSize(const wxSize &size)
{
  l->SetSize(-1, size.y);
  r->SetPosition(wxPoint(l->GetPosition().x - SIZE_R + 1, l->GetSize().y - SIZE_R));
}

void SliceGui::SetSelected(bool sel)
{
  if (sel)
    r->SetBackgroundColour(*wxBLACK);
  else
    r->SetBackgroundColour(*wxLIGHT_GREY);
  Selected = sel;
}
