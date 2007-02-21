// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __RULERLOOP_H__
#define __RULERLOOP_H__

#include <wx/wx.h>

#define RULER_HEIGHT 12

class RulerLoop : public wxWindow
{
 public:
  RulerLoop(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	const wxSize &size);
  ~RulerLoop();

  void SetBeats(int bars, int beats);

 protected:
  int MesCount;
  int Beats;

  void OnPaint(wxPaintEvent &event);

  DECLARE_EVENT_TABLE()
};

#endif/* __RULERLOOP_H__*/

