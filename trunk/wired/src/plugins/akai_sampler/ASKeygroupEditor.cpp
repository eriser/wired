#include "ASKeygroupEditor.h"

BEGIN_EVENT_TABLE(ASKeygroupEditor, wxDialog)
  EVT_PAINT(ASKeygroupEditor::OnPaint)
  EVT_CLOSE(ASKeygroupEditor::OnClose)
  EVT_LEFT_DOWN(ASKeygroupEditor::OnKeyDown)
  EVT_LEFT_UP(ASKeygroupEditor::OnKeyUp)
END_EVENT_TABLE()

ASKeygroupEditor::ASKeygroupEditor(wxWindow *parent, int id, wxString title, wxPoint pos, wxSize size) : 
  wxDialog(parent, id, title, pos, size)
{
  clav = new ASClavier(this, -1, wxPoint(0, 0), wxSize(ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT), 0, 0);
  SetSize(wxSize(ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT));
  Center();
  Show();
}

void ASKeygroupEditor::OnPaint(wxPaintEvent &ev)
{
}

void ASKeygroupEditor::OnClose(wxCloseEvent &ev)
{
  Hide();
}

void ASKeygroupEditor::PaintSelection()
{
  ASamplerKeygroup *askg = ass->GetKeygroup();
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

void ASKeygroupEditor::OnKeyUp(wxMouseEvent &ev)
{
  static char clkcnt = 0;

  ASKey *k = (ASKey *)ev.GetEventObject();
  ASamplerKeygroup *askg = ass->GetKeygroup();
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

void ASKeygroupEditor::OnKeyDown(wxMouseEvent &ev)
{
  ASKey *k = (ASKey *)ev.GetEventObject();
}
