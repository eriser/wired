#include <wx/wx.h>
#include "ASLoop.h"

BEGIN_EVENT_TABLE(ASLoop, wxWindow)
  EVT_PAINT(ASLoop::OnPaint)
  EVT_SIZE(ASLoop::OnResize)
  EVT_MOTION(ASLoop::OnMouseMove)
  EVT_LEFT_UP(ASLoop::OnLeftUp)
  EVT_LEFT_DOWN(ASLoop::OnLeftDown)
  EVT_RIGHT_DOWN(ASLoop::OnRightDown)
END_EVENT_TABLE()

ASLoop::ASLoop(class AkaiSampler *as, wxString Name) :
  ASPlugin(as, Name)
{
  wv = NULL;
}

ASLoop::~ASLoop()
{
}

wxWindow *ASLoop::CreateView(wxPanel *p, wxPoint &pt, wxSize &sz)
{
  Reparent(p);
  SetSize(sz);
  Move(pt);
  wv = new ASWaveView(this, -1, wxPoint(0, 0), sz);
  wv->Show(false);
  wv->Connect(wv->GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction) &ASLoop::OnMouseMove);
  wv->Connect(wv->GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction) &ASLoop::OnLeftDown);
  wv->Connect(wv->GetId(), wxEVT_LEFT_UP, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction) &ASLoop::OnLeftUp);
  wv->Connect(wv->GetId(), wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction) &ASLoop::OnRightDown);
  wv->SetSize(sz);
  if (this->ass)
    wv->SetSample(ass->GetSample());
  Show(true);
  return this;
}

void ASLoop::SetSample(ASamplerSample *ass)
{
  this->ass = ass;
  if (wv)
  {
    wv->SetSample(this->ass->GetSample());
    Refresh();
  }
}

void ASLoop::OnPaint(wxPaintEvent &e)
{
  if (wv && ass)
  {
    wv->OnPaint(e);
    wxMemoryDC mdc;
    wxBitmap *bmp = wv->GetBitmap();
    wxPaintDC dc(this);
    dc.BeginDrawing();
    if (bmp)
    {
      mdc.SelectObject(*bmp);
      dc.Blit(0, 0, GetSize().GetWidth(), GetSize().GetHeight(), &mdc, 0, 0, wxCOPY, FALSE);
    }
    int x;
    dc.SetPen(wxPen(wxColor(0x00, 0x00, 0xFF), 2));
    x = (GetSize().GetWidth() * ass->GetLoopStart()) / ass->GetSample()->GetNumberOfFrames();
    dc.DrawLine(x, 0, x, GetSize().GetHeight());
    dc.SetPen(wxPen(wxColor(0x00, 0xFF, 0x00), 2));
    x = (GetSize().GetWidth() * ass->GetLoopEnd()) / ass->GetSample()->GetNumberOfFrames();
    dc.DrawLine(x, 0, x, GetSize().GetHeight());
    dc.EndDrawing();
  }
}

void ASLoop::OnResize(wxSizeEvent &e)
{
  if (wv)
  {
    wv->SetSize(e.GetSize());
    Refresh();
  }
}

void ASLoop::OnMouseMove(wxMouseEvent &e)
{
  if (wv && ass)
  {
    if (e.LeftIsDown())
    {
      int x = (GetSize().GetWidth() * ass->GetLoopEnd()) / ass->GetSample()->GetNumberOfFrames();
      if (e.GetX() < x)
      {
        int d = ass->GetLoopStart();
        ass->SetLoopStart((e.GetX() * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth());
        Refresh();
      }
    }
    if (e.RightIsDown())
    {
      int x = (GetSize().GetWidth() * ass->GetLoopStart()) / ass->GetSample()->GetNumberOfFrames();
      if (e.GetX() > x)
      {
        x = (e.GetX() * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth();
        if (x <= ass->GetSample()->GetNumberOfFrames())
        {
          ass->SetLoopEnd(x);
          Refresh();
        }
      }
    }
  }
}

void ASLoop::OnLeftDown(wxMouseEvent &e)
{
  if (ass && wv)
  {
    int x = (GetSize().GetWidth() * ass->GetLoopEnd()) / ass->GetSample()->GetNumberOfFrames();
    if (e.GetX() < x)
    {
      int d = ass->GetLoopStart();
      ass->SetLoopStart((e.GetX() * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth());
      Refresh();
    }
 }
}

void ASLoop::OnLeftUp(wxMouseEvent &e)
{
}

void ASLoop::OnRightDown(wxMouseEvent &e)
{
  if (wv && ass)
  {
    int x = (GetSize().GetWidth() * ass->GetLoopStart()) / ass->GetSample()->GetNumberOfFrames();
    if (e.GetX() > x)
    {
      x = (e.GetX() * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth();
      if (x <= ass->GetSample()->GetNumberOfFrames())
      {
        ass->SetLoopEnd(x);
        Refresh();
      }
    }
  }
}

