// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/wx.h>
#include "ASLoop.h"
#include "Colour.h"

#define SBS 16
#define ID_SB_X 54327
#define ID_SB_Y 54328
#define ZSX 100
#define ID_ZOOMX  54329
#define ID_ZOOMY  54314

BEGIN_EVENT_TABLE(ASLoop, wxWindow)
  EVT_PAINT(ASLoop::OnPaint)
  EVT_SIZE(ASLoop::OnResize)
  EVT_MOTION(ASLoop::OnMouseMove)
  EVT_LEFT_UP(ASLoop::OnLeftUp)
  EVT_LEFT_DOWN(ASLoop::OnLeftDown)
  EVT_RIGHT_DOWN(ASLoop::OnRightDown)
  EVT_SCROLL(ASLoop::OnScroll)
  EVT_SCROLL_THUMBTRACK(ASLoop::OnDragThumb)
  EVT_SCROLL_THUMBRELEASE(ASLoop::OnStopDragThumb)
END_EVENT_TABLE()

ASLoop::ASLoop(class AkaiSampler *as, wxString Name) :
  ASPlugin(as, Name)
{
  wv = NULL;
  ZoomY = 1;
  ZoomX = 1;
  sbx = NULL;
  sby = NULL;
  zx = NULL;
  zy = NULL;
  thumbdrag = false;
  type = ASLoop::GetFXName();
}

ASLoop::~ASLoop()
{
}

wxWindow *ASLoop::CreateView(wxPanel *p, wxPoint &pt, wxSize &sz)
{
  Reparent(p);
  SetSize(sz);
  Move(pt);
  wv = new ASWaveView(wxSize(ZoomX * (sz.GetWidth() - SBS * 2), ZoomY * (sz.GetHeight() - SBS)));
  wv->SetBrushColor(wxColor(0, 0, 0));
  sbx = new wxScrollBar(this, ID_SB_X, wxPoint(ZSX, sz.GetHeight() - SBS), wxSize(sz.GetWidth() - SBS * 2 - ZSX, SBS), wxSB_HORIZONTAL);
  sbx->SetScrollbar(0, 10, wv->GetSize().GetWidth() - (sz.GetWidth() - SBS * 2), 10, false);
  sby = new wxScrollBar(this, ID_SB_Y, wxPoint(sz.GetWidth() - SBS, 0), wxSize(SBS, sz.GetHeight() - SBS), wxSB_VERTICAL);
  sby->SetScrollbar((wv->GetSize().GetHeight() - (sz.GetHeight() - SBS)) / 2, 10, wv->GetSize().GetHeight() - (sz.GetHeight() - SBS), 10, false);
  zx = new wxSlider(this, ID_ZOOMX, 1, 1, 10,
                   wxPoint(0, sz.GetHeight() - SBS),
                   wxSize(ZSX, SBS), wxSL_HORIZONTAL);
  zy = new wxSlider(this, ID_ZOOMY, 1, 1, 5,
                   wxPoint(0, 0),
                   wxSize(SBS, GetSize().GetHeight() - SBS), wxSL_VERTICAL);
  SetBackgroundColour(CL_OPTION_TOOLBAR);
  sbx->SetBackgroundColour(CL_OPTION_TOOLBAR);
  sby->SetBackgroundColour(CL_OPTION_TOOLBAR);
  sbx->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));
  sby->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));
  zx->SetBackgroundColour(CL_OPTION_TOOLBAR);
  zy->SetBackgroundColour(CL_OPTION_TOOLBAR);
  zx->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));
  zy->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));

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
    if (ass->GetLoopCount() == 0)
      ass->SetLoopCount(-1);
    Refresh();
  }
}

void ASLoop::OnPaint(wxPaintEvent &e)
{
  if (wv && ass)
  {
    int sx = GetSize().GetWidth() - SBS * 2;
    int sy = GetSize().GetHeight() - SBS;
    wxMemoryDC mdc;
    wxBitmap *bmp = wv->GetBitmap();
    wxPaintDC dc(this);

    if (bmp)
    {
      mdc.SelectObject(*bmp);
      int starty = sby->GetThumbPosition();
      int startx = sbx->GetThumbPosition();
      dc.Blit(SBS, 0, sx, sy, &mdc, startx, starty, wxCOPY, FALSE);
    }
    int x;
    dc.SetPen(wxPen(wxColor(0x00, 0x00, 0xFF), 2));
    x = ZoomX * GetSize().GetWidth() * ass->GetLoopStart() / ass->GetSample()->GetNumberOfFrames() - sbx->GetThumbPosition() + SBS;
    if (x >= SBS)
      dc.DrawLine(x, 0, x, GetSize().GetHeight() - SBS);
    dc.SetPen(wxPen(wxColor(0x00, 0xFF, 0x00), 2));
    x = ZoomX * GetSize().GetWidth() * ass->GetLoopEnd() / ass->GetSample()->GetNumberOfFrames() - sbx->GetThumbPosition() + SBS;
    if (x <= GetSize().GetWidth() - SBS)
      dc.DrawLine(x, 0, x, GetSize().GetHeight() - SBS);
  }
}

void ASLoop::OnResize(wxSizeEvent &e)
{
  if (wv)
  {
    wv->SetSize(wxSize((int)(e.GetSize().GetWidth() * ZoomX), (int)(e.GetSize().GetHeight() * ZoomY)));
    sbx->SetSize(ZSX, e.GetSize().GetHeight() - SBS, e.GetSize().GetWidth() - ZSX, SBS);
    sby->SetSize(e.GetSize().GetWidth() - SBS, 0, SBS, e.GetSize().GetHeight() - SBS);
    zx->SetSize(0, e.GetSize().GetHeight() - SBS, ZSX, SBS);
    zy->SetSize(0, 0, SBS, GetSize().GetHeight() - SBS);
    sbx->SetScrollbar(0, 10, wv->GetSize().GetWidth() - (e.GetSize().GetWidth() - SBS * 2), 10, false);
    sby->SetScrollbar((wv->GetSize().GetHeight() - (e.GetSize().GetHeight() - SBS)) / 2, 10, wv->GetSize().GetHeight() - (e.GetSize().GetHeight() - SBS), 10, false);
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
      if ((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX < x)
      {
        ass->SetLoopStart((((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX) * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth());
        Refresh();
      }
    }
    if (e.RightIsDown())
    {
      int x = (GetSize().GetWidth() * ass->GetLoopStart()) / ass->GetSample()->GetNumberOfFrames();
      if ((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX > x)
      {
        x = (((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX) * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth();
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
    if ((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX < x)
    {
      ass->SetLoopStart((((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX) * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth());
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
    if ((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX > x)
    {
      x = (((sbx->GetThumbPosition() + e.GetX() - SBS) / ZoomX) * ass->GetSample()->GetNumberOfFrames()) / GetSize().GetWidth();
      if (x <= ass->GetSample()->GetNumberOfFrames())
      {
        ass->SetLoopEnd(x);
        Refresh();
      }
    }
  }
}

void ASLoop::OnScroll(wxScrollEvent &e)
{
  if (((e.GetId() == ID_ZOOMX) || (e.GetId() == ID_ZOOMY)))
  {
    ZoomX = zx->GetValue();
    ZoomY = zy->GetValue();
    if (!thumbdrag)
    {
      wxSizeEvent ev(GetSize());
      OnResize(ev);
    }
    return;
  }
  Refresh();
}

void ASLoop::OnDragThumb(wxScrollEvent &e)
{
  thumbdrag = true;
}

void ASLoop::OnStopDragThumb(wxScrollEvent &e)
{
  thumbdrag = false;
  wxSizeEvent ev(GetSize());
  OnResize(ev);
}
