#ifndef __KNOBCTRL_H
#define __KNOBCTRL_H

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "wx/dragimag.h"
#include <iostream>
#include "StaticBitmap.h"

using namespace std;

class KnobCtrl : public wxWindow
{
 public:
  KnobCtrl(wxWindow *parent, wxWindowID id, wxImage *img_bg, wxImage *img_fg,
	    int begin_value, int end_value, int init_val, int pas,
	    const wxPoint &pos, const wxSize &size);
  ~KnobCtrl();
  virtual void OnPaint(wxPaintEvent &event);
  virtual void OnMouseEvent(wxMouseEvent &event);
  virtual void OnKeyDown(wxKeyEvent& event);
  int GetValue();
  void SetValue(int val);
  void SetValue_(unsigned val);

 protected:
  wxImage		*knob_fg;
  wxImage		*knob_bg;
  wxBitmap		*fg_tmp;
  wxStaticBitmap	*fg;
  wxMask		*fg_mask;
  wxSize		Size;
  int			Value;	
  int			OldValue;	
  int			BeginValue;
  int			EndValue;
  unsigned int		Pas;
  float      		coeff;
  int			oldy;
  DECLARE_EVENT_TABLE()

    
    };
    
#endif
