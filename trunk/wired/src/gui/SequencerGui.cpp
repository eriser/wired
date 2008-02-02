// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <cmath>
#include <iostream>
#include "SequencerGui.h"
#include "MLTree.h"
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
#include "AudioCenter.h"
#include "MidiDevice.h"
#include "Settings.h"
#include "WriteWaveFile.h"

#ifdef DEBUG_SEQUENCERGUI
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : "  << __LINE__ << " : " << __FUNCTION__  << endl; }
#else
#define LOG
#endif

DEFINE_EVENT_TYPE(EVT_DROP)
const struct s_combo_choice		ComboChoices[NB_COMBO_CHOICES + 1] =
{
  { _("Bar")	,	1	},
  { wxT("1/2")	,	2	},
  { wxT("1/4")	,	4	},
  { wxT("1/8")	,	8	},
  { wxT("1/16")	,	16	},
  { wxT("1/32")	,	32	},
  { wxT("1/64")	,	64	},
  { wxT("1/128"),	128	},
  { wxT("1/256"),	256	},
  { wxT("")	,	4242	}
};

SequencerGui::SequencerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size,
			   wxWindow *mainwindow, WiredDocument* docParent)
  : wxPanel(parent, -1, pos, size, wxSIMPLE_BORDER | wxWS_EX_PROCESS_IDLE),
    WiredDocument(wxT("SequencerGui"), docParent)
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

  //Floating = false;
  CurrentXScrollPos = 0;
  CurrentYScrollPos = 0;
  Tool = ID_TOOL_MOVE_SEQUENCER;
  HoriZoomFactor = 1.0f;
  VertZoomFactor = 1.0f;
  CurrentPos = 0.0;
  DoCut = false;
  VertNowPos = 0;
  HorizNowPos = 0;
  SetBackgroundColour(CL_SEQ_BACKGROUND);
  SetForegroundColour(CL_SEQ_FOREGROUND);
  Toolbar = new wxToolBar(this, -1, wxPoint(-1, -1), wxSize(-1, TOOLS_HEIGHT), wxTB_FLAT);
  Toolbar->SetToolBitmapSize(wxSize(ICONS_SIZE,ICONS_SIZE));
  Toolbar->AddRadioTool(ID_SEQ_MOVE, _("Move"), wxBitmap(WiredSettings->DataDir + HAND_UP, wxBITMAP_TYPE_PNG), wxBitmap(WiredSettings->DataDir + HAND_DOWN, wxBITMAP_TYPE_PNG), _("Move Pattern"), _("Move Pattern"), NULL);
  Toolbar->AddRadioTool(ID_SEQ_DRAW, _("Draw"), wxBitmap(WiredSettings->DataDir + DRAW_UP, wxBITMAP_TYPE_PNG), wxBitmap(WiredSettings->DataDir + DRAW_DOWN, wxBITMAP_TYPE_PNG), _("Draw Pattern"), _("Draw Pattern"), NULL);
  Toolbar->AddRadioTool(ID_SEQ_DEL, _("Delete"), wxBitmap(WiredSettings->DataDir + ERASE_UP, wxBITMAP_TYPE_PNG), wxBitmap(WiredSettings->DataDir + ERASE_DOWN, wxBITMAP_TYPE_PNG), _("Delete Pattern"), _("Deletes notes"), NULL);
  Toolbar->AddRadioTool(ID_SEQ_SPLIT, _("Split"), wxBitmap(WiredSettings->DataDir + SPLIT_UP, wxBITMAP_TYPE_PNG), wxBitmap(WiredSettings->DataDir + SPLIT_DOWN, wxBITMAP_TYPE_PNG), _("Split Pattern"), _("Split Pattern"), NULL);
  Toolbar->AddRadioTool(ID_SEQ_MERGE, _("Merge"), wxBitmap(WiredSettings->DataDir + MERGE_UP, wxBITMAP_TYPE_PNG), wxBitmap(WiredSettings->DataDir + MERGE_DOWN, wxBITMAP_TYPE_PNG), _("Merge Patterns"), _("Merge Patterns"), NULL);
  Toolbar->AddRadioTool(ID_SEQ_COLOR, _("Color"), wxBitmap(WiredSettings->DataDir + COLOR_UP, wxBITMAP_TYPE_PNG), wxBitmap(WiredSettings->DataDir + COLOR_DOWN, wxBITMAP_TYPE_PNG),  _("Color Pattern"),  _("Delete Pattern"), NULL);

  BrushColor = CL_DEFAULT_SEQ_BRUSH;
  ColorBox = new ColoredBox(Toolbar, ID_SEQ_COLORBOX, wxDefaultPosition, wxSize(ICONS_SIZE,ICONS_SIZE), BrushColor, CL_DEFAULT_SEQ_PEN);
  Toolbar->AddControl(ColorBox);

  ColorDialogBox = new wxColourDialog(mainwindow, 0);
  Connect(ID_SEQ_COLORBOX, wxEVT_SCROLL_TOP, (wxObjectEventFunction)(wxEventFunction)(wxScrollEventFunction) &SequencerGui::OnColoredBoxClick);
  Toolbar->AddSeparator();
  Toolbar->AddCheckTool(ID_SEQ_MAGNET, _("Magnet"), wxBitmap(WiredSettings->DataDir + MAGN_UP, wxBITMAP_TYPE_PNG), wxBitmap(WiredSettings->DataDir + MAGN_DOWN, wxBITMAP_TYPE_PNG), _("Magnetize"), _("Magnetize"), NULL);
  for (c = 0; c < NB_COMBO_CHOICES; c++)
    combo_choices[c] = ComboChoices[c].s;
  MagnetQuant = new wxComboBox(Toolbar, ID_SEQ_COMBO_MAGNET, DEFAULT_MAGNETISM_COMBO_VALUE,
			       wxPoint(-1, -1), wxSize(72, -1), 9, combo_choices, wxCB_READONLY);
  Toolbar->AddControl(MagnetQuant);
  Toolbar->AddSeparator();
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
				wxMAXIMIZE_BOX | wxNO_BORDER, wxDefaultValidator, _("Vertical Zoom"));
  HoriZoomSlider = new wxSlider(this, ID_SEQ_HSLIDER, 100, 25, 400, wxPoint(-1, -1),
				wxSize(-1, HorizScrollBar->GetSize().y));
  RulerPanel = new Ruler(this, ID_SEQ_RULER, wxPoint(-1, -1), wxSize(-1, RULER_HEIGHT));
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
  PopMenu->Append(ID_POPUP_MOVE_TO_CURSOR, _("Move to cursor"));
  PopMenu->Append(ID_POPUP_DELETE, _("Delete"));
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_POPUP_CUT, _("Cut"));
  PopMenu->Append(ID_POPUP_COPY, _("Copy"));
  PopMenu->Append(ID_POPUP_PASTE, _("Paste"));
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_POPUP_SELECT_ALL, _("Select all"));

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
  CleanChildren();
}

void					SequencerGui::CleanChildren()
{
  //DeleteAllTracks();
}

int					SequencerGui::GetSeqHeaderHeight()
{
  return ((int)floor(TRACK_HEIGHT * VertZoomFactor));
}

int					SequencerGui::GetCurrentYScrollPos()
{
  return ((int)floor(CurrentYScrollPos));
}

int					SequencerGui::GetCurrentXScrollPos()
{
  return ((int)floor(CurrentXScrollPos));
}

Track					*SequencerGui::CreateTrack(trackType type)
{
  LOG;
  // create a new track
  long		height = (long) floor(TRACK_HEIGHT * VertZoomFactor);
  Track*	newTrack;
  wxPoint	pos;
  wxSize	size(TRACK_WIDTH, height);

  // default insertion (insert to the end)
  pos = wxPoint(0, SeqView->GetTotalHeight() - (long) floor(CurrentYScrollPos));

  newTrack = new Track((WiredDocument*)Seq, type, pos, size, TrackView);

  UpdateTracks();
  SeqPanel->SetScrolling();
  ReSizeCursors();

  // refresh on-the-fly sequencer view (maybe it should be in only one method)
  wxClientDC				drawme(SeqView);
  PrepareDC(drawme);
  SeqView->DrawMeasures(drawme);
  SeqView->DrawTrackLines(drawme);

  return (newTrack);
}

void					SequencerGui::DeleteTrack(Track* track)
{
  // We need to keep SeqMutex locked until re-indexation is done
  wxMutexLocker				locker(SeqMutex);

  if (track)
    {
      // we should stop recording or something else instead return
      if (track->GetTrackOpt()->Record && Seq->Recording)
	return;

      delete track;
      ReindexTrackArray();

      // Refresh things
      UpdateTracks();
      SetScrolling();
      AdjustVScrolling();
    }
}

void					SequencerGui::ReindexTrackArray()
{
  // Change track index for each still existing tracks. Sort of reindexing.
  vector<Track *>::iterator		iterTrack;
  int					j;

  j = 0;
  for (iterTrack = Seq->Tracks.begin(); iterTrack != Seq->Tracks.end(); iterTrack++)
    (*iterTrack)->UpdateIndex(j++);
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
    for (p = (*t)->GetTrackPattern()->Patterns.begin(); p != (*t)->GetTrackPattern()->Patterns.end(); p++)
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
  {
    VertScrollBar->SetScrollbar((VertScrollBar->GetThumbPosition() >= z) ? z : VertScrollBar->GetThumbPosition(), VSCROLL_THUMB_WIDTH, z + VSCROLL_THUMB_WIDTH, 42, true);
  }
  else
  {
    VertScrollBar->SetScrollbar(0, VSCROLL_THUMB_WIDTH, VSCROLL_THUMB_WIDTH, 42, true);
  }
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
      (*i)->GetTrackOpt()->SetPosition(wxPoint(0, z));
      (*i)->GetTrackOpt()->SetSize(wxSize(TRACK_WIDTH, h));
      (*i++)->GetTrackOpt()->Refresh();
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
      //      (*t)->GetTrackPattern()->Update();
      for (p = (*t)->GetTrackPattern()->Patterns.begin(); p != (*t)->GetTrackPattern()->Patterns.end(); p++)
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
    if ((*i)->GetTrackOpt()->GetSelected())
      (*i)->GetTrackOpt()->SetSelected(false);
}

void					SequencerGui::MovePattern(Pattern *p,
								  long oldTrackIndex,
								  long newTrackIndex)
{
      /*
  vector<Track *>::iterator		iter;

  UnselectTracks();
  for (iter = Seq->Tracks.begin(); iter != Seq->Tracks.end(); iter++)
    if ((*iter)->GetIndex() == newTrackIndex)
      (*iter)->AddPattern(p);
    */
  if (oldTrackIndex < Seq->Tracks.size() && newTrackIndex < Seq->Tracks.size())
    {
      Seq->Tracks[oldTrackIndex]->DelPattern(p);
      Seq->Tracks[newTrackIndex]->AddPattern(p);
      p->ChangeParent(Seq->Tracks[newTrackIndex]);
    }
}

void					SequencerGui::DelPattern(Pattern *p, long trackindex)
{
  vector<Track *>::iterator		iter;

  UnselectTracks();
  for (iter = Seq->Tracks.begin(); iter != Seq->Tracks.end(); iter++)
    if ((*iter)->GetIndex() == trackindex)
      (*iter)->DelPattern(p);
}

bool					SequencerGui::IsAudioTrack(long trackindex)
{
  vector<Track *>::iterator		iter;

  UnselectTracks();
  for (iter = Seq->Tracks.begin(); iter != Seq->Tracks.end(); iter++)
    if ((*iter)->GetIndex() == trackindex)
      return (*iter)->IsAudioTrack();
}

void					SequencerGui::SelectTrack(long trackindex)
{
  vector<Track *>::iterator		iter;

  UnselectTracks();
  for (iter = Seq->Tracks.begin(); iter != Seq->Tracks.end(); iter++)
    if ((*iter)->GetIndex() == trackindex)
      (*iter)->GetTrackOpt()->SetSelected(true);
}

void					SequencerGui::SwapTracksPos(Track *t1, Track *t2)
{
  long					z;

  z = t1->GetTrackOpt()->GetPosition().y;
  t1->GetTrackOpt()->SetPosition(wxPoint(0, t2->GetTrackOpt()->GetPosition().y));
  t2->GetTrackOpt()->SetPosition(wxPoint(0, z));
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

	for (i = Seq->Tracks.begin(); (i != Seq->Tracks.end()) && !((*i)->GetTrackOpt()->GetSelected()); i++);
	if (i == Seq->Tracks.end())
    	return;
	if ((z = trackindexdelta) > 0)
	    for (j = i++, x = 0; (x < z) && (i != Seq->Tracks.end()); x++)
    	{
			SwapTracksPos(*i, *j);
			(*i)->UpdateIndex((*i)->GetIndex() - 1);
			u.push_back(*i);
			t = *i;
			*(i++) = *j;
			*(j++) = t;
	    }
	else
    	for (j = i--, x = 0; (x > z) && (j != Seq->Tracks.begin()); x--)
		{
			SwapTracksPos(*i, *j);
			(*i)->UpdateIndex((*i)->GetIndex() + 1);
			u.push_back(*i);
			t = *i;
			*(i--) = *j;
			*(j--) = t;
		}
	(*j)->UpdateIndex((*j)->GetIndex() + x);
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

// remove reference to Plug for each Track
void					SequencerGui::RemoveReferenceTo(Plugin *plug)
{
  vector<Track *>::iterator		i;

  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
    (*i)->GetTrackOpt()->RemoveReferenceTo(plug);
}

void					SequencerGui::RefreshConnectMenu()
{
  wxMutexLocker				locker(SeqMutex);
  vector<Track *>::iterator		i;

  for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
    (*i)->GetTrackOpt()->RebuildConnectList();
}

void					SequencerGui::DeleteAllTracks()
{
  wxMutexLocker				locker(SeqMutex);
  vector<Track *>::iterator		i;
  vector<Track *>::iterator		next;

  i = Seq->Tracks.begin();
  while (i != Seq->Tracks.end())
    {
      next = i + 1;
      delete (*i);
      i = next;
    }

  // should be unnecessary
  Seq->Tracks.clear();

  UpdateTracks();
  SetScrolling();
}

void					SequencerGui::DeleteSelectedTrack()
{
  vector<Track *>::iterator		iterTrack;
  long							j;

#ifdef __DEBUG__
  printf("SequencerGui::DeleteSelectedTrack()\n");
#endif

  for (iterTrack = Seq->Tracks.begin(); (iterTrack != Seq->Tracks.end()) && !((*iterTrack)->GetTrackOpt()->GetSelected()); iterTrack++)
    ;
  if (iterTrack == Seq->Tracks.end())
    return;
  DeleteTrack(*iterTrack);
}

void					SequencerGui::HideAllPatterns(wxMouseEvent &e)
{
  SeqPanel->SelectItem(0x0, e.ShiftDown());
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
	for (i = SelectedItems.begin(); (i != SelectedItems.end()) && (*i != p); i++)
	  ;
	if(i != SelectedItems.end())
	  SelectedItems.erase(i);
      }
}

void					SequencerGui::CopySelectedItems()
{
  vector<Pattern *>::iterator		j;
  vector<Pattern *>::iterator		save;
  int					i;
  int					nb_item_track;
  int					cpt_track;
  double				end_position;
  double				last_position;

  CopyItems.clear();

  for (cpt_track = 0; cpt_track != Seq->Tracks.size();)
    {
      last_position = 0;
      for (nb_item_track = 0, j = SelectedItems.begin(); j != SelectedItems.end(); j++)
	if (((Pattern *) *j)->GetTrackIndex() == cpt_track)
	  nb_item_track++;
       for (i = 0; i < nb_item_track;)
	{
	  for (j = SelectedItems.begin(), end_position = 50000000; j != SelectedItems.end(); j++)
	    {
	      if (((Pattern *) *j)->GetEndPosition() < end_position && ((Pattern *) *j)->GetEndPosition() > last_position && ((Pattern *) *j)->GetTrackIndex() == cpt_track)
		{
		  save = j;
		  end_position = ((Pattern *) *j)->GetEndPosition();
		}

	    }
	  if (end_position != 50000000)
	    {
	      last_position = end_position;
	      CopyItems.push_back(*save);
	      i++;
	    }

	}
      cpt_track++;
    }
}

void					SequencerGui::PasteItems()
{
  vector<Pattern *>::iterator		itSelected;
  vector<Pattern *>::iterator		itSelected_save;
  vector<Track *>::iterator		itTrackSelected;
  double       				last_pos = 0;
  double       				begin_pos = 0;
  double       				last_pos_save = 0;
  Pattern				*pattern;
  vector<Pattern *>::iterator		pattern_iterator;

  for (itSelected_save = itSelected = CopyItems.begin(); itSelected != CopyItems.end(); itSelected++)
    {
      for (itTrackSelected = Seq->Tracks.begin(); itTrackSelected != Seq->Tracks.end()
	     && (*itTrackSelected)->GetIndex() != (*itSelected)->GetTrackIndex();
	   itTrackSelected++)
	;

      if (itTrackSelected == Seq->Tracks.end())
	{
	  cout << "WARNING : Pattern NOT belongs to track" << endl;
	  return;
	}
      if (((Pattern *) *itSelected)->GetTrackIndex() != ((Pattern *) *itSelected_save)->GetTrackIndex())
	last_pos = 0;
      for (pattern_iterator = (*itTrackSelected)->TrackPattern->Patterns.begin();
	   pattern_iterator != (*itTrackSelected)->TrackPattern->Patterns.end();
	   pattern_iterator++)
	if (last_pos < (*pattern_iterator)->GetEndPosition())
	  {
	    last_pos = (*pattern_iterator)->GetEndPosition();
	  }
      if (itSelected != CopyItems.begin())
	{

	  while (((Pattern *) *itSelected)->GetTrackIndex() != ((Pattern *) *itSelected_save)->GetTrackIndex())
	    {
	      if (itSelected_save == CopyItems.begin())
		{
		  itSelected_save = itSelected;
		  break;
		}
	      itSelected_save--;
	    }
	  if (itSelected_save != itSelected)
	    {
	      last_pos_save = ((Pattern *) *itSelected_save)->GetEndPosition();
	      begin_pos = ((Pattern *) *itSelected)->GetPosition();
	    }
	  else
	    {
	      last_pos_save = 0;
	      begin_pos = 0;
	    }
	}


      pattern = ((Pattern *) *itSelected)->CreateCopy(last_pos + (begin_pos - last_pos_save));

      itSelected_save = itSelected;
    }
  if (DoCut)
    DeleteSelectedPatterns();

}

void					SequencerGui::DeleteSelectedPatterns()
{
  vector<Pattern *>			Patterns;
  vector<Pattern *>::iterator		itPattern;

  // we made a copy before deleting some items of SelectedItems vector.
  Patterns = SelectedItems;
  for (itPattern = Patterns.begin(); itPattern != Patterns.end(); itPattern++)
    DeletePattern(*itPattern);

  // we don't need to clear SelectedItems,
  // because each iteration are deleted in DeletePattern
}

void					SequencerGui::DeletePattern(Pattern *p)
{
  vector<Pattern *>::iterator		i;

  if (!p)
    return;
  WaveCenter.RemoveWaveFile(((AudioPattern*)(p))->GetWaveFile());
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
  delete p;
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
    for (p = (*t)->GetTrackPattern()->Patterns.begin(); p != (*t)->GetTrackPattern()->Patterns.end(); p++)
      (*p)->SetCursor(c);
}

void					SequencerGui::OnResizePattern(wxCommandEvent &event)
{
  Pattern				*p;

  p = (Pattern *) event.GetEventObject();
  if (p)
  {
      p->Modify(-1, Seq->CurrentPos);
      p->Update();
  }
}

void					SequencerGui::OnDrawMidi(wxCommandEvent &event)
{
  MidiPattern				*p;

  cout << "[SequencerGui] OnDrawMidi()" << endl;
  p = (MidiPattern *)event.GetEventObject();
  if (p)
  {
      p->DrawMidi();
      p->Update();
  }
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

void					SequencerGui::OnDrawClick(wxCommandEvent &event)
{
  Tool = ID_TOOL_DRAW_SEQUENCER;
  ChangeMouseCursor(wxNullCursor);
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

void					SequencerGui::OnMergeClick(wxCommandEvent &event)
{
  Tool = ID_TOOL_MERGE_SEQUENCER;
  ChangeMouseCursor(wxCursor(wxCURSOR_HAND));
}

void					SequencerGui::OnMagnetismToggle(wxCommandEvent &event)
{
  Magnetism = Toolbar->GetToolState(ID_SEQ_MAGNET) ? CURSOR_MASK | PATTERN_MASK : 0;
  /*
   *if(Magnetism)
   *  cout << "Magnetism [ OK ]" << endl;
   *else
   *  cout << "Magnetism [ NO ]" << endl;
   */
}

void					SequencerGui::OnMagnetismChange(wxCommandEvent &event)
{
  long					c;
  wxString				s;

  s =  MagnetQuant->GetValue();
  for (c = 0; (c < NB_COMBO_CHOICES) && (s != ComboChoices[c].s); c++);
  CursorMagnetism = (long) floor(ComboChoices[c].value);
  PatternMagnetism = (long) floor(ComboChoices[c].value);
  //cout << "Magnetism change " << MagnetQuant->GetValue() << " and " << ComboChoices[c].value << " " << endl;
}

void					SequencerGui::OnColorButtonClick(wxCommandEvent &event)
{
  vector<Pattern *>::iterator		p;

  Tool = ID_TOOL_PAINT_SEQUENCER;
  ChangeMouseCursor(wxNullCursor);
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

void					SequencerGui::Drop(wxCommandEvent &event)
{
  wxPoint   pos;
  wxString file;

  file = MediaLibraryPanel->MLTreeView->GetFile();
  pos = MediaLibraryPanel->MLTreeView->GetPos();
  SeqView->Drop(pos.x, pos.y, file);

}

// WiredDocument implementation
void					SequencerGui::Save()
{
  saveDocData(new SaveElement(wxT("HoriZoomFactor"), HoriZoomFactor));
  saveDocData(new SaveElement(wxT("VertZoomFactor"), VertZoomFactor));
  saveDocData(new SaveElement(wxT("CurrentXScrollPos"), CurrentXScrollPos));
  saveDocData(new SaveElement(wxT("CurrentYScrollPos"), CurrentYScrollPos));
  saveDocData(new SaveElement(wxT("FirstMeasure"), FirstMeasure));
  saveDocData(new SaveElement(wxT("LastMeasure"),LastMeasure ));
  saveDocData(new SaveElement(wxT("FollowPlayCursor"), FollowPlayCursor));
  saveDocData(new SaveElement(wxT("Magnetism"), Magnetism));
  saveDocData(new SaveElement(wxT("CursorMagnetism"), CursorMagnetism));
  saveDocData(new SaveElement(wxT("PatternMagnetism"), PatternMagnetism));
  saveDocData(new SaveElement(wxT("VertNowPos"), (int)VertNowPos));
  saveDocData(new SaveElement(wxT("HorizNowPos"), (int)HorizNowPos));
  saveDocData(new SaveElement(wxT("DoCut"), DoCut));
}

void					SequencerGui::Load(SaveElementArray data)
{
  wxMutexLocker				m(SeqMutex);
  int					i;

  SetCurrentPos(Seq->CurrentPos);
  for (i = 0; i < data.GetCount(); i++)
    {
      if (data[i]->getKey() == wxT("HoriZoomFactor"))		HoriZoomFactor = data[i]->getValueFloat();
      else if (data[i]->getKey() == wxT("VertZoomFactor"))	VertZoomFactor = data[i]->getValueFloat();
      else if (data[i]->getKey() == wxT("CurrentXScrollPos"))	CurrentXScrollPos = data[i]->getValueDouble();
      else if (data[i]->getKey() == wxT("CurrentYScrollPos"))	CurrentYScrollPos = data[i]->getValueDouble();
      else if (data[i]->getKey() == wxT("FirstMeasure"))	FirstMeasure = data[i]->getValueDouble();
      else if (data[i]->getKey() == wxT("LastMeasure"))		LastMeasure = data[i]->getValueDouble();
      else if (data[i]->getKey() == wxT("FollowPlayCursor"))	FollowPlayCursor = (char)data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("Magnetism"))		Magnetism = (char)data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("CursorMagnetism"))	CursorMagnetism = (unsigned short)data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("PatternMagnetism"))	PatternMagnetism = (unsigned short)data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("VertNowPos"))		VertNowPos = data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("HorizNowPos"))		HorizNowPos = data[i]->getValueInt();
      else if (data[i]->getKey() == wxT("DoCut"))		DoCut = data[i]->getValueInt();
    }
}

void					SequencerGui::SetSelectedSolo()
{
  vector<Track *>::iterator		iterTrack;
  bool					on;

#ifdef __DEBUG__
  cout << "SequencerGui::SetSelectedSolo()" << endl;
#endif

  on = true;
  for (iterTrack = Seq->Tracks.begin(); (iterTrack != Seq->Tracks.end()); iterTrack++)
    if (!(*iterTrack)->GetTrackOpt()->GetSelected() && \
	(*iterTrack)->GetTrackOpt()->GetMute())
      on = false;
  for (iterTrack = Seq->Tracks.begin(); (iterTrack != Seq->Tracks.end()); iterTrack++)
  {
    if (!(*iterTrack)->GetTrackOpt()->GetSelected())
      (*iterTrack)->GetTrackOpt()->SetMute(on);
    else
      (*iterTrack)->GetTrackOpt()->SetMute(false);
  }
}

void	SequencerGui::KeyDown(wxKeyEvent &event)
{
  int	key = event.GetKeyCode();

  cout << "[SEQUENCERGUI] key = " << key << endl;
  switch ( key )
  {
    case WXK_BACK :
      cout << "Backspace captured... deleting selected track" << endl;
      DeleteSelectedTrack();
      break;
    case WXK_RETURN :
      cout << "Enter key captured... setting selected track solo" << endl;
      SetSelectedSolo();
      break;
    default :
      break;
  }
}

BEGIN_EVENT_TABLE(SequencerGui, wxPanel)
  EVT_BUTTON(ID_CURSOR_PLAY, SequencerGui::OnPlayCursorMove)
  EVT_BUTTON(ID_CURSOR_BEGIN, SequencerGui::OnBeginLCursorMove)
  EVT_BUTTON(ID_CURSOR_REPEAT, SequencerGui::OnEndLCursorMove)
  EVT_BUTTON(ID_CURSOR_END, SequencerGui::OnEndCursorMove)
  EVT_SLIDER(ID_SEQ_VSLIDER, SequencerGui::OnVertSliderUpdate)
  EVT_SLIDER(ID_SEQ_HSLIDER, SequencerGui::OnHoriSliderUpdate)
  EVT_COMMAND_SCROLL(ID_SEQ_SCROLLING, SequencerGui::OnScroll)
  EVT_TOOL(ID_SEQ_MOVE, SequencerGui::OnMoveClick)
  EVT_TOOL(ID_SEQ_DRAW, SequencerGui::OnDrawClick)
  EVT_TOOL(ID_SEQ_DEL, SequencerGui::OnEraseClick)
  EVT_TOOL(ID_SEQ_SPLIT, SequencerGui::OnSplitClick)
  EVT_TOOL(ID_SEQ_MERGE, SequencerGui::OnMergeClick)
  EVT_TOOL(ID_SEQ_MAGNET, SequencerGui::OnMagnetismToggle)
  EVT_TOOL(ID_SEQ_COLOR, SequencerGui::OnColorButtonClick)
  EVT_COMBOBOX(ID_SEQ_COMBO_MAGNET, SequencerGui::OnMagnetismChange)
  EVT_SIZE(SequencerGui::OnSize)
  EVT_MOUSEWHEEL(SequencerGui::OnWheelMove)
  EVT_COMMAND(ID_EVT_DROP, EVT_DROP, SequencerGui::Drop)
  EVT_KEY_DOWN(SequencerGui::KeyDown)
END_EVENT_TABLE()
