// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "Colour.h"
#include "Settings.h"
#include "HelpCtrl.h"
#include "HelpPanel.h"

HelpPanel			*HelpWin;

HelpPanel::HelpPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size)
  : wxPanel(parent, -1, pos, size)
{
  wxLogNull noLog; // prevent indesired popup
  SetBackgroundColour(CL_SEQVIEW_BACKGROUND);
  /*  Text = new wxTextCtrl(this, -1, "This is Wired Help panel.", wxPoint(2, 2),
			wxSize(GetSize().x + 4, GetSize().y), 
			wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP | wxNO_BORDER);*/
  Help = new HelpCtrl(this, -1, wxString(WiredSettings->DataDir + wxString(L"help.zip")).c_str(), wxPoint(0,0), size);
}

HelpPanel::~HelpPanel()
{
  
}

void				HelpPanel::SetText(wxString &s)
{
  HelpWin->Help->Load_Text(s);
}

void				HelpPanel::OnSize(wxSizeEvent &event)
{
  Help->SetSize(wxSize(GetSize().x + 4, GetSize().y));
}

BEGIN_EVENT_TABLE(HelpPanel, wxPanel)
  EVT_SIZE(HelpPanel::OnSize)
END_EVENT_TABLE()
