// Copyright (C) 2004 by Wired Team
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
#include "Cursor.h"

/*
** Constructeurs d'un curseur :
*/

CursorH::CursorH(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		 const wxSize &size, Cursor *C, char name)
  : wxWindow(parent, id, pos, size, wxNO_BORDER)
{
  Name = wxString(name);
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
      //      printf("POS = %f, MAX SCROLL = %d, FIRST_MEASURE = %f, LAST MEASURE = %f\n", c->pos, max, SeqPanel->FirstMeasure, SeqPanel->LastMeasure);
      if (c->pos <= (SeqPanel->FirstMeasure))
	{
	  if ((x = (long) floor((c->pos * (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) / Seq->EndPos)) > 0)
	    SeqPanel->HorizScrollBar->SetThumbPosition(x);
	  else
	    SeqPanel->HorizScrollBar->SetThumbPosition(0);
	  SeqPanel->AdjustHScrolling();
	  //	  cout << "gauche [" << c->H->Name << "] pos = " << c->pos << endl;
	}
      else
	if (c->pos >= SeqPanel->LastMeasure)
	  {
	    if ((x = (long) floor((c->pos * (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) / Seq->EndPos))
		< (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH))
	      SeqPanel->HorizScrollBar->SetThumbPosition(x);
	    else
	      {
		Seq->EndPos = floor(c->pos + 1);
		if (Name != "E")
		  SeqPanel->EndCursor->SetPos(Seq->EndPos);
		SeqPanel->SetScrolling();
		SeqPanel->HorizScrollBar->SetThumbPosition(SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH);
	      }
	    SeqPanel->AdjustHScrolling();
	    //	    cout << "droite [" << c->H->Name << "] pos = " << c->pos << " [ x = " << x << " ] [ rangemax = " <<
	    //	      (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH) << " ] " << endl;
	  }
      SeqMutex.Unlock();
    }
}

Cursor::Cursor(char name, int id, double initpos, Ruler *R, SequencerGui *S,
	       wxColour cH, wxColour cL)
{
  wxSize				s;
  long					h;

  SeqGUI = S;
  pos = initpos;
  s = S->GetSize();
  h = R->GetClientSize().y;
  L = new ColoredLine(SeqGUI->SeqView, id, wxPoint(0, 0), wxSize(1, s.y), cL);
  H = new CursorH(R, id, wxPoint(0, h - CURSOR_HEIGHT), wxSize(CURSOR_WIDTH, CURSOR_HEIGHT), this, name);
  H->SetBackgroundColour(cH);
  SetPos(0.0);
}

Cursor::~Cursor()
{
  delete (H);
  delete (L);
}

void					Cursor::SetPos(double newpos)
{
  long					x;
  long					h;

  if (newpos < 0)
    newpos = 0;
  x = (long) floor((pos = newpos) * MEASURE_WIDTH * SeqGUI->HoriZoomFactor) - SeqGUI->SeqView->GetXScroll();
  h = SeqGUI->RulerPanel->GetClientSize().y;
  H->Move(x  - (CURSOR_WIDTH / 2) + 1, h - CURSOR_HEIGHT);
  L->Move(x, 0);
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

