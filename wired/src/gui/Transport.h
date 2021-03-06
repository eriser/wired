// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include <wx/wx.h>
#include <wx/gauge.h>
#include "WiredDocument.h"
#include "TransparentStaticText.h"

#define TRANSPORT_BACKGR_IMG	L"/ihm/player/tr_window_bg.png"
#define TRANSPORT_BACKGR_LOOP_IMG	L"/ihm/player/tr_window_bg_loop.png"
#define TRANSPORT_PLAYUP_IMG	L"/ihm/player/tr_window_play_up2.png"
#define TRANSPORT_PLAYDO_IMG	L"/ihm/player/tr_window_play_up.png"
#define TRANSPORT_STOPUP_IMG	L"/ihm/player/tr_window_stop_up.png"
#define TRANSPORT_STOPDO_IMG	L"/ihm/player/tr_window_stop_down.png"
#define TRANSPORT_REC_UP_IMG	L"/ihm/player/tr_window_rec_up.png"
#define TRANSPORT_REC_DO_IMG	L"/ihm/player/tr_window_rec_down.png"
#define TRANSPORT_BAC_UP_IMG	L"/ihm/player/tr_window_prev_up.png"
#define TRANSPORT_BAC_DO_IMG	L"/ihm/player/tr_window_prev_down.png"
#define TRANSPORT_FOR_UP_IMG	L"/ihm/player/tr_window_next_up.png"
#define TRANSPORT_FOR_DO_IMG	L"/ihm/player/tr_window_next_down.png"
#define TRANSPORT_LOOPUP_IMG	L"/ihm/player/tr_window_minibutton_up.png"
#define TRANSPORT_LOOPDO_IMG	L"/ihm/player/tr_window_minibutton_dwn.png"
#define TRANSPORT_UPUP_IMG	L"/ihm/player/trwindow_button_up.png"
#define TRANSPORT_UPDO_IMG	L"/ihm/player/trwindow_button_up_down.png"
#define TRANSPORT_DOWNUP_IMG	L"/ihm/player/trwindow_button_down_up.png"
#define TRANSPORT_DOWNDO_IMG	L"/ihm/player/trwindow_button_dwn_down.png"
#define TRANSPORT_CLICKUP_IMG	L"/ihm/player/tr_window_minibutton_up.png"
#define TRANSPORT_CLICKDO_IMG	L"/ihm/player/tr_window_minibutton_dwn.png"
#define TRANSPORT_METRO_UP_IMG	L"/ihm/player/tr_window_metronom_up.png"
#define TRANSPORT_METRO_DO_IMG	L"/ihm/player/tr_window_metronom_down.png"

#define VUM_GREEN			L"/ihm/widgets/vum_green.png"
#define VUM_ORANGE			L"/ihm/widgets/vum_orange.png"
#define VUM_RED				L"/ihm/widgets/vum_red.png"

class				MainWindow;
class				DownButton;
class				HoldButton;
class				StaticLabel;
class				VUMCtrl;

class				Transport : public wxPanel, WiredDocument
{
 public:
  Transport(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style, WiredDocument *docParent);
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
  void				OnIdle(wxIdleEvent &WXUNUSED(event));

  // WiredDocument implementation
  void				Save();
  void				Load(SaveElementArray data);

 protected:
  friend class			MainWindow;

  wxGauge			*vum;
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
  wxBitmap			*TrLoopBmp;

  TransparentStaticText	*MesLabel;
  TransparentStaticText	*SigLabel;
  TransparentStaticText	*MilliSigLabel;
  TransparentStaticText	*BpmLabel;
  TransparentStaticText	*SigNumLabel;
  TransparentStaticText	*SigDenLabel;

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
