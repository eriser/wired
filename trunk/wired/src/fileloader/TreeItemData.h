// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#ifndef __TREEITEMDATA_H__
#define __TREEITEMDATA_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
using		std::string;

class		TreeItemData: public wxTreeItemData
{
 public:
  TreeItemData(wxString _path);
  ~TreeItemData();
  
  wxString	GetPath();
  
 private:
  wxString	path;
}		;

#endif
