#include <wx/wx.h>
#include "ASKeygroupList.h"

BEGIN_EVENT_TABLE(ASKeygroupList, wxWindow)
  EVT_BUTTON(ASKeygroupList_AddKeygroup, ASKeygroupList::OnAddKeygroup)
  EVT_BUTTON(ASKeygroupList_DelKeygroup, ASKeygroupList::OnDelKeygroup)
  EVT_SIZE(ASKeygroupList::OnResize)
END_EVENT_TABLE()

ASKeygroupList *Keygroups = NULL;

ASKeygroupList::ASKeygroupList(wxString Name) :
  ASPlugin(Name)
{
  List = NULL;
  p = NULL;
}

ASKeygroupList::~ASKeygroupList()
{
}

void ASKeygroupList::OnResize(wxSizeEvent &e)
{
  if (List)
    List->SetSize(e.GetSize());
}

wxWindow *ASKeygroupList::CreateView(wxPanel *panel, wxPoint &pt, wxSize &sz)
{
  Reparent(panel);
  SetSize(sz);
  Move(pt);
  List = new ASList(this, -1, wxPoint(0, 0), sz);
  wxImage *btadd = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_ADD_KEYGROUP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *btdel = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_DEL_KEYGROUP)).c_str(), wxBITMAP_TYPE_PNG);
  List->AddControl(new wxBitmapButton(List, ASKeygroupList_AddKeygroup, wxBitmap(btadd)));
  List->AddControl(new wxBitmapButton(List, ASKeygroupList_DelKeygroup, wxBitmap(btdel)));
  Show(true);
  return this;
}

void  ASKeygroupList::OnAddKeygroup(wxCommandEvent &e)
{
  wxString keys[128];
  int oct = -2;
  wxString notes[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
  int note = 0;
  for (int i = 0; i < 128; i++)
  {
    keys[i] = notes[note] + " ";
    keys[i] << oct;
    note = (note + 1) % 12;
    if (!note)
      oct++;
  }
  int lokey = wxGetSingleChoiceIndex("Choose lowest key in group : ", "Keygroup creation", 128, keys);
  if (lokey == -1)
    return;
  int hikey = wxGetSingleChoiceIndex("Choose highest key in group : ", "Keygroup creation", 128, keys);
  if (hikey == -1)
    return;
  ASamplerKeygroup *askg = new ASamplerKeygroup(lokey, hikey);
  List->AddEntry("Keygroup " + keys[lokey] + " - " + keys[hikey], askg);
}
  
void  ASKeygroupList::OnDelKeygroup(wxCommandEvent &e)
{
  vector<ASListEntry *> v;
  v = List->GetSelected();
  vector<ASListEntry *>::iterator i;
  for (i = v.begin(); i != v.end(); i++)
  {
    List->DelEntry((*i)->GetEntry());
    delete (ASamplerKeygroup *)(*i)->GetEntry();
  }
}
 
ASamplerKeygroup *ASKeygroupList::FindKeygroup(int key)
{
  vector<ASListEntry *> v;
  v = List->GetEntries();
  for (vector<ASListEntry *>::iterator i = v.begin(); i != v.end(); i++)
  {
    ASamplerKeygroup *askg = (ASamplerKeygroup *)(*i)->GetEntry();
    if (askg->HasKey(key))
      return askg;
  }
  return NULL;
}
