#include "BeatBoxView.h"

#include "BeatBox.h"

BEGIN_EVENT_TABLE(BeatBoxTrack, wxWindow)
END_EVENT_TABLE()

BeatBoxTrack::BeatBoxTrack(wxWindow *parent, const wxPoint &pos, const wxSize &size)
{}
BeatBoxTrack::~BeatBoxTrack()
{}

BEGIN_EVENT_TABLE(BeatBoxScrollView, wxWindow)
END_EVENT_TABLE()

BeatBoxScrollView::BeatBoxScrollView(wxWindow *parent, const wxPoint &pos, const wxSize &size)
{}
BeatBoxScrollView::~BeatBoxScrollView()
{}

BEGIN_EVENT_TABLE(BeatBoxView, wxPanel)
END_EVENT_TABLE()

BeatBoxView::BeatBoxView(wxWindow* parent, WiredBeatBox* bb,
			 const wxPoint& pos, const wxSize& size, 
			 wxMutex* mutex)
  : wxPanel(parent, -1, pos, size)
{
  Mutex = mutex;
  DRM31 = bb;
  int s = DRM31->GetSig();
  cout << s << endl;
}

BeatBoxView::~BeatBoxView()
{}
