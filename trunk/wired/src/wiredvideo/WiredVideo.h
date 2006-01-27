#ifndef		_WIREDVIDEO_
#define		_WIREDVIDEO_

#include	<iostream>
#include	<string>
#include	<vector>
#include	<stdlib.h>
#include	"WiredMplayer.h"

#define		VIDEO_FILE_FILTER		"AVI files (*.avi)|*.avi|MPEG files (*.mpg)|*.mpg"

class			WiredVideo
{
public:
	WiredVideo();
	~WiredVideo();

	bool	asFile;
	bool	isDisplayed;
	
	int		OpenFile(const std::string& path = "");
	int		PlayFile();
	int		PauseFile();
	int		StopFile();
	int		CloseFile();
	int		SeekFile(eSeekMethod seekMethod, double position);
	int		SetSeek(bool SeekBool);

private:
	std::string	videoFilePath;
	
	bool	DisplayVideoFrame();
	int		InitMplayer();
	WiredMplayer	*mplayer;
};

extern WiredVideo			*WiredVideoObject;

#endif //_WIREDVIDEO_
