#ifndef __IDBUTTON_H__
#define __IDBUTTON_H__

#include <iostream>
#include <wx/wx.h>

#define UNCLICKED	0
#define CLICKED		1


class IdButton : public wxWindow
{
 public:
  IdButton(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
	   const wxSize& size, wxImage* up, wxImage* down,
	   unsigned int num_id);
  ~IdButton();
  
  void OnPaint(wxPaintEvent &event);
  void OnMouseEvent(wxMouseEvent &event);
  void OnEnter(wxMouseEvent &event);
  
  void SetOn(void);
  void SetOff(void);
  unsigned int	Num;
  
 protected:
  unsigned int	State;
  wxBitmap**	Bitmaps;

DECLARE_EVENT_TABLE()

};

#endif//__IDBUTTON_H__
