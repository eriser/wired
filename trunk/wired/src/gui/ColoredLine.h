// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __COLOREDLINE_H__
#define __COLOREDLINE_H__

#include <wx/wx.h>

/*
** Ligne permettant de compenser la faiblesse de la wxStaticLine
** qui n'est pas capable de changer de couleur ...
*/

class ColoredLine : public wxWindow
{
  wxColour		c0loR;
  
  void			OnPaint(wxPaintEvent &event);
  /*  virtual void		OnMouseEvent(wxMouseEvent &event);*/
  
 public:
  ColoredLine(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	      const wxSize &size, const wxColour c);
  
  ~ColoredLine();
  
  void			SetColor(wxColour c);
  void			SetVSize(unsigned long h);
  void			SetHSize(unsigned long w);
  void			ReSize(unsigned long w, unsigned long h);

  DECLARE_EVENT_TABLE()
};

#endif/*__COLOREDLINE_H__*/

