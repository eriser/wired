#ifndef _ASWAVEVIEW_H_
#define _ASWAVEVIEW_H_

#include <wx/wx.h>
#include "WaveView.h"
#include "WaveFile.h"
#include "ASKeygroupList.h"

class ASWaveView : public WaveView
{
  public:
    ASWaveView(wxWindow *, int, wxPoint, wxSize);
    void SetSample(WaveFile *);
    void SetSize(wxSize);
    void RedrawBitmap(wxSize s);
    void SetDrawing(wxSize s);
    void OnPaint(wxPaintEvent &ev);
    wxBitmap *GetBitmap();
    float GetCoef(long);
    void SetEnvel(class ASEnvelSeg *e) { env = e; SetDrawing(GetSize()); }

  private:
    class ASEnvelSeg *env;
};

#endif
