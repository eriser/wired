#ifndef __BEATBOXVIEW_H__
#define __BEATBOXVIEW_H__

#include <wx/wx.h>
#include <wx/slider.h>
#include "WaveFile.h"
#include "WaveEnv.h"


#define RULER_HEIGHT	16
#define TRACK_WIDTH	150

class WiredBeatBox;

class Ruler : public wxWindow
{
 public:
  Ruler(wxWindow *parent, wxWindowID id, 
	const wxPoint &pos, const wxSize &size);
  ~Ruler();  
 protected:

DECLARE_EVENT_TABLE()
};


class BeatBoxTrack : public wxWindow
{
 public:
  BeatBoxTrack(wxWindow *parent, wxWindowID id, 
	       const wxPoint &pos, const wxSize &size);
  ~BeatBoxTrack();  
 protected:

DECLARE_EVENT_TABLE()
};

class BeatBoxScrollView : public wxScrolledWindow
{
 public:
  BeatBoxScrollView(wxWindow *parent, wxWindowID id, 
		    const wxPoint &pos, const wxSize &size);
  ~BeatBoxScrollView();
 protected:

DECLARE_EVENT_TABLE()
};

class BeatBoxView : public wxPanel
{
 public:
  BeatBoxView(wxWindow* parent, wxWindowID id, WiredBeatBox* bb, 
	      const wxPoint& pos, const wxSize& size, 
	      wxMutex* mutex);
  ~BeatBoxView();
  
  void OnHZoom(wxCommandEvent& event);
  void OnVZoom(wxCommandEvent& event);

  WiredBeatBox* DRM31;
 protected:
  Ruler*	RulerView;
  BeatBoxTrack*	TrackView;
  BeatBoxScrollView* BeatView;
  wxSlider*	HZoomSlider;
  wxSlider*	VZoomSlider;
  wxMutex*	Mutex;
  
DECLARE_EVENT_TABLE()
};

enum
  {
    BeatBoxView_Env = 1,
    ID_HZoom,
    ID_VZoom    
  };

#endif//__BEATBOXVIEW_H__
