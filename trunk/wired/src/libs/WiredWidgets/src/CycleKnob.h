#ifndef __CYCLEKNOB_H__
#define __CYCLEKNOB_H__

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "wx/dragimag.h"
#include <iostream>

using namespace std;

class CycleKnob : public wxWindow
{
 public:
  CycleKnob(wxWindow *parent, wxWindowID id, 
		int n_imgs, wxImage **imgs, int mouse_sens,
		int begin_value, int end_value, int init_val,
		const wxPoint &pos, const wxSize &size);
  ~CycleKnob();
  
  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnMotionEvent(wxMouseEvent &event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeftDown(wxMouseEvent &event);
  virtual void OnRightDown(wxMouseEvent &event);
  virtual void OnKeyDown(wxKeyEvent& event);
  
  int GetValue();
  void SetValue(int val);

 protected:
  bool		Clicked;
  int		Steps;
  int		MouseStep;
  int		BeginVal;
  int		EndVal;
  int		CurVal;
  
  int		ClickPos;
  int		Count;
  wxBitmap**	Bmps;


DECLARE_EVENT_TABLE()
};
#endif//__CYCLEKNOB_H__
