// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __PATTERN_H__
#define __PATTERN_H__

using namespace std;

#include <string>
#include <wx/wx.h>

#define PATTERN_DRAG_SCROLL_UNIT		(MEASURE_WIDTH + 1)

class Pattern : public wxWindow
{
 protected:
  virtual void				OnClick(wxMouseEvent &e);
  virtual void				OnDoubleClick(wxMouseEvent &e);
  virtual void				OnRightClick(wxMouseEvent &e);
  virtual void				OnMotion(wxMouseEvent &e);

  wxPoint				m_pos;
  wxSize				m_size;
  wxPoint				m_click;

  double				Position;
  double				EndPosition;
  double				Length;
  long					TrackIndex;
  bool					Selected;
  string				Name;
  wxColour				PenColor;
  wxColour				BrushColor;

 public:
  Pattern(double pos, double endpos, long trackindex);
  virtual ~Pattern();

  void					Modify(double newpos = -1, double newendpos = -1, 
					       long newtrackindex = -1, double newlength = -1);
  virtual void				Update();
  void					UpdateMeasure();
  virtual void				SetSelected(bool sel);
  virtual Pattern			*CreateCopy(double pos) = 0x0;
  virtual void				OnBpmChange() {}
  virtual void				SetDrawColour(wxColour c) { PenColor = c; }

  wxPoint				GetMPosition() { return (m_pos); }
  void					SetMPosition(wxPoint p) { m_pos = p; }
  wxPoint				SetPos(double newpos, long track);
  wxSize				GetSize() { return (m_size); }
  void					SetSize(wxSize s) { m_size = s; }
  int					GetXPos(double pos);
  
  bool					IsSelected() { return (Selected); }
  double				SetPosition(double p) { Position = p; }
  double				GetPosition() { return (Position); }
  double				GetEndPosition() { return (EndPosition); }
  string				GetName() { return (Name); }
  void					SetName(string n) { Name = n; }
  long					GetTrackIndex() { return (TrackIndex); }
  void					SetTrackIndex(long t) { TrackIndex = t; }
};

#endif
