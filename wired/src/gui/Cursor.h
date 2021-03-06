// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __CURSOR_H__
#define __CURSOR_H__

#include <wx/gdicmn.h>

#define CURSOR_WIDTH			(12)
#define CURSOR_HEIGHT			(12)
#define CURSOR_NAME_HEIGHT		(6)
#define CURSOR_NAME_XOFFSET		(3)
#define CURSOR_NAME_YOFFSET		(1)
#define CURSOR_DRAG_SCROLL_UNIT		(MEASURE_WIDTH + 1)

/*
** Curseur ayant une partie "dragable" H sur la regle
** et une ligne verticale L dans le sequenceur.
*/

class					SequencerGui;
class					Cursor;
class					ColoredLine;

class					CursorH : public wxWindow
{
  friend class				Cursor;
  friend class				ColoredLine;
  Cursor				*c;
  wxPoint				m_click;
  wxString				Name;

 protected:
  virtual void				OnMouseEvent(wxMouseEvent &e);
  virtual void				OnClick(wxMouseEvent &e);
  virtual void				OnClickUp(wxMouseEvent &e);
  virtual void				OnPaint(wxPaintEvent &e);
  
 public:
  CursorH(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	  const wxSize &size, Cursor *C, const char name);
  ~CursorH();
  DECLARE_EVENT_TABLE()
};

class					Cursor
{
  friend class				CursorH;
  double				pos;
  long					Xpos;
  CursorH				*H;
  ColoredLine				*L;
  SequencerGui				*SeqGUI;
  
 public:
  Cursor(const char name, const int id, const double initpos, Ruler *R, SequencerGui *S, 
	 const wxColour &cH, const wxColour &cL);
  ~Cursor();
  void					SetPos(double newpos);
  double				GetPos();
  void					PutOnTop();
  void					ReSize();
};


#endif/*__CURSOR_H__*/
