#include  "ASList.h"

BEGIN_EVENT_TABLE(ASListEntry, wxPanel)
  EVT_PAINT(ASListEntry::OnPaint)
  EVT_LEFT_DOWN(ASListEntry::OnClick)
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
  selected = !selected;
  Refresh();
}

BEGIN_EVENT_TABLE(ASList, wxPanel)
  EVT_PAINT(ASList::OnPaint)
END_EVENT_TABLE()
  
ASList::ASList(wxWindow *parent, wxWindowID id, const wxPoint &pos,
    const wxSize& size, wxString title) : wxPanel(parent, id, pos, size)
{
  this->title = title;
  sw = new wxScrolledWindow(this, -1, wxPoint(0, TITLEHEIGHT), wxSize(size.GetWidth(), size.GetHeight() - TITLEHEIGHT));
  list = new wxPanel(sw, -1, wxPoint(0, 0), wxSize(sw->GetSize().GetWidth(), sw->GetSize().GetHeight()));
  list->SetBackgroundColour(*wxBLACK);
  Repos();
}

ASList::~ASList()
{
  entries.clear();
}

void ASList::OnPaint(wxPaintEvent &e)
{
  wxPaintDC dc(this);

  wxFont f = dc.GetFont();
  f.SetPointSize(10);
  dc.SetFont(f);
  dc.SetPen(wxPen(wxColour(100, 101, 203), 1));
  dc.SetBrush(wxBrush(wxColour(100, 101, 203)));
  dc.DrawRectangle(0, 0, GetSize().GetWidth(), TITLEHEIGHT);
  dc.SetTextForeground(*wxWHITE);
  dc.SetTextBackground(wxColour(100, 101, 203));
  dc.DrawText(title, 0, 0);
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
    int a = 6 * (*i)->GetName().length();
    if (a > sx)
      sx = a;
  }
  list->SetSize((sx > size.GetWidth()) ? sx : size.GetWidth(), (sy > size.GetHeight()) ? sy : size.GetHeight());
  sw->SetScrollbars(10, 10, list->GetSize().GetWidth() / 10 - 2, list->GetSize().GetHeight() / 10, 0, 0, true);
}

void ASList::AddEntry(wxString name, void *entry)
{
  entries.push_back(new ASListEntry(list, (int)entries.size(), wxPoint(0, ITEMHEIGHT * entries.size()), wxSize(GetSize().GetWidth(), ITEMHEIGHT), name, entry));
  Repos();
  Refresh();
}

void ASList::DelEntry(void *entry)
{
  vector<ASListEntry*>::iterator i;

  for (i = entries.begin(); i != entries.end(); i++)
    if ((*i)->GetEntry() == entry)
    {
      delete *i;
      entries.erase(i);
      Repos();
      Refresh();
      return;
    }
}
