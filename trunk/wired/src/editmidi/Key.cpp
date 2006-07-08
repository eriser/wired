// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "Key.h"

BEGIN_EVENT_TABLE(Key, wxPanel)
	EVT_PAINT(Key::OnPaint)
	EVT_LEFT_DOWN(Key::OnLeftDown)
	EVT_LEFT_UP(Key::OnLeftUp)
  //	EVT_ENTER_WINDOW(Key::OnEnterWindow)
  //	EVT_LEAVE_WINDOW(Key::OnLeaveWindow)
END_EVENT_TABLE()

#include <iostream>
using namespace std;

#include "Clavier.h"

Key::Key(wxWindow *parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, bool pisBlack, wxString pnote, int note_code):
wxPanel(parent, id, pos, size)
{
	isBlack = pisBlack;
	note = pnote;
        selected = false;
	code = note_code;
	cout << "str: " << pnote.mb_str() << "; code: " << code << endl;
}

void	Key::OnPaint(wxPaintEvent &event)
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

void	Key::OnLeftDown(wxMouseEvent &event)
{
  if (event.LeftIsDown())
    {
      selected = true;
      wxSize s = GetSize();
      wxRect rect(0, 0, s.GetWidth(), s.GetHeight());
      printf("click on %s\n", note.c_str());
      printf("MIDINOTE: 0x%02X\n", code);
      Refresh(&rect);

      wxMouseEvent evt(wxEVT_LEFT_DOWN);
      evt.SetEventObject(this);
      GetParent()->ProcessEvent(evt);
    }
}

void	Key::OnLeftUp(wxMouseEvent	&event)
{
  selected = false;
  wxSize s = GetSize();
  wxRect rect(0, 0, s.GetWidth(), s.GetHeight());
  Refresh(&rect);
  
  wxMouseEvent evt(wxEVT_LEFT_UP);
  evt.SetEventObject(this);
  GetParent()->ProcessEvent(evt);
}

/*
void	Key::OnEnterWindow(wxMouseEvent &event)
{
	if (event.LeftIsDown())
		OnLeftDown(event);
}

void	Key::OnLeaveWindow(wxMouseEvent	&event)
{
	if (selected)
	{
		OnLeftUp(event);
	}
}
*/
