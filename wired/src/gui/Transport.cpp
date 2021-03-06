// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/filename.h>
#include "Transport.h"
#include "Sequencer.h"
#include "SequencerGui.h"
#include "Colour.h"
#include "HelpPanel.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "VUMCtrl.h"
#include "Settings.h"
#include "AudioEngine.h"

Transport::Transport(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style, WiredDocument *docParent)
  : wxPanel(parent, -1, pos, size, style), WiredDocument(wxT("Transport"), docParent)
{
  SetBackgroundColour(CL_RULER_BACKGROUND);
  //wxColour(204, 199, 219));//*wxLIGHT_GREY);
  BpmText = NULL;
  BpmDownBtn = NULL;
  wxImage tr_bg(wxString(WiredSettings->DataDir + wxString(TRANSPORT_BACKGR_IMG)), wxBITMAP_TYPE_PNG);
  TrBmp = new wxBitmap(tr_bg);

  wxImage tr_bg_loop(wxString(WiredSettings->DataDir + wxString(TRANSPORT_BACKGR_LOOP_IMG)), wxBITMAP_TYPE_PNG);
  TrLoopBmp = new wxBitmap(tr_bg_loop);

  wxImage *play_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_PLAYUP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *play_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_PLAYDO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *stop_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_STOPUP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *stop_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_STOPDO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *rec_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_REC_UP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *rec_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_REC_DO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *backward_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_BAC_DO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *backward_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_BAC_UP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *forward_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_FOR_DO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *forward_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_FOR_UP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *loop_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_LOOPDO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *loop_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_LOOPUP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *up_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_UPUP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *up_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_UPDO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *down_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_DOWNUP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *down_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_DOWNDO_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *click_up =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_METRO_UP_IMG)), wxBITMAP_TYPE_PNG);
  wxImage *click_down =
    new wxImage(wxString(WiredSettings->DataDir + wxString(TRANSPORT_METRO_DO_IMG)), wxBITMAP_TYPE_PNG);

  PlayBtn =
    new DownButton(this, Transport_Play, wxPoint(138, 95), wxSize(41, 42), play_up, play_down);
  StopBtn =
    new DownButton(this, Transport_Stop, wxPoint(89, 98), wxSize(35, 40), stop_up, stop_down, true);
  RecordBtn =
    new DownButton(this, Transport_Record, wxPoint(189, 95), wxSize(41, 43), rec_up, rec_down);
  BackwardBtn = new HoldButton(this, Transport_Backward, wxPoint(17, 98), wxSize(35, 39), backward_up, backward_down);
  ForwardBtn = new HoldButton(this, Transport_Forward, wxPoint(53, 98), wxSize(35, 39), forward_up, forward_down);
  LoopBtn = new DownButton(this, Transport_Loop, wxPoint(277, 119), wxSize(11, 11), loop_up, loop_down);
  ClickBtn = new DownButton(this, Transport_Click, wxPoint(250, 102), wxSize(19, 30),
			    click_up, click_down);

  LoopBtn->Connect(Transport_Loop, wxEVT_ENTER_WINDOW,
		   (wxObjectEventFunction)(wxEventFunction)
		   (wxMouseEventFunction)&Transport::OnLoopHelp);
  ClickBtn->Connect(Transport_Click, wxEVT_ENTER_WINDOW,
		   (wxObjectEventFunction)(wxEventFunction)
		   (wxMouseEventFunction)&Transport::OnClickHelp);

  BpmUpBtn = new HoldButton(this, Transport_BpmUp, wxPoint(112, 21), wxSize(11, 8),
			    up_up, up_down);
  BpmUpBtn = new HoldButton(this, Transport_BpmDown, wxPoint(112, 30), wxSize(11, 8),
			    down_up, down_down);
  wxString s;

  s.Printf(wxT("%f"), Seq->BPM);

  BpmLabel = new TransparentStaticText(this, Transport_BpmClick, s, wxPoint(49, 20), wxSize(-1, 12));
  BpmLabel->SetFont(wxFont(11, wxDEFAULT, wxNORMAL, wxNORMAL));

  SetBpm(Seq->BPM);
  SigNumUpBtn = new HoldButton(this, Transport_SigNumUp, wxPoint(42, 48), wxSize(11, 8),
			       up_up, up_down);
  SigNumDownBtn = new HoldButton(this, Transport_SigNumDown, wxPoint(42, 57), wxSize(11, 8),
				 down_up, down_down);
  SigDenUpBtn = new HoldButton(this, Transport_SigDenUp, wxPoint(95, 48), wxSize(11, 8),
			       up_up, up_down);
  SigDenDownBtn = new HoldButton(this, Transport_SigDenDown, wxPoint(95, 57), wxSize(11, 8),
				 down_up, down_down);

  SigNumLabel = new TransparentStaticText(this, -1, wxT("4"), wxPoint(59, 48), wxSize(-1, 12));
  SigNumLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  SigNumLabel->SetLabel(wxT("4"));

  SigDenLabel = new TransparentStaticText(this, -1, wxT("4"), wxPoint(81, 48), wxSize(-1, 12));
  SigDenLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  SigDenLabel->SetLabel(wxT("4"));

  // Test VUMeter
  wxImage *green = new wxImage(wxString(WiredSettings->DataDir + wxString(VUM_GREEN)), wxBITMAP_TYPE_PNG);
  wxImage *orange = new wxImage(wxString(WiredSettings->DataDir + wxString(VUM_ORANGE)), wxBITMAP_TYPE_PNG);
  wxImage *red = new wxImage(wxString(WiredSettings->DataDir + wxString(VUM_RED)), wxBITMAP_TYPE_PNG);
  //vum = new VUMCtrl((wxWindow*)this, -1, 100, green, orange, red, wxPoint(70,78), wxSize(195,5), wxNO_BORDER);
  vum = new wxGauge((wxWindow*)this, -1, 100, wxPoint(70,78), wxSize(195,5));

  PlayBtn->SetBackgroundColour(wxColour(204, 199, 219));
  StopBtn->SetBackgroundColour(wxColour(204, 199, 219));
  RecordBtn->SetBackgroundColour(wxColour(204, 199, 219));

  MesLabel = new TransparentStaticText(this, -1, wxT("1"), wxPoint(182, 34), wxSize(32, 32));
  MesLabel->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  MesLabel->SetForegroundColour(*wxWHITE);

  SigLabel = new TransparentStaticText(this, -1, wxT("1"), wxPoint(215, 34), wxSize(30, 32));
  SigLabel->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  SigLabel->SetForegroundColour(*wxWHITE);

  MilliSigLabel = new TransparentStaticText(this, -1, wxT("000"), wxPoint(237, 34), wxSize(32, 32));
  MilliSigLabel->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  MilliSigLabel->SetForegroundColour(*wxWHITE);
  Connect(wxID_ANY, wxEVT_IDLE, (wxObjectEventFunction) &Transport::OnIdle);
}

Transport::~Transport()
{
    Disconnect(wxEVT_IDLE, (wxObjectEventFunction) &Transport::OnIdle);
	//if (vum) delete vum;
	if (PlayBtn) delete PlayBtn;
	if (StopBtn) delete StopBtn;
	if (RecordBtn) delete RecordBtn;
	if (BackwardBtn) delete BackwardBtn;
	if (ForwardBtn) delete ForwardBtn;
	if (LoopBtn) delete LoopBtn;
	if (ClickBtn) delete ClickBtn;
	if (BpmUpBtn) delete BpmUpBtn;
	if (BpmDownBtn) delete BpmDownBtn;
	if (SigNumUpBtn) delete SigNumUpBtn;
	if (SigNumDownBtn) delete SigNumDownBtn;
	if (SigDenUpBtn) delete SigDenUpBtn;
	if (SigDenDownBtn) delete SigDenDownBtn;
	if (TrBmp) delete TrBmp;
	if (TrLoopBmp) delete TrLoopBmp;
	if (BpmText) delete BpmText;
}

void				Transport::OnLoopHelp(wxMouseEvent &event)
{
  if (HelpWin == NULL)
      return;
  if (HelpWin->IsShown())
    {
      wxString s(_("Turns On or Off Loop mode. If On, the sequencer will loop between the L and R marker in the sequencer."));
      HelpWin->SetText(s);
    }
}

void				Transport::OnClickHelp(wxMouseEvent &event)
{
  if (HelpWin == NULL)
      return;
  if (HelpWin->IsShown())
    {
      wxString s(_("Turns On or Off Click mode. If On, a click will be played over each beat per bar."));
      HelpWin->SetText(s);
    }
}

void				Transport::OnPlay(wxCommandEvent &WXUNUSED(event))
{
  if (PlayBtn->GetOn())
    {
      //if (!Audio->StreamIsStarted)
      //Audio->StartStream();
      Seq->Play();
    }
  else
    PlayBtn->SetOn();
}

void				Transport::OnStop(wxCommandEvent &WXUNUSED(event))
{
  PlayBtn->SetOff();;
  Seq->Stop();
  if (RecordBtn->GetOn())
    RecordBtn->SetOff();
}

void				Transport::OnRecord(wxCommandEvent &WXUNUSED(event))
{
  if (RecordBtn->GetOn())
    {
  // TODO : Replace WiredSession with SaveCenter
//       wxFileName f(CurrentSession->AudioDir.c_str());
//       if (CurrentSession->AudioDir.empty() || (!f.DirExists()))
// 	{
// 	  wxDirDialog dir(this, _("Choose the Audio file directory"),
// 			  wxFileName::GetCwd());
// 	  if (dir.ShowModal() == wxID_OK)
// 	    {
// 	      CurrentSession->AudioDir = dir.GetPath().c_str();
// 	    }
// 	  else
// 	    {
// 	      RecordBtn->SetOff();
// 	      return;
// 	    }
// 	}
      Seq->Record();
    }
  else
    Seq->StopRecord();
}

void				Transport::OnLoop(wxCommandEvent &WXUNUSED(event))
{
  {
    wxMutexLocker m(SeqMutex);

    Seq->Loop = LoopBtn->GetOn();
  }
  Refresh();
}

void				Transport::OnMetronome(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  Seq->Click = ClickBtn->GetOn();
}

void				Transport::OnBackward(wxCommandEvent &WXUNUSED(event))
{
  {
    wxMutexLocker m(SeqMutex);
    Seq->SetCurrentPos(0.0);
  }
  SeqPanel->SetCurrentPos(0.0);
}

void				Transport::OnForward(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  Seq->SetCurrentPos(Seq->CurrentPos + 0.1);
  SeqPanel->SetCurrentPos(Seq->CurrentPos);
}

void				Transport::OnPaint(wxPaintEvent &WXUNUSED(event))
{
  wxMemoryDC			memDC;
  wxPaintDC			dc(this);

  if (LoopBtn->GetOn())
	  memDC.SelectObject(*TrLoopBmp);
  else
	  memDC.SelectObject(*TrBmp);
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
  while (upd)
    {
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(),
	      wxCOPY, FALSE);
      upd++;
    }
}

void				Transport::SetPlayPosition(double pos)
{
  int				mes, sig, millisig, w;
  wxString			s;

  mes = (int)pos;
  sig = (int)(((pos - mes) / (1 / (float)Seq->SigNumerator))) + 1;
  millisig = (int)((pos - mes) * 1000.f);
  if (millisig < 0)
    millisig = 0;
  s.Printf(wxT("%d"), mes);
  GetTextExtent(s, &w, 0x0);
  MesLabel->SetPosition(wxPoint(196 - w, -1));
  MesLabel->SetLabel(s);
  s.Printf(wxT("%d"), sig);
  SigLabel->SetLabel(s);
  s.Printf(wxT("%d"), millisig);
  MilliSigLabel->SetLabel(s);
}

void				Transport::OnBpmUp(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  if (Seq->BPM < 999.f)
    {
      Seq->SetBPM(Seq->BPM + 1);

      SetBpm(Seq->BPM);
    }
}

void				Transport::OnBpmDown(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  if (Seq->BPM > 20.f)
    {
      Seq->SetBPM(Seq->BPM - 1);

      SetBpm(Seq->BPM);
    }
}

void				Transport::OnSigNumUp(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigNumerator < 9)
    {
      wxString s;
      {
	wxMutexLocker m(SeqMutex);

	Seq->SetSigNumerator(Seq->SigNumerator + 1);
      }
      s.Printf(wxT("%d"), Seq->SigNumerator);
      SigNumLabel->SetLabel(s);
      SeqPanel->AdjustHScrolling();
    }
}

void				Transport::OnSigNumDown(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigNumerator > 1)
    {
      wxString s;
      {
	wxMutexLocker m(SeqMutex);

	Seq->SetSigNumerator(Seq->SigNumerator - 1);
      }
      s.Printf(wxT("%d"), Seq->SigNumerator);
      SigNumLabel->SetLabel(s);
      SeqPanel->AdjustHScrolling();
    }
}

void				Transport::OnSigDenUp(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigDenominator < 9)
    {
      wxString s;
      {
	wxMutexLocker m(SeqMutex);

	Seq->SetSigDenominator(Seq->SigDenominator + 1);
      }
      s.Printf(wxT("%d"), Seq->SigDenominator);
      SigDenLabel->SetLabel(s);
      SeqPanel->AdjustHScrolling();
    }
}

void				Transport::OnSigDenDown(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigDenominator > 1)
    {
      wxString s;
      {
	wxMutexLocker m(SeqMutex);

	Seq->SetSigDenominator(Seq->SigDenominator - 1);
      }
      s.Printf(wxT("%d"), Seq->SigDenominator);
      SigDenLabel->SetLabel(s);
      SeqPanel->AdjustHScrolling();
    }
}

void				Transport::OnBpmClick(wxCommandEvent &WXUNUSED(event))
{
  if (BpmText) BpmText->Destroy();
  BpmLabel->Hide();
  BpmText = new wxTextCtrl(this, Transport_BpmEnter, BpmLabel->GetLabel(),
			   BpmLabel->GetPosition(), wxSize(50, BpmLabel->GetSize().y),
			   wxTE_PROCESS_ENTER);
  Connect(Transport_BpmEnter, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&Transport::OnBpmEnter);
  BpmText->SetFocus();
  BpmText->SetSelection(-1, -1);
}

void				Transport::OnBpmEnter(wxCommandEvent &WXUNUSED(event))
{
  if (BpmText)
    {
      double d;
      wxString s = BpmText->GetValue();

      if (s.ToDouble(&d))
	{
	  {
	    wxMutexLocker m(SeqMutex);

	    Seq->SetBPM(d);
	  }
	  SetBpm(d);
	}
      Disconnect(Transport_BpmEnter, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)(wxEventFunction)
		 (wxCommandEventFunction)&Transport::OnBpmEnter);
      BpmText->Destroy();
      BpmText = NULL;
    }
    BpmLabel->Show();
}

void				Transport::SetBpm(float bpm)
{
  wxString s;
  s.Printf(wxT("%.0f"), bpm);
  BpmLabel->SetLabel(s);
}

void				Transport::SetSigNumerator(int n)
{
  wxString s;
  s.Printf(wxT("%d"), n);
  SigNumLabel->SetLabel(s);
}

void				Transport::SetSigDenominator(int d)
{
  wxString s;
  s.Printf(wxT("%i"), d);
  SigDenLabel->SetLabel(s);
}

void				Transport::SetLoop(bool loop)
{
  if (loop)
    LoopBtn->SetOn();
  else
    LoopBtn->SetOff();
}

void				Transport::SetClick(bool click)
{
  if (click)
    ClickBtn->SetOn();
  else
    ClickBtn->SetOff();
}

void				Transport::OnIdle(wxIdleEvent &WXUNUSED(event))
{
  if (Audio)
    vum->SetValue((int)(Audio->GetCpuLoad() * 100));
}

void				Transport::Save()
{
}

void				Transport::Load(SaveElementArray data)
{
  SetBpm(Seq->BPM);
  SetSigNumerator(Seq->SigNumerator);
  SetSigDenominator(Seq->SigDenominator);
  SetLoop(Seq->Loop);
  SetClick(Seq->Click);
}

BEGIN_EVENT_TABLE(Transport, wxPanel)
  EVT_BUTTON(Transport_Play, Transport::OnPlay)
  EVT_BUTTON(Transport_Stop, Transport::OnStop)
  EVT_BUTTON(Transport_Record, Transport::OnRecord)
  EVT_BUTTON(Transport_Loop, Transport::OnLoop)
  EVT_BUTTON(Transport_Backward, Transport::OnBackward)
  EVT_BUTTON(Transport_Forward, Transport::OnForward)
  EVT_BUTTON(Transport_BpmUp, Transport::OnBpmUp)
  EVT_BUTTON(Transport_BpmDown, Transport::OnBpmDown)
  EVT_BUTTON(Transport_SigNumUp, Transport::OnSigNumUp)
  EVT_BUTTON(Transport_SigNumDown, Transport::OnSigNumDown)
  EVT_BUTTON(Transport_SigDenUp, Transport::OnSigDenUp)
  EVT_BUTTON(Transport_SigDenDown, Transport::OnSigDenDown)
  EVT_BUTTON(Transport_Click, Transport::OnMetronome)
  EVT_BUTTON(Transport_BpmClick, Transport::OnBpmClick)
  EVT_PAINT(Transport::OnPaint)
  EVT_IDLE(Transport::OnIdle)
END_EVENT_TABLE()
