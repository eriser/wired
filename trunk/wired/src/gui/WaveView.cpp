// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "WaveView.h"
#include "Colour.h"
#include "Settings.h"

#include <math.h>
#include <iostream>

using namespace std;

WaveView::WaveView(wxWindow *parent, wxWindowID id, const wxPoint& pos, 
		   const wxSize& size, bool fulldraw, bool use_settings) 
  : wxWindow(parent, id, pos, size), WaveDrawer(size, fulldraw, use_settings)
{
  //printf("WaveView::WaveView(..., id=%d, fulldraw=%d) -- START\n", id, fulldraw);
  FullDraw = fulldraw;
  if (GetSize().x == 0)
    WaveDrawer::SetSize(1, GetSize().y);
  //printf("WaveView::WaveView(..., id=%d, fulldraw=%d) -- OVER\n", id, fulldraw);
}

WaveView::~WaveView()
{

}

void					WaveView::OnPaint(wxPaintEvent &event)
{
  wxPaintDC				dc(this);
  wxRegionIterator			region(GetUpdateRegion());
  
  WaveDrawer::OnPaint(dc, GetSize(), region);
}

void					WaveView::OnSize(wxSizeEvent &event)
{
  if (FullDraw && (Data || (Wave && !Wave->LoadedInMem)))
    {
      WaveDrawer::SetDrawing(GetSize());
      Refresh();
    }  
}

void					WaveView::SetWave(float **data, unsigned long frame_length, long channel_count)
{
  WaveDrawer::SetWave(data, frame_length, channel_count, GetSize());
}

void					WaveView::SetWave(WaveFile *w)
{
  WaveDrawer::SetWave(w, GetSize());
}

void					WaveView::SetDrawing()
{
  WaveDrawer::SetDrawing(GetSize());
}

void					WaveView::SetSize(wxSize s)
{
  if (s == GetSize())
    return;
  wxWindow::SetSize(s);
  if (Data || (Wave && !Wave->LoadedInMem))
    {
      WaveDrawer::SetDrawing(s);
      Refresh();
    }  
}

/*
void					WaveView::SetSize(int x, int y)
{
  wxSize				s(x, y);
  SetSize(s);
}

*/

BEGIN_EVENT_TABLE(WaveView, wxWindow)
  EVT_PAINT(WaveView::OnPaint)
  EVT_SIZE(WaveView::OnSize)
END_EVENT_TABLE()
