// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "Pattern.h"
#include "Colour.h"
#include "SequencerGui.h"
#include "OptionPanel.h"
#include <math.h>

Pattern::Pattern(double pos, double endpos, long trackindex) :
  wxWindow(SeqPanel->SeqView, -1, wxPoint(0, 0), wxSize(0, 0))
{
  //printf("Pattern::Pattern(%f, %f, %d)\n", pos, endpos, trackindex);
  Position = pos;
  EndPosition = endpos;
  Length = endpos - pos;
  TrackIndex = trackindex;
  Selected = false;
  wxWindow::Move((m_pos = wxPoint((int)(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * pos - SeqPanel->CurrentXScrollPos), 
			 (int)(TRACK_HEIGHT * SeqPanel->VertZoomFactor * trackindex - SeqPanel->CurrentYScrollPos))));
  wxWindow::SetSize((m_size = wxSize((int)(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor),
		  (int)(TRACK_HEIGHT * SeqPanel->VertZoomFactor))));
  PenColor = CL_WAVE_DRAW;
  BrushColor = CL_SEQ_BACKGROUND;
  //printf("NEW PATTERN from pos %f to %f (length=%f) on TRACK %d\n", Position, EndPosition, Length, TrackIndex);
}

Pattern::~Pattern()
{

}

void					Pattern::Modify(double newpos, double newendpos, 
							long newtrackindex,  double newlength)
{
  //printf("Pattern::Modify(%f, %f, %d, %f)\n", newpos, newendpos, newtrackindex, newlength);
  if (newpos != -1)
    {
      if (newpos < 0.0)
	newpos = 0.0;
      Position = newpos;
      EndPosition = Position + Length;
      m_pos.x = (int) (floor(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * newpos)
		       - SeqPanel->CurrentXScrollPos);
    }
  if (newendpos != -1)
    {
      EndPosition = newendpos;
      Length = EndPosition - Position;
      m_size.x = (int)(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
    }
  if (newlength != -1)
    {
      Length = newlength;
      EndPosition = Position + Length;
      m_size.x = (int)(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
    }
  if (newtrackindex != -1)
    {
      TrackIndex = newtrackindex;
      m_pos.y = (int) floor(TRACK_HEIGHT * SeqPanel->VertZoomFactor * newtrackindex
		      - SeqPanel->CurrentYScrollPos);
    }
  //  printf("MOD PATTERN (pos=%f) (endpos=%f) (length=%f) (trackindex=%d)\n", Position, EndPosition, Length, TrackIndex);
}

void					Pattern::Update()
{
  //  printf("UPDATING PATTERN : BeginPosition = %f, Position = %f, EndPosition = %f, Length = %f\n",
  //  BeginPosition, Position, EndPosition, Length);
  m_pos = wxPoint((int)(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * Position 
			- SeqPanel->CurrentXScrollPos), 
		  (int)(TRACK_HEIGHT * SeqPanel->VertZoomFactor * TrackIndex 
			- SeqPanel->CurrentYScrollPos));
  m_size = wxSize((int)(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor),
		  (int)(TRACK_HEIGHT * SeqPanel->VertZoomFactor));
//  printf("UPDATING PATTERN : BeginPosition = %f, Position = %f, EndPosition = %f, Length = %f\n",
  // BeginPosition, Position, EndPosition, Length);
}

void					Pattern::UpdateMeasure()
{
  Position = (m_pos.x + SeqPanel->CurrentXScrollPos) / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
}

void					Pattern::SetSelected(bool sel)
{
  Selected = sel;
  Refresh();
}

int					Pattern::GetXPos(double pos)
{
  return ((int)(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * pos));
}

/*
** A utiliser depuis une classe graphique avec un
** Move(SetPos(newpos, track));
*/

wxPoint					Pattern::SetPos(double newpos, long track)
{
  /*  printf("moving pattern at pos %f on track %d\n", newpos, track);*/
  if (newpos < 0)
    newpos = 0;
  m_pos.x = (long) floor((Position = newpos) * MEASURE_WIDTH * SeqPanel->HoriZoomFactor) - SeqPanel->SeqView->GetXScroll();
  m_pos.y = (long) floor((TrackIndex = track) * TRACK_HEIGHT * SeqPanel->VertZoomFactor) - SeqPanel->SeqView->GetYScroll();
  EndPosition = Position + Length;
  return (m_pos);
}

void					Pattern::OnClick(wxMouseEvent &e)
{
  if (SeqPanel->Tool == ID_TOOL_MOVE_SEQUENCER)
    {
      m_click.x = e.m_x;
      m_click.y = e.m_y;
#ifdef __DEBUG__
      printf("clik at (x = %d) (y = %d)\n", m_click.x, m_click.y);
#endif
      SeqPanel->SelectItem(this, e.ShiftDown());
    }
  else
    if (SeqPanel->Tool == ID_TOOL_DELETE_SEQUENCER)
      SeqPanel->DeletePattern(this); 
  /*    else
      if (SeqPanel->Tool == ID_TOOL_SPLIT_SEQUENCER)
	{
	}
  */
}

void					Pattern::OnDoubleClick(wxMouseEvent &e)
{
  OnClick(e);
}

void					Pattern::OnRightClick(wxMouseEvent &e)
{
  if (!Selected)
    OnClick(e);
  e.m_x += wxWindow::GetPosition().x;
  e.m_y += wxWindow::GetPosition().y;
  SeqPanel->SeqView->OnRightClick(e);
}

void					Pattern::OnMotion(wxMouseEvent &e)
{
  
  long					x;
  long					y;
  long					max;
  double				z;
  double				mes;

  if (e.Dragging() && (Seq->Tracks[TrackIndex]->Wave != this))
    {
      SeqMutex.Lock();
      if ((x = (e.GetPosition().x - m_click.x)) > (max = (long) floor(PATTERN_DRAG_SCROLL_UNIT * SeqPanel->HoriZoomFactor)))
	x = max;
      else
	if (x < -max)
	  x = -max;
      if ((y = (e.GetPosition().y - m_click.y)) > (max = (long) floor(PATTERN_DRAG_SCROLL_UNIT * SeqPanel->VertZoomFactor)))
	y = max;
      else
	if (y < -max)
	  y = -max;
      //printf("(X = %d) (Y = %d)\n", x, y);
      if ((Position = Position + (x / (mes = MEASURE_WIDTH * SeqPanel->HoriZoomFactor))) > 0)
	if (SeqPanel->Magnetism & PATTERN_MASK)
	  Position = round(Position * SeqPanel->PatternMagnetism) / SeqPanel->PatternMagnetism;
      Move(SetPos(Position, TrackIndex));
      
      if ((x < 0) && (Position <= SeqPanel->FirstMeasure))
	{
// 	  if ((x = (long) floor(((Position / SeqPanel->PatternMagnetism) * (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) / Seq->EndPos)) > 0)
// 	    SeqPanel->HorizScrollBar->SetThumbPosition(x);
	  if ((x = (long) floor((Position * (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) / Seq->EndPos)) > 0)
	    SeqPanel->HorizScrollBar->SetThumbPosition(x);
	  else
	    SeqPanel->HorizScrollBar->SetThumbPosition(0);
	  SeqPanel->AdjustHScrolling();
	  //printf("<<<<< gauche <<<<< : x = %d\n", x);
	}
      else
	if ((x > 0) && (Position + ceil(m_click.x / mes) >= SeqPanel->LastMeasure))
	  {
	    if ((x = (long) floor(((z = Position + ceil(m_size.x / mes))
				   * (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) / Seq->EndPos))
		< (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH))
	      SeqPanel->HorizScrollBar->SetThumbPosition(x);
	    else
	      {
		Seq->EndPos = floor(z + 1);
		SeqPanel->EndCursor->SetPos(Seq->EndPos);
		SeqPanel->SetScrolling();
		SeqPanel->HorizScrollBar->SetThumbPosition(SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH);
	      }
	    SeqPanel->AdjustHScrolling();
	    //printf(">>>>> droite >>>>> : x = %d\n", x);
	  }
      	else
	  {
	    //printf("[[[[[ nothing ]]]]] endpos = %f -- pattern_start = %f -- pattern_end = %f\n",
	    // Seq->EndPos, Position, Position + (double) m_size.x / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor));
	  }
      
      SeqMutex.Unlock();
    }
  
}
