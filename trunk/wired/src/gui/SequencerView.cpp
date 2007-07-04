// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "SequencerView.h"

#include "Colour.h"

// use extern of
#include "HelpPanel.h"
#include "Sequencer.h"
#include "AudioCenter.h"
#include "FileConversion.h"

extern FileConversion		*FileConverter;
SequencerView::SequencerView(SequencerGui *parent, const wxPoint &pos,
			     const wxSize &size)
  : wxWindow(parent, -1, pos, size, wxSUNKEN_BORDER),
    WiredDocument(wxT("SequencerView"), parent)
{
  XScroll = 0;
  YScroll = 0;
  TotalWidth = 0;
  TotalHeight = 0;

  TheZone = new SelectionZone(this);
  HAxl = new AccelCenter(ACCEL_TYPE_DEFAULT);
  VAxl = new AccelCenter(ACCEL_TYPE_DEFAULT);
  wxWindow::SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

SequencerView::~SequencerView()
{
  delete (TheZone);
  delete (HAxl);
  delete (VAxl);
}

void					SequencerView::OnClick(wxMouseEvent &e)
{
  SeqPanel->SelectItem(0x0, e.ShiftDown());
  if (!TheZone->IsVisible())
    TheZone->SetZone(e.m_x, e.m_y, 2, 2);
}

void					SequencerView::OnMotion(wxMouseEvent &e)
{
  wxCursor				hand(wxCURSOR_HAND);

#if wxCHECK_VERSION(2, 8, 0)
  if (!SeqPanel->GetCursor().IsSameAs(hand))
#else
  if (SeqPanel->GetCursor() != hand)
#endif
    SeqPanel->ChangeMouseCursor(hand);
  if (e.Dragging())
    {
      if (TheZone->IsVisible())
	{
	  TheZone->UpdateZone(e.m_x, e.m_y);
	  SelectZonePatterns(e.ShiftDown());
	}
    }
}

void					SequencerView::SelectZonePatterns(bool shift)
{
  vector<Track *>::iterator		t;
  vector<Pattern *>::iterator		p;
  vector<Pattern *>::iterator		i;
  double				x1;
  double				x2;
  unsigned long				y1;
  unsigned long				y2;

  if (!TheZone->IsXReversed())
    {
      x1 = (x2 = XScroll + TheZone->GetZoneX()) / (double) (MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
      x2 = (x2 + TheZone->GetZoneW()) / (double) (MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
    }
  else
    {
      x2 = (x1 = XScroll + TheZone->GetZoneX()) / (double) (MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
      x1 = (x1 - TheZone->GetZoneW()) / (double) (MEASURE_WIDTH * SeqPanel->HoriZoomFactor);
    }
  if (!TheZone->IsYReversed())
    {
      y1 = YScroll + TheZone->GetZoneY();
      y2 = y1 + TheZone->GetZoneH();
    }
  else
    {
      y2 = YScroll + TheZone->GetZoneY();
      y1 = y2 - TheZone->GetZoneH();
    }
  for (t = Seq->Tracks.begin(); t != Seq->Tracks.end(); t++)
    for (p = (*t)->GetTrackPattern()->Patterns.begin(); p != (*t)->GetTrackPattern()->Patterns.end(); p++)
      if ((((*p)->GetPosition() > x2) || ((*p)->GetEndPosition() < x1)) ||
	  ((((*p)->GetTrackIndex() * TRACK_HEIGHT * SeqPanel->VertZoomFactor) > y2) ||
	   ((((*p)->GetTrackIndex() + 1) * TRACK_HEIGHT * SeqPanel->VertZoomFactor) < y1)))
	{
	  if (!shift && (*p)->IsSelected())
	    {
	      (*p)->SetSelected(false);
	      for (i = SeqPanel->SelectedItems.begin(); (i != SeqPanel->SelectedItems.end()) && (*i != *p); i++);
	      SeqPanel->SelectedItems.erase(i);
	    }
	}
      else
	if (!(*p)->IsSelected())
	  {
	    (*p)->SetSelected(true);
	    SeqPanel->SelectedItems.push_back(*p);
	  }
}

void					SequencerView::OnLeftUp(wxMouseEvent &e)
{
  TheZone->Hide();
}

void					SequencerView::OnRightClick(wxMouseEvent &event)
{
  SeqPanel->ShowPopup(event.GetPosition() + GetPosition());
}

void					SequencerView::OnPaint(wxPaintEvent &event)
{
  wxPaintDC				dc(this);

  PrepareDC(dc);
  DrawMeasures(dc);
  DrawTrackLines(dc);
}

void					SequencerView::AutoScroll(double xmove, double ymove)
{
  if (xmove)
    AutoXScroll(xmove);
  if (ymove)
    AutoYScroll(ymove);
}

void					SequencerView::AutoXScroll(double xmove)
{
}

void					SequencerView::AutoXScrollBackward(long accel_type)
{
  long					x;

  HAxl->SetAccelType(accel_type);
  SeqPanel->HorizScrollBar->SetThumbPosition(((x = SeqPanel->HorizScrollBar->GetThumbPosition()
					       - (long) floor(HAxl->ForwardAccel())) < 0) ? 0 : x);
  SeqPanel->AdjustHScrolling();
}

void					SequencerView::AutoXScrollForward(long accel_type)
{
  long					x;

  HAxl->SetAccelType(accel_type);
  if ((x = (long) floor(HAxl->ForwardAccel()) + SeqPanel->HorizScrollBar->GetThumbPosition())
      < (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH))
    SeqPanel->HorizScrollBar->SetThumbPosition(x);
  else
    {
      Seq->EndPos += floor(HAxl->GetValue()) * Seq->EndPos / (double) SeqPanel->HorizScrollBar->GetRange();
      SeqPanel->EndCursor->SetPos(Seq->EndPos);
      SeqPanel->SetScrolling();
      SeqPanel->HorizScrollBar->SetThumbPosition(SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH);
    }
  SeqPanel->AdjustHScrolling();
}

void					SequencerView::AutoXScrollReset()
{
  HAxl->Reset();
}

void					SequencerView::AutoYScroll(double ymove)
{
}

void					SequencerView::AutoYScrollBackward(long accel_type)
{
}

void					SequencerView::AutoYScrollForward(long accel_type)
{
}

void					SequencerView::AutoYScrollReset()
{
  VAxl->Reset();
}
void					SequencerView::SetXScrollValue(long X)
{
  if (X >= 0)
    XScroll = X;
}

long					SequencerView::GetXScroll()
{
  return (XScroll);
}

void					SequencerView::SetXScroll(long x, long range, long seqwidth)
{
  if (!range || ((XScroll = (long) floor(((Seq->EndPos * MEASURE_WIDTH * SeqPanel->HoriZoomFactor
					   - seqwidth) * x) / range)) < 0))
    XScroll = 0;
}

void					SequencerView::SetYScrollValue(long Y)
{
  if (Y >= 0)
    YScroll = Y;
}

long					SequencerView::GetYScroll()
{
  return (YScroll);
}

void					SequencerView::SetYScroll(long y, long range, long seqwidth)
{
  if (!range || ((YScroll = (long) floor(((Seq->Tracks.size() * TRACK_HEIGHT * SeqPanel->VertZoomFactor
					   - GetClientSize().y) * y) / range)) < 0))
    YScroll = 0;
}

unsigned long				SequencerView::GetTotalWidth()
{
  return (TotalWidth);
}

void					SequencerView::SetTotalWidth(unsigned long w)
{
  TotalWidth = (w > GetClientSize().x) ? w : GetClientSize().x;
}

unsigned long				SequencerView::GetTotalHeight()
{
  return (TotalHeight);
}

void					SequencerView::SetTotalHeight(unsigned long h)
{
  // Adding space at the end to be able to add a new Track by DnD
  h += VSCROLL_THUMB_WIDTH;
  TotalHeight = (h > GetClientSize().y) ? h : GetClientSize().y;
}

void					SequencerView::OnHelp(wxMouseEvent &event)
{
  if (HelpWin == NULL)
      return;
  if (HelpWin->IsShown())
    {
      wxString s(_("This is the Wired sequencer. You can add here Audio or MIDI tracks, which can be output to plugins. Use the toolbar above to choose one of the sequencer editing tools."));
      HelpWin->SetText(s);
    }
}

void					SequencerView::DrawMeasures(wxDC &dc)
{
  wxSize				size;
  wxString				s;
  double				x;
  double				u = 1;
  long					m;

  size = GetClientSize();
  dc.SetPen(wxPen(CL_SEQVIEW_BACKGROUND, 1, wxSOLID));
  dc.SetBrush(wxBrush(CL_SEQVIEW_BACKGROUND));
  dc.SetTextForeground(CL_SEQVIEW_FOREGROUND);
  dc.DrawRectangle(0, 0, size.x, size.y);
  dc.SetPen(wxPen(CL_SEQVIEW_BAR, 1, wxSOLID));
  if (Seq->SigNumerator != 0)
    u = MEASURE_WIDTH * SeqPanel->HoriZoomFactor / Seq->SigNumerator;
  m = (long) ceil(XScroll / u);
  for (x = u * m - XScroll; (long) floor(x) < size.x; x += u)
    {
      if (Seq->SigNumerator != 0 && !(m++ % Seq->SigNumerator))
        {
	  dc.SetPen(wxPen(CL_SEQVIEW_MES, 1, wxSOLID));
	  dc.DrawLine((int) floor(x), 0,
		      (int) floor(x), size.y);
	  dc.SetPen(wxPen(CL_SEQVIEW_BAR, 1, wxSOLID));
	  }
      else
	dc.DrawLine((int) floor(x), 0,
		    (int) floor(x), size.y);
    }
}

void					SequencerView::Drop(int x, int y, wxString file)
{
  int					track;
  vector<Track *>::iterator		i;
  vector<Pattern *>::iterator		pattern_iterator;
  int					cpt;
  double       				last_pos = 0;
  Track					*track_to_add;
  WaveFile				*wave;
  WaveFile				*wave_tmp;
  long					nb_channel;
  double				x_seq;

  ScreenToClient(&x, &y);
  if (x >= 0 && y >= 0)
    {
      wxString				convertme;

      // Getting aware of current vertical scrolling position
      y += SeqPanel->GetCurrentYScrollPos();
      x += SeqPanel->GetCurrentXScrollPos();
      x_seq = (x == 0 ? 0 : (double)x / (double)SeqPanel->HoriZoomFactor / HORIZ_SEQ_RATIO);
      convertme << floor((y  * SeqPanel->VertZoomFactor) / TRACK_HEIGHT);
      convertme.ToLong((long*)&track);
      for (i = Seq->Tracks.begin(), cpt = 0; i != Seq->Tracks.end() && cpt != track; i++, cpt++)
	;
      if (Seq->Tracks.size() != 0 && track < Seq->Tracks.size() && (*i)->IsAudioTrack())
	{
	  wave_tmp = WaveCenter.AddWaveFile(file);
	  for (pattern_iterator = (*i)->GetTrackPattern()->Patterns.begin();
	       pattern_iterator != (*i)->GetTrackPattern()->Patterns.end();
	       pattern_iterator++)
	    if (last_pos < (*pattern_iterator)->GetEndPosition())
	      last_pos = (*pattern_iterator)->GetEndPosition();
	  std::cerr << "wave_tmp->GetNumberOfChannels()" << wave_tmp->GetNumberOfChannels() << std::endl;
	  for (nb_channel = 0; nb_channel < wave_tmp->GetNumberOfChannels() && i != Seq->Tracks.end(); nb_channel++)
	    {
	      std::cerr << "nb_channel = " << nb_channel << std::endl;
	      wave = WaveCenter.AddWaveFile(file);
	      wave->SetChannelToRead(nb_channel);
	      // should we put the new Pattern next to the last one or where the mouse cursor is ?
	      //(*i)->CreateAudioPattern(wave, last_pos);
	      (*i)->CreateAudioPattern(wave, x_seq);
	      i++;
	    }
	  for (;nb_channel < wave_tmp->GetNumberOfChannels(); nb_channel++)
	    {
	      std::cerr << "nb_channel = " << nb_channel << std::endl;
	      track_to_add = SeqPanel->CreateTrack(eAudioTrack);
	      wave = WaveCenter.AddWaveFile(file);
	      wave->SetChannelToRead(nb_channel);
	      track_to_add->CreateAudioPattern(wave, x_seq);
	    }
	}
      else
	{
	  wave_tmp = WaveCenter.AddWaveFile(file);
	  for (nb_channel = 0; nb_channel < wave_tmp->GetNumberOfChannels(); nb_channel++)
	  {
 	    track_to_add = SeqPanel->CreateTrack(eAudioTrack);
	    wave = WaveCenter.AddWaveFile(file);
	    wave->SetChannelToRead(nb_channel);
 	    track_to_add->CreateAudioPattern(wave, x_seq);
	  }
	}
      WaveCenter.RemoveWaveFile(wave_tmp);
    }
}

void					SequencerView::DrawTrackLines(wxDC &dc)
{
  vector<Track *>::iterator		i;
  long					h;

  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
    if ((*i)->GetTrackPattern())
      {
	if ((h = (*i)->GetTrackOpt()->GetPosition().y + (*i)->GetTrackOpt()->GetSize().y) > 0)
	  {
	    dc.SetPen(wxPen(CL_SEQVIEW_TRACK_LINE_TOP, 1, wxSOLID));
	    dc.DrawLine(0, h - 1, GetClientSize().x + 1, h - 1);
	  }
	dc.SetPen(wxPen(CL_SEQVIEW_TRACK_LINE, 1, wxSOLID));
	dc.DrawLine(0, h, GetClientSize().x + 1, h);
	if (h < TotalHeight - 1)
	  {
	    dc.SetPen(wxPen(CL_SEQVIEW_TRACK_LINE_BOTTOM, 1, wxSOLID));
	    dc.DrawLine(0, h + 1, GetClientSize().x + 1, h + 1);
	  }
      }
}

void				SequencerView::Save()
{
  saveDocData(new SaveElement(wxT("TotalWidth"), (int)TotalWidth));
  saveDocData(new SaveElement(wxT("TotalHeight"), (int)TotalHeight));
  saveDocData(new SaveElement(wxT("XScroll"), (int)XScroll));
  saveDocData(new SaveElement(wxT("YScroll"), (int)YScroll));
}

void				SequencerView::Load(SaveElementArray data)
{
  int				i;

  for (i = 0; i < data.GetCount(); i++)
    {
      if (data[i]->getKey() == wxT("TotalWidth"))
	TotalWidth = data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("TotalHeight"))
	TotalHeight = data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("XScroll"))
	XScroll = data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("YScroll"))
	YScroll = data[i]->getValueInt();
    }
}

BEGIN_EVENT_TABLE(SequencerView, wxWindow)
  EVT_PAINT(SequencerView::OnPaint)
  EVT_LEFT_DOWN(SequencerView::OnClick)
  EVT_LEFT_UP(SequencerView::OnLeftUp)
  EVT_MOTION(SequencerView::OnMotion)
  EVT_RIGHT_DOWN(SequencerView::OnRightClick)
  EVT_ENTER_WINDOW(SequencerView::OnHelp)
END_EVENT_TABLE()
