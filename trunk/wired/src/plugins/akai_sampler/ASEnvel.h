#ifndef _ASENVEL_H_
#define _ASENVEL_H_

#include <wx/wx.h>
#include "ASPlugin.h"
#include "Splines.h"

using namespace std;

class ASEnvelSeg
{
  public:
    ASEnvelSeg(wxPoint, wxPoint);
    ~ASEnvelSeg();
    void Paint(wxMemoryDC &, wxPoint);
    void AddPoint(wxPoint);
    void DelPoint(int);
    void SetPoint(int, wxPoint);
    int IsCtrlPoint(wxPoint, int);
    bool IsOnCurve(wxPoint, int);
  private:
    void Recalc();
    unsigned int nbpts;
    t_pt *points;
    t_pt *curve;
    Splines *spline;
    double step;
    unsigned int nbcurvept;
};

class ASEnvel : public ASPlugin
{
  public:
    ASEnvel(wxString Name);
    ~ASEnvel();
    wxWindow * CreateView(wxPanel *, wxPoint &, wxSize &);
    void OnPaint(wxPaintEvent &);
    void OnResize(wxSizeEvent &);
    void OnMouseMove(wxMouseEvent &e);
    void OnLeftDown(wxMouseEvent &e);
    void OnRightDown(wxMouseEvent &e);
    void OnLeftUp(wxMouseEvent &e);
  private:
    void MovePt(wxPoint);
    wxBitmap *Grid;
    void FillGrid();
    int dragging;
    ASEnvelSeg *seg;
    wxPoint *orig;
  DECLARE_EVENT_TABLE()
};

#endif
