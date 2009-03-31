// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MAINAPP_H__
#define __MAINAPP_H__

#include <wx/app.h>
#include <wx/thread.h>
#include <wx/dynarray.h>
#include <wx/cmdline.h>

WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);

class MainWindow;

class MainApp : public wxApp
{
public:
  virtual bool	OnInit ();
  virtual int 	OnExit ();
  virtual void	OnInitCmdLine(wxCmdLineParser& parser);
  virtual bool	OnCmdLineParsed(wxCmdLineParser& parser);

  // these popup should be called from every class
  void AlertDialog(const wxString& from, const wxString& msg);

  wxArrayThread m_threads;
  wxMutex       m_mutex;
  wxCondition*  m_condAllDone;

private:
  wxString	          SessionDir;
  bool			  nosplash;
  MainWindow              *Frame;

#if wxUSE_ON_FATAL_EXCEPTION
  void OnFatalException     ();
#endif
  void OnUnhandledException ();

  // 
  bool wxInitialization();

  // wx related
  void InitLocale();

  // popup related
  void ShowWelcome();
  void ShowWizard();
  void ShowSplash(bool show = true);

  // wizard related
  wxString ChooseSessionDir();
  void WiredStartSession(wxString sessionDir);
  // return true if another instance is running
  bool checkDoubleStart();
};

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
  { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT(""),
    wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
  { wxCMD_LINE_PARAM, wxT(""), wxT(""), wxT(""),
    wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_NONE }
};

DECLARE_APP(MainApp)

#endif/*__MAINAPP_H__*/
