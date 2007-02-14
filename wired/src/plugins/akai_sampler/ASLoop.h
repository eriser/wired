// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _ASLOOP_H_
#define _ASLOOP_H_

#include <wx/wx.h>
#include "ASPlugin.h"
#include "ASWaveView.h"
#include "ASKeygroupList.h"

using namespace std;

class ASLoop : public ASPlugin
{
  public:
    ASLoop(class AkaiSampler *as, wxString Name);
    ~ASLoop();
    wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
    void OnPaint(wxPaintEvent &);
    void OnResize(wxSizeEvent &);
    void OnMouseMove(wxMouseEvent &e);
    void OnLeftDown(wxMouseEvent &e);
    void OnRightDown(wxMouseEvent &e);
    void OnLeftUp(wxMouseEvent &e);
    void SetSample(ASamplerSample *ass);
    void OnScroll(wxScrollEvent &);
    void OnDragThumb(wxScrollEvent &);
    void OnStopDragThumb(wxScrollEvent &);
    static wxString GetFXName() { return wxT("Looping"); }
  private:
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
