#ifndef _ASKEYGROUPEDITOR_H_
#define _ASKEYGROUPEDITOR_H_

#include <wx/wx.h>
#include "ASKeygroupList.h"
#include "ASClavier.h"

class ASKeygroupEditor : public wxDialog
{
  public:
    ASKeygroupEditor(wxWindow *, int, wxString, wxPoint, wxSize);
    void OnPaint(wxPaintEvent &);
    void OnClose(wxCloseEvent &);
    void OnKeyUp(wxMouseEvent &ev);
    void OnKeyDown(wxMouseEvent &ev);
    void PaintSelection();
    void SetSample(ASamplerSample *ass) { this->ass = ass; PaintSelection(); }
    ASamplerSample *GetSample() { return ass; }
  private:
    ASamplerSample *ass;
    ASClavier *clav;
  DECLARE_EVENT_TABLE()
};

#endif

