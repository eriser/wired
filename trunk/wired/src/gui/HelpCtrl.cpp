// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "HelpCtrl.h"
#include <wx/fs_zip.h>

HelpCtrl::HelpCtrl(wxWindow *parent, wxWindowID id, wxString data_zip,
		   const wxPoint &pos, const wxSize &size)
  : wxHtmlWindow(parent, id, pos , size, wxHW_SCROLLBAR_AUTO | wxHW_NO_SELECTION, _("Wired Help"))
{
  wxLogNull noLog; // prevent indesired popup

  data_zip_path = data_zip;
  path_file = data_zip_path;
  path_file += L"#zip:index.html";
  wxFileSystem::AddHandler(new wxZipFSHandler);
  LoadPage(path_file);
}

void HelpCtrl::Load(wxString file)
{
  wxLogNull noLog; // prevent indesired popup
  wxString tmp;

  tmp = data_zip_path ;//+= "#zip:" ;
  tmp += file;
  LoadPage(tmp);
}

void HelpCtrl::Load_Text(wxString text)
{
  wxLogNull noLog; // prevent indesired popup
  wxString tmp = HTMLTOP;
  tmp += wxString(text + wxString(HTMLDOWN));
  SetPage(tmp);
}
HelpCtrl::~HelpCtrl()
{

}
