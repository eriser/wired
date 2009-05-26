// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <new>
#include <iostream>

#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/splash.h>
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/snglinst.h>
#include <wx/debugrpt.h>

#include "MainApp.h"
#include "MainWindow.h"
#include "Settings.h"
#include "SaveCenter.h"
#include "Wizard.h"

#include <config.h>
#include "version.h"

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
  //std::cout << "parser.GetParamCount() == " << parser.GetParamCount() << std::endl;
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

// check if Wired is already launched
bool		MainApp::checkDoubleStart()
{
  const wxString name = wxString::Format(wxT("wired-%s"), wxGetUserId().c_str());
  wxSingleInstanceChecker *Checker;

  Checker = new wxSingleInstanceChecker(name);
  if (Checker->IsAnotherRunning())
  {
    cout << "Another instance of Wired is already running, aborting." << endl;
    delete Checker;
    return (true);
  }
  delete Checker;
  return (false);
}

// locale initialization
void                MainApp::InitLocale()
{
  LOG;
  // disable extra output of wx
  wxLog		log(wxLogNull);
  wxString	prefix = wxT(PACKAGE_LOCALE_DIR);
  wxLocale	wLocale;

  wLocale.AddCatalogLookupPathPrefix(prefix);

  //try to set default language (is it really useful ? it seems to never work)
  if (wLocale.Init(wxLANGUAGE_DEFAULT) == true)
    cout << "[MAINWIN][InitLocale] locale initialized to wxLANGUAGE_DEFAULT" << endl;;

  // add wx basic translation (File, Window, About, ..) (It seems to never return true ...)
  if (wLocale.AddCatalog(wxT("wxstd")) == true)
    cout << "[MAINWIN][Initlocale] wxstd catalog added" << endl;;

  // add our translations
  if (wLocale.AddCatalog(wxT("wired")) == true)
    cout << "[MAINWIN][InitLocale] wired catalog added" << endl;;
}

// force user to select a directory
wxString	MainApp::ChooseSessionDir()
{
  LOG;

  wxDirDialog	dirDialog(NULL, _("Select a project folder"), wxGetCwd());

  while(dirDialog.ShowModal() != wxID_OK)
    AlertDialog(_("Warning"),
		_("You have to select a project folder."));

  return (dirDialog.GetPath());
}

void		MainApp::WiredStartSession(wxString sessionDir)
{
  wxFileName	path;

  WiredSettings->AddDirToRecent(sessionDir);
  path.AssignDir(sessionDir);
  path.MakeAbsolute();
  wxFileName::SetCwd(path.GetPath());

  saveCenter->setProjectPath(path);
}

// open a wizard window
void		MainApp::ShowWizard()
{
  LOG;

  Wizard	wiz;

  wiz.ShowModal();
  if (wxFileName::DirExists(wiz.GetDir()))
	WiredStartSession(wiz.GetDir());
  else
	WiredStartSession(ChooseSessionDir());
}

// show/hide splash
void MainApp::ShowSplash(bool show)
{
  static wxSplashScreen* splash = NULL;

  if (nosplash)
    return;

  if ( show )
    {
      wxBitmap        bitmap;

      if (bitmap.LoadFile(wxString(DATA_DIR, *wxConvCurrent) + wxString(wxT("/wired/ihm/splash/splash.png")), wxBITMAP_TYPE_PNG))
	{
	  if ( ! splash )
	    {
	      // we keep time-out very high for low cpu
	      splash = new wxSplashScreen(bitmap,
					  wxSPLASH_CENTRE_ON_SCREEN
					  | wxSPLASH_TIMEOUT,
					  120000,
					  NULL,
					  -1,
					  wxDefaultPosition,
					  wxDefaultSize,
					  wxSIMPLE_BORDER);
	    }
	  splash->Update();
	  splash->Refresh();
	  // alert dialog can use it before frame loading
	  wxYield();
        }
    }
  else
    {
      // Wired crash if loading time of main frame is longer than splash timeout
      if (splash)
	splash->Hide();
    }
}

bool MainApp::wxInitialization()
{
  LOG;

#if wxUSE_ON_FATAL_EXCEPTION
  wxHandleFatalExceptions();
#endif

  // load image handlers
#if wxUSE_LIBPNG
  wxImage::AddHandler(new wxPNGHandler);
#endif
  wxImage::AddHandler(new wxGIFHandler);

  // set static stuff
  SetAppName(wxT("wired"));
  SetUseBestVisual(true);
  SetVendorName(wxT("Wired Team"));

  InitLocale();

  // allow use of command line process provided by wxWidgets
  if (!wxApp::OnInit())
      return false;
  return true;
}

// start point of Wired
bool MainApp::OnInit()
{
  cout << "[MAINAPP] Wired initialization..." << endl;

  // begin with wxWidgets initialization
  if (!wxInitialization())
    return false;

  // this var is used in error dialog
  MainWin = (MainWindow *) NULL;

  // splash screen
  ShowSplash();

  // avoid two Wired's launch
  if (checkDoubleStart())
    return false;

  // load save/session stuff
  saveCenter = new SaveCenter();
  WiredSettings = new Settings(); // load settings

  // load wizard
  if (wxFileName::DirExists(SessionDir))
    WiredStartSession(SessionDir);
  else
    ShowWizard();
  ShowSplash();

  // init cond mutex to do a clean stop
  m_condAllDone = new wxCondition(m_mutex);

  // open the main wired window
  Frame = new MainWindow(WIRED_TITLE, wxDefaultPosition, wxSize(800,600), saveCenter);

  // now error dialog could be based on mainframe
  MainWin = Frame;

  // if we're loading a project from an existing saving directory
  saveCenter->LoadProject();

  // hide splash screen
  ShowSplash(false);

  // .. and show main windows
  Frame->Show(true);
  SetTopWindow(Frame);

  // welcome new users!
  if (WiredSettings->IsFirstLaunch())
    ShowWelcome();
  else if (WiredSettings->ConfIsDeprecated())
    {
      wxCommandEvent evt;

      AlertDialog(_("Warning"), _("Your configuration file is deprecated or empty, settings need to be set."));
    }

  // open stream, start fileconverter and co
  if (Frame->Init() != 0)
  {
    cerr << "[WIRED] Critical error, initialisation failed" << endl;
    // returning here segfault or what?
    return (false);
  }

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

void MainApp::AlertDialog(const wxString& from, const wxString& msg)
{
  LOG;

  // print on stdout
  cout << "[MAINAPP] Error :" << from.mb_str() << " msg: " << msg.mb_str() << endl;

  // alert user with a dialog popup
  wxMessageDialog mdialog(MainWin, msg, from,
			  wxICON_INFORMATION, wxDefaultPosition);
  mdialog.ShowModal();
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
  AlertDialog(_("Critical error"), _("An unhandled exception has occurred, aborting"));
}

int MainApp::OnExit()
{
  LOG;

  Frame->Destroy();
  return (wxApp::OnExit());
}
