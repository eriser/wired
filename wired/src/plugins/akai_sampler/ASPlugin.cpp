// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/wx.h>
#include "ASPlugPanel.h"
#include "ASPlugin.h"
#include "AkaiSampler.h"

ASPlugin::ASPlugin(class AkaiSampler *as, wxString Name) : 
  wxPanel(as->PlugPanel, -1, wxPoint(-1, -1), wxSize(-1, -1))
{
  this->as = as;
  this->Name = Name;
  this->ass = NULL;
  Show(false);
  this->type = GetFXName();
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

void ASPlugin::Process(float **buf, int nbchan, int pos, long len)
{
}
