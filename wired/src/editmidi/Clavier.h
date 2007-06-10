// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CLAVIER_H__
#define __CLAVIER_H__

#include <wx/wx.h>
#include <vector>
#include "Key.h"
#include "EditMidi.h"

#define	BLACKKEY_HEIGHT	10
#define WHITEKEY_WIDTH	30
#define	NB_WHITEKEY	75
#define	BLACKKEY_WIDTH	(WHITEKEY_WIDTH/2)
#define CLAVIER_HEIGHT	(BLACKKEY_HEIGHT * (NB_WHITEKEY * 12 / 7))
#define	CLAVIER_WIDTH	80

class Clavier: public wxControl
{
public:
    Clavier(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, class EditMidi* em);

    void SetZoomY(double pZoomY);
    void RecalcKeyPos();

    void OnPaint(wxPaintEvent &);
    void OnKeyDown(wxMouseEvent &event);
    void OnKeyUp(wxMouseEvent &event);

    EditMidi *em;
    double ZoomY;
    std::vector<Key *> keys;
    DECLARE_EVENT_TABLE()
};

#endif
