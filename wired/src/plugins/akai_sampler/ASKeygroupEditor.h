#ifndef _ASKEYGROUPEDITOR_H_
#define _ASKEYGROUPEDITOR_H_

#include <wx/wx.h>
#include "ASKeygroupList.h"
#include "ASClavier.h"

class ASKeygroupEditor : public ASPlugin
{
  public:
    ASKeygroupEditor(class AkaiSampler *as, wxString Name);
    wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
    void OnPaint(wxPaintEvent &);
    void OnKeyUp(wxMouseEvent &ev);
    void OnKeyDown(wxMouseEvent &ev);
    void PaintSelection();
    void SetSample(ASamplerSample *ass) { this->ass = ass; PaintSelection(); }
    ASamplerSample *GetSample() { return ass; }
  private:
    ASamplerSample *ass;
    ASClavier *clav;
    wxScrolledWindow *sw;
  DECLARE_EVENT_TABLE()
};

#endif

