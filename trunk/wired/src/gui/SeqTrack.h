// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SEQTRACK_H__
#define __SEQTRACK_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "RackTrack.h"
#include "Track.h"
#include "WiredDocument.h"

#define NONE_SELECTED_ID		(999)

#define VUM_GREEN			L"/ihm/widgets/vum_green.png"
#define VUM_ORANGE			L"/ihm/widgets/vum_orange.png"
#define VUM_RED				L"/ihm/widgets/vum_red.png"
#define REC_UP				L"/ihm/seqtrack/rec_up.png"
#define REC_DOWN			L"/ihm/seqtrack/rec_down.png"
#define MUTE_UP				L"/ihm/seqtrack/mute_up.png"
#define MUTE_DOWN			L"/ihm/seqtrack/mute_down.png"
#define UNASSIGNED			L"/ihm/seqtrack/unassigned.png"
#define CAN_ASSIGN			L"/ihm/seqtrack/can_assign.png"

class					ChannelGui;
class					Plugin;
class					RackTrack;
class					ChoiceButton;
class					DownButton;
class					VUMCtrl;

class					SeqTrack: public wxControl,
						  public WiredDocument
{
 public:
  SeqTrack(long index, wxWindow *winParent, const wxPoint& pos,
	   const wxSize& size, trackType type, WiredDocument* docParent);
  ~SeqTrack();

  inline void				SetChannelGui(ChannelGui* chan)
  { ChanGui = chan; };

  void					PropagateEvent(wxEvent &event);
  void					RebuildConnectList();
  void					OnConnectTo(wxCommandEvent &event);
  void					OnConnectSelected(wxCommandEvent &event);
  void					ConnectTo(Plugin *plug);
  void					RemoveReferenceTo(Plugin *plug);
  void					OnPaint(wxPaintEvent &event);
  void					OnMouseClick(wxMouseEvent &e);
  void					OnDeviceChoice(wxCommandEvent &event);
  void					FillChoices();
  void					OnRecordClick(wxCommandEvent &event);
  void					OnMuteClick(wxCommandEvent &event);
  void					OnConnectToHelp(wxMouseEvent &event);
  void					OnDeviceHelp(wxMouseEvent &event);
  void					SetSelected(bool sel);
  bool					GetSelected() { return Selected; }
  void					SetVuValue(long value);
  void					SetVuValue();
  void					SetRecording(bool rec);
  void					SetMute(bool mut);
  bool					GetMute();
  void					SetDeviceId(long devid);
  void					OnNameChange(wxCommandEvent& event);
  void					SetName(const wxString&);

  // WiredDocument implementation
  void					Save();
  void					Load(SaveElementArray data);

  long					DeviceId;
  long					Index;
  bool					IsAudio;
  bool					Record;
  bool					Mute;
  wxTextCtrl				*Text;
  wxBitmap				*UnassignedBmp;
  wxBitmap				*CanAssignBmp;
  ChannelGui*				ChanGui;
  Plugin				*Connected;
  RackTrack				*ConnectedRackTrack;
  int					VuValue;
  trackType				Type;

 protected:
  void					OnMotion(wxMouseEvent &e);

  ChoiceButton				*Image;
  wxMenu				*menu;
  DownButton				*RecBtn;
  DownButton				*MuteBtn;
  wxChoice				*DeviceBox;
  VUMCtrl				*Vu;
  bool					Selected;
  wxPoint				m_click;

 private:
  wxStaticBitmap*			trackTypeStatic;

  void					SelectTrack();

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

extern int				AudioTrackCount;
extern int				MidiTrackCount;

#endif
