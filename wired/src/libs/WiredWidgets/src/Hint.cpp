// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "Hint.h"

BEGIN_EVENT_TABLE(Hint, wxWindow)
  EVT_PAINT(Hint::OnPaint)
END_EVENT_TABLE()


Hint::Hint( wxWindow *parent, wxWindowID id, const wxString& label, 
	    const wxPoint& pos, const wxSize& size, 
	    const wxColour& bgcolour, const wxColour& fgcolour )
  : wxWindow(parent, id, pos, size)//, wxSIMPLE_BORDER)
  //: wxStaticText(parent, id, label, pos, size, wxALIGN_CENTRE, "")
{
  SetBackgroundColour(bgcolour);
  SetForegroundColour(fgcolour);
  Label = new wxStaticText(this, -1, label, wxPoint(0, 0), size, wxALIGN_RIGHT,
			   wxT(""));
  Label->SetFont(wxFont(10, wxMODERN, wxNORMAL, wxNORMAL));
  SetLabel(label);
}

Hint::~Hint()
{
	if (Label)
		delete Label;
}

void Hint::SetLabel(const wxString& label)
{
  Label->SetLabel(label);
  //  wxPaintDC dc(this);
  SetSize(Label->GetSize() + wxSize(3,0)
	  //GetBestSize()
	  //GetBestFittingSize()
	  );
  Label->SetLabel(label);
  /*PrepareDC(dc);
  dc.SetPen(*wxBLACK);
  dc.DrawRectangle(wxPoint(0,0), GetSize());*/
}

void Hint::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  PrepareDC(dc);
  dc.SetPen(*wxBLACK);
  dc.DrawRectangle(wxPoint(0,0), GetSize());// - wxSize(1,1));
}

