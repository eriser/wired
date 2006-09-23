// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <wx/string.h>
#include "MainWindow.h"
#include "MLTreeInfos.h"

//extern MainWindow		*MainWindowPanel;

MLTreeInfos::MLTreeInfos(wxWindow *MediaLibraryPanel, wxPoint p, wxSize s, long style)
  : wxWindow(MediaLibraryPanel->GetParent(), -1, p, s, style)
{
  // cout << "[MEDIALIBRARY] MLTreeInfos Constructor" << endl;

  wxTextCtrl *text = new wxTextCtrl();
  
  text->SetDefaultStyle(wxTextAttr(*wxRED));
  text->AppendText(_("Red text\n"));
//    text->SetDefaultStyle(wxTextAttr(wxNullColour, *wxLIGHT_GREY));
//    text->AppendText("Red on grey text\n");
//    text->SetDefaultStyle(wxTextAttr(*wxBLUE));
//    text->AppendText("Blue on grey text\n");
  Show(true);
}
 
MLTreeInfos::~MLTreeInfos()
{
  
}

void			MLTreeInfos::OnClick(wxMouseEvent& event)
{
  //cout << "[MEDIALIBRARY] Destroy infos" << endl;
  this->Destroy();
}

BEGIN_EVENT_TABLE(MLTreeInfos, wxWindow)
  EVT_LEFT_DOWN(MLTreeInfos::OnClick)
END_EVENT_TABLE();
  
