// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include  "ASList.h"
#include  "Colour.h"

enum
{
  ASTextCtrl_Rename = 12345,
  ID_SCROLLY
};

#define SBS 16

BEGIN_EVENT_TABLE(ASTextCtrl, wxTextCtrl)
  EVT_KILL_FOCUS(ASTextCtrl::KillControl)
  EVT_TEXT_ENTER(ASTextCtrl_Rename, ASTextCtrl::KillControl2)
END_EVENT_TABLE()

ASTextCtrl::ASTextCtrl(wxWindow *parent, int id, wxString title, wxPoint pos, wxSize size, ASListEntry *ale) : wxTextCtrl(parent, ASTextCtrl_Rename, title, pos, size, wxTE_PROCESS_ENTER)
{
  this->ale = ale;
}

void ASTextCtrl::KillControl2(wxCommandEvent &e)
{
  ale->Rename(GetValue());
  Destroy();
}

void ASTextCtrl::KillControl(wxFocusEvent &e)
{
  ale->Rename(GetValue());
  Destroy();
}

BEGIN_EVENT_TABLE(ASListEntry, wxPanel)
  EVT_PAINT(ASListEntry::OnPaint)
  EVT_LEFT_DOWN(ASListEntry::OnClick)
  EVT_LEFT_DCLICK(ASListEntry::OnRename)
END_EVENT_TABLE()

ASListEntry::ASListEntry(wxWindow *parent, int id, wxPoint pos, wxSize size, wxString &name, void *entry) :
  wxPanel(parent, id, pos, size)
{
  this->name = name;
  this->entry = entry;
  selected = false;
}

ASListEntry::~ASListEntry()
{
}

void ASListEntry::OnRename(wxMouseEvent &e)
{
  InvertSelection();
  ASTextCtrl *txt = new ASTextCtrl(this, -1, name, wxPoint(0, 0), GetSize(), this);
  txt->SetFocus();
}

void ASListEntry::OnPaint(wxPaintEvent &e)
{
  wxPaintDC dc(this);

  dc.SetPen(wxPen(*wxBLACK, 1));
  if (selected)
    dc.SetBrush(wxBrush(*wxBLUE));
  else
    dc.SetBrush(wxBrush(*wxBLACK));
  dc.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
  wxFont f = dc.GetFont();
  f.SetPointSize(10);
  dc.SetFont(f);
  dc.SetTextForeground(*wxWHITE);
  if (selected)
    dc.SetTextBackground(*wxBLUE);
  else
    dc.SetTextBackground(*wxBLACK);
  dc.DrawText(name, 0, 0);
}

void ASListEntry::OnClick(wxMouseEvent &e)
{
  ((ASList *)GetParent()->GetParent()->GetParent())->SetSelected(this);
  Refresh();
}

BEGIN_EVENT_TABLE(ASList, wxPanel)
  EVT_SIZE(ASList::OnResize)
  EVT_COMMAND_SCROLL(ID_SCROLLY, ASList::OnScroll)
END_EVENT_TABLE()

void ASList::OnResize(wxSizeEvent &e)
{
  sw->SetSize(wxSize(e.GetSize().GetWidth() - SBS, e.GetSize().GetHeight() - BUTTONSHEIGHT));
  buttons->SetSize(0, e.GetSize().GetHeight() - BUTTONSHEIGHT, e.GetSize().GetWidth(), BUTTONSHEIGHT);
  Repos();
}
  
ASList::ASList(wxWindow *parent, wxWindowID id, const wxPoint &pos,
    const wxSize& size) : wxPanel(parent, id, pos, size)
{
  sw = new wxScrolledWindow(this, -1, wxPoint(0, 0), wxSize(size.GetWidth() - SBS, size.GetHeight() - BUTTONSHEIGHT));
  list = new wxPanel(sw, -1, wxPoint(0, 0), wxSize(sw->GetSize().GetWidth(), sw->GetSize().GetHeight()));
  list->SetBackgroundColour(*wxBLACK);
  this->SetBackgroundColour(*wxBLACK);
  sb = new wxScrollBar(this, ID_SCROLLY, wxPoint(size.GetWidth() - SBS, 0), wxSize(SBS, size.GetHeight() - BUTTONSHEIGHT), wxSB_VERTICAL);
  sb->SetBackgroundColour(CL_OPTION_TOOLBAR);
  sb->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));
  sb->SetScrollbar(0, 10, list->GetSize().GetHeight() / 10, 10, false);
  buttons = new wxPanel(this, -1, wxPoint(0, GetSize().GetHeight() - BUTTONSHEIGHT), wxSize(GetSize().GetWidth(), BUTTONSHEIGHT));
  buttons->SetBackgroundColour(wxColor(100, 101, 203));
  bpos.x = 2;
  bpos.y = 2;
  Repos();
}

ASList::~ASList()
{
  for (vector<ASListEntry *>::iterator i = entries.begin(); i != entries.end(); i++)
    delete (*i);
  entries.clear();
}

void ASList::OnScroll(wxScrollEvent &e)
{
  static int oposy = 0;
  int posy = (oposy - sb->GetThumbPosition()) * 10;
  sw->ScrollWindow(0, posy, NULL);
  oposy = sb->GetThumbPosition();
  Refresh();
}

void ASList::Repos()
{
  vector<ASListEntry*>::iterator i;
  int j = 0;
  int sx = 0;
  int sy = ITEMHEIGHT * entries.size();
  wxSize size(sw->GetSize());
  for (i = entries.begin(); i != entries.end(); i++)
  {
    (*i)->Move(0, ITEMHEIGHT * j++);
    int a = 6 * (1 + (*i)->GetName().length());
    if (a > sx)
      sx = a;
  }
  list->SetSize((sx > size.GetWidth()) ? sx : size.GetWidth(), (sy > size.GetHeight()) ? sy : size.GetHeight());
  for (i = entries.begin(); i != entries.end(); i++)
    (*i)->SetSize(GetSize().GetWidth(), ITEMHEIGHT);
  sb->SetScrollbar(0, 10, list->GetSize().GetHeight() / 10, 10, true);
}

void ASList::SetSelected(ASListEntry *sel)
{
  vector<ASListEntry*>::iterator i;
  for (i = entries.begin(); i != entries.end(); i++)
    if ((*i) == sel)
      (*i)->SetSelected(true);
    else
      (*i)->SetSelected(false);
}

ASListEntry * ASList::GetSelected()
{
  vector<ASListEntry*>::iterator i;
  for (i = entries.begin(); i != entries.end(); i++)
    if ((*i)->IsSelected())
      return (*i);
  return NULL;
}

ASListEntry *ASList::AddEntry(wxString name, void *entry)
{
  ASListEntry *asle = new ASListEntry(list, (int)entries.size(), wxPoint(0, ITEMHEIGHT * entries.size()), wxSize(GetSize().GetWidth(), ITEMHEIGHT), name, entry);
  entries.push_back(asle);
  if (GetSelected() == NULL)
    asle->SetSelected(true);
  Repos();
  Refresh();
  return asle;
}

void ASList::DelEntry(void *entry)
{
  vector<ASListEntry*>::iterator i;

  for (i = entries.begin(); i != entries.end(); i++)
    if ((*i)->GetEntry() == entry)
    {
      (*i)->Destroy();
      i = entries.erase(i);
      if (GetSelected() == NULL)
        if (entries.size())
          entries[0]->SetSelected(true);
      Repos();
      Refresh();
      return;
    }
}

void  ASList::AddControl(wxWindow *control)
{
  control->Reparent(buttons);
  control->SetSize(bpos.x, bpos.y, BUTTONSHEIGHT - 4, BUTTONSHEIGHT - 4);
  bpos.x += 18;
}
