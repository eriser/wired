// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include	"WiredMplayer.h"

WiredMplayer::WiredMplayer()
{
  Playbool = false;
}

WiredMplayer::~WiredMplayer()
{
  SendMPlayerMessage(ACTION_QUIT, LEN_ACTION_QUIT);
  close(pfd[0]);
  close(pfd[1]);
  SetPlayBool(false);
}

int		WiredMplayer::PlayFile()
{
  return PauseFile();
}

int		WiredMplayer::PauseFile()
{
  if (Playbool == false)
    SetPlayBool(true);
  else
    SetPlayBool(false);
  return SendMPlayerMessage(ACTION_PAUSE, LEN_ACTION_PAUSE);
}

int		WiredMplayer::StopFile()
{
  //	SeekFile(absolute, 0);
  PauseFile();
}

int		WiredMplayer::CloseFile()
{
  SetPlayBool(false);
  return SendMPlayerMessage(ACTION_QUIT, LEN_ACTION_QUIT);
}

int		WiredMplayer::SeekFile(eSeekMethod seekMethod, double position)
{
	wxString	msg;
	wxString	tmpmsg;
	// We don't have asprintf on win32
	//char	*buf;
	char	buf[64];
	
	switch (seekMethod)
	{
			case relative: 
				msg = ACTION_SEEK_RELATIVE;
				//asprintf(&buf, "%g 0", position);
				sprintf(buf, "%g 0", position);
				msg += wxString(buf, *wxConvCurrent);
				msg += CRLF;
				cout << "[WIREDPLAYER] " << msg << endl;
				break;
			case percentage:
				msg = ACTION_SEEK_PERCENTAGE;
				//asprintf(&buf, "%g 1", position);
				sprintf(buf, "%g 1", position);
				msg += wxString(buf, *wxConvCurrent);
				msg += CRLF;
				cout << "[WIREDPLAYER] " << msg << endl;
				break;
			case absolute:
				msg = ACTION_SEEK_ABSOLUTE;
				//asprintf(&buf, "%g 2", position);
				sprintf(buf, "%g 2", position);
				tmpmsg = wxString(buf, *wxConvCurrent);
				msg += tmpmsg;
				msg += CRLF;
				cout << "[WIREDPLAYER] Pos: " << buf << endl;
				cout << "[WIREDPLAYER] Msg: " << msg << endl;
				break;
	}
	//free(buf);
	if (Playbool == false)
	  {
	    SendMPlayerMessage(msg.c_str(), msg.size());
	    SetPlayBool(true);
	    return PauseFile();
	  }
	else
	  return SendMPlayerMessage(msg.c_str(), msg.size());
}

bool		WiredMplayer::DisplayVideoFrame(const wxString& videoFilePath)
{
#ifndef WIN32
  extern char **environ;
  int	fd;
  wxString	launchString;
  pid_t		cpid;

  if (pipe(pfd) == -1)
  {
    cout << "[WIREDMPLAYER] Pipe error" << endl;
    return 0;
  }

  launchString = MPLAYER_LAUNCH_STRING + videoFilePath;
  char *argv[4];
  argv[0] = (char*)SHELL;
  argv[1] = (char*)SHELL_PARAM;
  argv[2] = strdup(launchString.mb_str(*wxConvCurrent));
  argv[3] = 0;
  cpid = fork();
  if (cpid == -1)
  {
    cout << "[WIREDMPLAYER] Fork error" << endl;
    return 0;
  }
  if (cpid == 0)
    {
      close(pfd[1]);
      dup2(pfd[0], 0);
      fd = open(TRASH_OUT, O_WRONLY);
      dup2(fd, 1);
      dup2(fd, 2);
      execve(CHILD_XTERM, argv, environ);
    }
  else
    close(pfd[0]);
  MuteFile();
  PauseFile();
  //  SetPlayBool(false);
  return 1;
#else
  cout << "[WIREDMPLAYER] not implemented yet" << endl;
  return 0;
#endif
}

int			WiredMplayer::SetPlayBool(bool value)
{
  Playbool = value;
  return 1;
}

int			WiredMplayer::MuteFile()
{
	return SendMPlayerMessage(ACTION_MUTE, LEN_ACTION_MUTE);
}

int			WiredMplayer::SendMPlayerMessage(const wxChar* message, unsigned int msgLen)
{
    if ((write(pfd[1], message, msgLen)) <=  0)
      {
	cout << "[WIREDMPLAYER] Can't " << message << endl;
	return 0;
      }
  //  else
  //  	wait();
  return 1;
}
