#ifndef _WIREDMPLAYER_H_
#define _WIREDMPLAYER_H_

#include	<iostream>
#include	<string>
#include	<vector>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>

#define		MPLAYER_LAUNCH_STRING	"mplayer -slave -quiet "
#define		TRASH_OUT							"/dev/null"
#define		SHELL									"sh"
#define		SHELL_PARAM						"-c"
#define		CHILD_XTERM						"/bin/sh"
#define		ACTION_PAUSE					"pause\n"
#define		LEN_ACTION_PAUSE			6
#define		ACTION_QUIT						"quit\n"
#define		LEN_ACTION_QUIT				5
#define		ACTION_MUTE						"mute\n"
#define		LEN_ACTION_MUTE				5
#define		ACTION_SEEK_RELATIVE		"seek "
#define		ACTION_SEEK_PERCENTAGE	"seek "
#define		ACTION_SEEK_ABSOLUTE		"seek "
#define		CRLF										"\n"

using namespace std;

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
	int		StopFile();
	int		CloseFile();
	int		SeekFile(eSeekMethod seekMethod, double position);
	bool	DisplayVideoFrame(const string& videoFilePath);
	
private:
	int		pfd[2];
	
	int			MuteFile();
	int			SendMPlayerMessage(const char* message, unsigned int msgLen);
};

#endif //_WIREDMPLAYER_H_
