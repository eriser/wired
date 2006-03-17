#ifndef _ASENVEL_H_
#define _ASENVEL_H_

#include <wx/wx.h>
#include "ASPlugin.h"
#include "Splines.h"
#include "ASWaveView.h"
#include "ASKeygroupList.h"

using namespace std;

class ASEnvelSeg
{
  public:
    ASEnvelSeg(unsigned long, wxSize);
    ~ASEnvelSeg();
    void Paint(wxMemoryDC &, int, int, int, int);
    void AddPoint(wxPoint);
    void DelPoint(int);
    void SetPoint(int, wxPoint);
    int IsCtrlPoint(wxPoint, int);
    void SetSize(wxSize sz) { size = sz; ratiox = ((double)size.GetWidth()) / wl; ratioy = ((double)size.GetHeight()) / 2000.0f; }
    void SetWaveLen(unsigned long wavelen) { wl = wavelen; ratiox = ((double)size.GetWidth()) / wl; }
    wxPoint GetPoint(int n);
    vector<wxPoint> GetPoints();
    void SetPoints(vector<wxPoint>);
    float GetCoef(long);
  private:
    unsigned int nbpts;
    t_pt *points;
    wxSize size;
    unsigned long wl;
    double ratiox;
    double ratioy;
    WaveFile *wav;
};

class ASEnvel : public ASPlugin
{
  public:
    ASEnvel(class AkaiSampler *as, wxString Name);
    ~ASEnvel();
    wxWindow * CreateView(wxPanel *, wxPoint &, wxSize &);
    void OnPaint(wxPaintEvent &);
    void OnResize(wxSizeEvent &);
    void OnMouseMove(wxMouseEvent &e);
    void OnLeftDown(wxMouseEvent &e);
    void OnRightDown(wxMouseEvent &e);
    void OnLeftUp(wxMouseEvent &e);
    void SetSample(ASamplerSample *ass);
    void ApplyEnvel(WaveFile *w);
    void OnScroll(wxScrollEvent &);
    void OnDragThumb(wxScrollEvent &);
    void OnStopDragThumb(wxScrollEvent &);
    void Process(float **, int, int, long);
    void Load(int, long);
    long Save(int);
    static wxString GetFXName() { return wxT("Envelope"); }
  private:
    void MovePt(wxPoint);
    wxBitmap *Grid;
    void FillGrid();
    int dragging;
    ASEnvelSeg *seg;
    wxPoint *orig;
    ASWaveView *wv;
    int ZoomX;
    int ZoomY;
    wxScrollBar *sbx;
    wxScrollBar *sby;
    wxSlider *zx;
    wxSlider *zy;
    bool thumbdrag;
  public:
  DECLARE_EVENT_TABLE()
};

#endif
