// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <wx/filename.h>
#include "Transport.h"
#include "Sequencer.h"
#include "SequencerGui.h"
#include "Colour.h"
#include "WiredSession.h"
#include "HelpPanel.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "StaticLabel.h"
#include "../engine/Settings.h"

extern WiredSession				*CurrentSession;

Transport::Transport(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style)
  : wxPanel(parent, -1, pos, size, style)
{
  SetBackgroundColour(CL_RULER_BACKGROUND);
  //wxColour(204, 199, 219));//*wxLIGHT_GREY);
  wxImage *tr_bg = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_BACKGR_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  TrBmp = new wxBitmap(tr_bg);
    
  wxImage *play_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_PLAYUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *play_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_PLAYDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *stop_up =
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_STOPUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *stop_down =
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_STOPDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *rec_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_REC_UP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *rec_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_REC_DO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *backward_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_BAC_DO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *backward_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_BAC_UP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *forward_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_FOR_DO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *forward_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_FOR_UP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *loop_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_LOOPDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *loop_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_LOOPUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *up_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_UPUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *up_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_UPDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *down_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_DOWNUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *down_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_DOWNDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *click_up = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_CLICKUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *click_down = 
    new wxImage(string(WiredSettings->DataDir + string(TRANSPORT_CLICKDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  
  PlayBtn = 
    new DownButton(this, Transport_Play, wxPoint(138, 95), wxSize(41, 42), play_up, play_down);
  StopBtn = 
    new DownButton(this, Transport_Stop, wxPoint(89, 98), wxSize(35, 40), stop_up, stop_down, true);
  RecordBtn = 
    new DownButton(this, Transport_Record, wxPoint(189, 95), wxSize(41, 43), rec_up, rec_down);
  BackwardBtn = new HoldButton(this, Transport_Backward, wxPoint(17, 98), wxSize(35, 39), backward_up, backward_down);
  ForwardBtn = new HoldButton(this, Transport_Forward, wxPoint(53, 98), wxSize(35, 39), forward_up, forward_down);
  LoopBtn = new DownButton(this, Transport_Loop, wxPoint(277, 119), wxSize(11, 11), loop_up, loop_down);
  ClickBtn = new DownButton(this, Transport_Click, wxPoint(254, 119), wxSize(11, 11), 
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
  BpmLabel = new StaticLabel(this, Transport_BpmClick, "096.00", wxPoint(48, 20), wxSize(-1, 12));
  BpmLabel->SetFont(wxFont(11, wxDEFAULT, wxNORMAL, wxNORMAL));
  BpmLabel->SetLabel("096.00");

  SigNumUpBtn = new HoldButton(this, Transport_SigNumUp, wxPoint(42, 48), wxSize(11, 8), 
			       up_up, up_down);
  SigNumDownBtn = new HoldButton(this, Transport_SigNumDown, wxPoint(42, 57), wxSize(11, 8), 
				 down_up, down_down);
  SigDenUpBtn = new HoldButton(this, Transport_SigDenUp, wxPoint(95, 48), wxSize(11, 8), 
			       up_up, up_down);
  SigDenDownBtn = new HoldButton(this, Transport_SigDenDown, wxPoint(95, 57), wxSize(11, 8), 
				 down_up, down_down);

  SigNumLabel = new wxStaticText(this, -1, "4", wxPoint(60, 48), wxSize(-1, 12));
  SigNumLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  SigNumLabel->SetLabel("4");

  SigDenLabel = new wxStaticText(this, -1, "4", wxPoint(82, 48), wxSize(-1, 12));
  SigDenLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  SigDenLabel->SetLabel("4");

  // Test VUMeter
  //vum = new VUMCtrl(this, -1, 10, wxPoint(80,50), wxSize(50,80));
  
  PlayBtn->SetBackgroundColour(wxColour(204, 199, 219));
  StopBtn->SetBackgroundColour(wxColour(204, 199, 219));
  RecordBtn->SetBackgroundColour(wxColour(204, 199, 219));

  MesLabel = new wxStaticText(this, -1, "1", wxPoint(186, 30), wxSize(32, 32));
  MesLabel->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  MesLabel->SetForegroundColour(*wxWHITE);

  SigLabel = new wxStaticText(this, -1, "1", wxPoint(215, 30), wxSize(30, 32));
  SigLabel->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  SigLabel->SetForegroundColour(*wxWHITE);

  MilliSigLabel = new wxStaticText(this, -1, "000", wxPoint(237, 30), wxSize(32, 32));
  MilliSigLabel->SetFont(wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL));
  MilliSigLabel->SetForegroundColour(*wxWHITE);

  
}

Transport::~Transport()
{

}

void				Transport::OnLoopHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("Turns On or Off Loop mode. If On, the sequencer will loop between the L and R marker in the sequencer.");
      HelpWin->SetText(s);
    }
}

void				Transport::OnClickHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("Turns On or Off Click mode. If On, a click will be played over each beat per bar.");
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
  Seq->Stop();
  PlayBtn->SetOff();
  if (RecordBtn->GetOn())
    RecordBtn->SetOff();
}

void				Transport::OnRecord(wxCommandEvent &WXUNUSED(event))
{
  if (RecordBtn->GetOn())
    {
      wxFileName f(CurrentSession->AudioDir.c_str());
      if (CurrentSession->AudioDir.empty() || (!f.DirExists()))
	{
	  wxDirDialog dir(this, "Choose the Audio file directory", 
			  wxFileName::GetCwd());
	  if (dir.ShowModal() == wxID_OK)
	    {
	      CurrentSession->AudioDir = dir.GetPath().c_str();
	    }
	  else
	    {
	      RecordBtn->SetOff();
	      return;
	    }
	}
      Seq->Record();
    }
  else
    Seq->StopRecord();
}

void				Transport::OnLoop(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  Seq->Loop = LoopBtn->GetOn();
}

void				Transport::OnMetronome(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  Seq->Click = ClickBtn->GetOn();
}

void				Transport::OnBackward(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);
  double newpos = Seq->CurrentPos - 0.1;

  if (newpos < 0.0)
    newpos = 0.0;
  Seq->SetCurrentPos(newpos);
  SeqPanel->SetCurrentPos(Seq->CurrentPos);
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
  s.Printf("%d", mes);
  GetTextExtent(s, &w, 0x0);
  MesLabel->SetPosition(wxPoint(196 - w, -1));
  MesLabel->SetLabel(s);
  s.Printf("%d", sig);
  SigLabel->SetLabel(s);
  s.Printf("%d", millisig);
  MilliSigLabel->SetLabel(s);
}

void				Transport::OnBpmUp(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  if (Seq->BPM < 999.f)
    {
      Seq->SetBPM(Seq->BPM + 0.01f);

      wxString s;
      s.Printf("%#06.2f", Seq->BPM);
      BpmLabel->SetLabel(s);
    }      
}

void				Transport::OnBpmDown(wxCommandEvent &WXUNUSED(event))
{
  wxMutexLocker m(SeqMutex);

  if (Seq->BPM > 20.f)
    {
      Seq->SetBPM(Seq->BPM - 0.01f);

      wxString s;
      s.Printf("%#06.2f", Seq->BPM);
      BpmLabel->SetLabel(s);   
    }
}

void				Transport::OnSigNumUp(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigNumerator < 9)
    {
      wxMutexLocker m(SeqMutex);
      wxString s;

      Seq->SetSigNumerator(Seq->SigNumerator + 1);
      
      s.Printf("%d", Seq->SigNumerator);
      SigNumLabel->SetLabel(s); 
      SeqPanel->AdjustHScrolling();
    }    
}

void				Transport::OnSigNumDown(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigNumerator > 1)
    {
      wxMutexLocker m(SeqMutex);
      wxString s;

      Seq->SetSigNumerator(Seq->SigNumerator - 1);
     
      s.Printf("%d", Seq->SigNumerator);
      SigNumLabel->SetLabel(s); 
      SeqPanel->AdjustHScrolling();
    }    
}

void				Transport::OnSigDenUp(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigDenominator < 9)
    {
      wxMutexLocker m(SeqMutex);
      wxString s;

      Seq->SetSigDenominator(Seq->SigDenominator + 1);

      s.Printf("%d", Seq->SigDenominator);
      SigDenLabel->SetLabel(s); 
      SeqPanel->AdjustHScrolling();
    }    
}

void				Transport::OnSigDenDown(wxCommandEvent &WXUNUSED(event))
{
  if (Seq->SigDenominator > 1)
    {
      wxMutexLocker m(SeqMutex);
      wxString s;

      Seq->SetSigDenominator(Seq->SigDenominator - 1);
    
      s.Printf("%d", Seq->SigDenominator);
      SigDenLabel->SetLabel(s); 
      SeqPanel->AdjustHScrolling();
    }    
}

void				Transport::OnBpmClick(wxCommandEvent &WXUNUSED(event))
{
  BpmText = new wxTextCtrl(this, Transport_BpmEnter, BpmLabel->GetLabel(), 
			   BpmLabel->GetPosition(), wxSize(50, BpmLabel->GetSize().y), 
			   wxTE_PROCESS_ENTER);
  Connect(Transport_BpmEnter, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&Transport::OnBpmEnter);
}

void				Transport::OnBpmEnter(wxCommandEvent &WXUNUSED(event))
{
  if (BpmText)
    {
      double d;
      wxString s = BpmText->GetValue();

      if (s.ToDouble(&d))
	{
	  SeqMutex.Lock();
	  Seq->SetBPM(d);
	  SeqMutex.Unlock();
	  BpmLabel->SetLabel(s);      
	}
      BpmText->Destroy();
    }
}

void				Transport::SetBpm(float bpm)
{
  wxString s;
  s.Printf("%#06.2f", bpm);
  BpmLabel->SetLabel(s);
}

void				Transport::SetSigNumerator(int n)
{
  wxString s;
  s.Printf("%d", n);
  SigNumLabel->SetLabel(s);
}

void				Transport::SetSigDenominator(int d)
{
  wxString s;
  s.Printf("%d", d);
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
END_EVENT_TABLE()
