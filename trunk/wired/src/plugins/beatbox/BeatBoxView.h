#ifndef __BEATBOXVIEW_H__
#define __BEATBOXVIEW_H__

#include <wx/wx.h>
#include "WaveFile.h"
#include "WaveEnv.h"

class WiredBeatBox;

class BeatBoxTrack : public wxWindow
{
 public:
  BeatBoxTrack(wxWindow *parent, const wxPoint &pos, const wxSize &size);
  ~BeatBoxTrack();  
 protected:

DECLARE_EVENT_TABLE()
};

class BeatBoxScrollView : public wxScrolledWindow
{
 public:
  BeatBoxScrollView(wxWindow *parent, const wxPoint &pos, const wxSize &size);
  ~BeatBoxScrollView();
 protected:

DECLARE_EVENT_TABLE()
};

class BeatBoxView : public wxPanel
{
 public:
  BeatBoxView(wxWindow* parent, WiredBeatBox* bb, 
	      const wxPoint& pos, const wxSize& size, 
	      wxMutex* mutex);
  ~BeatBoxView();
  
  WiredBeatBox* DRM31;
 protected:
  wxMutex*	Mutex;
  
DECLARE_EVENT_TABLE()
};

enum
  {
    BeatBoxView_Env = 1
  };

#endif//__BEATBOXVIEW_H__
