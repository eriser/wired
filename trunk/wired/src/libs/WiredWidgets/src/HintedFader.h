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
  
  virtual void OnMouseEvent(wxMouseEvent &event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeftDown(wxMouseEvent& event);
  virtual void OnKeyDown(wxKeyEvent& event);
  virtual void OnKeyUp(wxKeyEvent& event);
  virtual void OnEnterWindow(wxMouseEvent &event);
  virtual void OnLeave(wxMouseEvent &event);

 protected:
  Hint* Label;
  wxWindow  *HintParent;
DECLARE_EVENT_TABLE()
};

#endif//__HINTEDFADER_H__
