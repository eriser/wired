#ifndef __HINTEDFADER_H__
#define __HINTEDFADER_H__

#include <wx/wx.h>
#include "Hint.h"
#include "FaderCtrl.h"
#include <wx/tooltip.h>
#include <wx/tipwin.h>

class HintedFader : public FaderCtrl
{
 public:
  HintedFader(wxWindow *parent, wxWindowID id, wxWindow* hintparent,
	      wxImage *img_bg, wxImage  *img_fg,
	      long begin_value, long end_value, long val,
	      const wxPoint &pos, const wxSize &size,
	      const wxPoint &hintpos);
  virtual ~HintedFader();
  
  void OnMouseEvent(wxMouseEvent &event);
  void OnLeftUp(wxMouseEvent &event);
  void OnLeftDown(wxMouseEvent& event);
  void OnKeyDown(wxKeyEvent& event);
  void OnKeyUp(wxKeyEvent& event);
  
 protected:
  Hint* Label;
DECLARE_EVENT_TABLE()
};

#endif//__HINTEDFADER_H__
