#include "LoopCursor.h"
#include <iostream>
//#include <wx/wx.h>

using namespace std;

#define SIZE_R	12

BEGIN_EVENT_TABLE(LoopBtn, wxWindow)
  EVT_PAINT(LoopBtn::OnPaint)
  EVT_MOUSE_EVENTS(LoopBtn::OnMouseEvent)
END_EVENT_TABLE()

LoopBtn::LoopBtn(LoopCursor *s, wxWindow *parent, wxWindowID id, const wxPoint &pos, 
		   const wxSize &size, int style)
  : wxWindow(parent, id, pos, size, style), MyLoop(s)
{
  if (MyLoop->LoopBegin)
    SetBackgroundColour(wxColour( 50, 166, 211));   
  else
    SetBackgroundColour(wxColour(133,  96, 204));
}

LoopBtn::~LoopBtn()
{

}

void LoopBtn::OnMouseEvent(wxMouseEvent &event)
{
  if (event.LeftDown())
    {
      wxCommandEvent ev(TYPE_SLICE_SELECTED, GetId());
      ev.SetClientData(MyLoop);
      wxPostEvent(GetParent(), ev);
    }
  if (event.Dragging())
    {
      wxCommandEvent ev(TYPE_SLICE_MOVE, GetId());
      ev.SetClientData(MyLoop);
      ev.SetInt(event.m_x);
      wxPostEvent(GetParent(), ev);      
    }
}

void LoopBtn::OnPaint(wxPaintEvent &e)
{
  wxPaintDC dc(this);

  dc.SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  dc.SetTextForeground(*wxWHITE);
  if (MyLoop->LoopBegin)
    dc.DrawText(wxT("S"), 2, 0);
  else
    dc.DrawText(wxT("E"), 2, 0);
}

LoopCursor::LoopCursor(bool loop_begin, wxWindow *parent, wxWindowID id, const wxPoint &pos, 
		       const wxSize &size)
  : LoopBegin(loop_begin)
{
  l = new wxStaticLine(parent, -1, wxPoint(pos.x, pos.y + SIZE_R), wxSize(1, size.y - SIZE_R), 
		       wxLI_VERTICAL);
  l->SetForegroundColour(*wxBLACK);
 
  if (LoopBegin)
    r = new LoopBtn(this, parent, id, pos, wxSize(SIZE_R, SIZE_R),
		    wxSIMPLE_BORDER);
  else
    r = new LoopBtn(this, parent, id, wxPoint(pos.x - SIZE_R + 1, pos.y), wxSize(SIZE_R, SIZE_R),
		    wxSIMPLE_BORDER);
}

LoopCursor::~LoopCursor()
{

}

void LoopCursor::SetPosition(const wxPoint &pos)
{
  l->SetPosition(wxPoint(pos.x, pos.y + SIZE_R));
  if (LoopBegin)
    r->SetPosition(pos);
  else
    r->SetPosition(wxPoint(pos.x - SIZE_R + 1, pos.y));
}

void LoopCursor::SetSize(const wxSize &size)
{
  l->SetSize(-1, size.y - SIZE_R);
  if (LoopBegin)
    l->SetPosition(wxPoint(r->GetPosition().x, r->GetPosition().y + SIZE_R));
  else
    l->SetPosition(wxPoint(r->GetPosition().x + SIZE_R, r->GetPosition().y + SIZE_R));
  //r->SetPosition(wxPoint(l->GetPosition().x - SIZE_R + 1, l->GetSize().y - SIZE_R));
}
