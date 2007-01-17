// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "StaticPosKnob.h"

BEGIN_EVENT_TABLE(StaticPosKnob, wxWindow)
  EVT_MOTION(StaticPosKnob::OnMotionEvent) 
  EVT_PAINT(StaticPosKnob::OnPaint)
  EVT_LEFT_DOWN(StaticPosKnob::OnLeftDown)
  EVT_LEFT_UP(StaticPosKnob::OnLeftUp)
  EVT_KEY_DOWN(StaticPosKnob::OnKeyDown)
  EVT_ENTER_WINDOW(StaticPosKnob::OnEnter)
END_EVENT_TABLE()

StaticPosKnob::StaticPosKnob(wxWindow *parent, wxWindowID id, 
			     int steps, wxImage **imgs, int mouse_step,
			     int begin_value, int end_value, int init_val,
			     const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, 
	     wxSize(imgs[0]->GetWidth(), imgs[0]->GetHeight()))
{
  Clicked = false;
  Steps = steps;
  MouseStep = mouse_step;
  BeginVal = begin_value;
  EndVal = end_value;
  CurVal = init_val;
  Count = 0;
  /*
    if (EndVal != Steps + BeginVal)
    {
      cout << "[STATIC POS KNOB] error Steps+BeginVal != EndVal" << endl;
      throw;
      }*/

  Bmps = new wxBitmap*[Steps];
  for (int i = 0; i < Steps; i++)
    {
      Bmps[i] = new wxBitmap(*(imgs[i]));
    }
  
}

StaticPosKnob::~StaticPosKnob()
{
  for (int i = 0; i < Steps; i++)
    {
    	if (Bmps[i])
			delete Bmps[i];
    }
  delete [] Bmps;
}

void StaticPosKnob::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  wxBitmap  *tmp;
  wxMemoryDC memDC;
  
  tmp = Bmps[CurVal-BeginVal];
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

void StaticPosKnob::OnMotionEvent(wxMouseEvent &event)
{
  if (!Clicked)
    return;
  int move = event.GetPosition().y - ClickPos;
  
  Count += move;
  ClickPos = event.GetPosition().y;
  if (Count > MouseStep && CurVal + 1 <= EndVal)
    {
      Count = 0;
      CurVal++;
      Refresh();
      wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
      e.SetEventObject(this);
      wxPostEvent(GetParent(), e);
    }
  else if (-Count > MouseStep && CurVal - 1 >= BeginVal)
    {
      Count = 0;
      CurVal--;
      Refresh();
      wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
      e.SetEventObject(this);
      wxPostEvent(GetParent(), e);
    }
}

void StaticPosKnob::OnLeftDown(wxMouseEvent& event)
{
  Count = 0;
  Clicked = true;
  ClickPos = event.GetPosition().y;
}

void StaticPosKnob::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
  Count = 0;
  Clicked = false;
}

int StaticPosKnob::GetValue()
{
  return CurVal;
}

void StaticPosKnob::SetValue(int val)
{
  if (val > EndVal)
    CurVal = EndVal;
  else if (val < BeginVal)
    CurVal = BeginVal;
  else
    CurVal = val;
  Refresh();
}

void StaticPosKnob::OnKeyDown(wxKeyEvent& event)
{
  if ((event.GetKeyCode() == WXK_UP) && (CurVal > BeginVal))
    {
      CurVal--;
      Refresh();
      wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
      e.SetEventObject(this);
      wxPostEvent(GetParent(), e);
    }
  else if ((event.GetKeyCode() == WXK_DOWN) && (CurVal < EndVal))
    {
      CurVal++;
      Refresh();
      wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
      e.SetEventObject(this);
      wxPostEvent(GetParent(), e);
    }
}

void StaticPosKnob::OnEnter(wxMouseEvent &event)
{
  wxPostEvent(GetParent(), event);
}
