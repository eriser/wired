// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MIXERGUI_H__
#define __MIXERGUI_H__

using namespace					std;

#include <vector>
#include <wx/wx.h>

#define ID_MIXER_REFRESH			(101020)
#define TYPE_MIXER_REFRESH			(9876600)
#define CHANNELGUI_WIDTH			(100)
#define CHANNELGUI_HEIGHT			(120)
#define FADERFG					"ihm/mixer/mixer_fader.png"
#define FADERBG					"ihm/mixer/mixer_fader_bg.png"
#define MIXERLOCKUP				"ihm/mixer/mixer_lock_up.png"
#define MIXERLOCKDOWN				"ihm/mixer/mixer_lock_down.png"
#define MIXERHPUP				"ihm/mixer/mixer_mute_up.png"
#define MIXERHPDOWN				"ihm/mixer/mixer_mute_down.png"

class						Channel;
class						ChannelGui;
class						VUMCtrl;
class						FaderCtrl;

class						MixerGui : public wxScrolledWindow
{
 public:
  MixerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size);
  ~MixerGui();
  
  void						OnScroll(wxScrollEvent &event);
  void						OnMasterChange(wxCommandEvent &event);
  void						RemoveChannel(Channel *);
  void						RemoveChannel(ChannelGui *);
  float						MasterLeft;
  float						MasterRight;
  void						AddMasterChannel(Channel *channel);
  void						SetLabelByChan(Channel*, const wxString&);
  ChannelGui*					AddChannel(Channel *, const wxString&);
  ChannelGui*					GetGuiByChan(Channel*);
 protected:
  /* Master Channel stuff */
  void						UpdateChannelsPos(void);

  VUMCtrl					*vuMasterLeft;
  VUMCtrl					*vuMasterRight;
  FaderCtrl					*MasterLeftFader;
  FaderCtrl					*MasterRightFader;
  wxImage					*ImgFaderBg;
  wxImage					*ImgFaderFg;
  wxImage					*ImgLockUp;
  wxImage					*ImgLockDown;
  wxImage					*ImgHpUp;
  wxImage					*ImgHpDown;
  vector<ChannelGui*>				ChannelGuiVector;  
  
  DECLARE_EVENT_TABLE()
};

enum
{
  MixerGui_Left = 1,
  MixerGui_Right
};

extern MixerGui					*MixerPanel;

DEFINE_EVENT_TYPE(wxMixerRefresh)

#endif/*__MIXERGUI_H__*/
