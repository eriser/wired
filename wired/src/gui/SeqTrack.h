// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __SEQTRACK_H__
#define __SEQTRACK_H__

using namespace std;

#include <list>
#include <wx/wx.h>
#include "ChoiceButton.h"
#include "Plugin.h"
#include "Rack.h"
#include "DownButton.h"
#include "VUMCtrl.h"
//#include "MixerGui.h"

#define NONE_SELECTED_ID	999

#define VUM_GREEN		"ihm/widgets/vum_green.png"
#define VUM_ORANGE		"ihm/widgets/vum_orange.png"
#define VUM_RED			"ihm/widgets/vum_red.png"
#define REC_UP			"ihm/seqtrack/rec_up.png"
#define REC_DOWN		"ihm/seqtrack/rec_down.png"
#define MUTE_UP			"ihm/seqtrack/mute_up.png"
#define MUTE_DOWN		"ihm/seqtrack/mute_down.png"

class ChannelGui;

class SeqTrack: public wxControl
{
 public:
  SeqTrack(long index, wxWindow *parent, const wxPoint& pos, 
	   const wxSize& size, bool audio = true);
  ~SeqTrack();

  void					OnConnectTo(wxCommandEvent &event);
  void					OnConnectSelected(wxCommandEvent &event);
  void					ConnectTo(Plugin *plug);
  void					OnPaint(wxPaintEvent &event);
  void					OnClick(wxCommandEvent &event);
  void					OnMouseClick(wxMouseEvent &event);
  void					OnDeviceChoice(wxCommandEvent &event);
  void					FillChoices();
  void					OnRecordClick(wxCommandEvent &event);
  void					OnMuteClick(wxCommandEvent &event);
  void					OnConnectToHelp(wxMouseEvent &event);
  void					OnDeviceHelp(wxMouseEvent &event);

  void					SetSelected(bool sel);
  bool					GetSelected() { return Selected; }
  void					SetVuValue(long value);
  void					SetRecording(bool rec);
  void					SetMute(bool mut);
  void					SetDeviceId(long devid);
  void					OnNameChange(wxCommandEvent& event);

  long					Index;

  bool					Record;
  bool					Mute;

  Plugin				*Connected;
  RackTrack				*ConnectedRackTrack;
  long					DeviceId;
  bool					IsAudio;

  wxTextCtrl				*Text;
  
  ChannelGui*				ChanGui;
  void					SetName(const wxString&);

 protected:
  ChoiceButton				*Image;
  wxMenu				*menu;
  DownButton				*RecBtn;
  DownButton				*MuteBtn;
  wxChoice				*DeviceBox;
  VUMCtrl				*Vu;
  
  bool					Selected;

  DECLARE_EVENT_TABLE()
};

enum
{
  SeqTrack_ConnectTo = 1742,
  SeqTrack_OnClick,
  SeqTrack_ConnectSelected,
  SeqTrack_DeviceChoice,
  SeqTrack_Record,
  SeqTrack_Mute,
  SeqTrack_OnNameChange
};

extern int AudioTrackCount;
extern int MidiTrackCount;

#endif
