// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#include "VUMCtrl.h"
#include "Settings.h"
BEGIN_EVENT_TABLE(VUMCtrl, wxWindow)
  EVT_PAINT(VUMCtrl::OnPaint)
END_EVENT_TABLE()

  VUMCtrl::VUMCtrl(wxWindow *parent, wxWindowID id, int max_value,
		   wxImage *green , wxImage *orange, wxImage *red,
		   const wxPoint &pos, const wxSize &size, long style)
    : wxWindow(parent, id, pos, size, style), Max_Value(max_value), img_g(green), img_o(orange), img_r(red)
{
  SetBackgroundColour(*wxBLACK);
  height = size.GetHeight() - 4;
  width = size.GetWidth() -4 ;
  coeff = (float)height / (float)Max_Value;
  green_height = (height / 4) * 3;
  red_height = (height /4) /3;
  orange_height = red_height * 2 ;
}


void VUMCtrl::OnPaint(wxPaintEvent &event)
{
  wxPaintDC dc(this);
  wxMemoryDC memDC;
  int x,y;

  dc.GetSize(&w, &h);
  dc.SetPen(*wxTRANSPARENT_PEN);
  tmp = 0;
  memDC.SelectObject(*img_g);

  // Green part
  
  if(value >= green_height)
    {      
      x = green_height /3;
      tmp = value - green_height;
      for(int i = 0; i < x; i++)
	{
	  dc.Blit(0,h -= 3, img_g->GetWidth(), 3, &memDC, 0, 0, wxCOPY, FALSE);
	}
     }
  else
    {
      x = (value) /3;
      for(int i = 0; i < x; i++)
	{
	  dc.Blit(0,h -= 3, img_g->GetWidth(), 3, &memDC, 0, 0, wxCOPY, FALSE);
	}
      tmp =0 ;
    }

  // orange part
  memDC.SelectObject(*img_o);
  if(tmp >= orange_height)
    {
      x = orange_height /3 ;
      tmp = tmp - orange_height;
      for(int i = 0; i < x; i++)
	{
	  dc.Blit(0,h -= 3, img_g->GetWidth(), 3, &memDC, 0, 0, wxCOPY, FALSE);
	}
    }
  else
    {
      x = tmp / 3;
      for(int i = 0; i < x; i++)
	{
	  dc.Blit(0,h -= 3, img_g->GetWidth(), 3, &memDC, 0, 0, wxCOPY, FALSE);
	}
      tmp = 0;
    }

  // red part
  memDC.SelectObject(*img_r);
  if(tmp >= red_height)
    {
      x = orange_height /3 ;
      tmp = tmp - red_height;
      for(int i = 0; i < x; i++)
	{
	  dc.Blit(0,h -= 3, img_g->GetWidth(), 3, &memDC, 0, 0, wxCOPY, FALSE);
	}
    }
  else
    {
      x = tmp / 3;
      for(int i = 0; i < x; i++)
	{
	  dc.Blit(0,h -= 3, img_g->GetWidth(), 3, &memDC, 0, 0, wxCOPY, FALSE);
	}
    } 
  
}

void VUMCtrl::SetValue(int val)
{
  value = (int)(val * coeff);
  Refresh();
}

int VUMCtrl::GetValue()
{
  int tmp;
  tmp = (int)(value / coeff);
  return(tmp);
}


VUMCtrl::~VUMCtrl()
{
//	if (img_o)
//		delete img_o;
//	if (img_r)
//		delete img_r;
//	if (img_g)
//		delete img_g;
}
