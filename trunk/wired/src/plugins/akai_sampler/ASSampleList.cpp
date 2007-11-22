// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/wx.h>
#include "ASSampleList.h"
#include "ASKeygroupList.h"
#include "ASKeygroupEditor.h"
#include "WaveFile.h"
#include "AkaiSampler.h"

BEGIN_EVENT_TABLE(ASSampleList, wxWindow)
  EVT_BUTTON(ASSampleList_AddSample, ASSampleList::OnAddSample)
  EVT_BUTTON(ASSampleList_DelSample, ASSampleList::OnDelSample)
  EVT_SIZE(ASSampleList::OnResize)
END_EVENT_TABLE()

ASSampleList::ASSampleList(class AkaiSampler *as, wxString Name) :
  ASPlugin(as, Name)
{
  List = NULL;
}

ASSampleList::~ASSampleList()
{
}

void ASSampleList::OnResize(wxSizeEvent &e)
{
  if (List)
    List->SetSize(e.GetSize());
}

wxWindow *ASSampleList::CreateView(wxPanel *panel, wxPoint &pt, wxSize &sz)
{
  Reparent(panel);
  SetSize(sz);
  Move(pt);
  List = new ASList(this, -1, wxPoint(0, 0), sz);
  wxImage *btadd_up = new wxImage(wxString(as->GetDataDir() + wxString(IMAGE_BT_ADD_UP)), wxBITMAP_TYPE_PNG);
  wxImage *btdel_up = new wxImage(wxString(as->GetDataDir() + wxString(IMAGE_BT_DEL_UP)), wxBITMAP_TYPE_PNG);
  wxImage *btadd_down = new wxImage(wxString(as->GetDataDir() + wxString(IMAGE_BT_ADD_DOWN)), wxBITMAP_TYPE_PNG);
  wxImage *btdel_down = new wxImage(wxString(as->GetDataDir() + wxString(IMAGE_BT_DEL_DOWN)), wxBITMAP_TYPE_PNG);
  List->AddControl(new DownButton(List, ASSampleList_AddSample, wxPoint(0, 0), wxSize(12, 12), btadd_up, btadd_down, true));
  List->AddControl(new DownButton(List, ASSampleList_DelSample, wxPoint(0, 0), wxSize(12, 12), btdel_up, btdel_down, true));
  Show(true);
  return this;
}

void  ASSampleList::OnAddSample(wxCommandEvent &e)
{
  int fk = 24;
  vector<string> exts;
  wxString s = as->OpenFileLoader(_("Load Sample"), 0x0, false);
  if (!s.empty())
  {
    WaveFile *w = new WaveFile(s, true);
    int i;
    for (i = s.length(); i >= 0 && s[i] != '/'; i--);
    i++;
    ASamplerSample *ass = new ASamplerSample(as, w);
    List->AddEntry(wxString(s.substr(i, s.length() - i).c_str()), ass);
    fk += 12 * as->keygroupid;
    ASamplerKeygroup *askg = new ASamplerKeygroup(as, fk, fk + 11);
    as->Keygroups.push_back(askg);
    askg->SetSample(ass);
    ass->SetKeygroup(askg);
  }
}
  
void  ASSampleList::OnDelSample(wxCommandEvent &e)
{
  ASListEntry *ent;
  ent = List->GetSelected();
  if (ent)
  {
    ASamplerSample *ass = (ASamplerSample *)ent->GetEntry();
    List->DelEntry(ent->GetEntry());
    delete ass;
  }
}

ASamplerSample	*ASSampleList::GetSampleById(unsigned long askedId)
{
  vector<ASListEntry *>	samples;
  ASamplerSample	*sample;
  vector<ASListEntry *>::iterator i;

  samples = List->GetEntries();

  for(i = samples.begin(); i != samples.end(); i++)
  {
	ASamplerSample *ass = (ASamplerSample *)(*i)->GetEntry();
	if (ass->GetID() == askedId)
	  return ass;
  }
  return NULL;
}
