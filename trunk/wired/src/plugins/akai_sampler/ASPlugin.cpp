#include <wx/wx.h>
#include "ASPlugPanel.h"
#include "ASPlugin.h"

ASPlugin::ASPlugin(wxString Name) : 
  wxPanel(PlugPanel, -1, wxPoint(-1, -1), wxSize(-1, -1))
{
  this->Name = Name;
  this->ass = NULL;
  Show(false);
}

ASPlugin::~ASPlugin()
{
}

wxWindow *ASPlugin::CreateView(wxPanel *p, wxPoint &pt, wxSize &sz)
{
  Reparent(p);
  SetSize(sz);
  Move(pt);
  Show(true);
  return this;
}

void ASPlugin::OnAttach(wxWindow *w)
{
}

void ASPlugin::OnDetach(wxWindow *w)
{
}
