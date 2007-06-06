// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __COLORED_BOX_H__
#define __COLORED_BOX_H__

#include <wx/wx.h>
#include <wx/colour.h>

class			ColoredBox : public wxControl
{
  wxColour		Color;
  wxColour		BorderColor;

 public:
  ColoredBox(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,
	     const wxColour color, const wxColour bordercolor);
  virtual ~ColoredBox();

  virtual void			OnPaint(wxPaintEvent &e);
  virtual void			OnClick(wxMouseEvent &e);
  virtual void			SetColor(wxColour color) { Color = color; }
  virtual void			SetBorderColor(wxColour bordercolor) { BorderColor = bordercolor; }
  virtual void			SetColors(wxColour color, wxColour bordercolor);
  virtual wxColour		GetColor() { return (Color); }
  virtual wxColour		GetBorderColor() { return (BorderColor); }

 protected:
  DECLARE_EVENT_TABLE()
};

#endif/*__COLORED_BOX_H__*/
