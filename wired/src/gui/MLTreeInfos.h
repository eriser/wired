// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MLTREEINFOS_H__
#define __MLTREEINFOS_H__

using namespace			std;

#include "MLTree.h"

enum
  {

  };

class				MywxStaticText : public wxStaticText
{

 private:

  wxWindow	*infoParent;

 public:

  MywxStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style, const wxString& name);
  void InfoDestroy(wxMouseEvent& event);

  DECLARE_EVENT_TABLE()
};

class				MLTreeInfos: public wxWindow
{

 protected:

 friend class			MediaLibrary;
 friend class			MLTree;

 public:

  MLTreeInfos(wxWindow *MediaLibraryPanel, wxPoint p, wxSize s, long style, s_nodeInfo infos);
  ~MLTreeInfos();
};

#endif
