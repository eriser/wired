#ifndef _ASWAVEVIEW_H_
#define _ASWAVEVIEW_H_

#include <wx/wx.h>
#include "WaveView.h"
#include "WaveFile.h"
#include "ASKeygroupList.h"

class ASWaveView : public WaveView
{
  public:
    ASWaveView(wxWindow *, int, wxPoint, wxSize &);
    void SetSample(WaveFile *);
    void SetSize(wxSize);
    wxMemoryDC *GetWaveDC();

  private:
};

#endif
