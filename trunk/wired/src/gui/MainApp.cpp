// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <new>

#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/splash.h>
#include "MainApp.h"
#include "MainWindow.h"
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
  Frame->Init();
  return (true);
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
