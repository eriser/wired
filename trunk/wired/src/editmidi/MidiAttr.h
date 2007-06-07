// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MIDIATTR_H__
#define __MIDIATTR_H__

#include <wx/wx.h>
#include "Note.h"
#include <vector>

class MidiAttr: public wxPanel
{
	public:
	MidiAttr(wxWindow *parent, wxWindowID id, const wxPoint &pos, 
			const wxSize &size, long style);
	void	SetNotes(std::vector <Note *>);
	void	OnPaint(wxPaintEvent &);
	void	SetZoomX(double ZoomX) { this->ZoomX = ZoomX; }
	
	private:
		std::vector<Note *> Notes;
	double ZoomX;

	DECLARE_EVENT_TABLE()
};

#endif
