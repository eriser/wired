#ifndef __BEATBOXVIEW_H__
#define __BEATBOXVIEW_H__

#include <wx/wx.h>
#include "WaveFile.h"
#include "WaveEnv.h"

class WiredBeatBox;

class BeatBoxView : public wxPanel
{
 public:
  BeatBoxView(wxWindow* parent, WiredBeatBox* bb, 
	      const wxPoint& pos, const wxSize& size, 
	      wxMutex* mutex);
  ~BeatBoxView();
  
  void SetWaveFile(WaveFile* w);
  WiredBeatBox* BB;
 protected:
  WaveEnv*	Env;
  WaveFile*	Wave;
  wxMutex*	Mutex;
  
DECLARE_EVENT_TABLE()
};

enum
  {
    BeatBoxView_Env = 1
  };

#endif//__BEATBOXVIEW_H__
