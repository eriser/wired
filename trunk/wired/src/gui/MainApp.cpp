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
	SetAppName(L"wired");
	if (!wxApp::OnInit())
		return false;
  if (bitmap.LoadFile(wxString(INSTALL_PREFIX, *wxConvCurrent) + wxString(wxT("/share/wired/data/ihm/splash/splash.png")), wxBITMAP_TYPE_PNG))
    {
      splash = new wxSplashScreen(bitmap,
				  wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
				  6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
				  wxSIMPLE_BORDER|wxSTAY_ON_TOP);
      splash->Show();
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
  Frame = new MainWindow(wxString(WIRED_TITLE, *wxConvCurrent), wxDefaultPosition,
			 wxSize(APP_WIDTH, APP_HEIGHT));
  MainWin = Frame;
  Frame->Show(true);
  SetTopWindow(Frame);
  splash->Hide();
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

}

int				MainApp::OnExit()
{
#if 0
  delete Checker;
#endif
    Frame->Destroy();
//    delete Frame;
  return (wxApp::OnExit());
}

int				MainApp::FilterEvent(wxEvent& event)
{
  if ((event.GetEventType() == wxEVT_KEY_DOWN))
    {
      if (((wxKeyEvent &)event).GetKeyCode() == WXK_SPACE)
	{
	  Frame->OnSpaceKey();
	  return (true);
	}
      else if (((wxKeyEvent &)event).GetKeyCode() == WXK_TAB)
	{ 
	  if (((wxKeyEvent &)event).ShiftDown()) 
	    Frame->SwitchSeqOptView();
	  else
	    Frame->SwitchRackOptView();
	  return (true);
	}
    }
  return (-1);
}
