// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include <wx/window.h>
#include <wx/statline.h>
#include <wx/gdicmn.h>
#include <math.h>
#include "Colour.h"
#include "Sequencer.h"
#include "Ruler.h"
#include "ColoredLine.h"
#include "SequencerGui.h"
#include "AccelCenter.h"
#include "Cursor.h"

/*
** Constructeurs d'un curseur :
*/

CursorH::CursorH(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		 const wxSize &size, Cursor *C, const char name)
  : wxWindow(parent, id, pos, size, wxNO_BORDER)
{
  Name = (wxChar)name;
  c = C;
  /*
  Connect(c->L->GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &CursorH::OnMouseEvent);
  Connect(c->L->GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &CursorH::OnClick);
  Connect(c->L->GetId(), wxEVT_LEFT_UP, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &CursorH::OnClickUp);
  */
}

CursorH::~CursorH()
{

}

/*
** Fonctions gerant le drag d'un curseur.
*/

void					CursorH::OnClick(wxMouseEvent &e)
{
  m_click.x = e.m_x;
  m_click.y = e.m_y;
}

/*
** Lorsque l'utilisateur relache le bouton gauche de la souris, on envoit un evenement, afin 
** de par exemple set la nouvelle position dans le sequenceur
*/

void					CursorH::OnClickUp(wxMouseEvent &e)
{
  wxCommandEvent _event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  _event.SetEventObject(this);
  GetEventHandler()->ProcessEvent(_event);
}

void					CursorH::OnPaint(wxPaintEvent &e)
{
  wxPaintDC				dc(this);
  
  dc.SetFont(wxFont(CURSOR_NAME_HEIGHT, wxDEFAULT, wxNORMAL, wxNORMAL));
  dc.SetTextForeground(CL_CURSORZ_HEAD_NAME);
  dc.DrawText(Name, CURSOR_NAME_XOFFSET, CURSOR_NAME_YOFFSET);
}

void					CursorH::OnMouseEvent(wxMouseEvent &e)
{
  long					x;
  long					max;

  if(e.Dragging())
    {
      SeqMutex.Lock();
      if ((x = (e.GetPosition().x - m_click.x)) > (max = (long) floor(CURSOR_DRAG_SCROLL_UNIT * SeqPanel->HoriZoomFactor)))
	x = max;
      else
	if (x <-max)
	  x = -max;
      if ((c->pos = c->pos + (x / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor))) > 0)
	if (SeqPanel->Magnetism & CURSOR_MASK)
	  c->pos = round(c->pos * SeqPanel->CursorMagnetism) / SeqPanel->CursorMagnetism;
      c->SetPos(c->pos);
      if (c->pos <= SeqPanel->FirstMeasure)
	SeqPanel->SeqView->AutoXScrollBackward(ACCEL_TYPE_CURSOR);
      else
	if (c->pos >= SeqPanel->LastMeasure)
	  SeqPanel->SeqView->AutoXScrollForward(ACCEL_TYPE_CURSOR);
	else
	  SeqPanel->SeqView->AutoXScrollReset();
      SeqMutex.Unlock();
    }
}

Cursor::Cursor(const char name, const int id, const double initpos, Ruler *R, SequencerGui *S,
	       const wxColour &cH, const wxColour &cL)
{
  wxSize				s;
  long					h;

  pos = initpos;
  SeqGUI = S;
  s = S->GetSize();
  h = R->GetClientSize().y;
  L = new ColoredLine(SeqGUI->SeqView, id, wxPoint(0, 0), wxSize(1, s.y), cL);
  H = new CursorH(R, id, wxPoint(-(CURSOR_WIDTH / 2) + 1, h - CURSOR_HEIGHT), wxSize(CURSOR_WIDTH, CURSOR_HEIGHT), this, name);
  H->SetBackgroundColour(cH);
  Xpos = 0;
  SetPos(initpos);
}

Cursor::~Cursor()
{
  delete (H);
  delete (L);
}

void					Cursor::SetPos(double newpos)
{
  long					x;

  if (newpos < 0)
    newpos = 0;
  if (!(Xpos == (x = (long) floor((pos = newpos) * MEASURE_WIDTH * SeqGUI->HoriZoomFactor) - SeqGUI->SeqView->GetXScroll())))
    {
      H->SetSize((Xpos = x) - (CURSOR_WIDTH / 2) + 1, -1, -1, -1);
      L->SetSize(x, -1, -1, -1, wxSIZE_USE_EXISTING);
    }
}

double					Cursor::GetPos()
{
  return (pos);
}

void					Cursor::PutOnTop()
{
  L->Show(false);
  L->Show(true);
}

void					Cursor::ReSize()
{
#ifdef __DEBUG__
  printf("Cursor::ReSize()\n");
#endif
  L->SetSize(-1, -SeqGUI->SeqView->GetYScroll(), -1, (int) SeqGUI->SeqView->GetTotalHeight(), wxSIZE_USE_EXISTING);
}

BEGIN_EVENT_TABLE(CursorH, wxWindow)
  EVT_MOTION(CursorH::OnMouseEvent) 
  EVT_LEFT_DOWN(CursorH::OnClick)
  EVT_LEFT_UP(CursorH::OnClickUp)
  EVT_PAINT(CursorH::OnPaint)
END_EVENT_TABLE()

