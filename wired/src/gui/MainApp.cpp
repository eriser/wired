// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <new>

#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/splash.h>
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/image.h>

#include "MainApp.h"
#include "MainWindow.h"
#include "Settings.h"
#include "SaveCenter.h"

#include <config.h>

#ifdef DEBUG_MAINAPP
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : " << __LINE__ << " : " << __FUNCTION__ << endl; }
#else
#define LOG
#endif

void	AllocationErrorHandler(void)
{
	cout << "[MAINAPP] Allocation error or not enough memory, aborting" << endl;
	exit(-1);
}

IMPLEMENT_APP(MainApp)

MainWindow *MainWin;

void	MainApp::OnInitCmdLine(wxCmdLineParser& parser)
{
  parser.SetDesc (g_cmdLineDesc);
  parser.SetSwitchChars(wxT("-"));
}

bool	MainApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
  wxString	arg;
  wxString	nosplash_str = wxT("wired-nosplash");

  // silent_mode = parser.Found(wxT("s"));
  std::cout << "parser.GetParamCount() == " << parser.GetParamCount() << std::endl;
  if (MainApp::argc == 2)
    SessionDir = MainApp::argv[1];
  else
    SessionDir = wxT("");
  if (!nosplash_str.CompareTo(MainApp::argv[0]))
	nosplash = true;
  else
	nosplash = false;
  return (true);
}
  
bool MainApp::OnInit()
{
  LOG;
#if wxUSE_ON_FATAL_EXCEPTION
  wxHandleFatalExceptions();
#endif
  wxBitmap        bitmap;
  wxSplashScreen* splash = NULL;
#if wxUSE_LIBPNG
  wxImage::AddHandler(new wxPNGHandler);
#endif
  wxImage::AddHandler(new wxGIFHandler);
  SetAppName(wxT("wired"));
  // allow use of command line process provided by wxWidgets
  if (!wxApp::OnInit())
    return false;
  // used in error dialog
  MainWin = (MainWindow *) NULL;
  // init some conditions variables
  m_condAllDone = new wxCondition(m_mutex);
  // splash screen
  if (!nosplash)
  {
	if (bitmap.LoadFile(wxString(DATA_DIR, *wxConvCurrent) + wxString(wxT("/wired/ihm/splash/splash.png")), wxBITMAP_TYPE_PNG))
	{
	  // we keep time-out very high for low cpu
	  splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
		  120000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	  splash->Update();
	  splash->Refresh();
	  // alert dialog can use it before frame loading
	  wxYield();
	}
  }
#if 0
  const wxString name = wxString::Format(wxT("wired-%s"), wxGetUserId().c_str());
  Checker = new wxSingleInstanceChecker(name);
  if (Checker->IsAnotherRunning())
  {
    cout << "Another instance of Wired is already running, aborting." << endl;
    return (false);
  }
  delete Checker;
#endif
  SetUseBestVisual(true);
  SetVendorName(wxT("Wired Team"));
  //  Frame = new MainWindow(WIRED_TITLE, wxDefaultPosition, wxGetDisplaySize());
  saveCenter = new SaveCenter();
  Frame = new MainWindow(WIRED_TITLE, wxDefaultPosition, wxSize(800,600), saveCenter);
  Frame->Show(true);
  SetTopWindow(Frame);
  // Wired crash if loading main frame is more than splash timeout
  if (!nosplash && splash)
    splash->Hide();
  // now error dialog are based on mainframe
  MainWin = Frame;
  // its obvious that user can't have deprecated conf with a fresh install
  if (WiredSettings->ConfIsDeprecated() && !WiredSettings->IsFirstLaunch())
    MainWin->AlertDialog(_("Warning"), _("Your configuration file is deprecated, settings need to be set"));
  if (WiredSettings->IsFirstLaunch())
    ShowWelcome();
  // open stream, start fileconverter and co
  if (Frame->Init() != 0)
  {
    cerr << "[WIRED] Critical error, initialisation failed" << endl;
    // returning false segfault.. so we exit instead
    exit(-1);
  }

  if (wxFileName::DirExists(SessionDir))
    MainWin->WiredStartSession(SessionDir);
  else
    MainWin->OpenWizard();
  return (true);
}

void MainApp::ShowWelcome()
{
  LOG;
  wxString welcome;
  welcome = wxT("Welcome to WIRED_NAME vWIRED_VERSION !\nPlease note that WIRED_NAME is still under heavy development and that some of its features may not work fully and/or correctly yet.\n\nYour first step will be to configure your soundcard settings in the Edit/Settings dialog. Select your device, a 32 bits float sample format (as it is the only working option right now) and any sample rate and latency that your sound card supports. Please note that the smaller the latency value is set, the more real time WIRED_NAME will perform but setting it too low will cause drops and glitches. You can try different values and see which one is the best for your soundcard (2048 samples per buffer will be ok in most cases).\n\nYou will find a contextual Help at the bottom right corner of the WIRED_NAME window. If you move your mouse over a control, it will show you the help associated with this item. You can replace it by the WIRED_NAME Mixer by clicking on the tool icon in the top right corner of this area.\n\nIf you find any bug, please make a bug report at :\nWIRED_BUGS\n\nIf you need help or want to discuss about WIRED_NAME, pleast visit our web site and our forum :\nWIRED_FORUMS\nAny feedback is also appreciated.\n\nEnjoy the free music experience !");
  welcome.Replace(wxT("WIRED_VERSION"), WIRED_VERSION);
  welcome.Replace(wxT("WIRED_NAME"), WIRED_NAME);
  welcome.Replace(wxT("WIRED_BUGS"), WIRED_BUGS);
  welcome.Replace(wxT("WIRED_FORUMS"), WIRED_FORUMS);
  wxMessageDialog msg(MainWin, welcome, WIRED_NAME, wxOK | wxICON_INFORMATION | wxCENTRE);
  msg.ShowModal();
}

#if wxUSE_ON_FATAL_EXCEPTION
void MainApp::OnFatalException()
{
  LOG;
# if wxUSE_DEBUGREPORT
  wxDebugReportCompress Report;
  Report.AddAll();
  if (wxDebugReportPreviewStd().Show(Report))
  {
    Report.Process();
    Report.Reset();
  }
# endif
}
#endif

void MainApp::OnUnhandledException()
{
  LOG;
  cout << "UnhandledException" << endl;
  Frame->AlertDialog(_("Critical error"), _("An unhandled exception has occurred, aborting"));
}

int MainApp::OnExit()
{
  LOG;
#if 0
  delete Checker;
#endif
  Frame->Destroy();
  return (wxApp::OnExit());
}
