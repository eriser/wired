// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#include <wx/gdicmn.h>
#include "Colour.h"
#include "ColoredLine.h"
#include "SelectionZone.h"
#include "SequencerGui.h"

SelectionZone::SelectionZone(SequencerView *S)
{
  State = 0;
  x_pos = 0;
  y_pos = 0;
  width = 0;
  height = 0;
  T = new ColoredLine(S, -1, wxPoint(-1, -1), wxSize(0, 1), CL_SELECTED_ZONE);
  B = new ColoredLine(S, -1, wxPoint(-1, -1), wxSize(0, 1), CL_SELECTED_ZONE);
  L = new ColoredLine(S, -1, wxPoint(-1, -1), wxSize(1, 0), CL_SELECTED_ZONE);
  R = new ColoredLine(S, -1, wxPoint(-1, -1), wxSize(1, 0), CL_SELECTED_ZONE);
}

SelectionZone::SelectionZone(const unsigned long x, const unsigned long y,
			     const unsigned long w, const unsigned long h, SequencerView *S)
{
  State = STATE_VISIBLE;
  x_pos = 0;
  y_pos = 0;
  width = 0;
  height = 0;
  T = new ColoredLine(S, -1, wxPoint(x,         y), wxSize(w, 1), CL_SELECTED_ZONE);
  B = new ColoredLine(S, -1, wxPoint(x, h - 1 + y), wxSize(w, 1), CL_SELECTED_ZONE);
  L = new ColoredLine(S, -1, wxPoint(        x, y), wxSize(1, h), CL_SELECTED_ZONE);
  R = new ColoredLine(S, -1, wxPoint(w - 1 + x, y), wxSize(1, h), CL_SELECTED_ZONE);
}

SelectionZone::~SelectionZone()
{
  delete (T);
  delete (B);
  delete (L);
  delete (R);
}

void				SelectionZone::SetZone(const unsigned long x, const unsigned long y,
						       const unsigned long w, const unsigned long h)
{
  State = STATE_VISIBLE;
  x_pos = x;
  y_pos = y;
  height = h;
  width = w;
  T->SetSize((int)         x, (int)         y, (int) w - 1, 1, wxSIZE_USE_EXISTING);
  B->SetSize((int)     1 + x, (int) h - 1 + y, (int) w - 1, 1, wxSIZE_USE_EXISTING);
  L->SetSize((int)         x, (int)     1 + y, 1, (int) h - 1, wxSIZE_USE_EXISTING);
  R->SetSize((int) w - 1 + x, (int)         y, 1, (int) h - 1, wxSIZE_USE_EXISTING);
  T->Show(true);
  B->Show(true);
  L->Show(true);
  R->Show(true);
}

void				SelectionZone::UpdateZone(long x, long y)
{
  unsigned long			x_yop;
  unsigned long			y_yop;

  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  if (x_pos < x)
    {
      State &= ~STATE_X_REVERSED;
      x_yop = x_pos;
      width = x - x_pos;
    }
  else
    {
      State |= (unsigned char) STATE_X_REVERSED;
      x_yop = x;
      width = x_pos - x;
    }
  if (y_pos < y)
    {
      State &= ~STATE_Y_REVERSED;
      y_yop = y_pos;
      height = y - y_pos;
    }
  else
    {
      State |= (unsigned char) STATE_Y_REVERSED;
      y_yop = y;
      height = y_pos - y;
    }
  T->SetSize((int)             x_yop, (int)              y_yop, (int)  width - 1, 1, wxSIZE_USE_EXISTING);
  B->SetSize((int)         1 + x_yop, (int) height - 1 + y_yop, (int)  width - 1, 1, wxSIZE_USE_EXISTING);
  L->SetSize((int)             x_yop, (int)          1 + y_yop, 1, (int) height - 1, wxSIZE_USE_EXISTING);
  R->SetSize((int) width - 1 + x_yop, (int)              y_yop, 1, (int) height - 1, wxSIZE_USE_EXISTING);
}

void				SelectionZone::Hide()
{
  State = 0;
  T->Show(false);
  B->Show(false);
  L->Show(false);
  R->Show(false);
}
