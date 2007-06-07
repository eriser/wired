// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _WIREDMPLAYER_H_
#define _WIREDMPLAYER_H_

#include	<stdlib.h>
#ifndef WIN32
#include	<unistd.h>
#endif
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<stdio.h>
#include    <wx/wx.h>

#define		MPLAYER_LAUNCH_STRING	wxT("mplayer -slave -quiet ")
#define		TRASH_OUT							"/dev/null"
#define		SHELL									"sh"
#define		SHELL_PARAM						"-c"
#define		CHILD_XTERM						"/bin/sh"
#define		ACTION_PAUSE					wxT("pause\n")
#define		LEN_ACTION_PAUSE			6
#define		ACTION_QUIT						wxT("quit\n")
#define		LEN_ACTION_QUIT				5
#define		ACTION_MUTE						wxT("mute\n")
#define		LEN_ACTION_MUTE				5
#define		ACTION_HIDE						wxT("hide\n")
#define		LEN_ACTION_HIDE				5
#define		ACTION_SEEK_RELATIVE		wxT("seek ")
#define		ACTION_SEEK_PERCENTAGE	wxT("seek ")
#define		ACTION_SEEK_ABSOLUTE		wxT("seek ")
#define		CRLF										wxT("\n")

enum			eSeekMethod
{
	relative = 0, 
	percentage, 
	absolute
};

class WiredMplayer
{
public:
	WiredMplayer();
	~WiredMplayer();
	
	int		PlayFile();
	int		PauseFile();
	void		StopFile();
	int		CloseFile();
	int		SeekFile(eSeekMethod seekMethod, double position);
	bool		DisplayVideoFrame(const wxString& videoFilePath);

private:
	int		pfd[2];
	bool		Playbool;
	
    int		SetPlayBool(bool value);
	int	       	MuteFile();
	int	       	SendMPlayerMessage(const wxChar* message, unsigned int msgLen);
};

#endif //_WIREDMPLAYER_H_
