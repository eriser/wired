#include "ASPlugPanel.h"
#include "Colour.h"

ASPlugPanel   *PlugPanel;

void				ASPlugFrame::OnClose(wxCloseEvent &event)
{
  Plug->Attach();
  if (event.CanVeto())
    event.Veto();
  Destroy();
}

ASPlug::ASPlug(wxString name, int type, wxWindow *win)
{
  Name = name;
  Type = type;
  Panel = win;
  IsDetached = false;
  Frame = 0x0;
  Plugin = 0x0;
}
 
ASPlug::~ASPlug()
{
  Panel->Destroy();
  Frame->Destroy();
}

void				ASPlug::Attach()
{
  IsDetached = false;
  Panel->Reparent(PlugPanel);
  Panel->SetPosition(wxPoint(0, OPT_TOOLBAR_HEIGHT));
  Panel->SetSize(wxSize(PlugPanel->GetSize().GetWidth(), PlugPanel->GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT));
  PlugPanel->ShowPlug(this);
  Plugin->OnAttach(PlugPanel);
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

ASPlugPanel::ASPlugPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style, Plugin *p)
  : wxPanel(parent, -1, pos, size, style)
{
  this->p = p;
  SetBackgroundColour(CL_RULER_BACKGROUND);//wxColour(204, 199, 219));//*wxLIGHT_GREY);
  ToolbarPanel = new wxPanel(this, -1, wxPoint(0, 0), wxSize(GetSize().x, OPT_TOOLBAR_HEIGHT),
			     wxSIMPLE_BORDER);
  ToolbarPanel->SetBackgroundColour(CL_OPTION_TOOLBAR);
  Title = new wxStaticText(ToolbarPanel, -1, "Plugin", wxPoint(4, 0), 
			   wxSize(-1, OPT_TOOLBAR_HEIGHT));
  //Title->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  Title->SetForegroundColour(*wxWHITE);
  wxImage *list_up = new wxImage(string(p->GetDataDir() + string(OPT_LIST_TOOL_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *list_down = new wxImage(string(p->GetDataDir() + string(OPT_LIST_TOOL_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *detach_up = new wxImage(string(p->GetDataDir() + string(OPT_DETACH_TOOL_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *detach_down = new wxImage(string(p->GetDataDir() + string(OPT_DETACH_TOOL_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *close_up = new wxImage(string(p->GetDataDir() + string(OPT_CLOSE_TOOL_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *close_down = new wxImage(string(p->GetDataDir() + string(OPT_CLOSE_TOOL_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  ListPlugBtn = new DownButton(this, ID_TOOL_LIST_OPTIONPANEL, wxPoint(GetSize().x - 48, 2), wxSize(14, 12), 
			       list_up, list_down, true);
  DetachPlugBtn = new DownButton(this, ID_TOOL_DETACH_OPTIONPANEL, wxPoint(GetSize().x - 32, 2), wxSize(14, 12), 
				 detach_up, detach_down, true);
  ClosePlugBtn = new DownButton(this, ID_TOOL_CLOSE_OPTIONPANEL, wxPoint(GetSize().x - 16, 2), wxSize(14, 12), 
				 close_up, close_down, true);
  wxBoxSizer *right_sizer;
  right_sizer = new wxBoxSizer(wxHORIZONTAL);
  right_sizer->Add(ListPlugBtn, 0, wxALL, 2); 
  right_sizer->Add(DetachPlugBtn, 0, wxALL, 2); 
  right_sizer->Add(ClosePlugBtn, 0, wxALL, 2); 
  wxBoxSizer *sizer;
  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(4, OPT_TOOLBAR_HEIGHT, 0, 0); 
  sizer->Add(Title, 1, 0, 0); 
  sizer->Add(right_sizer, 0, wxEXPAND, 0); 
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
  
  p->SetPlugin(this->p);
  m = p->CreateView(this, pt, sz);
  tool = new ASPlug(p->Name, ID_TOOL_OTHER_OPTIONPANEL, m);
  tool->Plugin = p;
  PlugsList.push_back(tool);
  ShowPlug(tool);
}

void				ASPlugPanel::ShowPlug(ASPlug *t)
{
  vector<ASPlug *>::iterator	i;

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
      CurrentPlug->Frame->Show(false);
      CurrentPlug->Frame->Show(true);
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

void				ASPlugPanel::OnListPlugClick(wxCommandEvent &event)
{
  vector<ASPlug *>::iterator	i;
  long				k;
  wxMenu			*menu;
  
  menu = new wxMenu();
  for (k = OPT_TOOL_ID_START, i = PlugsList.begin(); i != PlugsList.end(); i++, k++)
    {
      menu->Append(k, (*i)->Name.c_str());
      Connect(k, wxEVT_COMMAND_MENU_SELECTED, 
	      (wxObjectEventFunction)(wxEventFunction)
	      (wxCommandEventFunction)&ASPlugPanel::OnSelectPlug);
    }
  wxPoint p(ListPlugBtn->GetPosition());
  PopupMenu(menu, p.x, p.y);
}

void				ASPlugPanel::OnDetachPlugClick(wxCommandEvent &event)
{
  if (CurrentPlug)
    {
      CurrentPlug->Detach();
      ShowLastPlug();
    }
}

void				ASPlugPanel::OnClosePlugClick(wxCommandEvent &event)
{
}

void				ASPlugPanel::OnSelectPlug(wxCommandEvent &event)
{
  unsigned int			id = event.GetId() - OPT_TOOL_ID_START;

  if (id < PlugsList.size())
    {
      ShowPlug(PlugsList[id]);
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
  Title->SetLabel("No Plug");      
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

void				ASPlugPanel::ClosePlug(ASPlugin *p)
{
  vector<ASPlug *>::iterator	i;
  
  for (i = PlugsList.begin(); i != PlugsList.end(); i++)
    {
      if ((*i)->Plugin == p)
	{
	  if (*i == CurrentPlug)
	    {	    
	      ShowLastPlug();
  	    }     
	  delete *i;
	  PlugsList.erase(i);
	  break;
	}
    }
}

BEGIN_EVENT_TABLE(ASPlugPanel, wxPanel)
  EVT_BUTTON(ID_TOOL_LIST_OPTIONPANEL, ASPlugPanel::OnListPlugClick)
  EVT_BUTTON(ID_TOOL_DETACH_OPTIONPANEL, ASPlugPanel::OnDetachPlugClick)
  EVT_BUTTON(ID_TOOL_CLOSE_OPTIONPANEL, ASPlugPanel::OnClosePlugClick)
END_EVENT_TABLE()
  
