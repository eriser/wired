#include "CycleKnob.h"

BEGIN_EVENT_TABLE(CycleKnob, wxWindow)
  EVT_MOTION(CycleKnob::OnMotionEvent) 
  EVT_PAINT(CycleKnob::OnPaint)
  EVT_LEFT_DOWN(CycleKnob::OnLeftDown)
  EVT_LEFT_UP(CycleKnob::OnLeftUp)
  EVT_KEY_DOWN(CycleKnob::OnKeyDown)
END_EVENT_TABLE()

CycleKnob::CycleKnob(wxWindow *parent, wxWindowID id, 
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
    }
  */

  Bmps = new wxBitmap*[Steps];
  for (int i = 0; i < Steps; i++)
    {
      Bmps[i] = new wxBitmap(imgs[i]);
    }
  
}

CycleKnob::~CycleKnob()
{
  for (int i = 0; i < Steps; i++)
    {
      delete Bmps[i];
    }
  delete [] Bmps;
}

void CycleKnob::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  wxBitmap  *tmp;
  wxMemoryDC memDC;
  
  assert((CurVal % Steps >= 0) && (CurVal % Steps < Steps));
  tmp = Bmps[CurVal % Steps];
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

void CycleKnob::OnMotionEvent(wxMouseEvent &event)
{
  if (!Clicked)
    return;
  int move = event.GetPosition().y - ClickPos;
  
  Count -= move;
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

void CycleKnob::OnLeftDown(wxMouseEvent& event)
{
  Count = 0;
  Clicked = true;
  ClickPos = event.GetPosition().y;
}

void CycleKnob::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
  Count = 0;
  Clicked = false;
}

int CycleKnob::GetValue()
{
  return CurVal;
}

void CycleKnob::SetValue(int val)
{
  if (val > EndVal)
    CurVal = EndVal;
  else if (val < BeginVal)
    CurVal = BeginVal;
  else
    CurVal = val;
  Refresh();
}

void CycleKnob::OnKeyDown(wxKeyEvent& event)
{
  if ((event.GetKeyCode() == WXK_UP) && (CurVal < EndVal))
    {
      CurVal++;
      Refresh();
      wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
      e.SetEventObject(this);
      wxPostEvent(GetParent(), e);
    }
  else if ((event.GetKeyCode() == WXK_DOWN) && (CurVal > BeginVal))
    {
      CurVal--;
      Refresh();
      wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, this->GetId());
      e.SetEventObject(this);
      wxPostEvent(GetParent(), e);
    }
}
