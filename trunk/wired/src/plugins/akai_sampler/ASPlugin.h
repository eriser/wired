// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _ASPLUGIN_H_
#define _ASPLUGIN_H_

#include <wx/wx.h>

class ASPlugin : public wxPanel
{
  public:
    ASPlugin(class AkaiSampler *as, wxString Name);
    ~ASPlugin();
    virtual wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
    wxString Name;
    virtual void OnAttach(wxWindow *);
    virtual void OnDetach(wxWindow *);
    void SetSample(class ASamplerSample *ass) { this->ass = ass; }
    virtual void Process(float **buf, int nbchan, int pos, long len);
    static wxString GetFXName() { return wxT("ASPlugin"); }
    wxString GetType() { return type; }
    virtual long Save(int fd) { return 0; }
    virtual void Load(int fd, long len) { return ; }
  protected:
    class ASamplerSample *ass;
    class AkaiSampler *as;
    wxString type;
};

#endif
