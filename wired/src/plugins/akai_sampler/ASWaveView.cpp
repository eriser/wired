#include "ASWaveView.h"

ASWaveView::ASWaveView(wxWindow *parent, int id, wxPoint pt, wxSize &sz) : WaveView(parent, id, pt, sz, true, false)
{
}

void ASWaveView::SetSample(WaveFile *w)
{
  WaveView::SetWave(w);
}

void ASWaveView::SetSize(wxSize sz)
{
  WaveView::SetSize(sz);
}

wxMemoryDC *ASWaveView::GetWaveDC()
{
  if (Bmp)
  {
    memDC.SelectObject(*Bmp);
    return &memDC;
  }
  return new wxMemoryDC();
}
