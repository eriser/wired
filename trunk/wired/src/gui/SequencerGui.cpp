// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include <iostream>
#include "AccelCenter.h"
#include "SequencerGui.h"
#include "Track.h"
#include "Sequencer.h"
#include "Ruler.h"
#include "Cursor.h"
#include "ColoredLine.h"
#include "ColoredBox.h"
#include "Colour.h"
#include "Transport.h"
#include "HelpPanel.h"
#include "SelectionZone.h"
#include "MixerGui.h"
#include "ChannelGui.h"
#include "WaveFile.h"
#include "WaveView.h"
#include "Pattern.h"
#include "AudioPattern.h"
#include "MidiPattern.h"
#include "SeqTrack.h"
#include "SeqTrackPattern.h"
#include "../midi/MidiDevice.h"
#include "../engine/Settings.h"
#include "../audio/WriteWaveFile.h"

SequencerGui				*SeqGui;

const struct s_combo_choice		ComboChoices[NB_COMBO_CHOICES + 1] =
{
  { "Bar"	,	1	},
  { "1/2"	,	2	},
  { "1/4"	,	4	},
  { "1/8"	,	8	},
  { "1/16"	,	16	},
  { "1/32"	,	32	},
  { "1/64"	,	64	},
  { "1/128"	,	128	},
  { "1/256"	,	256	},
  { ""		,	4242	}
};

SequencerView::SequencerView(wxWindow *parent, const wxPoint &pos, 
			     const wxSize &size)
  : wxWindow(parent, -1, pos, size, wxSUNKEN_BORDER)
{
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
  if (e.Dragging())
    if (TheZone->IsVisible())
      {
	TheZone->UpdateZone(e.m_x, e.m_y);
	SelectZonePatterns(e.ShiftDown());
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
    for (p = (*t)->TrackPattern->Patterns.begin(); p != (*t)->TrackPattern->Patterns.end(); p++)
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
  DrawMeasures();
  DrawTrackLines();
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
  TotalHeight = (h > GetClientSize().y) ? h : GetClientSize().y;
}

void					SequencerView::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s(_T("This is the Wired sequencer. You can add here Audio or MIDI tracks, which can be output to plugins. Use the toolbar above to choose one of the sequencer editing tools."));
      HelpWin->SetText(s);
    }
}

void					SequencerView::DrawMeasures()
{
  wxPaintDC				dc(this);
  wxSize				size;
  wxString				s;
  double				x;
  double				u;
  long					m;

  PrepareDC(dc);
  size = GetClientSize();
  dc.SetPen(wxPen(CL_SEQVIEW_BACKGROUND, 1, wxSOLID));
  dc.SetBrush(wxBrush(CL_SEQVIEW_BACKGROUND));
  dc.SetTextForeground(CL_SEQVIEW_FOREGROUND);
  dc.DrawRectangle(0, 0, size.x, size.y);
  dc.SetPen(wxPen(CL_SEQVIEW_BAR, 1, wxSOLID));
  u = MEASURE_WIDTH * SeqPanel->HoriZoomFactor / Seq->SigNumerator;
  m = (long) ceil(XScroll / u);
  for (x = u * m - XScroll; (long) floor(x) < size.x; x += u)
    {
      if (!(m++ % Seq->SigNumerator))
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

void					SequencerView::DrawTrackLines()
{
  wxPaintDC				dc(this);
  vector<Track *>::iterator		i;
  long					h;
  
  PrepareDC(dc);
  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)  
    if ((*i)->TrackPattern)
      {
	if ((h = (*i)->TrackOpt->GetPosition().y + (*i)->TrackOpt->GetSize().y) > 0)
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

SequencerGui::SequencerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size, wxWindow *mainwindow)
  : wxPanel(parent, -1, pos, size, wxSIMPLE_BORDER | wxWS_EX_PROCESS_IDLE)
{
  wxSize				s;
  wxSize				v;
  wxBoxSizer				*zer_1;
  wxBoxSizer				*zer_2;
  wxBoxSizer				*zer_3;
  wxBoxSizer				*zer_4;
  wxBoxSizer				*zer_5;
  wxString				combo_choices[NB_COMBO_CHOICES];
  long					c;
  long					r;

  Tool = ID_TOOL_MOVE_SEQUENCER;
  HoriZoomFactor = 1.0f;
  VertZoomFactor = 1.0f;
  CurrentPos = 0.0;
  DoCut = false;
  SetBackgroundColour(CL_SEQ_BACKGROUND);
  SetForegroundColour(CL_SEQ_FOREGROUND);
  Toolbar = new wxToolBar(this, -1, wxPoint(-1, -1), wxSize(-1, TOOLS_HEIGHT), wxTB_FLAT);
  Toolbar->AddRadioTool(ID_SEQ_MOVE, "Move", wxBitmap(string(WiredSettings->DataDir + string(HAND_UP)).c_str(), wxBITMAP_TYPE_PNG), wxBitmap(string(WiredSettings->DataDir + string(HAND_DOWN)).c_str(), wxBITMAP_TYPE_PNG), "Move Pattern", "Move Pattern", NULL);
  Toolbar->AddRadioTool(ID_SEQ_EDIT, "Draw", wxBitmap(string(WiredSettings->DataDir + string(DRAW_UP)).c_str(), wxBITMAP_TYPE_PNG), wxBitmap(string(WiredSettings->DataDir + string(DRAW_DOWN)).c_str(), wxBITMAP_TYPE_PNG), "Draw Pattern", "Draw Pattern", NULL);
  Toolbar->AddRadioTool(ID_SEQ_DEL, "Delete", wxBitmap(string(WiredSettings->DataDir + string(ERASE_UP)).c_str(), wxBITMAP_TYPE_PNG), wxBitmap(string(WiredSettings->DataDir + string(ERASE_DOWN)).c_str(), wxBITMAP_TYPE_PNG), "Delete Pattern", "Deletes notes", NULL);
  Toolbar->AddRadioTool(ID_SEQ_SPLIT, "Split", wxBitmap(string(WiredSettings->DataDir + string(SPLIT_UP)).c_str(), wxBITMAP_TYPE_PNG), wxBitmap(string(WiredSettings->DataDir + string(SPLIT_DOWN)).c_str(), wxBITMAP_TYPE_PNG), "Split Pattern", "Split Pattern", NULL);
  Toolbar->AddSeparator();
  Toolbar->AddCheckTool(ID_SEQ_MAGNET, "Magnet", wxBitmap(string(WiredSettings->DataDir + string(MAGN_UP)).c_str(), wxBITMAP_TYPE_PNG), wxBitmap(string(WiredSettings->DataDir + string(MAGN_DOWN)).c_str(), wxBITMAP_TYPE_PNG), "", "", NULL);
  for (c = 0; c < NB_COMBO_CHOICES; c++)
    combo_choices[c] = ComboChoices[c].s;
  MagnetQuant = new wxComboBox(Toolbar, ID_SEQ_COMBO_MAGNET, DEFAULT_MAGNETISM_COMBO_VALUE, 
			       wxPoint(-1, -1), wxSize(72, -1), 9, combo_choices);
  Toolbar->AddControl(MagnetQuant);
  Toolbar->AddSeparator();
  Toolbar->AddTool(ID_SEQ_COLOR, "Color", wxBitmap(string(WiredSettings->DataDir + string(COLOR_UP)).c_str(), wxBITMAP_TYPE_PNG), wxBitmap(string(WiredSettings->DataDir + string(COLOR_DOWN)).c_str(), wxBITMAP_TYPE_PNG));
  Toolbar->Realize();
  Toolbar->ToggleTool(ID_SEQ_MAGNET, MAGNETISM);
//   SetToolBar(Toolbar);
  VertScrollBar = new wxScrollBar(this, ID_SEQ_SCROLLING, wxPoint(-1, 0), 
			      wxSize(-1, -1), wxSB_VERTICAL);
  s = GetClientSize();
  v = VertScrollBar->GetSize();
  SeqView = new SequencerView(this, wxPoint(-1, -1), wxSize(-1, -1));
  HorizScrollBar = new wxScrollBar(this, ID_SEQ_SCROLLING, wxPoint(-1, -1), wxSize(-1, -1), wxSB_HORIZONTAL);
  TrackView = new wxScrolledWindow(this, ID_TRACK_VIEW, wxPoint(-1, -1), wxSize(TRACK_WIDTH, -1), wxSUNKEN_BORDER);
  VertZoomSlider = new wxSlider(this, ID_SEQ_VSLIDER, 100, 100, 400, wxPoint(-1, -1), wxSize(-1, RULER_HEIGHT),
				wxMAXIMIZE_BOX | wxNO_BORDER, wxDefaultValidator, _T("Vertical Zoom"));
  HoriZoomSlider = new wxSlider(this, ID_SEQ_HSLIDER, 100, 25, 400, wxPoint(-1, -1),
				wxSize(-1, HorizScrollBar->GetSize().y));
  RulerPanel = new Ruler(this, ID_SEQ_RULER, wxPoint(-1, -1), wxSize(-1, RULER_HEIGHT));
  ColorDialogBox = new wxColourDialog(mainwindow, 0);
  BrushColor = CL_DEFAULT_SEQ_BRUSH;
  ColorBox = new ColoredBox(this, ID_SEQ_COLORBOX, wxPoint(Toolbar->GetSize().x + COLORBOX_MARGINS, COLORBOX_MARGINS),
			    wxSize(TOOLS_HEIGHT - 2 * COLORBOX_MARGINS, TOOLS_HEIGHT - 2 * COLORBOX_MARGINS),
			    CL_DEFAULT_SEQ_BRUSH, CL_DEFAULT_SEQ_PEN);
  Connect(ID_SEQ_COLORBOX, wxEVT_SCROLL_TOP, (wxObjectEventFunction)(wxEventFunction)(wxScrollEventFunction) &SequencerGui::OnColoredBoxClick);
  /* Sizers */
  zer_5 = new wxBoxSizer(wxVERTICAL);
  zer_5->Add(RulerPanel, 0, wxALL | wxEXPAND | wxFIXED_MINSIZE, 0);
  zer_5->Add(SeqView, 1, wxALL | wxEXPAND, 0);
  zer_5->Add(HorizScrollBar, 0, wxALL | wxEXPAND, 0);
  zer_4 = new wxBoxSizer(wxVERTICAL);
  zer_4->Add(VertZoomSlider, 0, wxALL | wxEXPAND | wxFIXED_MINSIZE, 0);
  zer_4->Add(TrackView, 1, wxALL | wxEXPAND, 0);
  zer_4->Add(HoriZoomSlider, 0, wxALL | wxEXPAND | wxFIXED_MINSIZE, 0);
  zer_3 = new wxBoxSizer(wxHORIZONTAL);
  zer_3->Add(zer_4, 0, wxALL | wxEXPAND, 0);
  zer_3->Add(zer_5, 1, wxALL | wxEXPAND, 0);
  zer_3->Add(VertScrollBar, 0, wxALL | wxEXPAND | wxFIXED_MINSIZE, 0);
  //  TO BE UPDATED SOON (after having redone the toolbar with panels)
  //  zer_2 = new wxBoxSizer(wxHORIZONTAL);
  //  zer_2->Add(Toolbar, 1, wxALL | wxEXPAND | wxFIXED_MINSIZE, 0);
  //  zer_2->Add(ColorBox, 0, wxALIGN_CENTER_VERTICAL | wxALL | wxFIXED_MINSIZE, 0);
  zer_1 = new wxBoxSizer(wxVERTICAL);
  zer_1->Add(Toolbar, 0, wxALL | wxEXPAND, 0);
  zer_1->Add(zer_3, 1, wxALL | wxEXPAND, 0);
  SetSizer(zer_1);
  SeqView->SetBackgroundColour(CL_SEQVIEW_BACKGROUND);
  TrackView->SetScrollRate(10, 10);
  TrackView->SetBackgroundColour(wxColour(204, 199, 219));//*wxLIGHT_GREY);
  VertNowPos = 0;
  HorizNowPos = 0;
  Magnetism = MAGNETISM;
  CursorMagnetism = CURSOR_MAGNETISM ? CURSOR_DEFAULT_MAGNETISM : 0;
  PatternMagnetism = PATTERN_MAGNETISM ? PATTERN_DEFAULT_MAGNETISM : 0;
  FollowPlayCursor = PLAY_CURSOR_FOLLOWING;
  SetScrolling();
  AdjustHScrolling();
  SeqView->SetTotalWidth(0);
  SeqView->SetTotalHeight(0);
  /* Curseurs */
  BeginLoopCursor = new Cursor('L', ID_CURSOR_BEGIN, 0.0, RulerPanel, this,
			       CL_CURSORZ_HEAD_RIGHT, CL_CURSORZ_LINE_DARK);
  EndLoopCursor = new Cursor('R', ID_CURSOR_REPEAT, 4.0, RulerPanel, this,
			     CL_CURSORZ_HEAD_LEFT, CL_CURSORZ_LINE_DARK);
  EndCursor = new Cursor('E', ID_CURSOR_END, Seq->EndPos, RulerPanel, this,
			 CL_CURSORZ_HEAD_END, CL_CURSORZ_LINE_DARK);
  PlayCursor = new Cursor('P', ID_CURSOR_PLAY, 0.0, RulerPanel, this,
			  CL_CURSORZ_HEAD_PLAY, CL_CURSORZ_LINE_DARK);
  // evenement curseur
  Connect(ID_SEQ_SETPOS, TYPE_SEQ_SETPOS, (wxObjectEventFunction)&SequencerGui::OnSetPosition);
  // evenement resize pattern
  Connect(ID_SEQ_RESIZE, TYPE_SEQ_RESIZE, (wxObjectEventFunction)&SequencerGui::OnResizePattern);
  // evenement draw evenement midi
  Connect(ID_SEQ_DRAWMIDI, TYPE_SEQ_DRAWMIDI, (wxObjectEventFunction)&SequencerGui::OnDrawMidi);
  // Cr?ation du popup menu
  PopMenu = new wxMenu();  
  PopMenu->Append(ID_POPUP_MOVE_TO_CURSOR, "Move to cursor");
  PopMenu->Append(ID_POPUP_DELETE, "Delete");
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_POPUP_CUT, "Cut");
  PopMenu->Append(ID_POPUP_COPY, "Copy");
  PopMenu->Append(ID_POPUP_PASTE, "Paste");
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_POPUP_SELECT_ALL, "Select all");

  Connect(ID_POPUP_MOVE_TO_CURSOR, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &SequencerGui::OnMoveToCursorClick);
  Connect(ID_POPUP_DELETE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &SequencerGui::OnDeleteClick);
  Connect(ID_POPUP_COPY, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &SequencerGui::OnCopy);
  Connect(ID_POPUP_CUT, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &SequencerGui::OnCut);
  Connect(ID_POPUP_PASTE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &SequencerGui::OnPaste);
}

SequencerGui::~SequencerGui()
{
}

Track					*SequencerGui::AddTrack(bool is_audio)
{
  Track					*n;
  SeqTrack				*n1;
  SeqTrackPattern			*n2;
  long					yy;

  yy = SeqView->GetTotalHeight() - (long) floor(CurrentYScrollPos);
  wxPoint p(0, yy);
  wxSize  s(TRACK_WIDTH, (long) floor(TRACK_HEIGHT * VertZoomFactor));
//    printf("adding SEQTRACK %d with Y=%d\n", Seq->Tracks.size() + 1, yy);
  n1 = new SeqTrack(Seq->Tracks.size() + 1, TrackView, p, s, is_audio);
  //  printf("adding SEQTRACK PATTERN\n");
  n2 = new SeqTrackPattern(SeqView, n1, SeqView->GetTotalWidth());
  //  printf("adding TRACK\n");
  n = new Track(n1, n2, is_audio ? IS_AUDIO_TRACK : IS_MIDI_TRACK);
  if (is_audio)
    {
      n1->ChanGui = MixerPanel->AddChannel(n->Output, n1->Text->GetValue());
      n1->ChanGui->SetOpt(n1);
    }
  Seq->AddTrack(n);
  UpdateTracks();
  SetScrolling();
  ReSizeCursors();
  SeqView->DrawTrackLines();
  return (n);
}


void					SequencerGui::RemoveTrack()
{
  Seq->RemoveTrack();
  UpdateTracks();
  SetScrolling();
}

void					SequencerGui::PutCursorsOnTop()
{
  PlayCursor->PutOnTop();
  BeginLoopCursor->PutOnTop();
  EndLoopCursor->PutOnTop();
  EndCursor->PutOnTop();
}

void					SequencerGui::ReSizeCursors()
{
#ifdef __DEBUG__
  printf("SequencerGui::ReSizeCursors()\n");
#endif
  PlayCursor->ReSize();
  BeginLoopCursor->ReSize();
  EndLoopCursor->ReSize();
  EndCursor->ReSize();
}

void					SequencerGui::RedrawCursors()
{
  PlayCursor->SetPos(PlayCursor->GetPos());
  BeginLoopCursor->SetPos(BeginLoopCursor->GetPos());
  EndLoopCursor->SetPos(EndLoopCursor->GetPos());
  EndCursor->SetPos(EndCursor->GetPos());
}

void					SequencerGui::UpdateAudioPatterns(WaveFile *w)
{
  vector<Track *>::iterator		t;
  vector<Pattern *>::iterator		p;

  for (t = Seq->Tracks.begin(); t != Seq->Tracks.end(); t++)
    for (p = (*t)->TrackPattern->Patterns.begin(); p != (*t)->TrackPattern->Patterns.end(); p++)
      if ((*t)->IsAudioTrack() && (((AudioPattern *) *p)->GetWaveFile() == w))
	((AudioPattern *) *p)->SetDrawing();
}

void					SequencerGui::UpdateMidiPattern(MidiPattern *m)
{
  m->Update();
  m->Refresh();
}

void					SequencerGui::SetScrolling()
{
  long					z;
  
  SeqView->SetTotalHeight((unsigned long) (z = (long) floor(Seq->Tracks.size() * TRACK_HEIGHT * VertZoomFactor)));
  if (z < (SeqView->GetYScroll() + SeqView->GetClientSize().y))
    SeqView->SetTotalHeight(z);
  if ((z = (SeqView->GetTotalHeight() - SeqView->GetClientSize().y)) > 0)
    VertScrollBar->SetScrollbar((VertScrollBar->GetThumbPosition() >= z)
				? z : VertScrollBar->GetThumbPosition(), VSCROLL_THUMB_WIDTH, z + VSCROLL_THUMB_WIDTH, 42, true);
  else
    VertScrollBar->SetScrollbar(0, VSCROLL_THUMB_WIDTH, VSCROLL_THUMB_WIDTH, 42, true);
  SeqView->SetTotalWidth((unsigned long) (z = (long) floor(Seq->EndPos * MEASURE_WIDTH * HoriZoomFactor)));
  if (z < (SeqView->GetXScroll() + SeqView->GetClientSize().x))
    SeqView->SetTotalWidth(z);
  if ((z = (SeqView->GetTotalWidth() - SeqView->GetClientSize().x)) > 0)
    HorizScrollBar->SetScrollbar((HorizScrollBar->GetThumbPosition() >= z)
				 ? z : HorizScrollBar->GetThumbPosition(), HSCROLL_THUMB_WIDTH, z + HSCROLL_THUMB_WIDTH, 42, true);
  else
    HorizScrollBar->SetScrollbar(0, HSCROLL_THUMB_WIDTH, HSCROLL_THUMB_WIDTH, 42, true);
}

void					SequencerGui::AdjustHScrolling()
{
  long					thumb_pos;
  long					range;
  double				pos_tmp;
  
  if ((long) (floor(CurrentXScrollPos) - floor((pos_tmp = ((range = HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) ?
    (((double) (SeqView->GetTotalWidth() - SeqView->GetClientSize().x)
      * (double) (thumb_pos = (HorizScrollBar->GetThumbPosition() >= range) ?
		  range : HorizScrollBar->GetThumbPosition())) /
     (double) range) : 0))))
    {
      SeqView->ScrollWindow((long) (floor(CurrentXScrollPos) - floor(pos_tmp)), 0, (const wxRect *) NULL);
      SeqView->SetXScrollValue((long) ceil(pos_tmp));
      RulerPanel->ScrollWindow((long) (floor(CurrentXScrollPos) - floor(pos_tmp)), 0, (const wxRect *) NULL);
      RulerPanel->SetXScrollValue((long) ceil(pos_tmp));
      CurrentXScrollPos = pos_tmp;
    }
  FirstMeasure = pos_tmp / (MEASURE_WIDTH * HoriZoomFactor);
  LastMeasure = (pos_tmp + SeqView->GetClientSize().x) / (MEASURE_WIDTH * HoriZoomFactor);
  RulerPanel->Refresh();
  SeqView->Refresh();
}

void					SequencerGui::AdjustVScrolling()
{
  long					thumb_pos;
  long					range;
  double				pos_tmp;
  
  if ((long) (floor(CurrentYScrollPos) - floor((pos_tmp = ((range = VertScrollBar->GetRange() - VSCROLL_THUMB_WIDTH)) ?
    (((double) (SeqView->GetTotalHeight() - SeqView->GetClientSize().y)
      * (double) (thumb_pos = (VertScrollBar->GetThumbPosition() >= range) ?
		  range : VertScrollBar->GetThumbPosition())) /
     (double) range) : (double) 0))))
    {
      SeqView->ScrollWindow(0, (long) (floor(CurrentYScrollPos) - floor(pos_tmp)), (const wxRect *) NULL);
      TrackView->ScrollWindow(0, (long) (floor(CurrentYScrollPos) - floor(pos_tmp)), (const wxRect *) NULL);
      SeqView->SetYScrollValue((long) ceil(pos_tmp));
      CurrentYScrollPos = pos_tmp;
    }
  TrackView->Refresh();
  SeqView->Refresh();
}

void					SequencerGui::OnScroll(wxScrollEvent &event)
{
  if (event.GetOrientation() == wxHORIZONTAL)
    {
      HorizNowPos = HorizScrollBar->GetThumbPosition();
      AdjustHScrolling();
    }
  else
    {       
      VertNowPos = VertScrollBar->GetThumbPosition();
      AdjustVScrolling();
    }
}

void					SequencerGui::UpdateTracks()
{
  vector<Track *>::iterator		i;
  long					z;
  long					h;
  
  h = (long) floor(VertZoomFactor * TRACK_HEIGHT);
  for (z = -SeqView->GetYScroll(), i = Seq->Tracks.begin(); i != Seq->Tracks.end(); z += h)
    {
      (*i)->TrackOpt->SetPosition(wxPoint(0, z));
      (*i)->TrackOpt->SetSize(wxSize(TRACK_WIDTH, h)); 
      (*i++)->TrackOpt->Refresh();
    }
  UpdateMeasures();
}

void					SequencerGui::UpdateTrackList(vector<Track *> *track_list)
{
  vector<Track *>::iterator		t;
  
  for (t = track_list->begin(); t != track_list->end(); t++)
    (*t)->RefreshFullTrack();
}

void					SequencerGui::OnVertSliderUpdate(wxCommandEvent &event)
{
  VertScrollBar->SetThumbPosition(0);
  VertZoomFactor = VertZoomSlider->GetValue() / 100.0;
  UpdateTracks();
  SetScrolling();
  AdjustVScrolling();
  ReSizeCursors();
  PutCursorsOnTop();
}

void					SequencerGui::OnHoriSliderUpdate(wxCommandEvent &event)
{
  HorizScrollBar->SetThumbPosition(0);
  HoriZoomFactor = HoriZoomSlider->GetValue() / 100.0;
  UpdateMeasures();
  SetScrolling();
  AdjustHScrolling();
  RedrawCursors();
}

void					SequencerGui::UpdateMeasures()
{
  vector<Track *>::iterator		t;
  vector<Pattern *>::iterator		p;

  for (t = Seq->Tracks.begin(); t != Seq->Tracks.end(); t++)
    {
      //      (*t)->TrackPattern->Update();
      for (p = (*t)->TrackPattern->Patterns.begin(); p != (*t)->TrackPattern->Patterns.end(); p++)
	(*p)->Update();
    }  
}

void					SequencerGui::OnPaint(wxPaintEvent &event)
{
  wxPaintDC				dc(this);

  PrepareDC(dc);
  dc.SetBrush(wxBrush(CL_SEQ_BACKGROUND, wxSOLID));
  dc.SetPen(wxPen(CL_SEQ_BACKGROUND, 1, wxSOLID));
  dc.DrawRectangle(0, 0, GetSize().x, GetSize().y);
}

void					SequencerGui::OnSize(wxSizeEvent &event)
{
  Layout();
  SetScrolling();
  AdjustHScrolling();
  AdjustVScrolling();
  ReSizeCursors();
}

void					SequencerGui::UnselectTracks()
{
  vector<Track *>::iterator		i;

  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
    if ((*i)->TrackOpt->GetSelected())
      (*i)->TrackOpt->SetSelected(false);
}

void					SequencerGui::SelectTrack(long trackindex)
{
	vector<Track *>::iterator		iter;

	UnselectTracks();
	for (iter = Seq->Tracks.begin(); iter != Seq->Tracks.end(); iter++)
    	if ((*iter)->Index == trackindex)
			(*iter)->TrackOpt->SetSelected(true);
}

void					SequencerGui::SwapTracksPos(Track *t1, Track *t2)
{
  long					z;
  
  z = t1->TrackOpt->GetPosition().y;
  t1->TrackOpt->SetPosition(wxPoint(0, t2->TrackOpt->GetPosition().y));
  t2->TrackOpt->SetPosition(wxPoint(0, z));
}

void					SequencerGui::ChangeSelectedTrackIndex(long trackindexdelta)
{
	vector<Track *>::iterator		i;
	vector<Track *>::iterator		j;
	vector<Track *>					u;
	long							x;
	long							z;
	Track							*t;

#ifdef __DEBUG__
  printf("SequencerGui::ChangeSelectedTrackIndex(%d)\n", trackindexdelta);
#endif

	for (i = Seq->Tracks.begin(); (i != Seq->Tracks.end()) && !((*i)->TrackOpt->GetSelected()); i++);
	if (i == Seq->Tracks.end())
    	return;
	if ((z = trackindexdelta) > 0)
	    for (j = i++, x = 0; (x < z) && (i != Seq->Tracks.end()); x++)
    	{
			SwapTracksPos(*i, *j);
			(*i)->UpdateIndex((*i)->Index - 1);
			u.push_back(*i);
			t = *i;
			*(i++) = *j;
			*(j++) = t;
	    }
	else
    	for (j = i--, x = 0; (x > z) && (j != Seq->Tracks.begin()); x--)
		{
			SwapTracksPos(*i, *j);
			(*i)->UpdateIndex((*i)->Index + 1);
			u.push_back(*i);
			t = *i;
			*(i--) = *j;
			*(j--) = t;
		}
	(*j)->UpdateIndex((*j)->Index + x);
	u.push_back(*j);
	UpdateTrackList(&u);
	u.clear();
}

void					SequencerGui::ScrollTrackList(long track_delta)
{
  long					z;
  long					h;
  
  z = track_delta * (h = (long) floor(TRACK_HEIGHT * VertZoomFactor));
  if (track_delta > 0)
    VertScrollBar->SetThumbPosition(((z += VertScrollBar->GetThumbPosition()) > (SeqView->GetTotalHeight() - SeqView->GetClientSize().y - h)) ?
				    SeqView->GetTotalHeight() - SeqView->GetClientSize().y : VertScrollBar->GetThumbPosition() + z);
  else
    if (track_delta < 0)
      VertScrollBar->SetThumbPosition((-z > VertScrollBar->GetThumbPosition()) ? 0 : VertScrollBar->GetThumbPosition() + z);
  SetScrolling();
  AdjustVScrolling();
}

void					SequencerGui::RemoveReferenceTo(Plugin *plug)
{
  vector<Track *>::iterator		i;

  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
    if ((*i)->TrackOpt->Connected == plug)
      (*i)->TrackOpt->ConnectTo(0);
}

void					SequencerGui::DeleteAllTracks()
{
  vector<Track *>::iterator		i;
  
  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)  
    {
      if ((*i)->TrackOpt->ChanGui)
	MixerPanel->RemoveChannel((*i)->TrackOpt->ChanGui);
      delete (*i);
    }
  Seq->Tracks.clear();
  UpdateTracks();
  SetScrolling();
}

void					SequencerGui::DeleteSelectedTrack()
{
	vector<Track *>::iterator		iterTrack;
	vector<Pattern *>::iterator		iterPattern;
	long							j;
  
#ifdef __DEBUG__
  printf("SequencerGui::DeleteSelectedTrack()\n");
#endif

	for (iterTrack = Seq->Tracks.begin(); (iterTrack != Seq->Tracks.end()) && !((*iterTrack)->TrackOpt->GetSelected()); iterTrack++);
	if (iterTrack == Seq->Tracks.end())
    	return;
	if ((*iterTrack)->TrackOpt->Record && Seq->Recording)
    	return;
	if ((*iterTrack)->TrackOpt->ChanGui)
    	MixerPanel->RemoveChannel((*iterTrack)->TrackOpt->ChanGui);
	for (iterPattern = SelectedItems.begin(); iterPattern != SelectedItems.end(); )
    	if (((*iterTrack)->Index == (*iterPattern)->GetTrackIndex()) && (*iterPattern)->IsSelected())
			SelectedItems.erase(iterPattern);
		else
			iterPattern++;
	SeqMutex.Lock();
	delete (*iterTrack);
	Seq->Tracks.erase(iterTrack);
	for (iterTrack = Seq->Tracks.begin(), j = 0; iterTrack != Seq->Tracks.end(); iterTrack++)
    	(*iterTrack)->UpdateIndex(j++);
	UpdateTracks();
	SeqMutex.Unlock();
	SetScrolling();
	AdjustVScrolling();
}

void					SequencerGui::SelectItem(Pattern *p, bool shift)
{
  vector<Pattern *>::iterator		i;

  if (!shift)
    {
      for (i = SelectedItems.begin(); i != SelectedItems.end(); i++)
	if ((*i)->IsSelected())
	  (*i)->SetSelected(false);
      SelectedItems.clear();
    }
  if (p)
    if (!p->IsSelected())
      {
	p->SetSelected(true);
	SelectedItems.push_back(p);
      }
    else
      {
	p->SetSelected(false);
	for (i = SelectedItems.begin(); (i != SelectedItems.end()) && (*i != p); i++);
	SelectedItems.erase(i);
      }
}

void					SequencerGui::CopySelectedItems()
{
  vector<Pattern *>::iterator		j;

  CopyItems.clear();
  for (j = SelectedItems.begin(); j != SelectedItems.end(); j++)
    CopyItems.push_back(*j);
}

void					SequencerGui::PasteItems()
{
  vector<Pattern *>::iterator		j;
  
  for (j = CopyItems.begin(); j != CopyItems.end(); j++)
    ((Pattern *) *j)->CreateCopy(((Pattern *) *j)->GetEndPosition());
  if (DoCut)
    DeleteSelectedPatterns();
}

void					SequencerGui::DeleteSelectedPatterns()
{
  vector<Pattern *>::iterator		i;
  vector<Pattern *>::iterator		j;
  
  for (i = SelectedItems.begin(); i != SelectedItems.end(); i++)
    {
      for (j = CopyItems.begin(); j != CopyItems.end(); j++)
	if ((*j) == (*i))
	  {
	    CopyItems.erase(j);
	    break;
	  }
      Seq->DeletePattern(*i);
    }
  SelectedItems.clear();
}

void					SequencerGui::DeletePattern(Pattern *p)
{
  vector<Pattern *>::iterator		i;
  
  for (i = SelectedItems.begin(); i != SelectedItems.end(); i++)
    if (*i == p)
      {
	i = SelectedItems.erase(i);
	break;
      }
  for (i = CopyItems.begin(); i != CopyItems.end(); i++)
    if (*i == p)
      {
	i = CopyItems.erase(i);
	break;
      }
  Seq->DeletePattern(p);
}

void					SequencerGui::MoveToCursor()
{
 vector<Pattern *>::iterator		i, j;
 wxMutexLocker				m(SeqMutex);

 for (i = SelectedItems.begin(); i != SelectedItems.end(); i++)
   {
     (*i)->Modify(Seq->CurrentPos);
     (*i)->Update();
   }
}

void					SequencerGui::OnWheelMove(wxMouseEvent &e)
{
  if (e.GetWheelRotation() > 0)
    VertScrollBar->SetThumbPosition((VertScrollBar->GetThumbPosition() > WHEEL_VSCROLL_UNIT) ?
				    VertScrollBar->GetThumbPosition() - WHEEL_VSCROLL_UNIT : 0);
  else
    VertScrollBar->SetThumbPosition(((VertScrollBar->GetThumbPosition() + WHEEL_VSCROLL_UNIT) < SeqView->GetTotalHeight()) ?
				    VertScrollBar->GetThumbPosition() + WHEEL_VSCROLL_UNIT : SeqView->GetTotalHeight());
  SetScrolling();
  AdjustVScrolling();
}

void					SequencerGui::OnCopy(wxCommandEvent &event)
{
  DoCut = false;
  CopySelectedItems();
}

void					SequencerGui::OnCut(wxCommandEvent &event)
{
  DoCut = true;
  CopySelectedItems();
}

void					SequencerGui::OnPaste(wxCommandEvent &event)
{
  PasteItems();
}

void					SequencerGui::OnSetPosition(CursorEvent &event)
{
  long					x;
  double				p;

  SetCurrentPos((p = CurrentPos));
  if (SeqPanel->FollowPlayCursor)
    if (p <= SeqPanel->FirstMeasure)
      {
	if ((x = (long) floor((p * (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) / Seq->EndPos)) > 0)
	  SeqPanel->HorizScrollBar->SetThumbPosition(x);
	else
	  SeqPanel->HorizScrollBar->SetThumbPosition(0);
	SeqPanel->AdjustHScrolling();
      }
    else
      if (p >= SeqPanel->LastMeasure)
	{
	  if ((x = (long) floor(p * (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH) / Seq->EndPos))
	      < (SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH))
	    SeqPanel->HorizScrollBar->SetThumbPosition(x);
	  else
	    {
	      Seq->EndPos = ceil(p);
	      SeqPanel->SetScrolling();
	      SeqPanel->EndCursor->SetPos(Seq->EndPos);
	      SeqPanel->HorizScrollBar->SetThumbPosition(SeqPanel->HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH);
	    }
	  SeqPanel->AdjustHScrolling();
	}
}

void					SequencerGui::SetCurrentPos(double pos)
{
  PlayCursor->SetPos(pos);
  TransportPanel->SetPlayPosition(pos);  
}

void					SequencerGui::ChangeMouseCursor(wxCursor c)
{
  vector<Track *>::iterator		t;
  vector<Pattern *>::iterator		p;

  SeqView->SetCursor(c);
  for (t = Seq->Tracks.begin(); t != Seq->Tracks.end(); t++)
    for (p = (*t)->TrackPattern->Patterns.begin(); p != (*t)->TrackPattern->Patterns.end(); p++)
      (*p)->SetCursor(c);
}

void					SequencerGui::OnResizePattern(wxCommandEvent &event)
{
  Pattern				*p;

  p = (Pattern *) event.GetEventObject();
  p->Modify(-1, Seq->CurrentPos);
  p->Update();
}

void					SequencerGui::OnDrawMidi(wxCommandEvent &event)
{
  MidiPattern				*p;

  p = (MidiPattern *)event.GetEventObject();  
  p->DrawMidi();  
  p->Update();
}

void					SequencerGui::OnDeleteClick(wxCommandEvent &event)
{
  DeleteSelectedPatterns();
}

void					SequencerGui::OnSelectAll(wxCommandEvent &event)
{
  cout << "[SEQUENCERGUI] TODO: Select all" << endl;
}

void					SequencerGui::OnMoveToCursorClick(wxCommandEvent &event)
{
  MoveToCursor();
}

void					SequencerGui::ShowPopup(wxPoint pos)
{
  PopupMenu(PopMenu, pos);
}

void					SequencerGui::OnPlayCursorMove(wxCommandEvent &event)
{
  double				p;
  wxMutexLocker				m(SeqMutex);

  SetCurrentPos((p = PlayCursor->GetPos()));
  Seq->SetCurrentPos(p);
}

void					SequencerGui::OnBeginLCursorMove(wxCommandEvent &event)
{
  wxMutexLocker				m(SeqMutex);

  Seq->BeginLoopPos = BeginLoopCursor->GetPos();
}

void					SequencerGui::OnEndLCursorMove(wxCommandEvent &event)
{
  wxMutexLocker				m(SeqMutex);

  Seq->EndLoopPos = EndLoopCursor->GetPos();
}

void					SequencerGui::OnEndCursorMove(wxCommandEvent &event)
{
  wxMutexLocker				m(SeqMutex);

  Seq->EndPos = EndCursor->GetPos();
}

void					SequencerGui::OnMoveClick(wxCommandEvent &event)
{
  Tool = ID_TOOL_MOVE_SEQUENCER;
  ChangeMouseCursor(wxCursor(wxCURSOR_HAND));
}

void					SequencerGui::OnEraseClick(wxCommandEvent &event)
{
  Tool = ID_TOOL_DELETE_SEQUENCER;
  ChangeMouseCursor(wxCursor(wxCURSOR_NO_ENTRY));
}

void					SequencerGui::OnSplitClick(wxCommandEvent &event)
{
  Tool = ID_TOOL_SPLIT_SEQUENCER;
  ChangeMouseCursor(wxCursor(wxCURSOR_CROSS));
}

void					SequencerGui::OnMagnetismToggle(wxCommandEvent &event)
{
  Magnetism = Toolbar->GetToolState(ID_SEQ_MAGNET) ? CURSOR_MASK | PATTERN_MASK : 0;
  /*  printf("Magnetisn = %s\n", Magnetism ? "[ OK ]" : "[ NO ]");*/
}

void					SequencerGui::OnMagnetismChange(wxCommandEvent &event)
{
  long					c;
  wxString				s;

  s =  MagnetQuant->GetValue();
  for (c = 0; (c < NB_COMBO_CHOICES) && (s != ComboChoices[c].s); c++);
  CursorMagnetism = (long) floor(ComboChoices[c].value);
  PatternMagnetism = (long) floor(ComboChoices[c].value);
  /*  cout << "Magnetism change " << MagnetQuant->GetValue() << " and " << ComboChoices[c].value << " " << endl;*/
}

void					SequencerGui::OnColorButtonClick(wxCommandEvent &event)
{
  vector<Pattern *>::iterator		p;

  PenColor = ColorBox->GetColor();
  for (p = SelectedItems.begin(); p != SelectedItems.end(); p++)
    if ((*p)->IsSelected())
      {
	(*p)->SetDrawColour(PenColor);
	(*p)->Refresh();
      }
}

void					SequencerGui::OnColoredBoxClick(wxCommandEvent &event)
{
  if (ColorDialogBox->ShowModal() == wxID_OK)
    {
      ColorBox->SetColor(PenColor = ColorDialogBox->GetColourData().GetColour());
      ColorBox->Refresh();
    }
}

void					SequencerGui::SetBeginLoopPos(double pos)
{
  BeginLoopCursor->SetPos(pos);
}

void					SequencerGui::SetEndLoopPos(double pos)
{
  EndLoopCursor->SetPos(pos);
}

void					SequencerGui::SetEndPos(double pos)
{
  EndCursor->SetPos(pos);
}

/*

TODO :

Learn how to declare custom event types with wx,
it's quite weird tbh ...

this is the beginning :)

DEFINE_EVENT_TYPE(wxSetCursorPos)
DEFINE_EVENT_TYPE(wxResizePattern)
DEFINE_EVENT_TYPE(wxDrawMidi)
DEFINE_EVENT_TYPE(wxON_COLOREDBOX_CLICK)

*/

BEGIN_EVENT_TABLE(SequencerGui, wxPanel)
  EVT_BUTTON(ID_CURSOR_PLAY, SequencerGui::OnPlayCursorMove)
  EVT_BUTTON(ID_CURSOR_BEGIN, SequencerGui::OnBeginLCursorMove)
  EVT_BUTTON(ID_CURSOR_REPEAT, SequencerGui::OnEndLCursorMove)
  EVT_BUTTON(ID_CURSOR_END, SequencerGui::OnEndCursorMove)
  EVT_SLIDER(ID_SEQ_VSLIDER, SequencerGui::OnVertSliderUpdate)
  EVT_SLIDER(ID_SEQ_HSLIDER, SequencerGui::OnHoriSliderUpdate)
  EVT_COMMAND_SCROLL(ID_SEQ_SCROLLING, SequencerGui::OnScroll)
  EVT_TOOL(ID_SEQ_MOVE, SequencerGui::OnMoveClick)
  EVT_TOOL(ID_SEQ_DEL, SequencerGui::OnEraseClick)
  EVT_TOOL(ID_SEQ_SPLIT, SequencerGui::OnSplitClick)
  EVT_TOOL(ID_SEQ_MAGNET, SequencerGui::OnMagnetismToggle)
  EVT_TOOL(ID_SEQ_COLOR, SequencerGui::OnColorButtonClick)
  EVT_TEXT(ID_SEQ_COMBO_MAGNET, SequencerGui::OnMagnetismChange)
  EVT_SIZE(SequencerGui::OnSize)
  EVT_MOUSEWHEEL(SequencerGui::OnWheelMove)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SequencerView, wxWindow)
  EVT_PAINT(SequencerView::OnPaint)
  EVT_LEFT_DOWN(SequencerView::OnClick)
  EVT_LEFT_UP(SequencerView::OnLeftUp)
  EVT_MOTION(SequencerView::OnMotion)
  EVT_RIGHT_DOWN(SequencerView::OnRightClick)
  EVT_ENTER_WINDOW(SequencerView::OnHelp)
END_EVENT_TABLE()

