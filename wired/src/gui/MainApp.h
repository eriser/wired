// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __MAINAPP_H__
#define __MAINAPP_H__

#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/debugrpt.h>

#include "wx/thread.h"
#include "wx/dynarray.h"

#define APP_TITLE		"Wired 0.2"
#define APP_WIDTH		(800)
#define APP_HEIGHT		(600)

WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);

class MainWindow;

class MainApp : public wxApp
{
 public:
  virtual bool			OnInit();
  virtual int			OnExit();
  int				FilterEvent(wxEvent& event);
  wxArrayThread 	m_threads;
  wxCriticalSection m_critsect;
  wxSemaphore 		m_semAllDone;

 private:
  MainWindow				*Frame;
  wxSingleInstanceChecker	*Checker;
  void                      OnFatalException();
  void						OnUnhandledException();
};

DECLARE_APP(MainApp)

#endif/*__MAINAPP_H__*/
