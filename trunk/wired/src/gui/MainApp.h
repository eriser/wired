// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __MAINAPP_H__
#define __MAINAPP_H__

#include <wx/app.h>
#include <wx/snglinst.h>

#define APP_TITLE		"Wired 0.11"
#define APP_WIDTH		(800)
#define APP_HEIGHT		(600)

class MainWindow;

class MainApp : public wxApp
{
 public:
  virtual bool			OnInit();
  virtual int			OnExit();
  int				FilterEvent(wxEvent& event);

 private:
  MainWindow			*Frame;
  wxSingleInstanceChecker	*Checker;
};

#endif/*__MAINAPP_H__*/
