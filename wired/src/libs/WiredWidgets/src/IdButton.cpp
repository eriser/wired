// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "IdButton.h"

using namespace std;

BEGIN_EVENT_TABLE(IdButton, wxWindow)
  EVT_LEFT_DOWN(IdButton::OnMouseEvent) 
  EVT_PAINT(IdButton::OnPaint)
  EVT_ENTER_WINDOW(IdButton::OnEnter)
END_EVENT_TABLE()
  
IdButton::IdButton(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
		   const wxSize& size, wxImage* up, wxImage* down, 
		   unsigned int num_id)
  : wxWindow(parent, id, pos, size)
{
  State = UNCLICKED;
  Num = num_id;
  
  Bitmaps = new wxBitmap*[2];
  Bitmaps[UNCLICKED] = new wxBitmap(*up);
  Bitmaps[CLICKED]   = new wxBitmap(*down);
}

IdButton::~IdButton()
{
	if (Bitmaps)
	{
		if (Bitmaps[UNCLICKED])
			delete Bitmaps[UNCLICKED];
		if (Bitmaps[CLICKED])
			delete Bitmaps[CLICKED];
	  delete [] Bitmaps;
	}
}


void IdButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  wxBitmap  *tmp;
  wxMemoryDC memDC;
  
  tmp = Bitmaps[State];
  memDC.SelectObject(*tmp);
  
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), 
	      &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }     
}

void IdButton::SetOn()
{
  State = CLICKED;
  Refresh();
}

void IdButton::SetOff()
{
  State = UNCLICKED;
  Refresh();
}

void IdButton::OnMouseEvent(wxMouseEvent &event)
{
  if ((event.m_x > 0) && (event.m_x < Bitmaps[UNCLICKED]->GetWidth()) && 
      (event.m_y > 0) && (event.m_y < Bitmaps[UNCLICKED]->GetHeight()))
    {
      if (event.LeftDown())
        {
          if (State == UNCLICKED)
	    {
	      State = CLICKED;
	      this->Refresh(false);
	      wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
	      e.SetClientData(&Num);
	      e.SetEventObject(this);
	      wxPostEvent(GetParent(), e);
	    }
	}
    }
}

void IdButton::OnEnter(wxMouseEvent& event)
{
  wxPostEvent(GetParent(), event);
}
