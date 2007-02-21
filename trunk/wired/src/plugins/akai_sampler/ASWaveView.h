// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _ASWAVEVIEW_H_
#define _ASWAVEVIEW_H_

#include <wx/wx.h>
#include "WaveView.h"
#include "WaveFile.h"
#include "ASKeygroupList.h"

class ASWaveView : public WaveDrawer
{
  public:
    ASWaveView(wxSize);
    void SetSample(WaveFile *);
    void SetSize(wxSize sz);
    void RedrawBitmap(wxSize s);
    void SetDrawing(wxSize s);
    wxSize GetSize() { return size; }
    wxBitmap *GetBitmap();
    float GetCoef(long);
    void SetEnvel(class ASEnvelSeg *e) { env = e; SetDrawing(GetSize()); }

  private:
    class ASEnvelSeg *env;
    wxSize size;
};

#endif
