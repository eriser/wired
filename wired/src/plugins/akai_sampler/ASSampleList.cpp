#include <wx/wx.h>
#include "ASSampleList.h"
#include "ASKeygroupList.h"
#include "WaveFile.h"

// ICI METTRE TOUT LES EFFETS

#include "ASEnvel.h"
#include "ASLoop.h"

#define NB_EFFECTS 2
static wxString EFFECTSNAMES[NB_EFFECTS] = {
  _T("Enveloppe"),
  _T("Looping")
};

// FIN DES EFFETS

BEGIN_EVENT_TABLE(ASSampleList, wxWindow)
  EVT_BUTTON(ASSampleList_AddSample, ASSampleList::OnAddSample)
  EVT_BUTTON(ASSampleList_DelSample, ASSampleList::OnDelSample)
  EVT_BUTTON(ASSampleList_AssignSample, ASSampleList::OnAssignSample)
  EVT_BUTTON(ASSampleList_EffectSample, ASSampleList::OnEffectSample)
  EVT_SIZE(ASSampleList::OnResize)
END_EVENT_TABLE()

ASSampleList *Samples = NULL;
unsigned long sampleid = 1;

ASSampleList::ASSampleList(wxString Name) :
  ASPlugin(Name)
{
  List = NULL;
  p = NULL;
  pp = NULL;
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
  wxImage *bteffect = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_EFFECT_SAMPLE)).c_str(), wxBITMAP_TYPE_PNG);
  List->AddControl(new wxBitmapButton(List, ASSampleList_AddSample, wxBitmap(btadd)));
  List->AddControl(new wxBitmapButton(List, ASSampleList_DelSample, wxBitmap(btdel)));
  List->AddControl(new wxBitmapButton(List, ASSampleList_AssignSample, wxBitmap(btassign)));
  List->AddControl(new wxBitmapButton(List, ASSampleList_EffectSample, wxBitmap(bteffect)));
  Show(true);
  return this;
}

void  ASSampleList::OnAddSample(wxCommandEvent &e)
{
  static int fk = 0;
  vector<string> exts;
  string s = p->OpenFileLoader("Load Sample", 0x0, false);
  if (!s.empty())
  {
    WaveFile *w = new WaveFile(s);
    int i;
    for (i = s.length(); i >= 0 && s[i] != '/'; i--);
    i++;
    ASamplerSample *ass = new ASamplerSample(w);
    List->AddEntry(wxString(s.substr(i, s.length() - i).c_str()), ass);
    ASamplerKeygroup *askg = new ASamplerKeygroup(fk, fk + 11);
    wxString s = "Keygroup ";
    int oct = fk / 12 - 2;
    int note = fk % 12;
    wxString notes[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    s += notes[note];
    s += ' ';
    s << oct;
    s += " - ";
    oct = (fk + 11) / 12 - 2;
    note = (fk + 11) % 12;
    s += notes[note];
    s += ' ';
    s << oct;
    fk += 12;
    Keygroups->List->AddEntry(s, askg);
    askg->SetSample(ass);
    ass->SetKeygroup(askg);
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

void  ASSampleList::OnSelectEffect(wxCommandEvent &e)
{
  vector<ASListEntry *> v;
  v = List->GetSelected();
  if (!v.size())
    return;
  vector<ASListEntry *>::iterator i;
  i = v.begin();
  ASamplerSample *ass = (ASamplerSample *)(*i)->GetEntry();
  ASPlugin *p;
  switch (e.GetId())
  {
    case 1:
      // Enveloppe
      p = new ASEnvel(wxString("Enveloppe for ") + (*i)->GetName());
      break;
    case 2:
      // Looping
      p = new ASLoop(wxString("Looping for ") + (*i)->GetName());
      break;
    default:
      p = NULL;
  }
  if (p && pp)
  {
    ASPlugin *p2 = ass->GetEffect();
    if (p2)
      pp->ClosePlug(p2);
    delete p2;
    p->SetSample(ass);
    ass->SetEffect(p);
    pp->AddPlug(p);
    pp->ShowPlugin(p);
  }
}

void  ASSampleList::OnEffectSample(wxCommandEvent &e)
{
  long        k;
  wxMenu      *menu;
            
  menu = new wxMenu();
  for (k = 1; k <= NB_EFFECTS; k++)
  {
    menu->Append(k, EFFECTSNAMES[k - 1]);
    Connect(k, wxEVT_COMMAND_MENU_SELECTED,
      (wxObjectEventFunction)(wxEventFunction)
      (wxCommandEventFunction)&ASSampleList::OnSelectEffect);
  }       
  wxPoint p(((wxWindow *)e.GetEventObject())->GetPosition());
  PopupMenu(menu, p.x, p.y);
}
