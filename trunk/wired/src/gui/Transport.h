// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include <wx/wx.h>

#define TRANSPORT_BACKGR_IMG	"ihm/player/bg.png"
#define TRANSPORT_PLAYUP_IMG	"ihm/player/play_up.png"
#define TRANSPORT_PLAYDO_IMG	"ihm/player/play_down.png"
#define TRANSPORT_STOPUP_IMG	"ihm/player/stop_up.png"
#define TRANSPORT_STOPDO_IMG	"ihm/player/stop_down.png"
#define TRANSPORT_REC_UP_IMG	"ihm/player/rec_up.png"
#define TRANSPORT_REC_DO_IMG	"ihm/player/rec_down.png"
#define TRANSPORT_BAC_UP_IMG	"ihm/player/rwd_up.png"
#define TRANSPORT_BAC_DO_IMG	"ihm/player/rwd_down.png"
#define TRANSPORT_FOR_UP_IMG	"ihm/player/fwd_up.png"
#define TRANSPORT_FOR_DO_IMG	"ihm/player/fwd_down.png"
#define TRANSPORT_LOOPUP_IMG	"ihm/player/btn_red.png"
#define TRANSPORT_LOOPDO_IMG	"ihm/player/btn_green.png"
#define TRANSPORT_UPUP_IMG	"ihm/player/up_up.png"
#define TRANSPORT_UPDO_IMG	"ihm/player/up_down.png"
#define TRANSPORT_DOWNUP_IMG	"ihm/player/down_up.png"
#define TRANSPORT_DOWNDO_IMG	"ihm/player/down_down.png"
#define TRANSPORT_CLICKUP_IMG	"ihm/player/btn_red.png"
#define TRANSPORT_CLICKDO_IMG	"ihm/player/btn_green.png"

class				MainWindow;
class				DownButton;
class				HoldButton;
class				FaderCtrl;
class				StaticLabel;
class				VUMCtrl;

class				Transport : public wxPanel
{
 public:
  Transport(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style);
  ~Transport();

  void				SetPlayPosition(double pos);
  void				SetBpm(float bpm);
  void				SetSigNumerator(int n);
  void				SetSigDenominator(int d);
  void				SetLoop(bool loop);
  void				SetClick(bool click);

  void				OnPlay(wxCommandEvent &event);
  void				OnStop(wxCommandEvent &event);
  void				OnRecord(wxCommandEvent &event);
  void				OnLoop(wxCommandEvent &event);
  void				OnMetronome(wxCommandEvent &event);
  void				OnBackward(wxCommandEvent &event);
  void				OnForward(wxCommandEvent &event);
  void				OnBpmClick(wxCommandEvent &event);
  void				OnBpmEnter(wxCommandEvent &event);
  void				OnBpmUp(wxCommandEvent &event);
  void				OnBpmDown(wxCommandEvent &event);
  void				OnSigNumUp(wxCommandEvent &event);
  void				OnSigNumDown(wxCommandEvent &event);
  void				OnSigDenUp(wxCommandEvent &event);
  void				OnSigDenDown(wxCommandEvent &event);

  void				OnLoopHelp(wxMouseEvent &event);
  void				OnClickHelp(wxMouseEvent &event);

  void				OnPaint(wxPaintEvent &event);

 protected:
  friend class			MainWindow;

  VUMCtrl			*vum;
  DownButton			*PlayBtn;
  DownButton			*StopBtn;
  DownButton			*RecordBtn;
  HoldButton			*BackwardBtn;
  HoldButton			*ForwardBtn;
  DownButton			*LoopBtn;
  DownButton			*ClickBtn;

  HoldButton			*BpmUpBtn;
  HoldButton			*BpmDownBtn;
  HoldButton			*SigNumUpBtn;
  HoldButton			*SigNumDownBtn;
  HoldButton			*SigDenUpBtn;
  HoldButton			*SigDenDownBtn;
  
  wxBitmap			*TrBmp;

  wxStaticText			*MesLabel;
  wxStaticText			*SigLabel;
  wxStaticText			*MilliSigLabel;
  StaticLabel			*BpmLabel;
  wxStaticText			*SigNumLabel;
  wxStaticText			*SigDenLabel;

  wxTextCtrl			*BpmText;
  
  DECLARE_EVENT_TABLE()
};

enum
{
  Transport_Play = 1,
  Transport_Stop,
  Transport_Record,
  Transport_Backward,
  Transport_Forward,
  Transport_Loop,
  Transport_BpmDown,
  Transport_BpmUp,
  Transport_SigNumDown,
  Transport_SigNumUp,
  Transport_SigDenDown,
  Transport_SigDenUp,
  Transport_Click,
  Transport_BpmClick,
  Transport_BpmEnter
};

extern Transport		*TransportPanel;

#endif
