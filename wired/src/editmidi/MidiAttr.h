// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

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
    void	OnPaint    (wxPaintEvent &);
    void  OnMouseMove(wxMouseEvent &);
    void	OnLeftUp   (wxMouseEvent &);
    void	OnLeftDown (wxMouseEvent &);
    void	SetZoomX(double ZoomX) { this->ZoomX = ZoomX; }

  private:
    vector	<Note *> Notes;
    double ZoomX;

    void UpdateVelocity(wxMouseEvent &);

    DECLARE_EVENT_TABLE()
};

#endif
