// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/string.h>
#include "OptionPanel.h"
#include "EditMidi.h"
#include "WaveEditor.h"
#include "Colour.h"
#include "DownButton.h"
#include "MixerGui.h"
#include "HelpPanel.h"
#include "AudioPattern.h"
#include "MidiPattern.h"
#include "WavePanel.h"
#include "../editmidi/MidiPart.h"
#include "../redist/Plugin.h"
#include "../engine/Settings.h"

OptionPanel			*OptPanel;

WiredTool::WiredTool(wxString name, int type, wxWindow *win)
{
  Name = name;
  Type = type;
  Panel = win;
  IsDetached = false;
  Frame = 0x0;
  Data = 0x0;
}

WiredTool::~WiredTool()
{
  if (Type == ID_TOOL_OTHER_OPTIONPANEL)
    ((Plugin *)Data)->DestroyView();
  else if (Panel)
    Panel->Destroy();
  if (IsDetached && Frame)
    Frame->Destroy();
}

void				WiredFrame::OnClose(wxCloseEvent &event)
{
  Tool->Attach();
  if (event.CanVeto())
    event.Veto();
  Destroy();
}

void				WiredFrame::OnToolMove(wxCommandEvent &e)
{
  if (em)
    em->OnToolMove(e);
}

void				WiredFrame::OnToolEdit(wxCommandEvent &e)
{
  if (em)
    em->OnToolEdit(e);
}

void				WiredFrame::OnToolDel(wxCommandEvent &e)
{
  if (em)
    em->OnToolDel(e);
}

void				WiredTool::Attach()
{
  IsDetached = false;
  if (Type == ID_TOOL_MIDI_OPTIONPANEL)
    ((EditMidi *)Panel)->OnAttach();
  Panel->Reparent(OptPanel);
  Panel->SetPosition(wxPoint(0, OPT_TOOLBAR_HEIGHT));
  OptPanel->ShowTool(this);
  Frame = 0x0;
}

void				WiredTool::Detach()
{
  IsDetached = true;
  Frame = new WiredFrame(this, Name.c_str());
  Panel->Reparent(Frame);
  if (Type == ID_TOOL_MIDI_OPTIONPANEL)
    ((EditMidi *)Panel)->OnDetach(Frame);
  if (Type == ID_TOOL_AUDIO_OPTIONPANEL)
    ((WavePanel *)Panel)->OnDetach(Frame);
  Frame->Show();
}

OptionPanel::OptionPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style, WiredDocument* docParent)
  : wxPanel(parent, -1, pos, size, style), WiredDocument(wxT("OptionPanel"), docParent)
{
  SetBackgroundColour(CL_RULER_BACKGROUND);//wxColour(204, 199, 219));//*wxLIGHT_GREY);
  ToolbarPanel = new wxPanel(this, -1, wxPoint(0, 0), wxSize(GetSize().x, OPT_TOOLBAR_HEIGHT),
			     wxSIMPLE_BORDER);
  ToolbarPanel->SetBackgroundColour(CL_OPTION_TOOLBAR);
  Title = new wxStaticText(ToolbarPanel, -1, _("Midi pattern"), wxPoint(4, 0),
			   wxSize(-1, OPT_TOOLBAR_HEIGHT));
  //Title->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  Title->SetForegroundColour(*wxWHITE);
  wxImage *list_up = new wxImage(wxString(WiredSettings->DataDir + wxString(OPT_LIST_TOOL_UP)), wxBITMAP_TYPE_PNG);
  wxImage *list_down = new wxImage(wxString(WiredSettings->DataDir + wxString(OPT_LIST_TOOL_DOWN)), wxBITMAP_TYPE_PNG);
  wxImage *detach_up = new wxImage(wxString(WiredSettings->DataDir + wxString(OPT_DETACH_TOOL_UP)), wxBITMAP_TYPE_PNG);
  wxImage *detach_down = new wxImage(wxString(WiredSettings->DataDir + wxString(OPT_DETACH_TOOL_DOWN)), wxBITMAP_TYPE_PNG);
  wxImage *close_up = new wxImage(wxString(WiredSettings->DataDir + wxString(OPT_CLOSE_TOOL_UP)), wxBITMAP_TYPE_PNG);
  wxImage *close_down = new wxImage(wxString(WiredSettings->DataDir + wxString(OPT_CLOSE_TOOL_DOWN)), wxBITMAP_TYPE_PNG);
  ListToolBtn = new DownButton(this, ID_TOOL_LIST_OPTIONPANEL, wxPoint(GetSize().x - 34, 2), wxSize(14, 12),
			       list_up, list_down, true);
  DetachToolBtn = new DownButton(this, ID_TOOL_DETACH_OPTIONPANEL, wxPoint(GetSize().x - 18, 2), wxSize(14, 12),
				 detach_up, detach_down, true);
  CloseToolBtn = new DownButton(this, ID_TOOL_CLOSE_OPTIONPANEL, wxPoint(GetSize().x - 2, 2), wxSize(14, 12),
				 close_up, close_down, true);
  wxBoxSizer *right_sizer;
  right_sizer = new wxBoxSizer(wxHORIZONTAL);
  right_sizer->Add(ListToolBtn, 0, wxALL, 2);
  right_sizer->Add(DetachToolBtn, 0, wxALL, 2);
  right_sizer->Add(CloseToolBtn, 0, wxALL, 2);
  wxBoxSizer *sizer;
  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(4, OPT_TOOLBAR_HEIGHT, 0, 0);
  sizer->Add(Title, 1, 0, 0);
  sizer->Add(right_sizer, 0, wxEXPAND, 0);
  ToolbarPanel->SetSizer(sizer);
  CurrentTool = 0x0;
  // Creation Mixer
  MixerPanel = new MixerGui(this, wxPoint(0, OPT_TOOLBAR_HEIGHT),
			    wxSize(GetSize().GetWidth(), GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT), (WiredDocument*)this);
  MixerTool = new WiredTool(_("Wired Mixer"), ID_TOOL_MIXER_OPTIONPANEL, MixerPanel);

  HelpWin = new HelpPanel(this, wxPoint(0, OPT_TOOLBAR_HEIGHT),
			  wxSize(GetSize().GetWidth(), GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT));
  HelpTool = new WiredTool(_("Wired Help"), ID_TOOL_HELP_OPTIONPANEL, HelpWin);
  ToolsList.push_back(MixerTool);
  ToolsList.push_back(HelpTool);

  // default tool is HelpTool
  ShowTool(HelpTool);
}

OptionPanel::~OptionPanel()
{
  CleanChildren();
  // don't have to delete windows attached to another
}

void				OptionPanel::CleanChildren()
{
  DeleteTools();
}

void				OptionPanel::AddAudioTool(AudioPattern *p)
{
  WiredTool			*tool;
  WavePanel         *Panel;


  Panel = new WavePanel(this, -1, wxPoint(0, OPT_TOOLBAR_HEIGHT),
		     wxSize(GetSize().GetWidth(), GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT));

  // w = new WaveEditor(Panel, -1, wxPoint(0, OPT_TOOLBAR_HEIGHT),
// 		     wxSize(GetSize().GetWidth(), GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT), true);
  tool = new WiredTool(p->GetName(), ID_TOOL_AUDIO_OPTIONPANEL, Panel);
  tool->Data = p;
  ToolsList.push_back(tool);
  Panel->w->SetWave(p->GetWaveFile());
  Panel->AdjustScrollbar();
  ShowTool(tool);
}

void				OptionPanel::AddMidiTool(MidiPattern *p)
{
  EditMidi			*m;
  WiredTool			*tool;

  m = new EditMidi(this, -1, wxPoint(0, OPT_TOOLBAR_HEIGHT),
		   wxSize(GetSize().GetWidth(), GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT));
  tool = new WiredTool(p->GetName(), ID_TOOL_MIDI_OPTIONPANEL, m);
  tool->Data = p;
  ToolsList.push_back(tool);
  m->SetMidiPattern(p);
  ShowTool(tool);
}

void				OptionPanel::AddPlugTool(Plugin *p)
{
  wxWindow			*m;
  WiredTool			*tool;
  wxPoint			pt(0, OPT_TOOLBAR_HEIGHT);
  wxSize			sz(GetSize().GetWidth(), GetSize().GetHeight() - OPT_TOOLBAR_HEIGHT);

  m = p->CreateView(this, pt, sz);
  tool = new WiredTool(wxString(p->Name.c_str()), ID_TOOL_OTHER_OPTIONPANEL, m);
  tool->Data = p;
  ToolsList.push_back(tool);
  ShowTool(tool);
}

void				OptionPanel::ShowTool(WiredTool *t)
{
  vector<WiredTool *>::iterator	i;

  CurrentTool = t;
  if (!CurrentTool->IsDetached)
    {
      for (i = ToolsList.begin(); i != ToolsList.end(); i++)
	if (!(*i)->IsDetached && ((*i) != CurrentTool))
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
      CurrentTool->Frame->Show(false);
      CurrentTool->Frame->Show(true);
    }
}

void				OptionPanel::ShowWave(AudioPattern *p)
{
  vector<WiredTool *>::iterator	i;

  for (i = ToolsList.begin(); i != ToolsList.end(); i++)
    if (((*i)->Type == ID_TOOL_AUDIO_OPTIONPANEL) && ((*i)->Data == p))
      {
	ShowTool(*i);
	return;
      }
  AddAudioTool(p);
}

void				OptionPanel::ShowMidi(MidiPattern *p)
{
  vector<WiredTool *>::iterator	i;

  for (i = ToolsList.begin(); i != ToolsList.end(); i++)
    {
      if (((*i)->Type == ID_TOOL_MIDI_OPTIONPANEL) && ((*i)->Data == p))
	{
	  ShowTool(*i);
	  return;
	}
    }
  AddMidiTool(p);
}

void				OptionPanel::ShowPlug(Plugin *p)
{
  vector<WiredTool *>::iterator	i;

  for (i = ToolsList.begin(); i != ToolsList.end(); i++)
    {
      if (((*i)->Type == ID_TOOL_OTHER_OPTIONPANEL) && ((*i)->Data == p))
	{
	  ShowTool(*i);
	  return;
	}
    }
  AddPlugTool(p);
}

void				OptionPanel::ShowHelp()
{
  vector<WiredTool *>::iterator	i;

  for (i = ToolsList.begin(); i != ToolsList.end(); i++)
    {
      if ((*i)->Type == ID_TOOL_HELP_OPTIONPANEL)
	{
	  ShowTool(*i);
	  return;
	}
    }
}

void				OptionPanel::OnListToolClick(wxCommandEvent &event)
{
  vector<WiredTool *>::iterator	i;
  long				k;
  wxMenu			*menu;

  menu = new wxMenu();
  for (k = OPT_TOOL_ID_START, i = ToolsList.begin(); i != ToolsList.end(); i++, k++)
    {
      if (!(*i)->IsDetached)
	{
	  menu->Append(k, (*i)->Name.c_str());
	  Connect(k, wxEVT_COMMAND_MENU_SELECTED,
		  (wxObjectEventFunction)(wxEventFunction)
		  (wxCommandEventFunction)&OptionPanel::OnSelectTool);
	}
    }
  wxPoint p(ListToolBtn->GetPosition());
  PopupMenu(menu, p.x, p.y);
}

void				OptionPanel::OnDetachToolClick(wxCommandEvent &event)
{
  if (CurrentTool)
    {
      CurrentTool->Detach();
      ShowLastTool();
    }
}

void				OptionPanel::OnCloseToolClick(wxCommandEvent &event)
{
  vector<WiredTool *>::iterator	i;

  if (CurrentTool && (CurrentTool->Type != ID_TOOL_MIXER_OPTIONPANEL)
      && (CurrentTool->Type != ID_TOOL_HELP_OPTIONPANEL))
    {
      for (i = ToolsList.begin(); i != ToolsList.end(); i++)
	if (*i == CurrentTool)
	  {
	    ToolsList.erase(i);
	    break;
	  }
      delete CurrentTool;
      ShowLastTool();
    }
}

void				OptionPanel::OnSelectTool(wxCommandEvent &event)
{
  unsigned int			id = event.GetId() - OPT_TOOL_ID_START;

  if (id < ToolsList.size())
    {
      ShowTool(ToolsList[id]);
    }
}

void				OptionPanel::ShowLastTool()
{
  vector<WiredTool *>::iterator	i;

  for (i = ToolsList.begin(); i != ToolsList.end(); i++)
    if (!(*i)->IsDetached)
      {
	ShowTool(*i);
	return;
      }
  Title->SetLabel(_("No Tool"));
  CurrentTool = 0x0;
}

void				OptionPanel::DeleteTools()
{
  vector<WiredTool *>::iterator	i;

  for (i = ToolsList.begin(); i != ToolsList.end(); i++)
    {
      if (((*i)->Type != ID_TOOL_MIXER_OPTIONPANEL)
	  && ((*i)->Type != ID_TOOL_HELP_OPTIONPANEL))
	delete *i;
    }
  ToolsList.clear();
  ToolsList.push_back(MixerTool);
  ToolsList.push_back(HelpTool);
  ShowTool(MixerTool);
}

void				OptionPanel::DeleteTools(void *DataPointer)
{
  if (!DataPointer)
    return;
  vector<WiredTool *>::iterator	i;

  for (i = ToolsList.begin(); i != ToolsList.end(); i++)
    {
      if (((*i)->Type != ID_TOOL_MIXER_OPTIONPANEL)
	  && ((*i)->Type != ID_TOOL_HELP_OPTIONPANEL))
	    if ((*i)->Data == DataPointer)
  	    {
	      ShowTool(MixerTool);
	      ToolsList.erase(i);
	      delete *i;
	      break;
	    }
    }
}

void				OptionPanel::ClosePlug(Plugin *p)
{
  DeleteTools(p);
}

void				OptionPanel::Save()
{
  //Title
  saveDocData(new SaveElement(wxT("title"), this->Title->GetLabel()));

  //ListToolBtn
  saveDocData(new SaveElement(wxT("listToolBtn"), this->ListToolBtn->GetOn()));

  //DetachToolBtn
  saveDocData(new SaveElement(wxT("detachToolBtn"), this->DetachToolBtn->GetOn()));

  //CloseToolBtn
  saveDocData(new SaveElement(wxT("closeToolBtn"), this->CloseToolBtn->GetOn()));

  saveDocData(new SaveElement(wxT("CurrentTool"), CurrentTool->GetName()));

  //ToolsList
  //vector of wiredTools

}

void				OptionPanel::Load(SaveElementArray data)
{
  int		dataCompt;

  for (dataCompt = 0; dataCompt < data.GetCount(); dataCompt++)
    {
      if (data[dataCompt]->getKey() == wxT("title"))
	{
	  this->SetTitle(data[dataCompt]->getValue());
	}
      else if (data[dataCompt]->getKey() == wxT("listToolBtn"))
	{
	  if (data[dataCompt]->getValueInt())
	    this->SetListToolBtn(true);
	  else
	    this->SetListToolBtn(false);
	}
      else if (data[dataCompt]->getKey() == wxT("detachToolBtn"))
	{
	  if (data[dataCompt]->getValueInt())
	    this->SetDetachToolBtn(true);
	  else
	    this->SetDetachToolBtn(false);
	}
      else if (data[dataCompt]->getKey() == wxT("closeToolBtn"))
	{
	  if (data[dataCompt]->getValueInt())
	    this->SetCloseToolBtn(true);
	  else
	    this->SetCloseToolBtn(false);
	}
      else if (data[dataCompt]->getKey() == wxT("CurrentTool"))
	{
	  vector<WiredTool *>::iterator	it;

	  for (it = ToolsList.begin(); it != ToolsList.end(); it++)
	    if (data[dataCompt]->getValue() == (*it)->GetName())
	      ShowTool(*it);
	}
    }
}

//Setters
void		OptionPanel::SetTitle(const wxString& title)
{
  this->Title->SetLabel(title);
}

void		OptionPanel::SetListToolBtn(bool isDown)
{
  if (isDown)
    this->ListToolBtn->SetOn();
  else
    this->ListToolBtn->SetOff();
}

void		OptionPanel::SetDetachToolBtn(bool isDown)
{
  if (isDown)
    this->DetachToolBtn->SetOn();
  else
    this->DetachToolBtn->SetOff();
}

void		OptionPanel::SetCloseToolBtn(bool isDown)
{
  if (isDown)
    this->CloseToolBtn->SetOn();
  else
    this->CloseToolBtn->SetOff();
}



BEGIN_EVENT_TABLE(WiredFrame, wxFrame)
  EVT_MENU(ID_TOOL_MOVE_MIDIPART, WiredFrame::OnToolMove)
  EVT_MENU(ID_TOOL_EDIT_MIDIPART, WiredFrame::OnToolEdit)
  EVT_MENU(ID_TOOL_DEL_MIDIPART, WiredFrame::OnToolDel)
END_EVENT_TABLE();

BEGIN_EVENT_TABLE(OptionPanel, wxPanel)
  EVT_BUTTON(ID_TOOL_LIST_OPTIONPANEL, OptionPanel::OnListToolClick)
  EVT_BUTTON(ID_TOOL_DETACH_OPTIONPANEL, OptionPanel::OnDetachToolClick)
  EVT_BUTTON(ID_TOOL_CLOSE_OPTIONPANEL, OptionPanel::OnCloseToolClick)
END_EVENT_TABLE()

