#include <wx/wx.h>
#include "ASEnvel.h"

ASEnvelSeg::ASEnvelSeg(wxPoint start, wxPoint stop)
{
  step = 0.005;
  nbcurvept = (unsigned int)(1.0 / step);
  nbpts = 2;
  points = (t_pt *)malloc(sizeof(t_pt) * nbpts);
  points[0].x = start.x;
  points[0].y = start.y;
  points[1].x = stop.x;
  points[1].y = stop.y;
  curve = (t_pt *)malloc(sizeof(t_pt) * nbcurvept);
  spline = new Splines();
  Recalc();
}

ASEnvelSeg::~ASEnvelSeg()
{
  free(points);
  free(curve);
  delete(spline);
}

void ASEnvelSeg::Recalc()
{
  spline->SetPoints(nbpts, points);
  for (int t = 0; t < nbcurvept; t++)
  {
    t_pt *p = spline->GetPoint(step * t);
    curve[t].x = p->x;
    curve[t].y = p->y;
    free(p);
  }
}

int ASEnvelSeg::IsCtrlPoint(wxPoint p, int precision)
{
  for (unsigned int i = 0; i < nbpts; i++)
    if ((abs(p.x - points[i].x) <= precision) && (abs(p.y - points[i].y) <= precision))
      return i;
  return -1;
}

bool ASEnvelSeg::IsOnCurve(wxPoint p, int precision)
{
  for (unsigned int i = 0; i < nbcurvept; i++)
    if ((abs(p.x - curve[i].x) <= precision) && (abs(p.y - curve[i].y) <= precision))
      return true;
  return false;
}

void ASEnvelSeg::Paint(wxMemoryDC &dc, wxPoint orig)
{
  static wxPen    greenPen(wxColor(0x00, 0xFF, 0x00), 1);
  dc.SetPen(greenPen);
  for (unsigned int i = 0; i < nbpts; i++)
  {
    if (!i || (i == nbpts - 1))
      dc.DrawRectangle(orig.x + points[i].x - 4, orig.y - points[i].y - 4, 8, 8);
    else
      dc.DrawEllipse(orig.x + points[i].x - 4, orig.y - points[i].y - 4, 8, 8);
  }
  int x = points[0].x;
  int y = points[0].y;
  int ox;
  int oy;
  for (unsigned int t = 0; t < nbcurvept; t++)
  {
    ox = x;
    oy = y;
    x = curve[t].x;
    y = curve[t].y;
    dc.DrawLine(orig.x + ox, orig.y - oy, orig.x + x, orig.y - y);
  }
  dc.DrawLine(orig.x + x, orig.y - y, orig.x + points[nbpts - 1].x, orig.y - points[nbpts - 1].y);
}

void ASEnvelSeg::AddPoint(wxPoint p)
{
  t_pt *tmp = (t_pt *)malloc(sizeof(t_pt) * (nbpts + 1));
  memcpy(tmp, points, sizeof(t_pt) * nbpts);
  tmp[nbpts].x = points[nbpts - 1].x;
  tmp[nbpts].y = points[nbpts - 1].y;
  tmp[nbpts - 1].x = p.x;
  tmp[nbpts - 1].y = p.y;
  free(points);
  points = tmp;
  nbpts++;
  Recalc();
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
    Recalc();
  }
}

void ASEnvelSeg::SetPoint(int n, wxPoint p)
{
  if ((n >= 0) && (n < nbpts))
  {
    points[n].x = p.x;
    points[n].y = p.y;
    Recalc();
  }
}

BEGIN_EVENT_TABLE(ASEnvel, wxWindow)
  EVT_PAINT(ASEnvel::OnPaint)
  EVT_SIZE(ASEnvel::OnResize)
  EVT_MOTION(ASEnvel::OnMouseMove)
  EVT_LEFT_UP(ASEnvel::OnLeftUp)
  EVT_LEFT_DOWN(ASEnvel::OnLeftDown)
  EVT_RIGHT_DOWN(ASEnvel::OnRightDown)
END_EVENT_TABLE()

ASEnvel::ASEnvel(wxString Name) :
  ASPlugin(Name)
{
  Grid = NULL;
  orig = NULL;
  seg = new ASEnvelSeg(wxPoint(0, -70), wxPoint(560, 45));
  seg->AddPoint(wxPoint(280, 13));
  dragging = -1;
}

ASEnvel::~ASEnvel()
{
  if (Grid)
    delete Grid;
  if (seg)
    delete seg;
  if (orig)
    delete orig;
}

wxWindow *ASEnvel::CreateView(wxPanel *p, wxPoint &pt, wxSize &sz)
{
  Reparent(p);
  SetSize(sz);
  Move(pt);
  FillGrid();
  orig = new wxPoint(10, sz.GetHeight() / 2);
  Show(true);
  return this;
}

void ASEnvel::FillGrid()
{
  static wxPen    borderPen(wxColor(0xFF, 0xFF, 0xFF), 1);
  static wxBrush  blackBrush(wxColor(0x00, 0x00, 0x00));
  int sx = GetSize().GetWidth();
  int sy = GetSize().GetHeight();
  if (Grid)
    delete Grid;
  wxMemoryDC memDC;
  Grid = new wxBitmap(sx, sy);
  memDC.SelectObject(*Grid);
  memDC.SetBrush(blackBrush);
  memDC.SetPen(borderPen);
  memDC.DrawRectangle(0, 0, sx, sy);
  seg->Paint(memDC, *orig);
}

void ASEnvel::MovePt(wxPoint p)
{
  seg->SetPoint(dragging, wxPoint(-orig->x + p.x, orig->y - p.y));
  FillGrid();
  Refresh();
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
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(),
          wxCOPY, FALSE);
      upd++;
    }
  }
}

void ASEnvel::OnResize(wxSizeEvent &e)
{
  if (orig)
    delete (orig);
  orig = new wxPoint(10, e.GetSize().GetHeight() / 2);
  FillGrid();
}

void ASEnvel::OnMouseMove(wxMouseEvent &e)
{
  if (dragging != -1)
    MovePt(e.GetPosition());
}

void ASEnvel::OnLeftDown(wxMouseEvent &e)
{
  dragging = seg->IsCtrlPoint(wxPoint(-orig->x + e.GetPosition().x, orig->y - e.GetPosition().y), 8);
  if (dragging != -1)
    MovePt(e.GetPosition());
}

void ASEnvel::OnLeftUp(wxMouseEvent &e)
{
  dragging = -1;
}

void ASEnvel::OnRightDown(wxMouseEvent &e)
{
  int pt;

  pt = seg->IsCtrlPoint(wxPoint(-orig->x + e.GetPosition().x, orig->y - e.GetPosition().y), 8);
  if (pt != -1)
    seg->DelPoint(pt);
  else
    seg->AddPoint(wxPoint(-orig->x + e.GetPosition().x, orig->y - e.GetPosition().y));
  FillGrid();
  Refresh();
}
