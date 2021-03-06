// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __KEY_H__
#define __KEY_H__

#include <wx/wx.h>

class Key: public wxPanel
{
public:
    bool     selected;
    bool     isBlack;
    wxString note;
    int      code;

    Key(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, bool pisBlack, wxString pnote, int note_code);

    void OnPaint       (wxPaintEvent &);
    void OnLeftDown    (wxMouseEvent &);
    void OnLeftUp      (wxMouseEvent &);
    void OnEnterWindow (wxMouseEvent &);
    void OnLeaveWindow (wxMouseEvent &);

DECLARE_EVENT_TABLE()
};

#endif
