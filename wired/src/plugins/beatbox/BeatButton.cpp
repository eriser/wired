#include "BeatButton.h"
#include <stdio.h>

using namespace std;

BEGIN_EVENT_TABLE(BeatButton, wxWindow)
  EVT_LEFT_DOWN(BeatButton::OnMouseEvent)
  EVT_PAINT(BeatButton::OnPaint)
END_EVENT_TABLE()

BeatButton::BeatButton(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		       const wxSize &size, wxBitmap** bitmaps, 
		       unsigned char position)
  : wxWindow(parent, id, pos, size)
{
  Data[ID_POS] = position;
  Data[ID_STATE] = ID_UNCLICKED;
  
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

  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), 
	      &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }     
}

void BeatButton::OnMouseEvent(wxMouseEvent &event)
{
  if ( (event.m_x > 0) && (event.m_x < BTN_SIZE) && 
       (event.m_y > 0) && (event.m_y < BTN_SIZE) )
    {
      if (event.LeftDown())
        {
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
}

void BeatButton::SetState(unsigned char state)
{
  Data[ID_STATE] = state;
  Refresh();
}
