#include <wx/wx.h>
#include "ASEnvel.h"
#include "Colour.h"

#define SBS 16
#define ID_SB_X 54321
#define ID_SB_Y 54322
#define ZSX 100
#define ID_ZOOMX  54323
#define ID_ZOOMY  54324

ASEnvelSeg::ASEnvelSeg(unsigned long wavelen, wxSize sz)
{
  wl = wavelen;
  size = sz;
  ratiox = ((double)size.GetWidth()) / wl;
  ratioy = ((double)size.GetHeight()) / 2000.0f;
  nbpts = 2;
  points = (t_pt *)malloc(sizeof(t_pt) * nbpts);
  points[0].x = 0;
  points[0].y = 200;
  points[1].x = wl - 100;
  points[1].y = 200;
}

ASEnvelSeg::~ASEnvelSeg()
{
  free(points);
}

int ASEnvelSeg::IsCtrlPoint(wxPoint p, int precision)
{
  for (unsigned int i = 0; i < nbpts; i++)
    if ((abs(p.x - ratiox * points[i].x) <= precision) && (abs(p.y - ratioy * points[i].y) <= precision))
      return i;
  return -1;
}

void ASEnvelSeg::Paint(wxMemoryDC &dc, int ZoomX, int ZoomY, int startx, int starty)
{
  static wxPen    greenPen(wxColor(0x00, 0xFF, 0x00), 1);
  dc.SetPen(greenPen);
  for (unsigned int i = 0; i < nbpts; i++)
  {
    if (!i || (i == nbpts - 1))
    {
      dc.DrawRectangle((int)(ZoomX * ratiox * points[i].x) - 4 - startx, ZoomY * size.GetHeight() / 2 - (int)(ZoomY * ratioy * points[i].y) - 4 - starty, 8, 8);
      dc.DrawRectangle((int)(ZoomX * ratiox * points[i].x) - 4 - startx, ZoomY * size.GetHeight() / 2 + (int)(ZoomY * ratioy * points[i].y) - 4 - starty, 8, 8);
    }
    else
    {
      dc.DrawEllipse((int)(ZoomX * ratiox * points[i].x) - 4 - startx, ZoomY * size.GetHeight() / 2 - (int)(ZoomY * ratioy * points[i].y) - 4 - starty, 8, 8);
      dc.DrawEllipse((int)(ZoomX * ratiox * points[i].x) - 4 - startx, ZoomY * size.GetHeight() / 2 + (int)(ZoomY * ratioy * points[i].y) - 4 - starty, 8, 8);
    }
  }
  int x = points[0].x;
  int y = points[0].y;
  int ox;
  int oy;
  for (unsigned int i = 1; i < nbpts; i++)
  {
    ox = (int)(points[i - 1].x * ratiox * ZoomX) - startx;
    oy = (int)(points[i - 1].y * ratioy * ZoomY);
    x = (int)(points[i].x * ratiox * ZoomX) - startx;
    y = (int)(points[i].y * ratioy * ZoomY);
    dc.DrawLine(ox, ZoomY * size.GetHeight() / 2 - oy - starty, x, ZoomY * size.GetHeight() / 2 - y - starty);
    dc.DrawLine(ox, ZoomY * size.GetHeight() / 2 + oy - starty, x, ZoomY * size.GetHeight() / 2 + y - starty);
  }
}

void ASEnvelSeg::AddPoint(wxPoint p)
{
  int i, j;
  t_pt *tmp = (t_pt *)malloc(sizeof(t_pt) * (nbpts + 1));
  for (i = j = 0; i < nbpts; i++)
  {
    if ((points[i].x >= p.x / ratiox) && (j == i))
    {
      tmp[j].x = (int)(p.x / ratiox);
      tmp[j++].y = (int)(p.y / ratioy);
    }
    tmp[j].x = points[i].x;
    tmp[j++].y = points[i].y;
  }
  free(points);
  points = tmp;
  nbpts++;
}

void ASEnvelSeg::DelPoint(int n)
{
  if (n && (n != nbpts - 1))
  {
    t_pt *tmp = (t_pt *)malloc(sizeof(t_pt) * (nbpts - 1));
    unsigned int j, k;
    for (j = k = 0; j < nbpts; j++)
      if (j != n)
      {
        tmp[k].x = points[j].x;
        tmp[k].y = points[j].y;
        k++;
      }
    free(points);
    points = tmp;
    nbpts--;
  }
}

void ASEnvelSeg::SetPoint(int n, wxPoint p)
{
  if ((n >= 0) && (n < nbpts))
  {
    if ((n > 0) && (n < nbpts - 1))
      points[n].x = (int)(p.x / ratiox);
    points[n].y = (int)(p.y / ratioy);
  }
}


wxPoint ASEnvelSeg::GetPoint(int n)
{
  if ((n >= 0) && (n < nbpts))
    return wxPoint((int)(points[n].x * ratiox), (int)(points[n].y * ratioy));
  return wxPoint(-1, -1);
}

float ASEnvelSeg::GetCoef(long pos)
{
  if ((pos < 0) || (pos > wl))
    return 0;
  for (int i = 0; i < nbpts; i++)
  {
    if ((points[i].x > pos) && (points[i - 1].x <= pos))
    {
      double a = ((double)(points[i].y - points[i - 1].y)) / (points[i].x - points[i - 1].x);
      double b = points[i].y - points[i].x * a;
      return log(0.8f + ((a * pos + b) / 1000));
    }
  }
  return 0;
}

BEGIN_EVENT_TABLE(ASEnvel, wxWindow)
  EVT_PAINT(ASEnvel::OnPaint)
  EVT_SIZE(ASEnvel::OnResize)
  EVT_MOTION(ASEnvel::OnMouseMove)
  EVT_LEFT_UP(ASEnvel::OnLeftUp)
  EVT_LEFT_DOWN(ASEnvel::OnLeftDown)
  EVT_RIGHT_DOWN(ASEnvel::OnRightDown)
  EVT_SCROLL(ASEnvel::OnScroll)
  EVT_SCROLL_THUMBTRACK(ASEnvel::OnDragThumb)
  EVT_SCROLL_THUMBRELEASE(ASEnvel::OnStopDragThumb)
END_EVENT_TABLE()

ASEnvel::ASEnvel(wxString Name) :
    ASPlugin(Name)
{
  Grid = NULL;
  seg = NULL;
  dragging = -1;
  thumbdrag = false;
  ass = NULL;
  wv = NULL;
  ZoomY = 1;
  ZoomX = 1;
  sbx = NULL;
  sby = NULL;
}

ASEnvel::~ASEnvel()
{
  if (Grid)
    delete Grid;
  if (seg)
    delete seg;
}

wxWindow *ASEnvel::CreateView(wxPanel *p, wxPoint &pt, wxSize &sz)
{
  Reparent(p);
  SetSize(sz);
  Move(pt);

  if (!seg && ass)
    seg = new ASEnvelSeg(ass->GetSample()->GetNumberOfFrames(), wxSize(sz.GetWidth(), sz.GetHeight()));
  else if (seg)
    seg->SetSize(wxSize(sz.GetWidth(), sz.GetHeight()));
  wv = new ASWaveView(this, -1, wxPoint(0, 0), wxSize((int)(sz.GetWidth() * ZoomX), (int)(sz.GetHeight() * ZoomY)));
  wv->SetBrushColor(wxColor(0, 0, 0));
  wv->Show(false);
  if (ass)
    wv->SetSample(ass->GetSample());
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
  FillGrid();
  Show(true);
  return this;
}

void ASEnvel::SetSample(ASamplerSample *ass)
{
  this->ass = ass;
  if (!seg)
    seg = new ASEnvelSeg(ass->GetSample()->GetNumberOfFrames(), GetSize());
  else
    seg->SetWaveLen(ass->GetSample()->GetNumberOfFrames());
  if (wv)
  {
    wv->SetSample(this->ass->GetSample());
    FillGrid();
    Refresh();
  }
}

void ASEnvel::FillGrid()
{
  static wxPen    borderPen(wxColor(0xFF, 0xFF, 0xFF), 1);
  static wxBrush  blackBrush(wxColor(0x00, 0x00, 0x00));
  int sx = GetSize().GetWidth() - SBS * 2;
  int sy = GetSize().GetHeight() - SBS;
  if (Grid)
    delete Grid;
  wxMemoryDC memDC;
  Grid = new wxBitmap(sx, sy);
  memDC.SelectObject(*Grid);
  if (wv)
  {
    wxMemoryDC mdc;
    wxBitmap *bmp = wv->GetBitmap();
    if (bmp)
    {
      mdc.SelectObject(*bmp);
      int starty = sby->GetThumbPosition();
      int startx = sbx->GetThumbPosition();
      memDC.Blit(0, 0, sx, sy, &mdc, startx, starty, wxCOPY, FALSE);
    }
  }
  if (seg)
    seg->Paint(memDC, ZoomX, ZoomY, sbx->GetThumbPosition(), sby->GetThumbPosition());
}

void ASEnvel::MovePt(wxPoint p)
{
  if (seg)
  {
    wxPoint av = seg->GetPoint(dragging - 1);
    wxPoint ap = seg->GetPoint(dragging + 1);
    if ((av.x != -1) && ((sbx->GetThumbPosition() + p.x - SBS) / ZoomX > av.x + 4) && (ap.x != -1) && ((sbx->GetThumbPosition() + p.x - SBS) / ZoomX < ap.x - 4))
    {
      if (sby->GetThumbPosition() + p.y < ZoomY * GetSize().GetHeight() / 2)
        seg->SetPoint(dragging, wxPoint((sbx->GetThumbPosition() + p.x - SBS) / ZoomX, (ZoomY * GetSize().GetHeight() / 2 - p.y - sby->GetThumbPosition()) / ZoomY));
      else
        seg->SetPoint(dragging, wxPoint((sbx->GetThumbPosition() + p.x - SBS) / ZoomX, (-ZoomY * GetSize().GetHeight() / 2 + p.y + sby->GetThumbPosition())/ ZoomY));
    }
    else if ((av.x == -1) || (ap.x == -1))
    {
      if (sby->GetThumbPosition() + p.y < ZoomY * GetSize().GetHeight() / 2)
        seg->SetPoint(dragging, wxPoint(seg->GetPoint(dragging).x, (ZoomY * GetSize().GetHeight() / 2 - p.y - sby->GetThumbPosition()) / ZoomY));
      else
        seg->SetPoint(dragging, wxPoint(seg->GetPoint(dragging).x, (-ZoomY * GetSize().GetHeight() / 2 + p.y + sby->GetThumbPosition()) / ZoomY));
    }
    if (wv)
      wv->SetEnvel(seg);
    FillGrid();
    Refresh();
  }
}

void ASEnvel::OnPaint(wxPaintEvent &e)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  if (Grid)
  {
    memDC.SelectObject(*Grid);
    wxRegionIterator upd(GetUpdateRegion()); 
    while (upd)
    {
      int x = upd.GetX();
      if (x < SBS)
        x = SBS;
      dc.Blit(x, upd.GetY(), upd.GetW() - x + upd.GetX(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(),
            wxCOPY, FALSE);
      upd++;
    }
  }
}

void ASEnvel::OnResize(wxSizeEvent &e)
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
  }
  if (seg)
    seg->SetSize(wxSize(e.GetSize().GetWidth(), e.GetSize().GetHeight()));
  FillGrid();
  Refresh();
}

void ASEnvel::OnMouseMove(wxMouseEvent &e)
{
  if (dragging != -1)
    MovePt(e.GetPosition());
}

void ASEnvel::OnLeftDown(wxMouseEvent &e)
{
  if (seg)
  {
    if (e.GetPosition().y  + sby->GetThumbPosition() < ZoomY * GetSize().GetHeight() / 2)
      dragging = seg->IsCtrlPoint(wxPoint((sbx->GetThumbPosition() + e.GetPosition().x - SBS) / ZoomX, (ZoomY * GetSize().GetHeight() / 2 - e.GetPosition().y - sby->GetThumbPosition()) / ZoomY), 8);
    else
      dragging = seg->IsCtrlPoint(wxPoint((sbx->GetThumbPosition() + e.GetPosition().x - SBS) / ZoomX, (- ZoomY * GetSize().GetHeight() / 2 + e.GetPosition().y + sby->GetThumbPosition()) / ZoomY), 8);
    if (dragging != -1)
      MovePt(e.GetPosition());
  }
}

void ASEnvel::OnLeftUp(wxMouseEvent &e)
{
  dragging = -1;
}

void ASEnvel::OnRightDown(wxMouseEvent &e)
{
  int pt;

  if (seg)
  {
    if (sby->GetThumbPosition() + e.GetPosition().y < ZoomY * GetSize().GetHeight() / 2)
      pt = seg->IsCtrlPoint(wxPoint((sbx->GetThumbPosition() + e.GetPosition().x - SBS) / ZoomX, (ZoomY * GetSize().GetHeight() / 2 - e.GetPosition().y - sby->GetThumbPosition()) / ZoomY), 8);
    else
      pt = seg->IsCtrlPoint(wxPoint((sbx->GetThumbPosition() + e.GetPosition().x - SBS) / ZoomX, (- ZoomY * GetSize().GetHeight() / 2 + e.GetPosition().y + sby->GetThumbPosition()) / ZoomY), 8);
    if (pt != -1)
      seg->DelPoint(pt);
    else
      if (e.GetPosition().y + sby->GetThumbPosition() < ZoomY * GetSize().GetHeight() / 2)
        seg->AddPoint(wxPoint((sbx->GetThumbPosition() + e.GetPosition().x - SBS) / ZoomX, (ZoomY * GetSize().GetHeight() / 2 - e.GetPosition().y - sby->GetThumbPosition()) / ZoomY));
      else
        seg->AddPoint(wxPoint((sbx->GetThumbPosition() + e.GetPosition().x - SBS) / ZoomX, (-ZoomY *  GetSize().GetHeight() / 2 + e.GetPosition().y + sby->GetThumbPosition()) / ZoomY));
    if (wv)
      wv->SetEnvel(seg);
    FillGrid();
    Refresh();
  }
}

void ASEnvel::OnScroll(wxScrollEvent &e)
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
  FillGrid();
  Refresh();
}

void ASEnvel::Process(float **buf, int nbchan, int pos, long len)
{
  if (seg)
    for (int chan = 0; chan < nbchan; chan++)
      for (int idx = 0; idx < len; idx++)
        buf[chan][idx] *= 1 + 6.0f * seg->GetCoef(pos + idx);
}

void ASEnvel::OnDragThumb(wxScrollEvent &e)
{
  thumbdrag = true;
}

void ASEnvel::OnStopDragThumb(wxScrollEvent &e)
{
  thumbdrag = false;
  wxSizeEvent ev(GetSize());
  OnResize(ev);
}
