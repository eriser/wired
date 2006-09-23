// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MLTREEINFOS_H__
#define __MLTREEINFOS_H__

using namespace			std;

#include <vector>
#include <wx/wx.h>
#include <wx/string.h>

enum
  {

  };

class				MLTreeInfos: public wxWindow
{
 protected:

 public:
  
  
  MLTreeInfos(wxWindow *MediaLibraryPanel, wxPoint p, wxSize s, long style);
  ~MLTreeInfos();

  void		OnClick(wxMouseEvent& event);
  DECLARE_EVENT_TABLE();
};

#endif
