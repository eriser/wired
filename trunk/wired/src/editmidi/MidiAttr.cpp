// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include "MidiAttr.h"
#include "MidiPart.h"
#include "Clavier.h"

MidiAttr::MidiAttr(wxWindow *parent, wxWindowID id, const wxPoint &pos,
		const wxSize& size, long style):
  wxPanel(parent, id, pos, size, style)
{
}

void				MidiAttr::SetNotes(vector <Note *> recnote)
{
  this->Notes = recnote;
  this->ZoomX = 1;
}

void				MidiAttr::OnPaint(wxPaintEvent &e)
{
  wxPaintDC			dc(this);
  
  wxRegionIterator upd(GetUpdateRegion());
  while (upd)
    {
      for (unsigned int i = 0; i < Notes.size(); i++)
	{
	  if (Notes[i])
	    {
	      int x = (int)floor(Notes[i]->GetPos() * 4 * ROW_WIDTH * ZoomX);
	      int perc_height = (Notes[i]->GetVelocity() * GetSize().GetHeight()) / 128;
	      int y = GetSize().GetHeight() - perc_height;
	      
	      dc.SetPen(wxPen(wxColor(Notes[i]->GetVelocity() + 75, 0x00, 0x00)));
	      dc.SetBrush(wxBrush(wxColor(Notes[i]->GetVelocity() + 128, 0x00, 0x00)));
	      wxRect a(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH());
	      wxRect b(x, y, 5, perc_height);
	      wxRect *rect = CalcIntersection(a, b);
	      if (rect != NULL)
		{
		  dc.DrawRectangle(rect->x, rect->y, rect->width, rect->height);
		  delete rect;
		}
	    }
	}
      upd++;
    }
}

BEGIN_EVENT_TABLE(MidiAttr, wxPanel)
  EVT_PAINT(MidiAttr::OnPaint)
END_EVENT_TABLE()
  
