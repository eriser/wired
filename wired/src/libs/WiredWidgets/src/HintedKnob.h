#ifndef __HINTEDKNOB_H__
#define __HINTEDKNOB_H__

#include "Hint.h"
#include "KnobCtrl.h"

class HintedKnob : public KnobCtrl
{
 public:
  HintedKnob(wxWindow *parent, wxWindowID id, wxWindow* hintparent,
	     wxImage *img_bg, wxImage *img_fg,
	     int begin_value, int end_value, int init_val, int pas,
	     const wxPoint &pos, const wxSize &size, const wxPoint& hintpos);
  ~HintedKnob();
  virtual void OnMouseEvent(wxMouseEvent &event);
  virtual void OnKeyDown(wxKeyEvent& event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnKeyUp(wxKeyEvent& event);

 protected:
  Hint* Label;

DECLARE_EVENT_TABLE()
};

#endif//__HINTEDKNOB_H__
