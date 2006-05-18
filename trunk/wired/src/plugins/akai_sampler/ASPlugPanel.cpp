// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "ASPlugPanel.h"
#include "AkaiSampler.h"
#include "Colour.h"

void				ASPlugFrame::OnClose(wxCloseEvent &event)
{
  Plug->Attach();
  if (event.CanVeto())
    event.Veto();
  Destroy();
}

ASPlug::ASPlug(ASPlugPanel *aspp, wxString name, int type, wxWindow *win)
{
  Name = name;
  Type = type;
  Panel = win;
  IsDetached = false;
  Frame = 0x0;
  Plugin = 0x0;
  this->aspp = aspp;
}
 
ASPlug::~ASPlug()
{
  if (Panel)
    Panel->Destroy();
  if (Frame)
    Frame->Destroy();
}

void				ASPlug::Attach()
{
  IsDetached = false;
  Panel->Reparent(aspp);
  Panel->SetPosition(wxPoint(0, OPT_TOOLBAR_HEIGHT));
  Panel->SetSize(wxSize(aspp->GetSize().GetWidth(), aspp->GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT));
  aspp->ShowPlug(this);
  Plugin->OnAttach(aspp);
  Frame = 0x0;
}

void				ASPlug::Detach()
{
  IsDetached = true;
  Frame = new ASPlugFrame(this, Name.c_str());
  Panel->Reparent(Frame);
  Frame->SetSize(Panel->GetSize());
  Frame->Show();
  Plugin->OnDetach(Frame);
}

ASPlugPanel::ASPlugPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style, AkaiSampler *as)
  : wxPanel(parent, -1, pos, size, style)
{
  SetBackgroundColour(CL_RULER_BACKGROUND);
  ToolbarPanel = new wxPanel(this, -1, wxPoint(0, 0), wxSize(GetSize().x, OPT_TOOLBAR_HEIGHT),
			     wxSIMPLE_BORDER);
  ToolbarPanel->SetBackgroundColour(CL_OPTION_TOOLBAR);
  Title = new wxStaticText(ToolbarPanel, -1, _("Plugin"), wxPoint(18, 0), 
			   wxSize(-1, OPT_TOOLBAR_HEIGHT));
  Title->SetForegroundColour(*wxWHITE);
  wxImage *detach_up = new wxImage(wxString(as->GetDataDir() + wxString(OPT_DETACH_TOOL_UP)), wxBITMAP_TYPE_PNG);
  wxImage *detach_down = new wxImage(wxString(as->GetDataDir() + wxString(OPT_DETACH_TOOL_DOWN)), wxBITMAP_TYPE_PNG);
  DetachPlugBtn = new DownButton(this, ID_TOOL_DETACH_OPTIONPANEL, wxPoint(2, 2), wxSize(14, 12), 
				 detach_up, detach_down, true);
  wxBoxSizer *right_sizer;
  right_sizer = new wxBoxSizer(wxHORIZONTAL);
  right_sizer->Add(DetachPlugBtn, 0, wxALL, 2); 
  wxBoxSizer *sizer;
  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(4, OPT_TOOLBAR_HEIGHT, 0, 0); 
  sizer->Add(right_sizer, 0, wxEXPAND, 0); 
  sizer->Add(Title, 1, 0, 0); 
  ToolbarPanel->SetSizer(sizer);
  CurrentPlug = 0x0;
}

ASPlugPanel::~ASPlugPanel()
{

}

void				ASPlugPanel::AddPlug(ASPlugin *p)
{
  wxWindow			*m;
  ASPlug      *tool;
  wxPoint			pt(0, OPT_TOOLBAR_HEIGHT);
  wxSize			sz(GetSize().GetWidth(), GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT);
  
  m = p->CreateView(this, pt, sz);
  tool = new ASPlug(this, p->Name, ID_TOOL_OTHER_OPTIONPANEL, m);
  tool->Plugin = p;
  PlugsList.push_back(tool);
  tool->Panel->Show(false);
  if (!CurrentPlug)
    CurrentPlug = tool;
  ShowPlug(CurrentPlug);
}

void				ASPlugPanel::ShowPlug(ASPlug *t)
{
  vector<ASPlug *>::iterator	i;

  if (!t)
    return;
  CurrentPlug = t;
  if (!CurrentPlug->IsDetached)
    {
      for (i = PlugsList.begin(); i != PlugsList.end(); i++)
	if (!(*i)->IsDetached && ((*i) != CurrentPlug))
	  (*i)->Panel->Show(false);
      TopSizer = new wxBoxSizer(wxVERTICAL);    
      TopSizer->Add(ToolbarPanel, 0, wxEXPAND | wxALL, 0);
      TopSizer->Add(t->Panel, 1, wxEXPAND | wxALL, 0);
      SetSizer(TopSizer);
      Title->SetLabel(t->Name.c_str());
      t->Panel->SetSize(wxSize(GetSize().x, GetSize().y - OPT_TOOLBAR_HEIGHT));
      t->Panel->Show(true);  
    }
  else
    {
      if (CurrentPlug != NULL)
      {
        CurrentPlug->Frame->Show(false);
        CurrentPlug->Frame->Show(true);
      }
    }
}

void        ASPlugPanel::RemovePlugin(ASPlugin *p)
{
  vector<ASPlug *>::iterator	i;
  
  for (i = PlugsList.begin(); i != PlugsList.end(); i++)
    if ((*i)->Plugin == p)
    {
      i = PlugsList.erase(i);
      return;
    }
}

void				ASPlugPanel::ShowPlugin(ASPlugin *p)
{
  vector<ASPlug *>::iterator	i;
  
  for (i = PlugsList.begin(); i != PlugsList.end(); i++)
    {
      if (((*i)->Type == ID_TOOL_OTHER_OPTIONPANEL) && ((*i)->Plugin == p))
	{
	  ShowPlug(*i);
	  return;
	}
    }
  AddPlug(p);  
}

void				ASPlugPanel::OnDetachPlugClick(wxCommandEvent &event)
{
  if (CurrentPlug)
    {
      CurrentPlug->Detach();
      ShowLastPlug();
    }
}

void				ASPlugPanel::ShowLastPlug()
{
  vector<ASPlug *>::iterator	i;
  
  for (i = PlugsList.begin(); i != PlugsList.end(); i++)
    if (!(*i)->IsDetached)
      {
	ShowPlug(*i);
	return;
      }
  Title->SetLabel(_("No Plugin"));
  CurrentPlug = 0x0;
}

void				ASPlugPanel::DeletePlugs()
{
  vector<ASPlug *>::iterator	i;
  
  for (i = PlugsList.begin(); i != PlugsList.end(); i++)
    {
      if (((*i)->Type != ID_TOOL_MIXER_OPTIONPANEL) 
	  && ((*i)->Type != ID_TOOL_HELP_OPTIONPANEL))
	delete *i;
    }
  PlugsList.clear();
}

BEGIN_EVENT_TABLE(ASPlugPanel, wxPanel)
  EVT_BUTTON(ID_TOOL_DETACH_OPTIONPANEL, ASPlugPanel::OnDetachPlugClick)
END_EVENT_TABLE()
  
