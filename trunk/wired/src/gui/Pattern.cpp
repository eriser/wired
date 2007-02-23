// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include <wx/font.h>
#include "Colour.h"
#include "SequencerGui.h"
#include "OptionPanel.h"
#include "Cursor.h"
#include "Pattern.h"
#include "AudioPattern.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/Track.h"
#include "Ruler.h"
#include "AccelCenter.h"

Pattern::Pattern(WiredDocument *parent, wxString name, double pos, double endpos, long trackindex) :
  wxWindow(SeqPanel->SeqView, -1, wxPoint(0, 0), wxSize(0, 0)),
  WiredDocument(name, parent)
{
#ifdef __DEBUG__
  printf("Pattern::Pattern(%f, %f, %d)\n", pos, endpos, trackindex);
#endif
  Position = pos;
  EndPosition = endpos;
  Length = endpos - pos;
  TrackIndex = trackindex;
  StateMask = 0;
  m_pos = wxPoint((int) floor(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * pos), 
		  (int) floor(TRACK_HEIGHT * SeqPanel->VertZoomFactor * trackindex));
  m_size = wxSize((int) ceil(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor),
		  (int) ceil(TRACK_HEIGHT * SeqPanel->VertZoomFactor));
  wxWindow::SetSize(m_pos.x - (int) floor(SeqPanel->CurrentXScrollPos), m_pos.y - (int) floor(SeqPanel->CurrentYScrollPos), m_size.x, m_size.y);
  PenColor = CL_WAVE_DRAW;
  BrushColor = CL_SEQ_BACKGROUND;
  wxWindow::SetBackgroundStyle(wxBG_STYLE_CUSTOM);
#ifdef __DEBUG__
  printf(">> NEW PATTERN from pos %f to %f (length=%f) on TRACK %d\n", Position, EndPosition, Length, TrackIndex);
#endif

  // add itself in track's array
  wxMutexLocker		locker(SeqMutex);

  if (trackindex >= 0 && trackindex < Seq->Tracks.size() && Seq->Tracks[trackindex])
    Seq->Tracks[trackindex]->AddPattern(this);
  else
    cerr << "[Pattern] oooops, bad track index!" << endl;

}

Pattern::~Pattern()
{
  // remove itself in track's array
  wxMutexLocker		locker(SeqMutex);

  if (TrackIndex >= 0 && TrackIndex < Seq->Tracks.size() && Seq->Tracks[TrackIndex])
    Seq->Tracks[TrackIndex]->DelPattern(this);
}

void					Pattern::Modify(double newpos, double newendpos, 
							long newtrackindex,  double newlength)
{
#ifdef __DEBUG__
  printf("Pattern::Modify(%f, %f, %d, %f)\n", newpos, newendpos, newtrackindex, newlength);
#endif
  if (newpos != -1)
    {
      if (newpos < 0.0)
	newpos = 0.0;
      Position = newpos;
      EndPosition = Position + Length;
      m_pos.x = (int) floor(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * newpos);
    }
  if (newendpos != -1)
    {
      EndPosition = newendpos;
      Length = EndPosition - Position;
      m_size.x = (int) ceil(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
    }
  if (newlength != -1)
    {
      Length = newlength;
      EndPosition = Position + Length;
      m_size.x = (int) ceil(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
    }
  if (newtrackindex != -1)
    {
      TrackIndex = newtrackindex;
      m_pos.y = (int) floor(TRACK_HEIGHT * SeqPanel->VertZoomFactor * newtrackindex);
    }
#ifdef __DEBUG__
  printf("MOD PATTERN (pos=%f) (endpos=%f) (length=%f) (trackindex=%d)\n", Position, EndPosition, Length, TrackIndex);
#endif
}

void					Pattern::Update()
{
  //  printf("UPDATING PATTERN : BeginPosition = %f, Position = %f, EndPosition = %f, Length = %f\n",
  //  BeginPosition, Position, EndPosition, Length);
  m_pos = wxPoint((int) floor(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * Position), 
		  (int) floor(TRACK_HEIGHT * SeqPanel->VertZoomFactor * TrackIndex));
  m_size = wxSize((int) ceil(Length * MEASURE_WIDTH * SeqPanel->HoriZoomFactor),
		  (int) ceil(TRACK_HEIGHT * SeqPanel->VertZoomFactor));
  //  printf("UPDATING PATTERN : BeginPosition = %f, Position = %f, EndPosition = %f, Length = %f\n",
  // BeginPosition, Position, EndPosition, Length);
}

void					Pattern::UpdateMeasure()
{
  Position = (m_pos.x + SeqPanel->CurrentXScrollPos) / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
}

void					Pattern::SetSelected(bool sel)
{
  StateMask = (sel) ? StateMask | PATTERN_MASK_SELECTED :
    StateMask = StateMask & ~PATTERN_MASK_SELECTED;
  Refresh();
}

int					Pattern::GetXPos(double pos)
{
  return ((int) floor(MEASURE_WIDTH * SeqPanel->HoriZoomFactor * pos));
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
  m_pos.x = (int) floor((Position = newpos) * MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
  m_pos.y = (int) floor((TrackIndex = track) * TRACK_HEIGHT * SeqPanel->VertZoomFactor);
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
      if (!e.ShiftDown() || !(StateMask & PATTERN_MASK_SELECTED))
	{
	  StateMask |= (unsigned char) PATTERN_MASK_TOGGLED;
	  SeqPanel->SelectItem(this, e.ShiftDown());
	}
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

void					Pattern::OnLeftUp(wxMouseEvent &e)
{
  m_click.x = -1;
  m_click.y = -1;
  if ((SeqPanel->Tool == ID_TOOL_MOVE_SEQUENCER) && e.ShiftDown() &&
      !(StateMask & PATTERN_MASK_DRAGGED) && !(StateMask & PATTERN_MASK_TOGGLED))
    {
      StateMask &= PATTERN_MASK_SELECTED;
      SeqPanel->SelectItem(this, true);
    }
  StateMask &= ~PATTERN_MASK_DRAGGED & ~PATTERN_MASK_TOGGLED;
}

void					Pattern::OnDoubleClick(wxMouseEvent &e)
{
  OnClick(e);
}

void					Pattern::OnRightClick(wxMouseEvent &e)
{
  if (!(StateMask & PATTERN_MASK_SELECTED))
    OnClick(e);
  e.m_x += wxWindow::GetPosition().x;
  e.m_y += wxWindow::GetPosition().y;
  SeqPanel->SeqView->OnRightClick(e);
}

void					Pattern::XMove(double Motion)
{
  if ((Position = Position + Motion) >= 0)
    if (SeqPanel->Magnetism & PATTERN_MASK)
      Position = floor(Position * SeqPanel->PatternMagnetism) / SeqPanel->PatternMagnetism;
  Move(SetPos(Position, TrackIndex) - wxPoint((int) floor(SeqPanel->CurrentXScrollPos), (int) floor(SeqPanel->CurrentYScrollPos)));
}

void					Pattern::OnMotion(wxMouseEvent &e)
{
  vector<Pattern *>::iterator		p;
  long					x;
  long					y;
  long					max;
  double				z;
  int					trackto;
  double				mes;
  
 
  if (IsSelected() && (SeqPanel->Tool == ID_TOOL_MOVE_SEQUENCER) && e.Dragging() && (Seq->Tracks[TrackIndex]->GetAudioPattern() != this))
    {
      if (m_click.x != -1)
	StateMask |= (unsigned char) PATTERN_MASK_DRAGGED;
      SeqMutex.Lock();
      if ((x = (e.GetPosition().x - m_click.x)) > (max = (long) floor(PATTERN_DRAG_SCROLL_UNIT * SeqPanel->HoriZoomFactor)))
	  x = max;
      else if (x < -max)
	x = -max;
      if ((y = (e.GetPosition().y - m_click.y)) > (max = (long) floor(PATTERN_DRAG_SCROLL_UNIT * SeqPanel->VertZoomFactor)))
	y = max;
      else if (y < -max)
	y = -max;
      if ((z = x / (mes = MEASURE_WIDTH * SeqPanel->HoriZoomFactor)) != 0)
	{
	  if (SeqPanel->SelectedItems.size() == 1)
	    {
	      trackto = TrackIndex + floor((e.GetPosition().y ) / (TRACK_HEIGHT * SeqPanel->VertZoomFactor));
	      if (SeqPanel->IsAudioTrack(TrackIndex) == SeqPanel->IsAudioTrack(trackto))
		{
		  SeqPanel->ChangeMouseCursor(wxCursor(wxCURSOR_HAND));
		  SeqPanel->MovePattern(this, TrackIndex, trackto);
		  TrackIndex = trackto;
		  XMove(z);
		}
	      else
		{
		  SeqPanel->ChangeMouseCursor(wxCursor(wxCURSOR_NO_ENTRY));
		  SeqMutex.Unlock();
		  return;
		}
	    }
	  else
	    {
	      for (p = SeqPanel->SelectedItems.begin(); p != SeqPanel->SelectedItems.end() && (z < 0); p++)
		if ((*p)->GetPosition() < -z)
		  z = -(*p)->GetPosition();
	      for (p = SeqPanel->SelectedItems.begin(); p != SeqPanel->SelectedItems.end() && (z != 0); p++)
		(*p)->XMove(z);
	    }
	  if ((m_pos.x + e.GetPosition().x - PATTERN_AUTO_SCROLL_BOX) < (long) floor(SeqPanel->CurrentXScrollPos))
	    SeqPanel->SeqView->AutoXScrollBackward(ACCEL_TYPE_PATTERN);
	  else
	    if ((m_pos.x + e.GetPosition().x + PATTERN_AUTO_SCROLL_BOX) > ((long) floor(SeqPanel->CurrentXScrollPos) + SeqPanel->SeqView->GetClientSize().x))
	      SeqPanel->SeqView->AutoXScrollForward(ACCEL_TYPE_PATTERN);
	    else
	      SeqPanel->SeqView->AutoXScrollReset();
	}
      SeqMutex.Unlock();
    }

//   else
//     if (SeqPanel->Tool == ID_TOOL_SPLIT_SEQUENCER)
//       SeqPanel->RulerPanel->MoveXMark(e.GetPosition().x + GetXPos(Position));
}

void					Pattern::DrawName(wxPaintDC &dc, const wxSize &s)
{
  long					x;
  long					y;
  long					w;
  long					h;

  dc.SetPen(PenColor);
  dc.SetBrush(CL_PATTERN_NAME_BRUSH);
  dc.SetTextForeground(CL_PATTERN_NAME);
  dc.SetFont(wxFont(PATTERN_NAME_HEIGHT - 2 * PATTERN_NAME_OFFSET, wxDEFAULT, wxNORMAL, wxNORMAL));
  dc.GetTextExtent(Name, &w, &h);
  //dc.SetBrush(*wxTRANSPARENT_BRUSH);
//   dc.DrawRectangle((x = s.x - (PATTERN_NAME_MARGINS + w + 4 * PATTERN_NAME_OFFSET)),
// 		   (y = s.y - (PATTERN_NAME_MARGINS + h + 2 * PATTERN_NAME_OFFSET)),
// 		   w + 4 * PATTERN_NAME_OFFSET, h + 2 * PATTERN_NAME_OFFSET);
//   dc.DrawText(Name, x + 2 * PATTERN_NAME_OFFSET, y + PATTERN_NAME_OFFSET);
  if ((((x = w + 4 * PATTERN_NAME_OFFSET) + 2 * PATTERN_NAME_MARGINS) <= s.x) && (h <= s.y))
    {
      dc.DrawRectangle(PATTERN_NAME_MARGINS,
		       (y = s.y - (PATTERN_NAME_MARGINS + h + 2 * PATTERN_NAME_OFFSET)),
		       x, h + 2 * PATTERN_NAME_OFFSET);
      dc.DrawText(Name, PATTERN_NAME_MARGINS + 2 * PATTERN_NAME_OFFSET, y + PATTERN_NAME_OFFSET);
    }
}
double					Pattern::GetEndPos()
{
  return EndPosition;
}

void				Pattern::Save()
{
  SaveElement*			saved;

  saveDocData(new SaveElement(wxT("Name"), Name));
  saveDocData(new SaveElement(wxT("TrackIndex"), (int)TrackIndex));
  saveDocData(new SaveElement(wxT("Position"), Position));
  saveDocData(new SaveElement(wxT("EndPosition"), EndPosition));
  saveDocData(new SaveElement(wxT("Length"), Length));
  saveDocData(new SaveElement(wxT("StateMask"), (int)StateMask));

  saved = new SaveElement();
  saved->setKey(wxT("PenColor"));
  saved->addAttribute(wxT("red"), (int)PenColor.Red());
  saved->addAttribute(wxT("green"), (int)PenColor.Green());
  saved->addAttribute(wxT("blue"), (int)PenColor.Blue());
  saveDocData(saved);

  saved = new SaveElement();
  saved->setKey(wxT("BrushColor"));
  saved->addAttribute(wxT("red"), (int)BrushColor.Red());
  saved->addAttribute(wxT("green"), (int)BrushColor.Green());
  saved->addAttribute(wxT("blue"), (int)BrushColor.Blue());
  saveDocData(saved);
}

void				Pattern::Load(SaveElementArray data)
{
  int				i;
  double			pos = -1.f;
  double			endpos = -1.f;
  double			length = -1.f;

  for (i = 0; i < data.GetCount(); i++)
    {
      if (data[i]->getKey() == wxT("Name"))
	Name = data[i]->getValue();
      else if (data[i]->getKey() == wxT("TrackIndex"))
        TrackIndex = data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("Position"))
        pos = data[i]->getValueDouble();
      else if (data[i]->getKey() == wxT("EndPosition"))
	endpos = data[i]->getValueDouble();
      else if (data[i]->getKey() == wxT("Length"))
	length = data[i]->getValueDouble();
      else if (data[i]->getKey() == wxT("StateMask"))
	StateMask = data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("PenColor"))
	{
	  unsigned char		red = data[i]->getAttributeInt(wxT("red"));
	  unsigned char		green = data[i]->getAttributeInt(wxT("green"));
	  unsigned char		blue = data[i]->getAttributeInt(wxT("blue"));
	  SetDrawColour(wxColour(red, green, blue));
	}
      else if (data[i]->getKey() == wxT("BrushColor"))
	{
	  unsigned char		red = data[i]->getAttributeInt(wxT("red"));
	  unsigned char		green = data[i]->getAttributeInt(wxT("green"));
	  unsigned char		blue = data[i]->getAttributeInt(wxT("blue"));
	  BrushColor = wxColour(red, green, blue);
	}
    }
  // modify update internal vars like m_pos, m_size, ...
  Modify(pos, endpos, TrackIndex, length);
  Update();
}

void			Pattern::Dump()
{
  std::cerr << "PatternDump()" << std::endl;
  std::cerr << "Position = " << Position << std::endl;
  std::cerr << "EndPosition = " << EndPosition << std::endl;
  std::cerr << "Length = " << Length << std::endl;
  std::cerr << "TrackIndex = " << TrackIndex << std::endl;
  std::cerr << "StateMask = " << StateMask << std::endl;
  std::cerr << "m_pos.x = " << m_pos.x << std::endl;
  std::cerr << "m_pos.y = " << m_pos.y << std::endl;
  std::cerr << "m_size.GetWidth() = " << m_size.GetWidth() << std::endl;
  std::cerr << "m_size.GetHeighy() = " << m_size.GetHeight() << std::endl;
  std::cerr << "m_click.x = " << m_click.x << std::endl;
  std::cerr << "m_click.y = " << m_click.y << std::endl;
  std::cerr << "Name.mb_str() = " << Name.mb_str() << std::endl;
}
