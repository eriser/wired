// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CHANNELGUI_H__
#define __CHANNELGUI_H__

#include <wx/wx.h>
#include <vector>
#include "../libs/WiredWidgets/src/FaderCtrl.h"
#include "../save/WiredDocument.h"

#define LABEL_MAXCHAR			(20)
#define BG				L"/ihm/mixer/mixer_bg.png"
#define MIXERLOCKUP			L"/ihm/mixer/mixer_lock_up.png"
#define MIXERLOCKDOWN			L"/ihm/mixer/mixer_lock_down.png"
#define MIXERHPUP			L"/ihm/mixer/mixer_mute_up.png"
#define MIXERHPDOWN			L"/ihm/mixer/mixer_mute_down.png"
#define VUM_GREEN			L"/ihm/widgets/vum_green.png"
#define VUM_ORANGE			L"/ihm/widgets/vum_orange.png"
#define VUM_RED				L"/ihm/widgets/vum_red.png"

class					SeqTrack;
class					Channel;
class					HintedFader;
class					VUMCtrl;
class					DownButton;


class					ChannelGui : public wxPanel, public WiredDocument
{
 public:
  ChannelGui(Channel *channel, wxImage* img_bg, wxImage* img_fg,
	     wxWindow* parent, wxWindowID id,
	     const wxPoint& pos, const wxSize& size,
	     const wxString& label, WiredDocument* docParent = NULL);
  ~ChannelGui();

  void					OnFaderLeft(wxScrollEvent &e);
  void					OnFaderRight(wxScrollEvent &e);
  void					OnLock(wxCommandEvent& e);
  void					OnMuteLeft(wxCommandEvent& e);
  void					OnMuteRight(wxCommandEvent& e);
  void					OnPaint(wxPaintEvent& e);
  void					SetOpt(SeqTrack*);
  void					UpdateScreen();

  /**
   * WiredDocument implementation
   */
  void					Load(SaveElementArray data);

  /**
   * WiredDocument implementation
   */
  void					Save();


  SeqTrack				*ConnectedSeqTrack;
  FaderCtrl				*FaderLeft;
  FaderCtrl				*FaderRight;
  VUMCtrl				*VumLeft;
  VUMCtrl				*VumRight;
  Channel				*Chan;
  bool					Stereo;

  void					SetLabel(const wxString& label);

 private:
  //Setters
  void					SetStereo(bool stereo);
  void					SetLock(bool lock);
  void					SetMuteLeftButton(bool isDown);
  void					SetMuteRightButton(bool isDown);
  void					SetLockButton(bool isDown);


 protected:
  bool					Lock;
  DownButton				*MuteLeftButton;
  DownButton				*MuteRightButton;
  DownButton				*LockButton;
  wxStaticText				*Label;
  //  wxStaticText				*VolumeLeft;
  //  wxStaticText				*VolumeRight;

  //to delete
  wxImage				*hp_up;
  wxImage				*hp_dn;
  wxImage				*lock_up;
  wxImage				*lock_dn;
  wxImage				*ImgFaderBg;
  wxImage				*ImgFaderFg;
  wxBitmap				*MixerBmp;

  DECLARE_EVENT_TABLE()
};

class					MasterChannelGui : public ChannelGui
{
 public:
  MasterChannelGui(Channel* channel, wxImage* img_bg, wxImage* img_fg,
		   wxWindow* parent, wxWindowID id,
		   const wxPoint& pos, const wxSize& size, WiredDocument* docParent = NULL);
  ~MasterChannelGui();

  void					OnFaderLeft(wxScrollEvent& e);
  void					OnFaderRight(wxScrollEvent& e);
  void					OnMuteLeft(wxCommandEvent& e);
  void					OnMuteRight(wxCommandEvent& e);
  void					OnLock(wxCommandEvent& e);

 private:
  DECLARE_EVENT_TABLE()
};

enum
  {
    FaderLeftId = 11230,
    FaderRightId,
    MuteLeftId,
    MuteRightId,
    LockId
  };

#endif//__CHANNELGUI_H__
