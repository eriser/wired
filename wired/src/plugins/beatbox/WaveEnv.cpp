#include "WaveEnv.h"

WaveEnv::WaveEnv(wxWindow *parent, wxWindowID id,
		 const wxPoint& pos, const wxSize& size)
  : WaveView(parent, id, pos, size, true)
{
  Connect(GetId(), wxEVT_LEFT_DOWN, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WaveEnv::OnClick);
  Connect(GetId(), wxEVT_SIZE, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxSizeEventFunction)&WaveEnv::OnSize);
  Connect(GetId(), wxEVT_PAINT, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxPaintEventFunction)&WaveEnv::OnPaint);
}

WaveEnv::~WaveEnv()
{

}

void WaveEnv::OnSize(wxSizeEvent &event)
{
  WaveView::OnSize(event);
}

void WaveEnv::OnPaint(wxPaintEvent &event)
{
  if (!Wave)
    return;
  WaveView::OnPaint(event);
}

void WaveEnv::OnClick(wxMouseEvent &event)
{
}

