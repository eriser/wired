#ifndef __BEATBOXVIEW_H__
#define __BEATBOXVIEW_H__

#include <wx/wx.h>
#include <wx/slider.h>
#include <vector>
#include "WaveFile.h"
#include "WaveEnv.h"

#define DEC		3

#define NB_COMBO_CHOICES 8
#define TOOLBAR_HEIGHT	24
#define RULER_HEIGHT	16
#define TRACK_HEIGHT	78//100 - TOOLBAR_HEIGHT
#define TRACK_WIDTH	150
#define BEAT_WITDH	200
#define BEAT_HEIGHT	87//113 - TOOLBAR_HEIGHT

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
  void OnLeftDown(wxMouseEvent& event);
  
  vector<BeatTrack*>	BeatTracks;
  long SelectedTrack;
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
  void OnRightDown(wxMouseEvent& event);
  void OnMotion(wxMouseEvent& event);
  
  void ClearSelected(void);
  void SelectNote(BeatNote* note);
  void NewNote(wxCommandEvent& event);
  void DeleteNotes(wxCommandEvent& event);
  void CutNotes(wxCommandEvent& event);
  void CopyNotes(wxCommandEvent& event);
  void PasteNotes(wxCommandEvent& event);
  void SelectAllNotes(wxCommandEvent& event);
  
  long SubDiv;
  
  BeatNote* SelectedNote;
  list<BeatNote*> SelectedNotes;
  list<BeatNote*> TmpNotes;

 protected:
  wxMenu* PopMenu;
  bool OnSelecting;
  long ClickPosX, ClickPosY, MotionPosX, MotionPosY;
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
  void OnSubdivChange(wxCommandEvent& event);
  void OnPosChange(wxCommandEvent& event);
  void OnVelChange(wxCommandEvent& event);
  
  wxToolBar* ToolBar;
  wxComboBox* SubCombo;
  wxTextCtrl* PosTextCtrl;
  wxTextCtrl* VelTextCtrl;
  void UpdateToolBar(void);
  //vector<BeatTrack*>	BeatTracks;
  
  WiredBeatBox*		DRM31;
  // protected:
  double XScrollCoef, YScrollCoef;
  long SelectedTrack;
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
    ID_VScroll,
    ID_PopNew,
    ID_PopDelete,
    ID_PopCut,
    ID_PopCopy,
    ID_PopPaste,
    ID_PopSelect,
    ID_SubCombo,
    ID_PosTextCtrl,
    ID_VelTextCtrl
  };

#endif//__BEATBOXVIEW_H__
