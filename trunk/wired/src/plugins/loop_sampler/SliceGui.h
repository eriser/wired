// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __SLICEGUI_H__
#define __SLICEGUI_H__

#include <wx/statline.h>
#include <wx/window.h>

class SliceGui;

#define TYPE_SLICE_SELECTED	3131312
#define TYPE_SLICE_MOVE		3131313

class SliceBtn : public wxWindow
{
 public:
  SliceBtn(SliceGui *s, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, 
	   int style);
  ~SliceBtn();

 protected:
  void OnMouseEvent(wxMouseEvent &event);
  SliceGui *MySlice;

  DECLARE_EVENT_TABLE()
};

class SliceGui
{
 protected:
  wxStaticLine *l;
  SliceBtn *r;

 public:
  SliceGui(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
  ~SliceGui();

  void SetPosition(const wxPoint &pos);
  wxPoint GetPosition() { return l->GetPosition(); } 
  void SetSize(const wxSize &size);
  void SetSelected(bool sel);

  friend class SliceBtn;

  bool Selected;

};

#endif
