// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __TRANSPARENTSTATICTEXT_H__
#define __TRANSPARENTSTATICTEXT_H__

#include <wx/stattext.h>
#include <wx/dcclient.h>

class TransparentStaticText : public wxStaticText
{
    DECLARE_DYNAMIC_CLASS (TransparentStaticText)

public:
    TransparentStaticText();
    TransparentStaticText(
        wxWindow* parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name= wxStaticTextNameStr
    );

    bool Create(
        wxWindow* parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name= wxStaticTextNameStr
    );

    virtual bool HasTransparentBackground() { return true; };

    virtual void OnPaint(wxPaintEvent& event);
    virtual void OnMouseEvent(wxMouseEvent& event);
    virtual void SetLabel(const wxString&  label);

    DECLARE_EVENT_TABLE()
};

#endif
