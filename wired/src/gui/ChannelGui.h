// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __CHANNELGUI_H__
#define __CHANNELGUI_H__

#include <wx/wx.h>
#include <vector>

#include "Mixer.h"
#include "Channel.h"
#include "HintedFader.h"
#include "DownButton.h" 
//#include "FaderCtrl.h"
#include "VUMCtrl.h"

#define LABEL_MAXCHAR	20
#define BG		"ihm/mixer/bg.png"
#define MIXERLOCKUP	"ihm/mixer/mixer_lock_up.png"
#define MIXERLOCKDOWN	"ihm/mixer/mixer_lock_down.png"
#define MIXERHPUP	"ihm/mixer/mixer_hp_up.png"
#define MIXERHPDOWN	"ihm/mixer/mixer_hp_down.png"
#define VUM_GREEN	"ihm/widgets/vum_green.png"
#define VUM_ORANGE	"ihm/widgets/vum_orange.png"
#define VUM_RED		"ihm/widgets/vum_red.png"

class SeqTrack;

class ChannelGui : public wxPanel
{
 public:
  ChannelGui( Channel *channel, wxImage* img_bg, wxImage* img_fg,
	      wxWindow* parent, wxWindowID id,
	      const wxPoint& pos, const wxSize& size,
	      const wxString& label);
  ~ChannelGui();
  
  void OnFaderLeft(wxScrollEvent &e);
  void OnFaderRight(wxScrollEvent &e);
  void OnLock(wxCommandEvent& e);
  void OnMuteLeft(wxCommandEvent& e);
  void OnMuteRight(wxCommandEvent& e);
  void OnPaint(wxPaintEvent& e);
  
  void SetLabel(const wxString&);
  void SetOpt(SeqTrack*);
  void UpdateScreen();
  
  SeqTrack*	ConnectedSeqTrack;
  HintedFader*	FaderLeft;
  HintedFader*	FaderRight;
  
  VUMCtrl*	VumLeft;
  VUMCtrl*	VumRight;
  
  Channel*	Chan;
  bool		Stereo;
  
 protected:
  bool		Lock;
  DownButton*	MuteLeftButton;
  DownButton*	MuteRightButton;
  DownButton*	LockButton;
  wxStaticText*	Label;
  wxStaticText*	VolumeLeft;
  wxStaticText*	VolumeRight;
  
  //to delete
  wxImage*	hp_up;
  wxImage*	hp_dn;
  wxImage*	lock_up;
  wxImage*	lock_dn;
  wxImage*	ImgFaderBg;
  wxImage*	ImgFaderFg;
  wxBitmap*	MixerBmp;
  wxImage*	bg;

DECLARE_EVENT_TABLE()
};

class MasterChannelGui : public ChannelGui
{
 public:
  MasterChannelGui( Channel *channel, wxImage* img_bg, wxImage* img_fg,
		    wxWindow* parent, wxWindowID id,
		    const wxPoint& pos, const wxSize& size );
  ~MasterChannelGui();
  
  void OnFaderLeft(wxScrollEvent &e);
  void OnFaderRight(wxScrollEvent &e);
  void OnMuteLeft(wxCommandEvent& e);
  void OnMuteRight(wxCommandEvent& e);
  void OnLock(wxCommandEvent& e);
 private:
  
  DECLARE_EVENT_TABLE()
};

enum
{
  FaderLeftId = 100000,
  FaderRightId,
  MuteLeftId,
  MuteRightId,
  LockId
};

#endif//__CHANNELGUI_H__




