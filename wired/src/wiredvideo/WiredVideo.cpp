#include	"WiredVideo.h"
#include	"wx/wx.h"
#include	<unistd.h>

WiredVideo::WiredVideo()
{
	videoFilePath = "";
	asFile = false;
	isDisplayed = false;
    mplayer = NULL;
}

WiredVideo::~WiredVideo()
{
	if (mplayer)   
	  delete mplayer;
}

int		WiredVideo::InitMplayer()
{
//    if (mplayer)
//        delete mplayer;
    mplayer = new WiredMplayer();
}

int		WiredVideo::OpenFile(const std::string& path)
{
  wxFileDialog	*dlg = new wxFileDialog(0, "[WIREDVIDEO] Loading video file", path, "", VIDEO_FILE_FILTER, wxOPEN);

  if (dlg->ShowModal() == wxID_OK)
	{
		if (videoFilePath != "") CloseFile();
		videoFilePath = dlg->GetPath();
		InitMplayer();
		asFile = true;
  }
	DisplayVideoFrame();
}

int		WiredVideo::SetSeek(bool SeekBool)
{

}

int		WiredVideo::PlayFile()
{
	if (asFile == false || !mplayer) return 0;
	std::cout << "[WIREDVIDEO] Playing video file" << std::endl;
	return mplayer->PlayFile();
}

int		WiredVideo::StopFile()
{
	if (asFile == false || !mplayer) return 0;
	std::cout << "[WIREDVIDEO] Video file stopped" << std::endl;
	return mplayer->StopFile();
}

int		WiredVideo::PauseFile()
{
	if (asFile == false || !mplayer) return 0;
	std::cout << "[WIREDVIDEO] Video file paused" << std::endl;
	return mplayer->PauseFile();
}

int		WiredVideo::CloseFile()
{
	if (asFile == false || !mplayer) return 0;
	isDisplayed = false;
	std::cout << "[WIREDVIDEO] Closing video file" << std::endl;
	videoFilePath = "";
	asFile = false;
	return mplayer->CloseFile();
}

int		WiredVideo::SeekFile(eSeekMethod seekMethod, double position)
{
	if (asFile == false || !mplayer) return 0;
	std::cout << "[WIREDVIDEO] Seeking video file with method " << seekMethod << " at " << position << std::endl;
	  return mplayer->SeekFile(seekMethod, position);
}

bool	WiredVideo::DisplayVideoFrame()
{
	if (asFile == false || !mplayer) return 0;
	isDisplayed = mplayer->DisplayVideoFrame(videoFilePath);
	std::cout << "[WIREDVIDEO] Displaying video file" << std::endl;
	return isDisplayed;
}
