// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef	__EDITNOTE_H__
#define	__EDITNOTE_H__

#include <wx/wx.h>

class				Note;

class				EditNote: public wxPanel
{
 public:
  EditNote(wxWindow *parent, wxWindowID id, const wxPoint &pos,
	   const wxSize &size, Note *note);
  
  void				OnPaint(wxPaintEvent &);
  void				OnMouseMove(wxMouseEvent &);
  void				OnLeftUp(wxMouseEvent &);
  void				OnLeftDown(wxMouseEvent &);
  void				SetZoomX(double);
  
  bool				dragging;
  long				mx;
  long				my;
  long				dx;
  long				dy;
  wxBitmap			*backsave;
  Note				*n;
  double			ZoomX;

  DECLARE_EVENT_TABLE()
};

#endif/*__EDITNOTE_H__*/
