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
    static const wxString GetFXName() { return "Looping"; }
  private:
    ASWaveView *wv;
  public:
  DECLARE_EVENT_TABLE()
};

#endif
