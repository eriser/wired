#ifndef __STATICPOSKNOB_H__
#define __STATICPOSKNOB_H__

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "wx/dragimag.h"
#include <iostream>

using namespace std;

class StaticPosKnob : public wxWindow
{
 public:
  StaticPosKnob(wxWindow *parent, wxWindowID id, 
		int step, wxImage **img_bg, int mouse_step,
		int begin_value, int end_value, int init_val,
		const wxPoint &pos, const wxSize &size);
  ~StaticPosKnob();
  
  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnMotionEvent(wxMouseEvent &event);
  virtual void OnLeftUp(wxMouseEvent &event);
  virtual void OnLeftDown(wxMouseEvent &event);
  virtual void OnKeyDown(wxKeyEvent& event);
  void OnEnter(wxMouseEvent& event);
  
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
#endif//__STATICPOSKNOB_H__
