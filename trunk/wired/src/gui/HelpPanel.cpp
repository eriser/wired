// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "HelpPanel.h"
#include "Colour.h"
#include "Settings.h"

HelpPanel *HelpWin;

BEGIN_EVENT_TABLE(HelpPanel, wxPanel)
  EVT_SIZE(HelpPanel::OnSize)
END_EVENT_TABLE()

HelpPanel::HelpPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size)
  : wxPanel(parent, -1, pos, size)
{
  SetBackgroundColour(CL_SEQVIEW_BACKGROUND);
  /*  Text = new wxTextCtrl(this, -1, "This is Wired Help panel.", wxPoint(2, 2),
			wxSize(GetSize().x + 4, GetSize().y), 
			wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP | wxNO_BORDER);*/
  Help = new HelpCtrl(this, -1, string(WiredSettings->DataDir + string("help.zip")).c_str(), wxPoint(0,0), size);
}

HelpPanel::~HelpPanel()
{
  
}

void HelpPanel::SetText(wxString &s)
{
  //  Text->Clear();
  //Text->AppendText(s);
}

void HelpPanel::OnSize(wxSizeEvent &event)
{
  Help->SetSize(wxSize(GetSize().x + 4, GetSize().y));
}








