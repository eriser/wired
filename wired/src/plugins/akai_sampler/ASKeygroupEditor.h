// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

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
    void OnSize(wxSizeEvent &);
    void OnScroll(wxScrollEvent &);
    void OnKeyUp(wxMouseEvent &ev);
    void OnKeyDown(wxMouseEvent &ev);
    void PaintSelection();
    void SetSample(ASamplerSample *ass) { this->ass = ass; PaintSelection(); }
    ASamplerSample *GetSample() { return ass; }
  private:
    ASamplerSample *ass;
    ASClavier *clav;
    wxScrolledWindow *sw;
    wxScrollBar *sb;
    wxStaticText *lokey;
    wxStaticText *hikey;
  DECLARE_EVENT_TABLE()
};

#endif

