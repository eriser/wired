#include "BeatBoxView.h"

#include "BeatBox.h"

BEGIN_EVENT_TABLE(Ruler, wxWindow)
END_EVENT_TABLE()

Ruler::Ruler(wxWindow *parent, wxWindowID id, 
	const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxBLUE);
  wxStaticText* text = 
    new wxStaticText(this, -1, _T("This is the ruler view"), wxPoint(0,0), 
		     wxDefaultSize);
  
}

Ruler::~Ruler()
{}


BEGIN_EVENT_TABLE(BeatBoxTrack, wxWindow)
END_EVENT_TABLE()

BeatBoxTrack::BeatBoxTrack(wxWindow *parent,  wxWindowID id,
			   const wxPoint &pos, const wxSize &size)
  : wxWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxGREEN);
  wxStaticText* text = 
    new wxStaticText(this, -1, _T("This is the track view"), wxPoint(0,0), 
		     wxDefaultSize);

}
BeatBoxTrack::~BeatBoxTrack()
{}

BEGIN_EVENT_TABLE(BeatBoxScrollView, wxScrolledWindow)
END_EVENT_TABLE()

BeatBoxScrollView::BeatBoxScrollView(wxWindow *parent, wxWindowID id,
				     const wxPoint &pos, const wxSize &size)
  : wxScrolledWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxRED);
  wxStaticText* text = 
    new wxStaticText(this, -1, _T("This is the scroll view"), wxPoint(0,0), 
		     wxDefaultSize);
}
BeatBoxScrollView::~BeatBoxScrollView()
{}

BEGIN_EVENT_TABLE(BeatBoxView, wxPanel)
  EVT_SLIDER(ID_HZoom, BeatBoxView::OnHZoom)
  EVT_SLIDER(ID_VZoom, BeatBoxView::OnVZoom)
END_EVENT_TABLE()

BeatBoxView::BeatBoxView(wxWindow* parent, wxWindowID id, WiredBeatBox* bb,
			 const wxPoint& pos, const wxSize& size, 
			 wxMutex* mutex)
  : wxPanel(parent, id, pos, size)
{
  Mutex = mutex;
  DRM31 = bb;
  
  wxBoxSizer *row_1;
  wxBoxSizer *row_2;
  wxBoxSizer *row_3;
  
  wxBoxSizer *col_1;
  wxBoxSizer *glob;
  
  /*
  
  HZoomSlider = new wxSlider(this, ID_HZoom, 100, 25, 400, 
			     wxPoint(0,0), 
			     wxSize(TRACK_WIDTH,RULER_HEIGHT)); 
  RulerView = new Ruler(this, -1, wxPoint(TRACK_WIDTH,0), 
			wxSize(GetClientSize().x - TRACK_WIDTH, RULER_HEIGHT));
  
  TrackView = new BeatBoxTrack(this, -1, 
			       wxPoint(0,RULER_HEIGHT), 
			       wxSize(TRACK_WIDTH, 
				      100));//GetClientSize().y - 2*RULER_HEIGHT));//, wxSUNKEN_BORDER);
  //GetClientSize().y - 2*RULER_HEIGHT));
  
  //BeatView = new BeatBoxScrollView(this, -1, wxPoint(TRACK_WIDTH,RULER_HEIGHT),
  //			   wxSize(-1, -1));//GetClientSize()-wxSize(150,20)));
  
  VZoomSlider = new wxSlider(this, ID_VZoom, 100, 25, 400, 
			     wxPoint(0,TrackView->GetClientSize().y+RULER_HEIGHT), 
			     wxSize(TRACK_WIDTH, RULER_HEIGHT));
  
  
  row_1 = new wxBoxSizer(wxHORIZONTAL);
  row_1->Add(HZoomSlider, 0, wxALL, 0);
  row_1->Add(RulerView, 1, wxALL, 0);
  
  row_2 = new wxBoxSizer(wxHORIZONTAL);
  row_2->Add(TrackView, 0, wxALL | wxEXPAND, 0);
  //row_2->Add(BeatView,  1, wxALL | wxEXPAND, 0);

  row_3 = new wxBoxSizer(wxHORIZONTAL);
  row_3->Add(VZoomSlider, 0, wxALL, 0);
  
  
  col_1 = new wxBoxSizer(wxVERTICAL);
  col_1->Add(row_1, 0, wxALL | wxEXPAND, 0);
  col_1->Add(row_2, 1, wxALL | wxEXPAND, 0);
  col_1->Add(row_3, 1, wxALL | wxEXPAND, 0);
  
  glob = new wxBoxSizer(wxHORIZONTAL);
  glob->Add(col_1, 1, wxALL | wxEXPAND, 0);
  
  */
  SetSizer(glob);
}

BeatBoxView::~BeatBoxView()
{
  delete HZoomSlider;
  delete VZoomSlider;
}

void BeatBoxView::OnHZoom(wxCommandEvent& event)
{
}
void BeatBoxView::OnVZoom(wxCommandEvent& event)
{
}
