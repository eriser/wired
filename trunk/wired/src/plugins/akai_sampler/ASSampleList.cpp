#include <wx/wx.h>
#include "ASSampleList.h"
#include "ASKeygroupList.h"
#include "ASKeygroupEditor.h"
#include "WaveFile.h"

BEGIN_EVENT_TABLE(ASSampleList, wxWindow)
  EVT_BUTTON(ASSampleList_AddSample, ASSampleList::OnAddSample)
  EVT_BUTTON(ASSampleList_DelSample, ASSampleList::OnDelSample)
//  EVT_BUTTON(ASSampleList_AssignSample, ASSampleList::OnAssignSample)
//  EVT_BUTTON(ASSampleList_EffectSample, ASSampleList::OnEffectSample)
  EVT_SIZE(ASSampleList::OnResize)
END_EVENT_TABLE()

ASSampleList *Samples = NULL;
unsigned long sampleid = 1;

ASSampleList::ASSampleList(wxString Name) :
  ASPlugin(Name)
{
  List = NULL;
  p = NULL;
//  pp = NULL;
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
  wxImage *btadd_up = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_ADD_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *btdel_up = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_DEL_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *btadd_down = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_ADD_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *btdel_down = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_DEL_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  /*
  wxImage *btassign = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_ASSIGN_SAMPLE)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *bteffect = new wxImage(string(p->GetDataDir() + string(IMAGE_BT_EFFECT_SAMPLE)).c_str(), wxBITMAP_TYPE_PNG);
  */
  //List->AddControl(new wxBitmapButton(List, ASSampleList_AddSample, wxBitmap(btadd)));
  //List->AddControl(new wxBitmapButton(List, ASSampleList_DelSample, wxBitmap(btdel)));

  List->AddControl(new DownButton(List, ASSampleList_AddSample, wxPoint(0, 0), wxSize(12, 12), btadd_up, btadd_down, true));
  List->AddControl(new DownButton(List, ASSampleList_DelSample, wxPoint(0, 0), wxSize(12, 12), btdel_up, btdel_down, true));
  /*
  List->AddControl(new wxBitmapButton(List, ASSampleList_AssignSample, wxBitmap(btassign)));
  List->AddControl(new wxBitmapButton(List, ASSampleList_EffectSample, wxBitmap(bteffect)));
  */
  Show(true);
  return this;
}

void  ASSampleList::OnAddSample(wxCommandEvent &e)
{
  static int fk = 24;
  vector<string> exts;
  string s = p->OpenFileLoader("Load Sample", 0x0, false);
  if (!s.empty())
  {
    WaveFile *w = new WaveFile(s, false);
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
  ASListEntry *ent;
  ent = List->GetSelected();
  if (ent)
  {
    List->DelEntry(ent->GetEntry());
    delete (ASamplerSample *)ent->GetEntry();
  }
}
/*
void  ASSampleList::OnAssignSample(wxCommandEvent &e)
{
  vector<ASListEntry *> v;
  v = List->GetSelected();
  if (!v.size())
    return;
  vector<ASListEntry *>::iterator i;
  i = v.begin();
  ASamplerSample *ass = (ASamplerSample *)((*i)->GetEntry());
  ASKeygroupEditor *aske = ass->GetKgEditor();
  ASamplerKeygroup *askg = ass->GetKeygroup();
  if (!aske)
  {
    aske = new ASKeygroupEditor(this, -1, wxString(_T("Keygroup editor for ")) + (*i)->GetName(), wxPoint(0, 0), wxSize(400, 400));
    aske->SetSample(ass);
    ass->SetKgEditor(aske);
  }
  aske->Show();
  aske->Raise();
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
*/
