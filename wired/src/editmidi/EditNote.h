#ifndef	__EDITNOTE_H__
#define	__EDITNOTE_H__

#include <wx/wx.h>
#include <iostream>
#include "Note.h"

using namespace std;

class EditNote: public wxPanel
{
	public:
        EditNote(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		const wxSize &size, Note *note);

	void OnPaint(wxPaintEvent &);
	void OnMouseMove(wxMouseEvent &);
	void OnLeftUp(wxMouseEvent &);
	void OnLeftDown(wxMouseEvent &);
	void SetZoomX(double);

	bool dragging;
	long mx;
	long my;
	long dx;
	long dy;
	wxBitmap *backsave;
	Note *n;
	double ZoomX;

	
DECLARE_EVENT_TABLE()
};

#endif
