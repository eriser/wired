#ifndef __BEATBOXVIEW_H__
#define __BEATBOXVIEW_H__

#include <wx/wx.h>
#include <wx/slider.h>
#include <vector>
#include "WaveFile.h"
#include "WaveEnv.h"

#define DEC		3

#define RULER_HEIGHT	16
#define TRACK_HEIGHT	100
#define TRACK_WIDTH	150
#define BEAT_WITDH	200
#define BEAT_HEIGHT	113

#define VIEW_BGCOLOR	*wxBLACK
#define VIEW_FGCOLOR	*wxWHITE
#define VIEW_BARCOLOR	*wxBLUE

class WiredBeatBox;
class BeatBoxChannel;
class BeatBoxView;

class BeatTrack : public wxWindow
{
 public:
  //BeatTrack(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, BeatBoxChannel* channel);
  BeatTrack(BeatBoxChannel* channel);
  ~BeatTrack();
  BeatBoxChannel* Channel;
  //void OnPaint(wxPaintEvent& event);
 protected:
  //DECLARE_EVENT_TABLE()
};

class Ruler : public wxWindow
{
 public:
  Ruler(wxWindow *parent, wxWindowID id, 
	const wxPoint &pos, const wxSize &size, BeatBoxView* view_ptr);
  ~Ruler();
  void OnPaint(wxPaintEvent& event);
 protected:
  BeatBoxView* ViewPtr;

DECLARE_EVENT_TABLE()
};

class BeatBoxTrackView : public wxWindow
{
 public:
  
  BeatBoxTrackView(wxWindow *parent, wxWindowID id, 
		   const wxPoint &pos, const wxSize &size,
		   BeatBoxView* view_ptr);
  

  ~BeatBoxTrackView();  
  virtual void OnPaint(wxPaintEvent& event);
  vector<BeatTrack*>	BeatTracks;
 protected:
  BeatBoxView* ViewPtr;
DECLARE_EVENT_TABLE()
};


class BeatBoxScrollView : public wxScrolledWindow
{
 public:
  BeatBoxScrollView(wxWindow *parent, wxWindowID id, 
		    const wxPoint &pos, const wxSize &size,
		    BeatBoxView* view_ptr);
  ~BeatBoxScrollView();
  
  void OnPaint(wxPaintEvent& event);
  void OnLeftDown(wxMouseEvent& event);
  void OnLeftUp(wxMouseEvent& event);
  void OnMotion(wxMouseEvent& event);
  
  void SelectNote(BeatNote* note);
  list<BeatNote*> SelectedNotes;
 protected:
  BeatBoxView* ViewPtr;
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
  void OnHScroll(wxScrollEvent& event);
  void OnVScroll(wxScrollEvent& event);
  void OnSize(wxSizeEvent& event);
  
  //vector<BeatTrack*>	BeatTracks;
  
  WiredBeatBox*		DRM31;
  // protected:
  double XScrollCoef, YScrollCoef;
  long XScroll;
  long YScroll;
  long XSize;
  long YSize;
  long TrackHeight;
  double HZoom;
  double VZoom;
  
  void			AdjustHScrolling(void);
  void			AdjustVScrolling(void);
  
  Ruler*		RulerView;
  BeatBoxTrackView*	TrackView;
  BeatBoxScrollView*	BeatView;
  wxScrollBar*		HScrollBar;
  wxScrollBar*		VScrollBar;
  wxSlider*		HZoomSlider;
  wxSlider*		VZoomSlider;
  wxMutex*		Mutex;
  
  
DECLARE_EVENT_TABLE()
};

enum
  {
    BeatBoxView_Env = 10001,
    ID_HZoom,
    ID_VZoom,
    ID_HScroll,
    ID_VScroll
  };

#endif//__BEATBOXVIEW_H__
