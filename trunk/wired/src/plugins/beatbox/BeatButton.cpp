#include "BeatButton.h"
#include <stdio.h>

using namespace std;

BEGIN_EVENT_TABLE(BeatButton, wxWindow)
  EVT_LEAVE_WINDOW(BeatButton::OnLeave)
  EVT_ENTER_WINDOW(BeatButton::OnEnter)
  EVT_LEFT_DOWN(BeatButton::OnMouseEvent)
  EVT_RIGHT_DOWN(BeatButton::OnRightDownEvent)
  EVT_PAINT(BeatButton::OnPaint)
  EVT_MOTION(BeatButton::OnMotion)
END_EVENT_TABLE()

BeatButton::BeatButton(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		       const wxSize &size, wxBitmap** bitmaps, 
		       unsigned int position)
  : wxWindow(parent, id, pos, size)
{
  Data[ID_POS] = position;
  Data[ID_STATE] = ID_UNCLICKED;
  Data[ID_X] = 0;
  Data[ID_COPY] = 0;
  Bitmaps = bitmaps;
}

BeatButton::~BeatButton()
{
}

void BeatButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  wxBitmap  *tmp;
  wxMemoryDC memDC;
  
  tmp = Bitmaps[Data[ID_STATE]];
  memDC.SelectObject(*tmp);

  wxRegionIterator upd(GetUpdateRegion());
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), 
	      &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }     
}
void BeatButton::OnRightDownEvent(wxMouseEvent& event)
{
  if (event.ShiftDown())
    {
      Data[ID_COPY] = 1;
      wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
      event.SetClientData((void*)Data);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
  else if ( (event.m_x > 0) && (event.m_x < BTN_SIZE) && 
       (event.m_y > 0) && (event.m_y < BTN_SIZE) )
    {
      Data[ID_COPY] = 0;
      if (Data[ID_STATE] == ID_UNCLICKED)
	return;
      Data[ID_STATE] = ID_UNCLICKED;
      this->Refresh(false);
      wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
      event.SetClientData((void*)Data);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
}

void BeatButton::OnMouseEvent(wxMouseEvent &event)
{
  if ( (event.m_x > 0) && (event.m_x < BTN_SIZE) && 
       (event.m_y > 0) && (event.m_y < BTN_SIZE) )
    {
      Data[ID_COPY] = 0;
      char tmp_state;
      GET_STATE(tmp_state, event.m_x, event.m_y );
      if (tmp_state == Data[ID_STATE])
	Data[ID_STATE] = 0;
      else
	Data[ID_STATE] = tmp_state;
      this->Refresh(false);
      wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
      event.SetClientData((void*)Data);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
}

void BeatButton::OnMotion(wxMouseEvent &event)
{
  if (event.Dragging())
    {
      if ( ((event.m_x < 0) || (event.m_x > BTN_SIZE))  &&
	   ((event.m_y > 0) && (event.m_y < BTN_SIZE)) )
	{
	  Data[ID_COPY] = 0;
	  wxCommandEvent e(EVT_MOTION_OUT, BEATBUTTON_ID);
	  Data[ID_X] = GetPosition().x + event.m_x;
	  e.SetClientData((void*)Data);
	  e.SetEventObject(this);
	  GetEventHandler()->ProcessEvent(e);
	}
    }
}

void BeatButton::SetState(unsigned int state)
{
  Data[ID_STATE] = state;
  Refresh();
}

void BeatButton::OnLeave(wxMouseEvent& event)
{
  wxPostEvent(GetParent(), event);
}

void BeatButton::OnEnter(wxMouseEvent& event)
{
  wxPostEvent(GetParent(), event);
}
