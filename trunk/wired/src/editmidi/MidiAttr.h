#ifndef __MIDIATTR_H__
#define __MIDIATTR_H__

#include <wx/wx.h>
#include "Note.h"
#include <iostream>
#include <vector>

using namespace std;
using std::vector;

class MidiAttr: public wxPanel
{
	public:
	MidiAttr(wxWindow *parent, wxWindowID id, const wxPoint &pos, 
			const wxSize &size, long style);
	void	SetNotes(vector <Note *>);
	void	OnPaint(wxPaintEvent &);
	void	SetZoomX(double ZoomX) { this->ZoomX = ZoomX; }
	
	private:
	vector	<Note *> Notes;
	double ZoomX;

	DECLARE_EVENT_TABLE()
};

#endif
