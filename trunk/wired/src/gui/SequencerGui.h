// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SEQUENCERGUI_H__
#define __SEQUENCERGUI_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <wx/statline.h>

#include <wx/toolbar.h>
#include <wx/colordlg.h>

#include <vector>

#include "Ruler.h"
#include "Cursor.h"
#include "SelectionZone.h"
#include "ColoredBox.h"
#include "Track.h"
#include "AudioPattern.h"
#include "MidiPattern.h"
#include "AccelCenter.h"
#include "MLTree.h"
#include "SequencerView.h"

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(EVT_DROP, -1)
END_DECLARE_EVENT_TYPES()

#define ID_SEQ_SETPOS			(101010)
#define ID_SEQ_RESIZE			(101011)
#define ID_SEQ_DRAWMIDI			(101012)

#define TYPE_SEQ_SETPOS			(9876543)
#define TYPE_SEQ_RESIZE			(9876544)
#define TYPE_SEQ_DRAWMIDI		(9876545)

#define TRACK_HEIGHT			(80)
#define TRACK_WIDTH			(120)
#define TOOLS_HEIGHT			(46)

#define HSCROLL_THUMB_WIDTH		(42)
#define VSCROLL_THUMB_WIDTH		(42)

#define WHEEL_VSCROLL_UNIT		(42)
#define WHEEL_HSCROLL_UNIT		(42)

#define MAGNETISM			(3)
#define MEASURE_WIDTH			(80)


#define CURSOR_MASK			(0x1)
#define CURSOR_MAGNETISM		(MAGNETISM & CURSOR_MASK)
#define CURSOR_DEFAULT_MAGNETISM	(4)
#define PATTERN_MASK			(0x2)
#define PATTERN_MAGNETISM		(MAGNETISM & PATTERN_MASK)
#define PATTERN_DEFAULT_MAGNETISM	(4)
#define DEFAULT_MAGNETISM_COMBO_VALUE	wxT("1/4")

#define PLAY_CURSOR_FOLLOWING		(42)

#define HAND_UP				wxT("ihm/toolbar/hand_up.png")
#define HAND_DOWN			wxT("ihm/toolbar/hand_down.png")
#define DRAW_UP				wxT("ihm/toolbar/draw_up.png")
#define DRAW_DOWN			wxT("ihm/toolbar/draw_down.png")
#define ERASE_UP			wxT("ihm/toolbar/erase_up.png")
#define ERASE_DOWN			wxT("ihm/toolbar/erase_down.png")
#define SPLIT_UP			wxT("ihm/toolbar/split_up.png")
#define SPLIT_DOWN			wxT("ihm/toolbar/split_down.png")
#define MERGE_UP			wxT("ihm/toolbar/merge_up.png")
#define MERGE_DOWN			wxT("ihm/toolbar/merge_down.png")
#define MAGN_UP				wxT("ihm/toolbar/magn_up.png")
#define MAGN_DOWN			wxT("ihm/toolbar/magn_down.png")
#define COLOR_UP			wxT("ihm/toolbar/color_up.png")
#define COLOR_DOWN			wxT("ihm/toolbar/color_down.png")

#define COLORBOX_MARGINS		(8)

enum
  {
    ID_TOOL_MOVE_SEQUENCER = 942,
    ID_TOOL_DRAW_SEQUENCER,
    ID_TOOL_DELETE_SEQUENCER,
    ID_TOOL_SPLIT_SEQUENCER,
    ID_TOOL_MERGE_SEQUENCER,
    ID_TOOL_PAINT_SEQUENCER,
    ID_EVENT_DROP,
  };

#define NB_COMBO_CHOICES		(9)

typedef struct				s_combo_choice
{
  wxString				s;
  double				value;
} t_combo_choice;

extern const struct s_combo_choice	ComboChoices[NB_COMBO_CHOICES + 1];

enum
  {
    ID_POPUP_CUT = 24242,
    ID_POPUP_COPY,
    ID_POPUP_PASTE,
    ID_POPUP_DELETE,
    ID_POPUP_MOVE_TO_CURSOR,
    ID_POPUP_SELECT_ALL,
    ID_SEQ_HSLIDER,
    ID_SEQ_VSLIDER,
    ID_SEQ_RULER,
    ID_SEQ_SCROLLING,
    ID_CURSOR_PLAY,
    ID_CURSOR_BEGIN,
    ID_CURSOR_REPEAT,
    ID_CURSOR_END,
    ID_TRACK_VIEW,
    ID_SEQ_MOVE,
    ID_SEQ_DRAW,
    ID_SEQ_DEL,
    ID_SEQ_SPLIT,
    ID_SEQ_MERGE,
    ID_SEQ_MAGNET,
    ID_SEQ_COMBO_MAGNET,
    ID_SEQ_COLOR,
    ID_SEQ_COLORBOX,
    ID_EVT_DROP
  };

class				Ruler;
class				Cursor;
class				SelectionZone;
class				ColoredBox;
class				Track;
class				Plugin;
class				Pattern;
class				WaveFile;
class				MidiEvent;
class				AccelCenter;

class				CursorEvent: public wxEvent
{
  double			Position;

 public:
  CursorEvent(int id = 0, wxEventType eventType = wxEVT_NULL)
    : wxEvent(id, eventType) { Position = 0.0; }
  ~CursorEvent() {}

  virtual wxEvent*		Clone() const { return new CursorEvent(*this); }
};

class				SequencerGui: public wxPanel, public WiredDocument
{
  friend class			Pattern;
  friend class			AudioPattern;
  friend class			MidiPattern;
  friend class			Cursor;
  friend class			CursorH;
  friend class			Ruler;
  friend class			ColoredBox;
  friend class			SequencerView;
  friend class			SeqTrack;

  float				HoriZoomFactor;
  float				VertZoomFactor;
  int				Tool;
  double			CurrentXScrollPos;
  double			CurrentYScrollPos;
  double			FirstMeasure;
  double			LastMeasure;
  char				FollowPlayCursor;
  char				Magnetism;
  unsigned short		CursorMagnetism;
  unsigned short		PatternMagnetism;
  long				VertNowPos;
  long				HorizNowPos;
  bool				DoCut;
  SequencerView			*SeqView;
  wxMenu			*PopMenu;
  Ruler				*RulerPanel;
  wxScrolledWindow		*TrackView;
  wxScrollBar			*VertScrollBar;
  wxScrollBar			*HorizScrollBar;
  wxSlider			*VertZoomSlider;
  wxSlider			*HoriZoomSlider;
  std::vector<wxStaticLine *>	Measures;
  std::vector<Pattern *>	SelectedItems;
  std::vector<Pattern *>	CopyItems;
  Cursor			*PlayCursor;
  Cursor			*EndCursor;
  Cursor			*BeginLoopCursor;
  Cursor			*EndLoopCursor;
  wxToolBar			*Toolbar;
  wxComboBox			*MagnetQuant;
  wxColourDialog		*ColorDialogBox;
  wxColour			BrushColor;
  wxColour			PenColor;
  ColoredBox			*ColorBox;

 public:
  SequencerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size,
	       wxWindow *mainwindow, WiredDocument* docParent);
  ~SequencerGui();

  void				HideAllPatterns(wxMouseEvent &e);
  Track				*AddTrack(bool is_audio = true);
  void				RemoveTrack();
 private:

  void				ReindexTrackArray();

 public:

  // track creation and deletion
  Track				*CreateTrack(trackType type = eAudioTrack);
  void				DeleteTrack(Track* track);

  // accessors
  inline SequencerView*		GetView() { return (SeqView); };
  int				GetCurrentYScrollPos();
  int				GetSeqHeaderHeight();

  void				UnselectTracks();
  void				SelectTrack(long trackindex);
  void				MovePattern(Pattern *p, long oldTrackIndex, long newTrackIndex);
  bool				IsAudioTrack(long trackindex);
  void				ChangeSelectedTrackIndex(long trackindex);
  void				ScrollTrackList(long track_delta);
  void				AdjustHScrolling();
  void				AdjustVScrolling();
  void				SetScrolling();
  void				PutCursorsOnTop();
  void				ReSizeCursors();
  void				RedrawCursors();
  void				SetCurrentPos(double pos);
  void				SetBeginLoopPos(double pos);
  void				SetEndLoopPos(double pos);
  void				SetEndPos(double pos);
  void				RefreshConnectMenu();
  void				RemoveReferenceTo(Plugin *plug);
  void				DeleteSelectedTrack();
  void				DeleteAllTracks();
  void				DeleteSelectedPatterns();
  void				DeletePattern(Pattern *p);
  void				UpdateAudioPatterns(WaveFile *w);
  void				UpdateMidiPattern(MidiPattern *m);
  void				MoveToCursor();
  void				SelectItem(Pattern *p, bool shift);
  void				CopySelectedItems();
  void				PasteItems();
  void				ShowPopup(wxPoint pos);
  void				ChangeMouseCursor(wxCursor c);

  // wx events
  void				KeyDown(wxKeyEvent &event);
  void				OnScroll(wxScrollEvent &event);
  void				OnWheelMove(wxMouseEvent &e);
  void				OnVertSliderUpdate(wxCommandEvent &event);
  void				OnHoriSliderUpdate(wxCommandEvent &event);
  void				OnPaint(wxPaintEvent &event);
  void				OnSize(wxSizeEvent &event);
  void				OnCopy(wxCommandEvent &event);
  void				OnCut(wxCommandEvent &event);
  void				OnPaste(wxCommandEvent &event);
  void				OnSetPosition(CursorEvent &event);
  void				OnResizePattern(wxCommandEvent &event);
  void				OnDrawMidi(wxCommandEvent &event);
  void				OnDeleteClick(wxCommandEvent &event);
  void				OnMoveToCursorClick(wxCommandEvent &event);
  void				OnSelectAll(wxCommandEvent &event);
  void				OnPlayCursorMove(wxCommandEvent &event);
  void				OnBeginLCursorMove(wxCommandEvent &event);
  void				OnEndLCursorMove(wxCommandEvent &event);
  void				OnEndCursorMove(wxCommandEvent &event);
  void				OnMoveClick(wxCommandEvent &event);
  void				OnDrawClick(wxCommandEvent &event);
  void				OnEraseClick(wxCommandEvent &event);
  void				OnSplitClick(wxCommandEvent &event);
  void				OnMergeClick(wxCommandEvent &event);
  void				OnMagnetismToggle(wxCommandEvent &event);
  void				OnMagnetismChange(wxCommandEvent &event);
  void				OnColorButtonClick(wxCommandEvent &event);
  void				OnColoredBoxClick(wxCommandEvent &event);
  void				Drop(wxCommandEvent &event);
  //bool				Floating;
  // WiredDocument things
  void				Save();
  void				Load(SaveElementArray data);
  void				CleanChildren();

  double			CurrentPos;

  void				UpdateTracks();
  void				DelPattern(Pattern *p, long trackindex);
 protected:
  void				SwapTracksPos(Track *t1, Track *t2);
  void				UpdateTrackList(std::vector<Track *> *track_list);
  void				UpdateMeasures();
  void				DrawMeasures();
  
  DECLARE_EVENT_TABLE()
};

extern SequencerGui		*SeqPanel;

#endif
