// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef		_WIREDVIDEO_
#define		_WIREDVIDEO_

#include	<iostream>
#include	<string>
#include	<vector>
#include	<stdlib.h>
#include	"WiredMplayer.h"
#include <wx/wx.h>

#define		VIDEO_FILE_FILTER		wxT("AVI files (*.avi)|*.avi|MPEG files (*.mpg)|*.mpg")

class			WiredVideo
{
public:
	WiredVideo();
	~WiredVideo();

	bool	asFile;
	bool	isDisplayed;
	
	int		OpenFile(const wxString& path = wxT(""));
	int		PlayFile();
	int		PauseFile();
	int		StopFile();
	int		CloseFile();
	int		SeekFile(eSeekMethod seekMethod, double position);
	int		SetSeek(bool SeekBool);

private:
	wxString	videoFilePath;
	
	bool	DisplayVideoFrame();
	int		InitMplayer();
	WiredMplayer	*mplayer;
};

extern WiredVideo			*WiredVideoObject;

#endif //_WIREDVIDEO_
