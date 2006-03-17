#include "ASKeygroupEditor.h"
#include "AkaiSampler.h"
#include "Colour.h"

#define ID_SCROLLX 14312
#define SBS 16

BEGIN_EVENT_TABLE(ASKeygroupEditor, wxWindow)
  EVT_SIZE(ASKeygroupEditor::OnSize)
  EVT_LEFT_DOWN(ASKeygroupEditor::OnKeyDown)
  EVT_LEFT_UP(ASKeygroupEditor::OnKeyUp)
  EVT_COMMAND_SCROLL(ID_SCROLLX, ASKeygroupEditor::OnScroll)
END_EVENT_TABLE()

ASKeygroupEditor::ASKeygroupEditor(class AkaiSampler *as, wxString Name)
   : ASPlugin(as, Name) 
{
  clav = NULL;
  sw = NULL;
  sb = NULL;
  ass = NULL;
  lokey = NULL;
  hikey = NULL;
}

wxWindow *ASKeygroupEditor::CreateView(wxPanel *p, wxPoint &pt, wxSize &sz)
{
  Reparent(p);
  SetSize(sz);
  Move(pt);
  sw = new wxScrolledWindow(this, -1, wxPoint(sz.GetHeight() - SBS - ASCLAVIER_HEIGHT, 0), wxSize(sz.GetWidth(), ASCLAVIER_HEIGHT), wxSIMPLE_BORDER);
  SetBackgroundColour(wxColour(0, 0, 0));
  clav = new ASClavier(sw, -1, wxPoint(0, 0), wxSize(ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT), 0, 0);
  sb = new wxScrollBar(this, ID_SCROLLX, wxPoint(0, sz.GetHeight() - SBS), wxSize(sz.GetWidth(), SBS), wxSB_HORIZONTAL);
  sb->SetBackgroundColour(CL_OPTION_TOOLBAR);
  sb->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));
  sb->SetScrollbar(0, 10, (clav->GetSize().GetWidth() - sz.GetWidth()), 10, false);
  lokey = new wxStaticText(this, -1, wxT(""), wxPoint(20, 10));
  hikey = new wxStaticText(this, -1, wxT(""), wxPoint(160, 10));
  lokey->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));
  hikey->SetForegroundColour(wxColour(0xFF, 0xFF, 0xFF));
  PaintSelection();
  Show();
  return this;
}

void ASKeygroupEditor::OnSize(wxSizeEvent &ev)
{
  if (sw && sb && clav)
  {
    clav->SetSize(0, 0, ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT);
    sw->SetSize(0, ev.GetSize().GetHeight() - ASCLAVIER_HEIGHT - SBS, ev.GetSize().GetWidth(), ASCLAVIER_HEIGHT);
    sb->SetSize(0, ev.GetSize().GetHeight() - SBS, ev.GetSize().GetWidth(), SBS);
    sb->SetScrollbar(0, 10, (ASCLAVIER_WIDTH - ev.GetSize().GetWidth()), 10, false);
  }
}

void ASKeygroupEditor::PaintSelection()
{
  if (clav && ass)
  {
    ASamplerKeygroup *askg = ass->GetKeygroup();
    if (!askg)
      return;
    wxString notes[12] = { _("C"), _("C#"), _("D"), _("D#"), _("E"), _("F"), _("F#"), _("G"), _("G#"), _("A"), _("A#"), _("B") };
    wxString s(_("Low key: "));
    s += notes[askg->GetLowKey() % 12] + wxT(" ");
    s << (int)(askg->GetLowKey() / 12 - 2);
    lokey->SetLabel(s);
    s = _("High key: ");
    s += notes[askg->GetHighKey() % 12] + wxT(" ");
    s << (int)(askg->GetHighKey() / 12 - 2);
    hikey->SetLabel(s);
    vector<ASKey *> v = clav->GetKeys();
    for (vector<ASKey *>::iterator i = v.begin(); i != v.end(); i++)
    {
      if (((*i)->code < askg->GetLowKey()) || 
          ((*i)->code > askg->GetHighKey()))
        (*i)->selected = false;
      else
        (*i)->selected = true;
      (*i)->Refresh();
    }
  }
}

void ASKeygroupEditor::OnKeyUp(wxMouseEvent &ev)
{
  if (clav)
  {
    static char clkcnt = 0;

    ASKey *k = (ASKey *)ev.GetEventObject();
    ASamplerKeygroup *askg = ass->GetKeygroup();
    if (!askg)
      return;
    if (!clkcnt)
    {
      if (k->code <= askg->GetHighKey())
        askg->SetLowKey(k->code);
      else
      {
        askg->SetLowKey(askg->GetHighKey());
        askg->SetHighKey(k->code);
      }
    }
    else
    {
      if (k->code >= askg->GetLowKey())
        askg->SetHighKey(k->code);
      else
      {
        askg->SetHighKey(askg->GetLowKey());
        askg->SetLowKey(k->code);
      }
    }
    clkcnt = (++clkcnt) & 1;
    PaintSelection();
  }
}

void ASKeygroupEditor::OnKeyDown(wxMouseEvent &ev)
{
}

void ASKeygroupEditor::OnScroll(wxScrollEvent &e)
{
  static int oposx = 0;
  int posx = (oposx - sb->GetThumbPosition());
  sw->ScrollWindow(posx, 0, NULL);
  oposx = sb->GetThumbPosition();
  Refresh();
}
