#include "EditNote.h"
#include "MidiPart.h"

BEGIN_EVENT_TABLE(EditNote, wxControl)
	EVT_PAINT(EditNote::OnPaint)
	EVT_MOTION(EditNote::OnMouseMove)
	EVT_LEFT_UP(EditNote::OnLeftUp)
	EVT_LEFT_DOWN(EditNote::OnLeftDown)
END_EVENT_TABLE()
	
EditNote::EditNote(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		const wxSize &size, Note *note):

wxPanel(parent, id, pos, size)
{
	n = note;
	dragging = false;
	backsave = NULL;
	ZoomX = 1;
}

void EditNote::OnPaint(wxPaintEvent &e)
{
    wxPaintDC     dc(this);
    if (!dragging)
      dc.SetBrush(wxColor(0x00, 0xff, 0x00));
    else
      dc.SetBrush(wxColor(0xD0, 0XD0, 0XD0));
    dc.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
}

void EditNote::SetZoomX(double zoom)
{
	ZoomX = zoom;
}

void EditNote::OnLeftUp(wxMouseEvent &e)
{
	dragging = false;
	Move(GetPosition().x + mx - dx, GetPosition().y + my - dy);
	n->SetPos(GetPosition().x / (4 * ZoomX * ROW_WIDTH));	
	n->SetNote(127 - GetPosition().y / ROW_HEIGHT);
	delete backsave;
	backsave = NULL;
	Refresh(true);
	GetParent()->Refresh(true);
	((MidiPart *)GetParent())->em->ma->Refresh(true);
}

void EditNote::OnLeftDown(wxMouseEvent &e)
{
	dx = e.GetPosition().x;
	dy = e.GetPosition().y;
	dragging = true;
	Refresh(true);
}

void EditNote::OnMouseMove(wxMouseEvent &e)
{
	if (e.Dragging())
	{
    	  wxClientDC	dc(GetParent());
          dc.BeginDrawing();
	  wxMemoryDC	memdc;
	  if (backsave != NULL)
	  {
	    memdc.SelectObject(*backsave);
	    dc.Blit(GetPosition().x + mx - dx, GetPosition().y + my - dy, GetSize().GetWidth(), GetSize().GetHeight(), &memdc, 0, 0);
	  }
	  else
	  {
	    backsave = new wxBitmap(GetSize().GetWidth(), GetSize().GetHeight());
	    memdc.SelectObject(*backsave);
	  }
	  mx = e.GetPosition().x;
	  my = e.GetPosition().y;
	  while ((my - dy) % ROW_HEIGHT) my++;
	  memdc.Blit(0, 0, GetSize().GetWidth(), GetSize().GetHeight(), &dc, GetPosition().x + mx - dx, GetPosition().y + my - dy);
          dc.SetBrush(wxColor(0x00, 0xff, 0x00));
          dc.DrawRectangle(GetPosition().x + mx - dx, GetPosition().y + my - dy, GetSize().GetWidth(), GetSize().GetHeight());
          dc.EndDrawing();
       }
}




