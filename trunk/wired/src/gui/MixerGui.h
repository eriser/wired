// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
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
#define FADERFG					L"/ihm/mixer/mixer_fader.png"
#define FADERBG					L"/ihm/mixer/mixer_fader_bg.png"
#define MIXERLOCKUP				L"/ihm/mixer/mixer_lock_up.png"
#define MIXERLOCKDOWN				L"/ihm/mixer/mixer_lock_down.png"
#define MIXERHPUP				L"/ihm/mixer/mixer_mute_up.png"
#define MIXERHPDOWN				L"/ihm/mixer/mixer_mute_down.png"

class						Channel;
class						ChannelGui;
class						VUMCtrl;
class						FaderCtrl;

/**
 * The MixerGui handle the display of the mixer in wired.
 * It is derived from the wxScrolledWindow class
*/
class						MixerGui : public wxScrolledWindow
{
 public:
  /**
   * This is the default constructor for MixerGui class derived from wxScrolledWindow
   */
  MixerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size);
  /**
   * This is the default destructor for MixerGui class
   */
  ~MixerGui();
  /**
   * OnScroll Function. Not implemented yet
   * \param event, a wxScrollEvent&. Normal wx event handling.
   * \return void
   */
  void						OnScroll(wxScrollEvent &event);
  /**
   * OnMaterChange function, called when changin the master volume using the volume controler
   * \param event, a wxCommandEvent. Normal wx event handling
   * \return void
   */
  void						OnMasterChange(wxCommandEvent &event);
  /**
   * RemoveChannel function, called when removing a channel from the channel list
   * \param channel, a Channel *
   * \return void
   */
  void						RemoveChannel(Channel *);
  /**
   * RemoveChannel function, called when removing a channel from the channel list
   * \param gui, a ChannelGui *
   * \return void
   */
  void						RemoveChannel(ChannelGui *);
  /**
   * MasterLeft, a float that hold the volume level for the left master channel
   */
  float						MasterLeft;
  /**
   * MasterRight, a float that hold the volume level for the Right master channel
   */
  float						MasterRight;
  /**
   * AddMasterChannel function. This function adds a new MasterChannelGui and displays it.
   * \param Channel, a Channel *
   * \return void
   */
  void						AddMasterChannel(Channel *channel);
  /**
   * SetLabelByChan function. This function is used to set a new label to an existing channel
   * \param Channel, a Channel *. The channel to update
   * \param label, a wxString&. The new label to apply
   * \return void
   */
  void						SetLabelByChan(Channel*, const wxString&);
  /**
   * AddChannel function, called when adding a new channel to the mixer Gui
   * \param Channel, a Channel *. The channel to add
   * \param label, a wxString&. The label of the new channel
   * \return void
   */
  ChannelGui*					AddChannel(Channel *, const wxString&);
  /**
   * GetGuiByChan function. This function browse the ChannelGuiVector and return a ChannelGui*
   * Corresponding to the Channel * parameter.
   * \param Channel, a Channel *
   * \return a ChannelGui *
   */
  ChannelGui*					GetGuiByChan(Channel*);
 protected:
  /**
   * UpdateChannelsPos function. This function update the channels positions on the gui
   * \return void
   */
  void						UpdateChannelsPos(void);
  /**
   * vuMasterLeft a VUMCtrl for left volume control
   */
  VUMCtrl					*vuMasterLeft;
  /**
   * vuMasterRight a VUMCtrl for right volume control
   */
  VUMCtrl					*vuMasterRight;
  /**
   * ImgFaderBg a wxImage*
   */
  wxImage					*ImgFaderBg;
  /**
   * ImgFaderFg a wxImage*
   */
  wxImage					*ImgFaderFg;
  /**
   * ImgLockUp a wxImage*
   */
  wxImage					*ImgLockUp;
  /**
   * ImgLockDown a wxImage*
   */
  wxImage					*ImgLockDown;
  /**
   * ImgHpUp a wxImage*
   */
  wxImage					*ImgHpUp;
  /**
   * ImgHpDown wxImage*
   */
  wxImage					*ImgHpDown;
  /**
   * ChannelGuiVector a vector holding a list a ChannelGui*
   */
  vector<ChannelGui*>				ChannelGuiVector;

  DECLARE_EVENT_TABLE()
};

/**
 * The enum table for MixerGui class
 */

enum
{
  MixerGui_Left = 1,
  MixerGui_Right
};

extern MixerGui					*MixerPanel;

DEFINE_EVENT_TYPE(wxMixerRefresh)

#endif/*__MIXERGUI_H__*/
