// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "ASKey.h"

BEGIN_EVENT_TABLE(ASKey, wxPanel)
	EVT_PAINT(ASKey::OnPaint)
	EVT_LEFT_DOWN(ASKey::OnLeftDown)
	EVT_LEFT_UP(ASKey::OnLeftUp)
  	EVT_LEFT_DCLICK(ASKey::OnLeftDown)
END_EVENT_TABLE()

#include <iostream>
	using namespace std;

#include "ASClavier.h"

	ASKey::ASKey(wxWindow *parent, wxWindowID id, const wxPoint& pos,
			const wxSize& size, bool pisBlack, wxString pnote, int note_code):
		wxPanel(parent, id, pos, size)
{
	isBlack = pisBlack;
	note = pnote;
	selected = false;
	code = note_code;
}

void	ASKey::OnPaint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);
	wxSize 	  	s;
	wxBrush		myGreyBrush(wxColor(0xE6, 0xE6, 0xE6));

	s = GetSize();
	if (isBlack)
	{
		if (selected)
			dc.SetPen(*wxWHITE_PEN);
		else
			dc.SetPen(*wxBLACK_PEN);
		dc.SetBrush(*wxBLACK_BRUSH);
	}
	else
	{
		dc.SetPen(*wxBLACK_PEN);
		if (selected)
			dc.SetBrush(*wxGREY_BRUSH);
		else
			dc.SetBrush(myGreyBrush);
	}
	dc.DrawRectangle(0, 0, s.GetWidth(), s.GetHeight()); 
}

void	ASKey::OnLeftDown(wxMouseEvent &event)
{
	if (event.LeftIsDown())
	{
		selected = !selected;
		wxSize s = GetSize();
		wxRect rect(0, 0, s.GetWidth(), s.GetHeight());
		Refresh(&rect);

		wxMouseEvent evt(wxEVT_LEFT_DOWN);
		evt.SetEventObject(this);
		GetParent()->ProcessEvent(evt);
	}
}

void	ASKey::OnLeftUp(wxMouseEvent	&event)
{
	selected = !selected;
	wxSize s = GetSize();
	wxRect rect(0, 0, s.GetWidth(), s.GetHeight());
	Refresh(&rect);

	wxMouseEvent evt(wxEVT_LEFT_UP);
	evt.SetEventObject(this);
	GetParent()->ProcessEvent(evt);
}
