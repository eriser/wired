// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <new>

#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/splash.h>
#include "MainApp.h"
#include "MainWindow.h"
#include "Settings.h"

#include "../include/config.h"

void	AllocationErrorHandler(void)
{
	cout << "[MAINAPP] Allocation error or not enough memory, exiting" << endl;
	exit(-1);
}

IMPLEMENT_APP(MainApp)

MainWindow			*MainWin;

bool				MainApp::OnInit()
{
  wxHandleFatalExceptions();
	//std::set_new_handler(&AllocationErrorHandler);
  wxBitmap			bitmap;
  wxSplashScreen*		splash;


#if wxUSE_LIBPNG
  wxImage::AddHandler(new wxPNGHandler);
#endif

  wxImage::AddHandler(new wxGIFHandler);

  SetAppName(L"wired");
  if (!wxApp::OnInit())
    return false;

  // used in error dialog
  MainWin = (MainWindow*)NULL;

  // init some conditions variables
  m_condAllDone = new wxCondition(m_mutex);

  // splash screen
  if (bitmap.LoadFile(wxString(INSTALL_PREFIX, *wxConvCurrent) + wxString(wxT("/share/wired/data/ihm/splash/splash.png")), wxBITMAP_TYPE_PNG))
    {
      splash = new wxSplashScreen(bitmap,
				  wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
				  6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
				  wxSIMPLE_BORDER|wxSTAY_ON_TOP);
      splash->Update();
      splash->Refresh();
      // alert dialog can use it before frame loading
      wxYield();
    }
#if 0
  const wxString		name = wxString::Format(L"wired-%s", wxGetUserId().c_str());
  Checker = new wxSingleInstanceChecker(name);
  if (Checker->IsAnotherRunning())
    {
      cout << "Another Wired is already running, aborting." << endl;      
      return (false);
    }
  delete Checker;
#endif  
  SetUseBestVisual(true);
  SetVendorName(L"Wired Team");
  Frame = new MainWindow(WIRED_TITLE, wxDefaultPosition, wxSize(APP_WIDTH, APP_HEIGHT));
  Frame->Show(true);
  SetTopWindow(Frame);
  splash->Hide();

  // now error dialog are based on mainframe 
  MainWin = Frame;

  // its obvious that user can't have deprecated conf with a fresh install
  if (WiredSettings->ConfIsDeprecated() && !WiredSettings->IsFirstLaunch())
    MainWin->AlertDialog(_("Warning"),
			 _("Your configuration file is deprecated, settings need to be set"));
  if (WiredSettings->IsFirstLaunch())
    ShowWelcome();

  // open stream, start fileconverter and co
  Frame->Init();
  return (true);
}

void	MainApp::ShowWelcome()
{
  wxString	welcome;

  welcome = wxT("Welcome to version WIRED_VERSION of WIRED_NAME.\nWIRED_NAME is currently a beta software, some of its features may not work completly yet.\nWe recommend to do not use the following features at this time :\n- Undo/Redo\n- Drag and drop of plugins\n- Codec management is known to be quite unstable at this time\n\nThe next step is to configure your soundcard settings in WIRED_NAME Settings dialog.Select 32 bits float for sample format, 44100hz for sample rate (or whatever you prefer) and choose a latency which your soundcard is capable of. You can try different values (the lower the most realtime WIRED_NAME will perform) and see which one is the best for your soundcard. Setting the latency too low will cause drops and glitch to appear in the sound output.\n\nYou will find in the Help menu, a \"Show integrated help\" item which will display an interactive help window on the bottom right corner of WIRED_NAME. If you move your mouse over a control in WIRED_NAME or in a plugin, it will show you the help associated with this item. \nIf you find any bugs in WIRED_NAME, please make a bug report at :\nWIRED_BUGS\n\nIf you need help or want to discuss about WIRED_NAME, pleast visit :\nWIRED_FORUMS");
  welcome.Replace(wxT("WIRED_VERSION"), WIRED_VERSION);
  welcome.Replace(wxT("WIRED_NAME"), WIRED_NAME);
  welcome.Replace(wxT("WIRED_BUGS"), WIRED_BUGS);
  welcome.Replace(wxT("WIRED_FORUMS"), WIRED_FORUMS);

  wxMessageDialog msg(MainWin, welcome, WIRED_NAME, wxOK | wxICON_INFORMATION | wxCENTRE);
  msg.ShowModal();
}

void              MainApp::OnFatalException()
{
#if wxUSE_DEBUGREPORT
	wxDebugReportCompress Report;
    Report.AddAll();
    if (wxDebugReportPreviewStd().Show(Report))
//	if (ReportPreview->Show(*Report))
	{
		Report.Process();
		Report.Reset();
        //send a mail
	}
#endif
}

void				MainApp::OnUnhandledException()
{
  cout << "UnhandledException" << endl;
  Frame->AlertDialog(_("Critical error"), _("An unhandled exception occurs, Wired stop now!"));
}

int				MainApp::OnExit()
{
#if 0
  delete Checker;
#endif
  Frame->Destroy();
  return (wxApp::OnExit());
}
