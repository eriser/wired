#ifndef _ASPLUGIN_H_
#define _ASPLUGIN_H_

#include <wx/wx.h>
#include "Plugin.h"

class ASPlugin : public wxPanel
{
  public:
    ASPlugin(wxString Name);
    ~ASPlugin();
    virtual wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
    wxString Name;
    virtual void OnAttach(wxWindow *);
    virtual void OnDetach(wxWindow *);
    void SetPlugin(Plugin *p) { this->p = p; }
  protected:
    Plugin *p;
};

#endif
