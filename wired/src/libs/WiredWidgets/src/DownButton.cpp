// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#include "DownButton.h"
//using namespace std;
BEGIN_EVENT_TABLE(DownButton, wxWindow)
  EVT_LEFT_DOWN(DownButton::OnMouseEvent) 
  EVT_RIGHT_DOWN(DownButton::OnMouseEvent) 
  EVT_LEFT_UP(DownButton::OnLeftUp) 
  EVT_LEAVE_WINDOW(DownButton::OnLeave) 
  EVT_PAINT(DownButton::OnPaint)
  EVT_ENTER_WINDOW(DownButton::OnEnterWindow)
END_EVENT_TABLE()

DownButton::DownButton(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		   const wxSize &size, wxImage *up, wxImage *down, bool simple)
  : wxWindow(parent, id, pos, size)
{
  state = 0;
  isdown = false;
  Simple = simple;
  Up =  new wxBitmap(*up);
  Down = new wxBitmap(*down);
}

DownButton::~DownButton()
{
	if (Up)
	  delete Up;
	if (Down)
	  delete Down;
}

void DownButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  wxBitmap  *tmp;
  wxMemoryDC memDC;

  tmp = GetCurrentBitmap();
  memDC.SelectObject(*tmp);

  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }     
  //dc.Blit(0, 0, tmp->GetWidth(), tmp->GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
}

void DownButton::OnMouseEvent(wxMouseEvent &event)
{
  if ((event.m_x > 0) && (event.m_x < Up->GetWidth()) && 
      (event.m_y > 0) && (event.m_y < Up->GetHeight()))
    {
      if (event.LeftDown())
        {
          isdown = !isdown;
          if (isdown)
            state = B_DOWN;
          else
            state = B_UP;
	  this->Refresh(false);

	  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
	  event.SetEventObject(this);
	  GetEventHandler()->ProcessEvent(event);
        }      
    }
  if (event.RightDown())
    wxPostEvent(GetParent(), event);
}

void DownButton::OnLeftUp(wxMouseEvent &event)
{
  if (Simple)
    {
      isdown = false;
      state = B_UP;
      this->Refresh(false);
    }
}

void DownButton::OnLeave(wxMouseEvent &event)
{
  OnLeftUp(event);
}

wxBitmap *DownButton::GetCurrentBitmap()
{
  if (isdown)
    return (Down);
  else if (state == B_UP)
    return (Up);
  else
    return (Down);
}

void DownButton::SetOn()
{
  isdown = true;
  state = B_DOWN;  
  Refresh();
}

void DownButton::SetOff()
{
  isdown = false;
  state = B_UP;  
  Refresh();
}

bool DownButton::GetOn()
{
  return (isdown);
}

void DownButton::OnEnterWindow(wxMouseEvent &event)
{
  wxPostEvent(GetParent(), event);
}
