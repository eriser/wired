// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MIXERGUI_H__
#define __MIXERGUI_H__

#include <wx/wx.h>
#include "FaderCtrl.h"
#include "VUMCtrl.h"
#include "ChannelGui.h"
#include "Track.h"

#define ID_MIXER_REFRESH	101020
#define TYPE_MIXER_REFRESH	9876600

#define CHANNELGUI_WIDTH	100
#define CHANNELGUI_HEIGHT	131

#define FADERFG "ihm/mixer/fader_fg.png"
#define FADERBG "ihm/mixer/fader_bg.png"
#define MIXERLOCKUP "ihm/mixer/mixer_lock_up.png"
#define MIXERLOCKDOWN "ihm/mixer/mixer_lock_down.png"
#define MIXERHPUP "ihm/mixer/mixer_hp_up.png"
#define MIXERHPDOWN "ihm/mixer/mixer_hp_down.png"

DEFINE_EVENT_TYPE(wxMixerRefresh)

class MixerGui : public wxScrolledWindow
{
 public:
  MixerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size);
  ~MixerGui();
  
  void OnScroll(wxScrollEvent &event);
  void OnMasterChange(wxCommandEvent &event);
  void AddChannel(Channel *);
  void RemoveChannel(Channel *);
  float	  MasterLeft;
  float	  MasterRight;
  void AddMasterChannel(Channel *channel);
  void SetLabelByChan(Channel*, const wxString&);
  void SetChanOpt(Track*);
  ChannelGui* GetGuiByChan(Channel*);
 protected:
  /* Master Channel stuff */
  VUMCtrl *vuMasterLeft;
  VUMCtrl *vuMasterRight;
  FaderCtrl *MasterLeftFader;
  FaderCtrl *MasterRightFader;
  
  wxImage *ImgFaderBg;
  wxImage *ImgFaderFg;
  wxImage *ImgLockUp;
  wxImage *ImgLockDown;
  wxImage *ImgHpUp;
  wxImage *ImgHpDown;

  vector<ChannelGui*> ChannelGuiVector;  
  
  void		UpdateChannelsPos(void);
  
    
  DECLARE_EVENT_TABLE()
};

enum
{
  MixerGui_Left = 1,
  MixerGui_Right
};

extern MixerGui *MixerPanel;

#endif
