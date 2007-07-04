#include "TransparentStaticText.h"

IMPLEMENT_DYNAMIC_CLASS (TransparentStaticText, wxStaticText)

BEGIN_EVENT_TABLE(TransparentStaticText, wxStaticText)
    EVT_PAINT(TransparentStaticText::OnPaint)
END_EVENT_TABLE()


TransparentStaticText::TransparentStaticText() {}

TransparentStaticText::TransparentStaticText(wxWindow* parent, wxWindowID id, const wxString& label,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {
    Create(parent, id, label, pos, size, style, name);
}


bool TransparentStaticText::Create(wxWindow* parent, wxWindowID id, const wxString& label,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {
    bool bRetVal = wxStaticText::Create(parent, id, label, pos, size, style|wxTRANSPARENT_WINDOW, name);

    SetBackgroundColour(parent->GetBackgroundColour());
    SetBackgroundStyle(wxBG_STYLE_COLOUR);
    SetForegroundColour(parent->GetForegroundColour());

    return bRetVal;
}


void TransparentStaticText::OnPaint(wxPaintEvent& /*event*/) {
    wxPaintDC dc(this);
    dc.SetFont(GetFont());
    dc.DrawText(GetLabel(), 0, 0);
}


