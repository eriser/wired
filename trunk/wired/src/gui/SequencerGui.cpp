// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include "Sequencer.h"
#include "Ruler.h"
#include "Cursor.h"
#include "ColoredLine.h"
#include "Colour.h"
#include "SequencerGui.h"
#include "Transport.h"
#include "HelpPanel.h"

#include "MixerGui.h"

#include <iostream>

SequencerGui				*SeqGui;

#include "WaveFile.h"
#include "WaveView.h"

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
  : wxScrolledWindow(parent, -1, pos, size, wxSUNKEN_BORDER)
{
  /*  Connect(GetId(), wxEVT_PAINT, (wxObjectEventFunction)(wxEventFunction)(wxPaintEventFunction)
	  &SequencerView::OnPaint);
  Connect(GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &SequencerView::OnClick);
  Connect(GetId(), wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
  &SequencerView::OnRightClick);*/
}

SequencerView::~SequencerView()
{

}

void					SequencerView::OnClick(wxMouseEvent &event)
{
  SeqPanel->SelectItem(0x0, false);
}

void					SequencerView::OnRightClick(wxMouseEvent &event)
{
  SeqPanel->ShowPopup(event.GetPosition() + GetPosition());
}

void					SequencerView::OnPaint(wxPaintEvent &event)
{
  DrawMeasures();
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

void					SequencerView::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      HelpWin->Help->Load_Text("This is the Wired sequencer. You can add here Audio or MIDI tracks, which can be output to plugins. Use the toolbar above for sequencer editing tools.");
    }
}

/*** DEBUTE SequencerGui ***/

SequencerGui::SequencerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size)
  : wxPanel(parent, -1, pos, size, wxSIMPLE_BORDER | wxWS_EX_PROCESS_IDLE)
{
  wxSize				s;
  wxBoxSizer				*row_1;
  wxBoxSizer				*row_2;
  wxBoxSizer				*row_3;
  wxBoxSizer				*row_4;
  wxBoxSizer				*col_1;
  wxBoxSizer				*globz;
  wxString				combo_choices[NB_COMBO_CHOICES];
  long					c;

  Tool = ID_TOOL_MOVE_SEQUENCER;
  CurrentPos = 0.0;
  HoriZoomFactor = 1.0f;
  VertZoomFactor = 1.0f;
  DoCut = false;
  SetBackgroundColour(CL_DEFAULT_BACKGROUND);
  SetForegroundColour(CL_DEFAULT_FOREGROUND);
  VertScrollBar = new wxScrollBar(this, ID_SEQ_SCROLLING, wxPoint(-1, 0), 
			      wxSize(-1, -1), wxSB_VERTICAL);
  s = VertScrollBar->GetSize();
  VertScrollBar->Move(wxPoint(size.x - s.x, 0));
  SeqView = new SequencerView(this, wxPoint(0, 0), 
			      wxSize(size.x - TRACK_WIDTH - s.x, size.y - RULER_HEIGHT));
  HorizScrollBar = new wxScrollBar(this, ID_SEQ_SCROLLING, 
				   wxPoint(TRACK_WIDTH, SeqView->GetClientSize().y), 
				   wxSize(SeqView->GetClientSize().x, -1), 
				   wxSB_HORIZONTAL);
  TrackView = new wxScrolledWindow(this, ID_TRACK_VIEW, wxPoint(0, RULER_HEIGHT), 
				   wxSize(TRACK_WIDTH, size.y - RULER_HEIGHT), wxSUNKEN_BORDER);
  VertZoomSlider = new wxSlider(this, ID_SEQ_VSLIDER, 100, 100, 400, wxPoint(0, 0), 
				wxSize(TRACK_WIDTH, RULER_HEIGHT));
  HoriZoomSlider = new wxSlider(this, ID_SEQ_HSLIDER, 100, 25, 400, 
				wxPoint(0, SeqView->GetClientSize().y + RULER_HEIGHT),
				wxSize(TRACK_WIDTH, 16));
  RulerPanel = new Ruler(this, ID_SEQ_RULER, wxPoint(TRACK_WIDTH, 0), 
			 wxSize(size.x - TRACK_WIDTH - s.x, RULER_HEIGHT));
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
			       wxPoint(-1, -1), wxSize(68, -1), 9, combo_choices);
  Toolbar->AddControl(MagnetQuant);
  Toolbar->AddSeparator();
  Toolbar->AddTool(ID_SEQ_COLOR, "Color", wxBitmap(string(WiredSettings->DataDir + string(COLOR_UP)).c_str(), wxBITMAP_TYPE_PNG), wxBitmap(string(WiredSettings->DataDir + string(COLOR_DOWN)).c_str(), wxBITMAP_TYPE_PNG));

  Toolbar->Realize();
  Toolbar->ToggleTool(ID_SEQ_MAGNET, MAGNETISM);
  //SetToolBar(Toolbar);
  ColorDialogBox = new wxColourDialog(this, 0);
  BrushColor = CL_DEFAULT_SEQ_BRUSH;
  ColorBox = new ColoredBox(this, ID_SEQ_COLORBOX, wxPoint(Toolbar->GetSize().x + COLORBOX_MARGINS, COLORBOX_MARGINS),
			    wxSize(TOOLS_HEIGHT - 2 * COLORBOX_MARGINS, TOOLS_HEIGHT - 2 * COLORBOX_MARGINS),
			    CL_DEFAULT_SEQ_BRUSH, CL_DEFAULT_SEQ_PEN);
  Connect(ID_SEQ_COLORBOX, wxEVT_SCROLL_TOP, (wxObjectEventFunction)(wxEventFunction)(wxScrollEventFunction) &SequencerGui::OnColoredBoxClick);
  /* Sizers */
  row_1 = new wxBoxSizer(wxHORIZONTAL);
  row_1->Add(Toolbar, 1, wxALL, 0); 
  row_2 = new wxBoxSizer(wxHORIZONTAL);
  row_2->Add(VertZoomSlider, 0, wxALL, 0); 
  row_2->Add(RulerPanel, 1, wxALL, 0); 

  row_3 = new wxBoxSizer(wxHORIZONTAL);
  row_3->Add(TrackView, 0, wxALL | wxEXPAND, 0); 
  row_3->Add(SeqView, 1, wxALL | wxEXPAND, 0);

  row_4 = new wxBoxSizer(wxHORIZONTAL);
  row_4->Add(HoriZoomSlider, 0, wxALL, 0); 
  row_4->Add(HorizScrollBar, 1, wxALL, 0);  

  col_1 = new wxBoxSizer(wxVERTICAL);
  col_1->Add(row_1, 0, wxEXPAND | wxALL, 0);
  col_1->Add(row_2, 0, wxEXPAND | wxALL, 0);
  col_1->Add(row_3, 1, wxEXPAND | wxALL, 0);
  col_1->Add(row_4, 0, wxEXPAND | wxALL, 0);

  globz = new wxBoxSizer(wxHORIZONTAL);
  globz->Add(col_1, 1, wxALL | wxEXPAND , 0); 
  globz->Add(VertScrollBar, 0, wxALL | wxEXPAND , 0); 

  SetSizer(globz);

  SeqView->SetScrollRate(10, 10);
  SeqView->SetBackgroundColour(CL_SEQVIEW_BACKGROUND);
  TrackView->SetScrollRate(10, 10);
  TrackView->SetBackgroundColour(wxColour(204, 199, 219));//*wxLIGHT_GREY);
  VertZoomSlider->SetBackgroundColour(*wxLIGHT_GREY);
  VertNowPos = 0;
  HorizNowPos = 0;
  Magnetism = MAGNETISM;
  CursorMagnetism = CURSOR_MAGNETISM ? CURSOR_DEFAULT_MAGNETISM : 0;
  PatternMagnetism = PATTERN_MAGNETISM ? PATTERN_DEFAULT_MAGNETISM : 0;
  FollowPlayCursor = PLAY_CURSOR_FOLLOWING;
  SetScrolling();
  AdjustHScrolling();

  /* Curseurs */
  BeginLoopCursor = new Cursor('L', ID_CURSOR_BEGIN, 0.0, RulerPanel, this,
			       CL_CURSORZ_HEAD_BEGINL, CL_CURSORZ_LINE_DARK);
  BeginLoopCursor->SetPos(0);

  EndLoopCursor = new Cursor('R', ID_CURSOR_REPEAT, 0.0, RulerPanel, this,
			       CL_CURSORZ_HEAD_ENDL, CL_CURSORZ_LINE_DARK);
  EndLoopCursor->SetPos(4);

  EndCursor = new Cursor('E', ID_CURSOR_END, 0.0, RulerPanel, this,
			       CL_CURSORZ_HEAD_END, CL_CURSORZ_LINE_DARK);
  EndCursor->SetPos(16);

  PlayCursor = new Cursor('P', ID_CURSOR_PLAY, 0.0, RulerPanel, this,
				CL_CURSORZ_HEAD_PLAY, CL_CURSORZ_LINE_DARK);
  PlayCursor->SetPos(0);

  // evenement curseur
  Connect(ID_SEQ_SETPOS, TYPE_SEQ_SETPOS, (wxObjectEventFunction)&SequencerGui::OnSetPosition);
  // evenement resize pattern
  Connect(ID_SEQ_RESIZE, TYPE_SEQ_RESIZE, (wxObjectEventFunction)&SequencerGui::OnResizePattern);
  // evenement draw evenement midi
  Connect(ID_SEQ_DRAWMIDI, TYPE_SEQ_DRAWMIDI, (wxObjectEventFunction)&SequencerGui::OnDrawMidi);

  // Création du popup menu
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

Track					*SequencerGui::AddTrack(bool is_audio)
{
  Track					*n;
  SeqTrack				*n1;
  SeqTrackPattern			*n2;
  long					yy;

  /*  TrackView->CalcScrolledPosition(0, Seq->Tracks.size() * 
      (TRACK_HEIGHT * VertZoomFactor), &xx, &yy);  */
  yy = Seq->Tracks.size() * (long)(TRACK_HEIGHT * VertZoomFactor) - (long)CurrentYScrollPos;
  wxPoint p(0, yy);
  wxSize  s(TRACK_WIDTH, (long)(TRACK_HEIGHT * VertZoomFactor));
  printf("adding SEQTRACK %d with Y=%d\n", Seq->Tracks.size() + 1, yy);
  n1 = new SeqTrack(Seq->Tracks.size() + 1, TrackView, p, s, is_audio);
  //  n1 = new SeqTrack(Seq->Tracks.size(), TrackView, p, s, is_audio);
  printf("adding SEQTRACK PATTERN\n");
  n2 = new SeqTrackPattern(SeqView, n1, 
			(long)(Seq->EndPos * MEASURE_WIDTH * HoriZoomFactor));
  printf("adding TRACK\n");
  n = new Track(n1, n2, is_audio ? IS_AUDIO_TRACK : IS_MIDI_TRACK);
  if (is_audio)
    {
      n1->ChanGui = MixerPanel->AddChannel(n->Output, n1->Text->GetValue());
      n1->ChanGui->SetOpt(n1);
    }
  Seq->AddTrack(n);
  UpdateTracks();
  SetScrolling();
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

void					SequencerGui::SetScrolling()
{
  long					z;
  
  if ((z = ((long) floor(Seq->Tracks.size() * TRACK_HEIGHT * VertZoomFactor) + VSCROLL_THUMB_WIDTH
	    - SeqView->GetClientSize().y)) > 0)
    VertScrollBar->SetScrollbar(VertScrollBar->GetThumbPosition(), VSCROLL_THUMB_WIDTH, z, 1, true);
  else
    VertScrollBar->SetScrollbar(VertScrollBar->GetThumbPosition(), VSCROLL_THUMB_WIDTH, VSCROLL_THUMB_WIDTH, 1, true);
  if ((z = ((long) floor(Seq->EndPos * MEASURE_WIDTH * HoriZoomFactor) + HSCROLL_THUMB_WIDTH
	    - SeqView->GetClientSize().x)) > 0)
    HorizScrollBar->SetScrollbar(HorizScrollBar->GetThumbPosition(), HSCROLL_THUMB_WIDTH, z, 1, true);
  else
    HorizScrollBar->SetScrollbar(HorizScrollBar->GetThumbPosition(), HSCROLL_THUMB_WIDTH, HSCROLL_THUMB_WIDTH, 1, true);
}

void					SequencerGui::AdjustHScrolling()
{
  long					thumb_pos;
  long					range;
  double				pos_tmp;
  
  pos_tmp = ((range = HorizScrollBar->GetRange() - HSCROLL_THUMB_WIDTH)) ?
    (((double) (Seq->EndPos * MEASURE_WIDTH *
		HoriZoomFactor - SeqView->GetClientSize().x)
      * (double) (thumb_pos = HorizScrollBar->GetThumbPosition())) /
     (double) range) : 0;
  SeqView->ScrollWindow((long) (floor(CurrentXScrollPos) - floor(pos_tmp)), 0, (const wxRect *) NULL);
  SeqView->SetXScrollValue((long) ceil(pos_tmp));
  SeqView->Refresh();
  RulerPanel->ScrollWindow((long) (floor(CurrentXScrollPos) - floor(pos_tmp)), 0, (const wxRect *) NULL);
  RulerPanel->SetXScrollValue((long) ceil(pos_tmp));
  RulerPanel->Refresh();
  CurrentXScrollPos = pos_tmp;
  FirstMeasure = pos_tmp / (MEASURE_WIDTH * HoriZoomFactor);
  LastMeasure = (pos_tmp + SeqView->GetClientSize().x) / (MEASURE_WIDTH * HoriZoomFactor);
  //  cout << "FIRST VISIBLE MEASURE = " << FirstMeasure << endl;
  //  cout << "LAST VISIBLE MEASURE = " << LastMeasure << endl;
}

void					SequencerGui::AdjustVScrolling()
{
  long					thumb_pos;
  long					range;
  double				pos_tmp;
  
  pos_tmp = ((range = VertScrollBar->GetRange() - VSCROLL_THUMB_WIDTH)) ?
    (((double) (Seq->Tracks.size() * TRACK_HEIGHT *
		VertZoomFactor - SeqView->GetClientSize().y)
      * (double) (thumb_pos = VertScrollBar->GetThumbPosition())) /
     (double) range) : 0;
  SeqView->ScrollWindow(0, (long) (floor(CurrentYScrollPos) - floor(pos_tmp)), (const wxRect *) NULL);
  TrackView->ScrollWindow(0, (long) (floor(CurrentYScrollPos) - floor(pos_tmp)), (const wxRect *) NULL);
  SeqView->SetYScrollValue((long) ceil(pos_tmp));
  SeqView->Refresh();
  TrackView->Refresh();
  CurrentYScrollPos = pos_tmp;
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

void					SequencerGui::OnVertSliderUpdate(wxCommandEvent &event)
{
  VertScrollBar->SetThumbPosition(0);
  VertZoomFactor = VertZoomSlider->GetValue() / 100.f;
  UpdateTracks();
  SetScrolling();
  AdjustVScrolling();
  PutCursorsOnTop();
}

void					SequencerGui::OnHoriSliderUpdate(wxCommandEvent &event)
{
  HorizScrollBar->SetThumbPosition(0);
  HoriZoomFactor = HoriZoomSlider->GetValue() / 100.f;
  UpdateMeasures();
  SetScrolling();
  AdjustHScrolling();
  RedrawCursors();
}

void					SequencerGui::UpdateMeasures()
{
  long					x_begin;
  long					mes;
  long					x_end;
  long					h_len;
  long					h;
  vector<Track *>::iterator		t;
  vector<wxStaticLine *>::iterator	m;
  vector<Pattern *>::iterator		p;

  h = (long) (VertZoomFactor * TRACK_HEIGHT) * Seq->Tracks.size();
  mes = (long) ceil(MEASURE_WIDTH * HoriZoomFactor);
  x_end = (long) (Seq->EndPos * mes);
  h_len = (long) SeqView->GetSize().x;
  if (h_len < x_end)
    h_len = x_end;
  for (t = Seq->Tracks.begin(); t != Seq->Tracks.end(); t++)
    {
      (*t)->TrackPattern->Update(h_len);
      for (p = (*t)->TrackPattern->Patterns.begin(); p != (*t)->TrackPattern->Patterns.end(); p++)
	(*p)->Update();
    }  
  for (x_begin = 0, m = Measures.begin(); (x_begin < x_end) && (m != Measures.end()); m++)
    {
      (*m)->SetPosition(wxPoint(x_begin, 0));
      (*m)->SetSize(wxSize(1, h));      
      x_begin += mes;
    }
}

void					SequencerGui::OnPaint(wxPaintEvent &event)
{
  wxPaintDC				dc(this);

  PrepareDC(dc);
  dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
  dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxSOLID));
  dc.DrawRectangle(0, 0, GetSize().x, GetSize().y);
}

void					SequencerGui::OnSize(wxSizeEvent &event)
{
  printf(" FUCK SIZE WIDTH %d HEIGTH %d\n", GetSize().x, GetSize().y);
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
    delete (*i);
  Seq->Tracks.clear();
  UpdateTracks();
  SetScrolling();
}

void					SequencerGui::DeleteSelectedTrack()
{
  vector<Track *>::iterator		i;
  long					j;

  //printf("SequencerGui::DeleteSelectedTrack()\n");
  
  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
    if ((*i)->TrackOpt->GetSelected())
      {
	if ((*i)->TrackOpt->Record && Seq->Recording)
	  return;
	if ((*i)->TrackOpt->ChanGui)
	  {
	    MixerPanel->RemoveChannel((*i)->TrackOpt->ChanGui);
	  }
	SeqMutex.Lock();
	delete *i;
	Seq->Tracks.erase(i);
	SeqMutex.Unlock();

	break;
      }
  // mise a jour des index des tracks
  SeqMutex.Lock();
  for (i = Seq->Tracks.begin(), j = 0; i != Seq->Tracks.end(); i++)
    {
      (*i)->UpdateIndex(j);
      j++;
    }
  UpdateTracks();
  SeqMutex.Unlock();

  SetScrolling();
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
  Pattern				*p;
  
  //SeqMutex.Lock();
  for (j = CopyItems.begin(); j != CopyItems.end(); j++)
    {
      p = *j;
      p->CreateCopy(p->GetEndPosition());
    }
  if (DoCut)
    DeleteSelectedPatterns();
  //SeqMutex.Unlock();
}

void					SequencerGui::DeleteSelectedPatterns()
{
  vector<Pattern *>::iterator		i, j;
  
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
    if ((*i) == p)
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

  p = (Pattern *)event.GetEventObject();
  p->Modify(-1, Seq->CurrentPos);// - p->Position);
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
  wxMutexLocker m(SeqMutex);

  Seq->BeginLoopPos = BeginLoopCursor->GetPos();
}

void					SequencerGui::OnEndLCursorMove(wxCommandEvent &event)
{
  wxMutexLocker m(SeqMutex);

  Seq->EndLoopPos = EndLoopCursor->GetPos();
}

void					SequencerGui::OnEndCursorMove(wxCommandEvent &event)
{
  wxMutexLocker m(SeqMutex);

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

  //  Tool = ID_TOOL_PAINT_SEQUENCER;
  //  ChangeMouseCursor(wxCursor(wxCURSOR_PAINT_BRUSH));
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
  //EVT_MOTION	    (SequencerGui::OnMouseEvent) 
  //EVT_LEFT_DOWN	    (SequencerGui::OnLeftIsDownEvent)
  EVT_SIZE(SequencerGui::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SequencerView, wxScrolledWindow)
  EVT_PAINT(SequencerView::OnPaint)
  EVT_LEFT_DOWN(SequencerView::OnClick)
  EVT_RIGHT_DOWN(SequencerView::OnRightClick)
  EVT_ENTER_WINDOW(SequencerView::OnHelp)
END_EVENT_TABLE()

