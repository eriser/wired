// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __RULER_H__
#define __RULER_H__

#include <wx/wx.h>

#define RULER_HEIGHT			(20)

class Ruler : public wxWindow
{
  long		XScroll;

  virtual void	OnPaint(wxPaintEvent &event);
  virtual void	OnMouseEvent(wxMouseEvent &event);

 public:
  Ruler(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	const wxSize &size);
  ~Ruler();
  void		SetXScroll(long x, long range, long seqwidth);
  void		SetXScrollValue(long X);
  long		GetXScroll();
  DECLARE_EVENT_TABLE()
};

#endif/* __RULER_H__*/

