// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __HELPCTRL_H
#define __HELPCTRL_H

#include <wx/wx.h>
#include "wx/image.h"
#include "wx/html/htmlwin.h"
#include "wx/fs_zip.h"

#define HTMLTOP "<html><body bgcolor=\"#889399\" leftmargin=\"0\" topmargin=\"0\" marginwidth=\"0\" marginheight=\"0\"><table width=\"100%\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td><img src=\"logo.png\" width=\"263\" height=\"74\"></td></tr></table><table width=\"100%\"  border=\"0\" cellspacing=\"0\" cellpadding=\"8\"><tr><td><strong><font size=-2>"

#define HTMLDOWN "</font></strong></td></tr></table></body></html>"

using namespace std;

class HelpCtrl : public wxHtmlWindow
{
 public:
  HelpCtrl(wxWindow *parent, wxWindowID id,wxString data_zip,const wxPoint &pos, const wxSize &size);
  
  ~HelpCtrl();
  void Load(wxString file);
  void Load_Text(wxString text);
 protected:
  wxHtmlWindow		*html;
  wxString		data_zip_path;
  wxString		path_file;
  wxString		file;
  wxString		text;
};

#endif
