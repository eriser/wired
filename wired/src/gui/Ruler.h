// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __RULER_H__
#define __RULER_H__

#include <wx/wx.h>

#define RULER_HEIGHT			(16)
#define RULER_NUMBER_HEIGHT		(6)
#define RULER_NUMBER_XOFFSET		(2)
#define RULER_NUMBER_YOFFSET		(-1)
#define RULER_MEASURE_DIV_HEIGHT	(3)
#define RULER_XMARK_HEIGHT		(4)

class					ColoredLine;

class					Ruler : public wxWindow
{
  virtual void				OnPaint(wxPaintEvent &event);
  virtual void				OnMouseEvent(wxMouseEvent &event);

  long					XMark;
  long					XScroll;
  ColoredLine				*T1;
  ColoredLine				*T2;
  ColoredLine				*T3;

 public:
  Ruler(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
  ~Ruler();
  void					SetXScroll(long x, long range, long seqwidth);
  void					SetXScrollValue(long X);
  long					GetXScroll();
  int					GetXPos(double pos);
  void					MoveXMark(long x);

  DECLARE_EVENT_TABLE()
};

#endif/* __RULER_H__*/

