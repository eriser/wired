// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/splash.h>
#include "MainApp.h"
#include "MainWindow.h"

//void*	operator new(size_t size)
//{
//	write(1, "new\n", 4);
//	return (malloc(size));
//}
//
//void	operator delete(void *ptr)
//{
//	write(1, "delete\n", 7);
//	free(ptr);
//}
//
//void*	operator new[](size_t size, int index)
//{
//	write(1, "new[]\n", 6);
//	return (malloc(size * index));
//}
//
//void	operator delete[](void *ptr)
//{
//	write(1, "delete[]\n", 9);
//	free(ptr);
//}


IMPLEMENT_APP(MainApp)

MainWindow			*MainWin;

bool				MainApp::OnInit()
{
  wxBitmap			bitmap;
#if wxUSE_LIBPNG
  wxImage::AddHandler(new wxPNGHandler);
#endif
  if (bitmap.LoadFile("/usr/local/share/wired/data/ihm/splash/splash.png", wxBITMAP_TYPE_PNG))
    {
      wxSplashScreen*		splash = 
	new wxSplashScreen(bitmap,
			   wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
			   6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
			   wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    }
#if 0
  const wxString		name = wxString::Format("wired-%s", wxGetUserId().c_str());
  Checker = new wxSingleInstanceChecker(name);
  if (Checker->IsAnotherRunning())
    {
      cout << "Another Wired is already running, aborting." << endl;      
      return (false);
    }
#endif
  SetUseBestVisual(true);
  Frame = new MainWindow(APP_TITLE, wxDefaultPosition,
			 wxSize(APP_WIDTH, APP_HEIGHT));
  MainWin = Frame;
  Frame->Show(true);
  SetTopWindow(Frame);
  return (true);
}

int				MainApp::OnExit()
{
#if 0
  delete Checker;
#endif
  return (0);
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