#ifndef __HINT_H__
#define __HINT_H__

#include "wx/wx.h"

class Hint : public wxWindow
{
 public:
  Hint(wxWindow *parent, wxWindowID id, const wxString& label, 
       const wxPoint& pos, const wxSize& size,
       const wxColour& bgcolour, const wxColour& fgcolour);
  virtual ~Hint();
  void SetLabel(const wxString& label);
  virtual void OnPaint(wxPaintEvent& e);
  //virtual void SetValue(unsigned int);
 protected:
  wxStaticText* Label;
DECLARE_EVENT_TABLE()
};
#endif//__HINT_H__
