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

#include "HoldButton.h"

BEGIN_EVENT_TABLE(HoldButton, wxWindow)
  EVT_LEFT_DOWN(HoldButton::OnLeftDown) 
  EVT_LEFT_UP(HoldButton::OnLeftUp) 
  EVT_LEAVE_WINDOW(HoldButton::OnLeaveWindow) 
  EVT_PAINT(HoldButton::OnPaint)
  EVT_TIMER(31, HoldButton::OnTimer)
END_EVENT_TABLE()

HoldButton::HoldButton(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		   const wxSize &size, wxImage *up, wxImage *down)
  : wxWindow(parent, id, pos, wxSize(up->GetWidth(), up->GetHeight()))
{
  isdown = false;
  Up =  new wxBitmap(up);
  Down = new wxBitmap(down);
  Timer = new wxTimer(this, 31);
  CountTime = 0;
  Interval2 = false;
}

HoldButton::~HoldButton()
{
	if (Up)
	  delete Up;
	if (Down)
	  delete Down;
	if (Timer)
	  delete Timer;
}

void HoldButton::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  wxBitmap  *tmp;
  wxMemoryDC memDC;

  if (isdown)
    tmp = Down;
  else
    tmp = Up;
  memDC.SelectObject(*tmp);

  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }     
}

void HoldButton::OnLeftDown(wxMouseEvent &event)
{
  Timer->Start(TIME_INTERVAL1);
  CountTime = 0;
  Interval2 = false;
  isdown = true;
  this->Refresh(false);
}

void HoldButton::OnLeftUp(wxMouseEvent &event)
{
  Timer->Stop();
  isdown = false;
  this->Refresh(false);
}

void HoldButton::OnLeaveWindow(wxMouseEvent &event)
{
  OnLeftUp(event);
}

void HoldButton::OnTimer(wxTimerEvent &event)
{
  wxCommandEvent _event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  _event.SetEventObject(this);
  GetEventHandler()->ProcessEvent(_event);
  if (!Interval2)
    {
      if (CountTime < TIME_CHANGE)
	CountTime += TIME_INTERVAL1;
      else
	{
	  Interval2 = true;
	  Timer->Stop();
	  Timer->Start(TIME_INTERVAL2);
	}
    }
}
