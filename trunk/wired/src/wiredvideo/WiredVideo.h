// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef		_WIREDVIDEO_
#define		_WIREDVIDEO_

#include <wx/wx.h>
#include <wx/mediactrl.h>

#if wxUSE_MEDIACTRL

// wxMediaCtrl implementation

#define	WIRED_VIDEO_NAME _("Wired Video")
#define	VIDEO_FILE_FILTER _("AVI files (*.avi)|*.avi|MPEG files (*.mpg)|*.mpg")

class		WiredVideo : public wxMediaCtrl
{
 private:
  bool		_created;
  wxString	_fileName;

 public:
  WiredVideo();
  ~WiredVideo();

  bool		OpenFile();
  int		CloseFile();
};

#else
// we use a dummy class if wxMediaCtrl is disabled

class		WiredVideo
{
 private:
 public:
  WiredVideo(){};
  ~WiredVideo(){};

  inline bool		OpenFile(){ return false; };
  inline int		CloseFile(){ return 0; };
  inline bool		Play(){ return false; };
  inline bool		Stop(){ return false; };
};

#endif

extern WiredVideo			*WiredVideoObject;

#endif //_WIREDVIDEO_
