// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MAINAPP_H__
#define __MAINAPP_H__

#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/debugrpt.h>

#include <wx/thread.h>
#include <wx/dynarray.h>

#include "version.h"

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
  wxArrayThread			m_threads;
  wxMutex			m_mutex;
  wxCondition*			m_condAllDone;

 private:
  MainWindow			*Frame;
  wxSingleInstanceChecker	*Checker;
  void			        OnFatalException();
  void				OnUnhandledException();
};

DECLARE_APP(MainApp)

#endif/*__MAINAPP_H__*/
