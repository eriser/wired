// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include	"MainApp.h"
#include	"MainWindow.h"
#include	"WiredVideo.h"

#if wxUSE_MEDIACTRL

WiredVideo::WiredVideo()
  : wxMediaCtrl( )
{
  _created = false;
}

WiredVideo::~WiredVideo()
{
  
}

bool		WiredVideo::OpenFile()
{
  wxFileDialog	*dlg;
  wxString	fileFormat;
  wxString	filePath;

  fileFormat = wxString(VIDEO_FILE_FILTER) + wxT("|All Files (*.*)|*.*");
  dlg = new wxFileDialog(MainWin,
			 wxString(wxT("[WIREDVIDEO] Loading video file"), *wxConvCurrent),
			 filePath,
			 wxString(wxT(""), *wxConvCurrent),
			 fileFormat,
			 wxOPEN);

  if (dlg->ShowModal() == wxID_OK)
    {
      // load the video player
      if ( ! _created )
	{
	  if ( Create( MainWin, wxID_ANY ) == false )
	    {
	      wxGetApp().AlertDialog(_("Wired Video"),
				     _("Can't load the video player"));
	      return false;
	    }
	  _created = true;
	}
      // load the video file
      if ( Load( filePath ) == false )
	{
	  wxGetApp().AlertDialog(_("Wired Video"),
				 _("Can't open the video file"));
	  return false;
	}
      _fileName = filePath;
    }
  return true;
}

int		WiredVideo::CloseFile()
{
  Stop();
  return 0;
}

#endif
