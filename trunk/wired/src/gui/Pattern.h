// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <string>
#include <vector>
#include <wx/string.h>
#include <wx/wx.h>

using namespace				std;

#define PATTERN_DRAG_SCROLL_UNIT	(MEASURE_WIDTH + 1)
#define PATTERN_NAME_WIDTH		(42)
#define PATTERN_NAME_HEIGHT		(10)
#define PATTERN_NAME_MARGINS		(3)
#define PATTERN_NAME_OFFSET		(2)
#define PATTERN_MOVE_BOX_SIZE		(4)
#define PATTERN_MASK_SELECTED		(1)
#define PATTERN_MASK_DRAGGED		(2)
#define PATTERN_MASK_TOGGLED		(4)

class					WaveFile;
class					MidiEvent;

class					Pattern : public wxWindow
{
  double				xdrag;			/* sill unused, will serve to avoid wrong way scroll */
  double				ydrag;			/* during a pattern dragging */

 protected:
  virtual void				OnClick(wxMouseEvent &e);
  virtual void				OnLeftUp(wxMouseEvent &e);
  virtual void				OnDoubleClick(wxMouseEvent &e);
  virtual void				OnRightClick(wxMouseEvent &e);
  virtual void				OnMotion(wxMouseEvent &e);
  void					XMove(double Motion);
  void					DrawName(wxPaintDC &dc, const wxSize &s);

  double				Position;
  double				EndPosition;
  double				Length;
  unsigned long				TrackIndex;
  unsigned char				StateMask;
  wxPoint				m_pos;
  wxSize				m_size;
  wxPoint				m_click;
  wxString				Name;
  wxColour				PenColor;
  wxColour				BrushColor;

 public:
  Pattern(double pos, double endpos, long trackindex);
  virtual ~Pattern();

  void					Modify(double newpos = -1, double newendpos = -1, 
					       long newtrackindex = -1, double newlength = -1);
  void					UpdateMeasure();
  virtual void				Update();
  virtual void				SetSelected(bool sel);
  virtual void				OnBpmChange() {}
  virtual void				SetDrawColour(wxColour c) { PenColor = c; }
  virtual Pattern			*CreateCopy(double pos) = 0x0;
  wxPoint				GetMPosition() { return (m_pos); }
  void					SetMPosition(wxPoint p) { m_pos = p; }
  wxPoint				SetPos(double newpos, long track);
  wxSize				GetSize() { return (m_size); }
  void					SetSize(wxSize s) { m_size = s; }
  int					GetXPos(double pos);
  bool					IsSelected() { return (StateMask & PATTERN_MASK_SELECTED); }
  double				SetPosition(double p) { Position = p; }
  double				GetPosition() { return (Position); }
  double				GetEndPosition() { return (EndPosition); }
  wxString				GetName() { return (Name); }
  void					SetName(wxString n) { Name = n; }
  long					GetTrackIndex() { return (TrackIndex); }
  void					SetTrackIndex(long t) { TrackIndex = t; }
};

#endif
