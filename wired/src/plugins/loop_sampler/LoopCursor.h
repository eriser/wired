// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __LOOPCURSOR_H__
#define __LOOPCURSOR_H__

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/window.h>

class LoopCursor;

#define TYPE_SLICE_SELECTED	3131312
#define TYPE_SLICE_MOVE		3131313

class LoopBtn : public wxWindow
{
 public:
  LoopBtn(LoopCursor *s, wxWindow *parent, wxWindowID id, const wxPoint &pos, 
	  const wxSize &size, int style);
  ~LoopBtn();

 protected:
  void OnMouseEvent(wxMouseEvent &event);
  void OnPaint(wxPaintEvent &e);

  LoopCursor *MyLoop;

  DECLARE_EVENT_TABLE()
};

class LoopCursor
{
 protected:
  wxStaticLine *l;
  LoopBtn *r;

 public:
  LoopCursor(bool loop_begin, wxWindow *parent, wxWindowID id, const wxPoint &pos, 
	     const wxSize &size);
  ~LoopCursor();

  bool LoopBegin;

  void SetPosition(const wxPoint &pos);
  wxPoint GetPosition() { return l->GetPosition(); } 
  void SetSize(const wxSize &size);
};

#endif
