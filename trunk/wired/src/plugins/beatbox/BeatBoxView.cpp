#include <math.h>
#include "BeatBox.h"

BEGIN_EVENT_TABLE(Ruler, wxWindow)
END_EVENT_TABLE()

Ruler::Ruler(wxWindow *parent, wxWindowID id, 
	const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxBLUE);
  /*wxStaticText* text = 
    new wxStaticText(this, -1, _T("This is the ruler view"), wxPoint(0,0), 
		     wxDefaultSize);
  */
}

Ruler::~Ruler()
{}


BEGIN_EVENT_TABLE(BeatBoxTrackView, wxWindow)
  EVT_PAINT(BeatBoxTrackView::OnPaint)
END_EVENT_TABLE()

BeatBoxTrackView::BeatBoxTrackView(wxWindow *parent,  wxWindowID id,
				   const wxPoint &pos, const wxSize &size,
				     BeatBoxView* view_ptr)
  : wxWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxGREEN);
  /*wxStaticText* text = 
    new wxStaticText(this, -1, _T("This is the track view"), wxPoint(0,0), 
		     wxDefaultSize);
  */
  ViewPtr = view_ptr;
  BeatTrack* beat_track;
  //wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);
  
  for (int i = 0; i < 11; i++)
    {
      try 
	{
	  beat_track = new BeatTrack(ViewPtr->DRM31->Channels[i]);
	  //beat_track = new BeatTrack(this, -1, wxPoint(0, i * ViewPtr->TrackHeight), wxSize(TRACK_WIDTH, ViewPtr->TrackHeight), ViewPtr->DRM31->Channels[i]);
	  //top->Add(beat_track, 1, 0, 0);
	}
      catch (std::bad_alloc)
	{
	  cout << "[BeatBoxView] bad alloc" << endl;
	}
      catch (...) 
	{ 
	  cout <<"[BeatBoxView] unexpected error during new" << endl; 
	}
      BeatTracks.push_back(beat_track);
    }
  //SetSizer(top);
}

BeatBoxTrackView::~BeatBoxTrackView()
{}

void BeatBoxTrackView::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  PrepareDC(dc);
  
  int y = 0 - ViewPtr->YScroll;
  
  
  dc.SetPen(*wxMEDIUM_GREY_PEN); 
  
  dc.SetBrush(wxBrush(*wxBLACK, wxTRANSPARENT));//*wxLIGHT_GREY_BRUSH); 
  
  for (vector<BeatTrack*>::iterator bt = BeatTracks.begin(); 
       bt != BeatTracks.end(); bt++)
    {
      dc.DrawRoundedRectangle(0, y, TRACK_WIDTH, ViewPtr->TrackHeight, 3);
      y += ViewPtr->TrackHeight;
    }
}

//BEGIN_EVENT_TABLE(BeatTrack, wxWindow)
  //EVT_PAINT(BeatTrack::OnPaint)
//END_EVENT_TABLE()

//BeatTrack::BeatTrack(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, BeatBoxChannel* channel)
BeatTrack::BeatTrack(BeatBoxChannel* channel)
  //  : wxWindow(parent, id, pos, size)
{
  Channel = channel;
  /*
    wxStaticText* text = 
    new wxStaticText(this, -1, _T("channel"), wxPoint(0,0), 
		     wxDefaultSize);
  */
}

BeatTrack::~BeatTrack()
{}
/*
void BeatTrack::OnPaint(wxPaintEvent& event)
{
wxPaintDC dc(this);
  wxSize s;
  
  PrepareDC(dc);
  s = GetSize();
  dc.SetPen(*wxMEDIUM_GREY_PEN); 
  
  dc.SetBrush(wxBrush(*wxBLACK, wxTRANSPARENT));//*wxLIGHT_GREY_BRUSH); 
  dc.DrawRoundedRectangle(0, 0, s.x, s.y, 3);
  
}
*/

BEGIN_EVENT_TABLE(BeatBoxScrollView, wxScrolledWindow)
  EVT_PAINT(BeatBoxScrollView::OnPaint)
END_EVENT_TABLE()

BeatBoxScrollView::BeatBoxScrollView(wxWindow *parent, wxWindowID id,
				     const wxPoint &pos, const wxSize &size,
				     BeatBoxView* view_ptr)
  : wxScrolledWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxWHITE);
  ViewPtr = view_ptr;
  /*
    wxStaticText* text = 
    new wxStaticText(this, -1, _T("This is the scroll view"), wxPoint(0,0), 
		     wxDefaultSize);
  */
  
}

BeatBoxScrollView::~BeatBoxScrollView()
{}

void BeatBoxScrollView::OnPaint(wxPaintEvent&event)
{
  wxPaintDC dc(this);
  wxSize size;
  double x;
  double u;
  long	 m;
  long y = 0 - ViewPtr->YScroll;
  
  PrepareDC(dc);
  size = GetClientSize();
  
  dc.SetPen(wxPen(VIEW_BGCOLOR, 1, wxSOLID));
  dc.SetBrush(wxBrush(VIEW_BGCOLOR));
  dc.SetTextForeground(VIEW_FGCOLOR);
  
  for (vector<BeatTrack*>::iterator bt = ViewPtr->TrackView->BeatTracks.begin();
       bt != ViewPtr->TrackView->BeatTracks.end(); 
       bt++)
    {
      y += ViewPtr->TrackHeight;
      dc.DrawLine(0, y, GetClientSize().x, y);
    }
}


BEGIN_EVENT_TABLE(BeatBoxView, wxPanel)
  EVT_SLIDER(ID_HZoom, BeatBoxView::OnHZoom)
  EVT_SLIDER(ID_VZoom, BeatBoxView::OnVZoom)
  EVT_COMMAND_SCROLL(ID_HScroll, BeatBoxView::OnHScroll)
  EVT_COMMAND_SCROLL(ID_VScroll, BeatBoxView::OnVScroll)
  EVT_SIZE(BeatBoxView::OnSize)
  //EVT_PAINT(BeatBoxView::OnPaint)
END_EVENT_TABLE()

BeatBoxView::BeatBoxView(wxWindow* parent, wxWindowID id, WiredBeatBox* bb,
			 const wxPoint& pos, const wxSize& size, 
			 wxMutex* mutex)
  : wxPanel(parent, id, pos, size)
{
  Mutex = mutex;
  DRM31 = bb;
  
  
  HZoom = VZoom = 1.0;
  XScroll = YScroll = 0;
  TrackHeight = BEAT_HEIGHT;
  
  wxBoxSizer *col_1;
  wxBoxSizer *col_2;
  wxBoxSizer *glob;
  
  HZoomSlider = new wxSlider(this, ID_HZoom, 100, 100, 400, 
			     wxPoint(0,0), 
			     wxSize(TRACK_WIDTH,RULER_HEIGHT)); 
  RulerView = new Ruler(this, -1, wxPoint(TRACK_WIDTH,0), 
			wxSize(GetClientSize().x - TRACK_WIDTH, RULER_HEIGHT));
  
  TrackView = new BeatBoxTrackView(this, -1, wxPoint(0,RULER_HEIGHT), 
				   wxSize(TRACK_WIDTH, GetClientSize().y),
				   this);
  /*
  BeatTrack* beat_track;
  for (int i = 0; i < 11; i++)
    {
      try 
	{
	  beat_track = new BeatTrack(TrackView, -1,wxPoint(0, i * TrackHeight),
				     wxSize(TRACK_WIDTH, TrackHeight), 
				     DRM31->Channels[i]);
	}
      catch (std::bad_alloc)
	{
	  cout << "[BeatBoxView] bad alloc" << endl;
	}
      catch (...) 
	{ 
	  cout <<"[BeatBoxView] unexpected error during new" << endl; 
	}
      TrackView->BeatTracks.push_back(beat_track);
    }
  */
  BeatView = new BeatBoxScrollView(this, -1, wxPoint(TRACK_WIDTH,RULER_HEIGHT),
				   GetClientSize()-wxSize(150,20), this);
  
  VZoomSlider = new wxSlider(this, ID_VZoom, 100, 100, 400, 
			     wxPoint(0,GetClientSize().y-RULER_HEIGHT), 
			     wxSize(TRACK_WIDTH, RULER_HEIGHT));
  
  HScrollBar = new wxScrollBar(this, ID_HScroll, wxDefaultPosition, 
			       wxSize(-1, RULER_HEIGHT), wxSB_HORIZONTAL);
  
  VScrollBar = new wxScrollBar(this, ID_VScroll, wxDefaultPosition, 
			       wxSize(RULER_HEIGHT, -1), wxSB_VERTICAL);
  
  
  col_1 = new wxBoxSizer(wxVERTICAL);
  col_1->Add(HZoomSlider, 0, 0, 0);
  col_1->Add(TrackView, 1, 0, 0);
  col_1->Add(VZoomSlider, 0,0,0);
  
  col_2 = new wxBoxSizer(wxVERTICAL);
  col_2->Add(RulerView, 0, wxEXPAND, 0);
  
  wxBoxSizer* row;
  
  row = new wxBoxSizer(wxHORIZONTAL);
  row->Add(BeatView, 1, wxEXPAND, 0);
  row->Add(VScrollBar, 0, wxEXPAND, 0);
  col_2->Add(row, 1, wxEXPAND, 0);
  col_2->Add(HScrollBar, 0, wxEXPAND, 0);
  
  glob = new wxBoxSizer(wxHORIZONTAL);
  glob->Add(col_1, 0, wxEXPAND, 0);
  glob->Add(col_2, 1, wxEXPAND, 0);
  
  SetSizer(glob);
  
  XSize = BeatView->GetClientSize().x;
  YSize = BEAT_HEIGHT * TrackView->BeatTracks.size();
  
  AdjustHScrolling();
  AdjustVScrolling();
}

BeatBoxView::~BeatBoxView()
{
  delete HZoomSlider;
  delete VZoomSlider;
}

void BeatBoxView::OnHZoom(wxCommandEvent& event)
{
  HZoom = static_cast<double>(HZoomSlider->GetValue() / 100.0);
  XSize = static_cast<long>(floor(BeatView->GetClientSize().x * HZoom));
  AdjustHScrolling();
  RulerView->Refresh();
  BeatView->Refresh();
}

void BeatBoxView::OnVZoom(wxCommandEvent& event)
{
  VZoom = static_cast<double>(VZoomSlider->GetValue() / 100.0);
  TrackHeight = static_cast<long>(floor(BEAT_HEIGHT * VZoom));
  
  YSize =  TrackHeight * TrackView->BeatTracks.size();
  
  
  AdjustVScrolling();
  BeatView->Refresh();
  /*
    for (vector<BeatTrack*>::iterator bt = TrackView->BeatTracks.begin(); 
       bt != TrackView->BeatTracks.end(); bt++)
    {
      (*bt)->SetSize(TRACK_WIDTH, TrackHeight);
    }
  */
  //TrackView->SetSize(TRACK_WIDTH, YSize);
  TrackView->Refresh();
}

void BeatBoxView::OnHScroll(wxScrollEvent& event)
{
  XScroll = HScrollBar->GetThumbPosition();
  
}

void BeatBoxView::OnVScroll(wxScrollEvent& event)
{
  long yscroll = VScrollBar->GetThumbPosition();

  BeatView->ScrollWindow( 0, YScroll - yscroll, (const wxRect *) NULL);
  TrackView->ScrollWindow( 0, YScroll - yscroll, (const wxRect *) NULL);
  
  YScroll = yscroll;
  BeatView->Refresh();
  TrackView->Refresh();
}

void BeatBoxView::OnSize(wxSizeEvent& event)
{
  Layout();
  XSize = static_cast<long>(floor(BeatView->GetClientSize().x * HZoom));
  AdjustHScrolling();
  AdjustVScrolling();
}

inline void BeatBoxView::AdjustHScrolling(void)
{
  HScrollBar->SetScrollbar(XScroll, BeatView->GetClientSize().x, 
			   XSize, 1, true);
}

inline void BeatBoxView::AdjustVScrolling(void)
{
  VScrollBar->SetScrollbar(XScroll, BeatView->GetClientSize().y, 
			   YSize, TrackHeight, true);
}

/*
void BeatBoxView::OnPaint(wxPaintEvent& event)
{
  
}
*/
