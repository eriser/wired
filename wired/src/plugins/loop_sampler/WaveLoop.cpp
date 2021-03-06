// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include "WaveLoop.h"
#include "Colour.h"
#include <iostream>
#include <math.h>

/*BEGIN_EVENT_TABLE(WaveLoop, WaveView)
  EVT_LEFT_DOWN(WaveLoop::OnClick)
  END_EVENT_TABLE()*/

#define SLICE_ID	3131
#define START_ID	3132

WaveLoop::WaveLoop(wxMutex *mutex, LoopPos *loopinfo, wxWindow *parent, wxWindowID id, 
		   const wxPoint& pos, const wxSize& size)
  : WaveView(parent, id, pos, size, true, false), Mutex(mutex), LoopInfo(loopinfo)
{
  LoopStart = 0;
  LoopEnd = 0;
  Drawing = false;
  Select = true;
  NoteNumber = 0x48; //C3
  SamplingRate = 44100;

  LoopEndCursor = new LoopCursor(false, this, START_ID, wxPoint(GetSize().x, 0), 
			   wxSize(1, GetSize().y));
  LoopStartCursor = new LoopCursor(true, this, START_ID, wxPoint(0, 0), wxSize(1, GetSize().y));

  Connect(GetId(), wxEVT_LEFT_DOWN, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WaveLoop::OnClick);
  Connect(GetId(), wxEVT_SIZE, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxSizeEventFunction)&WaveLoop::OnSize);
  Connect(GetId(), wxEVT_PAINT, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxPaintEventFunction)&WaveLoop::OnPaint);
  Connect(SLICE_ID, TYPE_SLICE_SELECTED, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&WaveLoop::OnSliceBtnSelected);
  Connect(SLICE_ID, TYPE_SLICE_MOVE, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&WaveLoop::OnSliceMove);
  Connect(START_ID, TYPE_SLICE_MOVE, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&WaveLoop::OnLoopMove);
}

WaveLoop::~WaveLoop()
{

}

void WaveLoop::SetWave(WaveFile *w)
{
  WaveView::SetWave(w);
  LoopStart = 0;
  LoopEnd = EndWavePos;
}

void WaveLoop::OnLoopMove(wxCommandEvent &event)
{
  LoopCursor *cur = (LoopCursor *)event.GetClientData();
  int pos_x = event.GetInt() + cur->GetPosition().x;

  if (pos_x < 0)
    {
      if (cur->LoopBegin)
	cur->SetPosition(wxPoint(0, 0)); 
      else
	cur->SetPosition(wxPoint(4, 0)); 
    }
  else if (pos_x > GetClientSize().x)
    cur->SetPosition(wxPoint(GetClientSize().x, 0)); 
  else
    cur->SetPosition(wxPoint(pos_x, 0)); 

  Mutex->Lock();

  if (cur->LoopBegin)
    {
      LoopInfo->Start = cur->GetPosition().x * (EndWavePos / GetClientSize().x);
    }
  else
    {
      LoopInfo->End = cur->GetPosition().x * (EndWavePos / GetClientSize().x);
    }

  Mutex->Unlock();
}

void WaveLoop::SetLoopCursors()
{
  double inc = (double)GetClientSize().x / (double)EndWavePos;

  LoopStartCursor->SetPosition(wxPoint((int)(inc * LoopInfo->Start), 0));
  LoopEndCursor->SetPosition(wxPoint((int)(inc * LoopInfo->End), 0));
}

void WaveLoop::OnSliceBtnSelected(wxCommandEvent &event)
{
  list<Slice *>::iterator i;
  SliceGui *g;

  for (i = Slices->begin(); i != Slices->end(); i++)
    {
      g = (SliceGui *)((*i)->Data);
      if ((*i)->Data == event.GetClientData())
	g->SetSelected(true);
      else
	g->SetSelected(false);
    }
}

void WaveLoop::OnSliceMove(wxCommandEvent &event)
{
  list<Slice *>::iterator i, j;
  SliceGui *g;
  Slice *tmp = 0x0;

  for (i = Slices->begin(); i != Slices->end(); i++)
    {     
      g = (SliceGui *)((*i)->Data);
      if ((*i)->Data == event.GetClientData())
	{
	  g->SetPosition(wxPoint(event.GetInt() + g->GetPosition().x, 
				 -1));
	  for (j = Slices->begin(); j != Slices->end(); j++)
	    {
	      if (*j == *i)
		continue;
	      if ((*j)->Position > (*i)->Position)
		{
		  (*i)->EndPosition = (*j)->Position;
		  if (tmp)
		    tmp->EndPosition = (*i)->Position;
		  break;
		}
	      tmp = *i;
	    }	
	  tmp = Slices->back();
	  if (tmp)
	    tmp->EndPosition = (*i)->Position;
	  (*i)->EndPosition = EndWavePos;
	  
	  break;
	}
    }      
}

void WaveLoop::AddSlice(int x, int m_x)
{
  //wxStaticLine *l;
  list<Slice *>::iterator i;
  Slice *tmp = 0x0;
  SliceGui *g;

  Slice *s = new Slice(x, x * BarCoeff, SamplingRate);
  s->AffectMidi = NoteNumber++;

  //l = new wxStaticLine(this, -1, wxPoint(m_x, 0), wxSize(1, GetSize().y), wxLI_VERTICAL);
  //l->SetBackgroundColour(*wxBLACK);

  g = new SliceGui(this, SLICE_ID, wxPoint(m_x, 0), wxSize(1, GetSize().y));
  
  s->Data = (void *)g;
  
  for (i = Slices->begin(); i != Slices->end(); i++)
    {
      if ((*i)->Position > s->Position)
	{
	  s->EndPosition = (*i)->Position;
	  if (tmp)
	    tmp->EndPosition = s->Position;
	  Slices->insert(i, s);
	  return;
	}
      tmp = *i;
    }	
  tmp = Slices->back();
  if (tmp)
    tmp->EndPosition = s->Position;
  s->EndPosition = EndWavePos;
  Slices->push_back(s);	
}

void WaveLoop::OnClick(wxMouseEvent &event)
{
  int size_x;
  GetSize(&size_x, 0x0);
  long inc = (EndWavePos / size_x);
  long x = inc * event.m_x;
  list<Slice *>::iterator i;
  Slice *tmp = 0x0;

  if (Drawing)
    {
      Mutex->Lock();

      AddSlice(x, event.m_x);

      Mutex->Unlock();
    }
  else
    {
      bool done = false;
      tmp = *(Slices->begin());
      for (i = Slices->begin(); i != Slices->end(); i++)
	{
	  (*i)->Selected = false;
	  if (!done && ((*i)->Position > x))
	    {
	      tmp->Selected = true;
	      done = true;
	    }
	  tmp = *i;
	}	
      if (!done)
	tmp->Selected = true;
      wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
  Refresh();
}

void WaveLoop::OnSize(wxSizeEvent &event)
{
  list<Slice *>::iterator i;
  int size_x;
  GetSize(&size_x, 0x0);
  double inc = (double)size_x / (double)EndWavePos;
  SliceGui *s;
  int x, y;

  y = GetSize().y;
  WaveView::OnSize(event);
  if (!Slices)
    return;

  Mutex->Lock();

  for (i = Slices->begin(); i != Slices->end(); i++)
    {
      s = (SliceGui *)((*i)->Data);
      x = (int)(inc * (*i)->Position);
      s->SetPosition(wxPoint(x, -1));
      s->SetSize(wxSize(-1, y));
    }

  LoopStartCursor->SetPosition(wxPoint((int)(inc * LoopInfo->Start), 0)); 
  LoopEndCursor->SetPosition(wxPoint((int)(inc * LoopInfo->End), 0)); 

  Mutex->Unlock();
}

void WaveLoop::OnPaint(wxPaintEvent &event)
{
  if (!Wave)
    return;
  WaveView::OnPaint(event);
  wxPaintDC dc(this);

  dc.SetLogicalFunction(wxXOR);
  dc.SetBrush(CL_PATTERN_SEL);//GetPenColor());

  list<Slice *>::iterator i;
  Slice *s = 0x0;
  int x, size_x, size_y;
  double inc;

  GetSize(&size_x, &size_y);
  x = 0;
  inc = (1.0 / (double)(EndWavePos / size_x));

  for (i = Slices->begin(); i != Slices->end(); i++)
    {
      if ((*i)->Selected)
	{
	  x = (int)(inc * (*i)->Position);
	  dc.DrawRectangle(x, 0, (int)(inc * (*i)->EndPosition) - x, size_y);
	}
      s = *i;
    }
}

void WaveLoop::SetDrawing(bool draw)
{
  Drawing = draw;
  if (draw)
    SetCursor(wxCursor(wxCURSOR_PENCIL));
}

void WaveLoop::SetSelect(bool select)
{
  Select = select;
  if (select)
    SetCursor(wxCursor(wxCURSOR_ARROW));
}

void WaveLoop::SetSlices(list<Slice *> *slices)
{
  Slices = slices;  
  if (slices->empty())
    {
      Slice *s = new Slice(0, 0.0, SamplingRate);
      SliceGui *g;

      s->AffectMidi = NoteNumber++;
      s->EndPosition = EndWavePos;
  
      g = new SliceGui(this, SLICE_ID, wxPoint(0, 0), wxSize(1, GetSize().y));
    
      s->Data = (void *)g;
      Slices->push_back(s);
    }
  else
    {
      list<Slice *>::iterator i;
      long inc = (EndWavePos / GetSize().x);
      SliceGui *g;
	  
      for (i = Slices->begin(); i != Slices->end(); i++)
	{
	  g = new SliceGui(this, SLICE_ID, wxPoint((*i)->Position / inc, 0), 
			   wxSize(1, GetSize().y));

	  (*i)->Data = (void *)g; 
	}
    }
}

void WaveLoop::CreateSlices(int beats, int mescount)
{
  int size_x;
  GetSize(&size_x, 0x0);
  long inc = (EndWavePos / size_x);
  double x;
  double u = size_x / beats /  mescount;

  for (x = 0; (long) floor(x) < size_x; x += u)
    {
      AddSlice((long)floor(x) * inc, (long)floor(x));
    }
}
