#include <wx/wx.h>
#include "ASSampleList.h"
#include "ASKeygroupList.h"
#include "WaveFile.h"

BEGIN_EVENT_TABLE(ASSampleList, wxWindow)
  EVT_BUTTON(ASSampleList_AddSample, ASSampleList::OnAddSample)
  EVT_BUTTON(ASSampleList_DelSample, ASSampleList::OnDelSample)
  EVT_BUTTON(ASSampleList_AssignSample, ASSampleList::OnAssignSample)
  EVT_SIZE(ASSampleList::OnResize)
END_EVENT_TABLE()

ASSampleList *Samples = NULL;

ASSampleList::ASSampleList(wxString Name) :
  ASPlugin(Name)
{
  List = NULL;
  p = NULL;
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
  wxImage *btadd = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_ADD_SAMPLE)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *btdel = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_DEL_SAMPLE)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *btassign = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_ASSIGN_SAMPLE)).c_str(), wxBITMAP_TYPE_PNG);
  List->AddControl(new wxBitmapButton(List, ASSampleList_AddSample, wxBitmap(btadd)));
  List->AddControl(new wxBitmapButton(List, ASSampleList_DelSample, wxBitmap(btdel)));
  List->AddControl(new wxBitmapButton(List, ASSampleList_AssignSample, wxBitmap(btassign)));
  Show(true);
  return this;
}

void  ASSampleList::OnAddSample(wxCommandEvent &e)
{
  vector<string> exts;
  string s = p->OpenFileLoader("Load Sample", 0x0, false);
  if (!s.empty())
  {
    WaveFile *w = new WaveFile(s);
    int i;
    for (i = s.length(); i >= 0 && s[i] != '/'; i--);
    i++;
    List->AddEntry(wxString(s.substr(i, s.length() - i).c_str()), new ASamplerSample(w));
  }
}
  
void  ASSampleList::OnDelSample(wxCommandEvent &e)
{
  vector<ASListEntry *> v;
  v = List->GetSelected();
  vector<ASListEntry *>::iterator i;
  for (i = v.begin(); i != v.end(); i++)
  {
    List->DelEntry((*i)->GetEntry());
    delete (ASamplerSample *)(*i)->GetEntry();
  }
}

void  ASSampleList::OnAssignSample(wxCommandEvent &e)
{
  vector<ASListEntry *> v;
  v = List->GetSelected();
  if (!v.size())
    return;
  vector<ASListEntry *>::iterator i;
  i = v.begin();
  vector<ASListEntry *> kg = Keygroups->GetEntries();
  int num = kg.size() + 1;
  wxArrayString labels;
  labels.Add("None");
  for (int j = 1; j < num; j++)
    labels.Add(kg[j - 1]->GetName());
  int selkg = wxGetSingleChoiceIndex("Choose keygroup : ", "Assign sample to keygroup", labels);
  if (selkg == -1)
    return;
  if (!selkg)
  {
    ASamplerKeygroup *askg = ((ASamplerSample *)(*i)->GetEntry())->GetKeygroup();
    if (askg)
      askg->SetSample(NULL);
    ((ASamplerSample *)(*i)->GetEntry())->SetKeygroup(NULL);
  }
  else
  {
    ((ASamplerSample *)(*i)->GetEntry())->SetKeygroup((ASamplerKeygroup *)kg[selkg - 1]->GetEntry());
    ((ASamplerKeygroup *)kg[selkg - 1]->GetEntry())->SetSample((ASamplerSample *)(*i)->GetEntry());
  }
}
