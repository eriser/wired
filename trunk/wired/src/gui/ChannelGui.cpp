// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include "ChannelGui.h"
#include "Colour.h"
#include "SeqTrack.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "FaderCtrl.h"
#include "StaticLabel.h"
#include "VUMCtrl.h"
#include "HintedFader.h"
#include "../engine/Settings.h"
#include "../mixer/Channel.h"
#include "../mixer/Mixer.h"

ChannelGui::ChannelGui(Channel *channel, wxImage *img_bg, wxImage *img_fg,
		       wxWindow* parent, wxWindowID id,
		       const wxPoint& pos, const wxSize& size, 
		       const wxString& label)
  : wxPanel( parent, id, pos, size )
{
  
  ConnectedSeqTrack = 0x0;
  SetBackgroundColour(*wxBLACK);//CL_RULER_BACKGROUND);
  bg = new wxImage(string(WiredSettings->DataDir + string(BG)).c_str(), wxBITMAP_TYPE_PNG);
  MixerBmp = new wxBitmap(bg);
  //cout << bg->GetWidth() << " " << bg->GetHeight() << endl;
  Chan = channel;
  ImgFaderBg = img_bg;
  ImgFaderFg = img_fg;
  if (Chan->Stereo)
    {
      FaderLeft  = new HintedFader(this, FaderLeftId, this->GetParent(), 
				   ImgFaderBg, ImgFaderFg,
				   0, 127, 0, 
				   wxPoint(17, 10), wxDefaultSize, 
				   GetPosition() + wxPoint(5,15));
      //cout << ImgFaderBg->GetWidth() << " " << ImgFaderBg->GetHeight() << endl;
      wxImage *green = new wxImage(string(WiredSettings->DataDir + string(VUM_GREEN)).c_str(), wxBITMAP_TYPE_PNG);
      wxImage *orange = new wxImage(string(WiredSettings->DataDir + string(VUM_ORANGE)).c_str(), wxBITMAP_TYPE_PNG);
      wxImage *red = new wxImage(string(WiredSettings->DataDir + string(VUM_RED)).c_str(), wxBITMAP_TYPE_PNG);
      VumLeft  = new VUMCtrl(this, -1, 100, green, orange, red, wxPoint(8, 23), wxSize(4, 65), wxNO_BORDER);
      VumRight = new VUMCtrl(this, -1, 100, green, orange, red, wxPoint(60, 23), wxSize(4, 65), wxNO_BORDER);
      FaderRight = new HintedFader(this, FaderRightId, this->GetParent(), 
				   ImgFaderBg, ImgFaderFg, 
				   0, 127, 0,
				   wxPoint(69, 10), wxDefaultSize, 
				   GetPosition() + wxPoint(75,15));
      /*VolumeLeft = new wxStaticText(this, -1, "100", wxPoint(5, 90));
      VolumeLeft->SetFont(wxFont(7, wxBOLD, wxBOLD, wxBOLD));
      VolumeRight = new wxStaticText(this, -1, "100", wxPoint(75, 90));
      VolumeRight->SetFont(wxFont(7, wxBOLD, wxBOLD, wxBOLD));*/
      Label = new wxStaticText(this, -1, label, wxPoint(25, 0));
      Label->SetForegroundColour(*wxWHITE);
      //Label->SetForegroundColour(*wxBLACK);
      Label->SetFont(wxFont(8, wxBOLD, wxBOLD, wxBOLD));//wxNORMAL, wxNORMAL));
      FaderLeft->SetValue( (int)(Chan->VolumeLeft * 100) );
      FaderRight->SetValue( (int)(Chan->VolumeRight * 100) );
      VumLeft->SetValue( 0 );
      VumRight->SetValue( 0 );
      hp_up = new wxImage(string(WiredSettings->DataDir + string(MIXERHPUP)).c_str(), wxBITMAP_TYPE_PNG);
      hp_dn = new wxImage(string(WiredSettings->DataDir + string(MIXERHPDOWN)).c_str(), wxBITMAP_TYPE_PNG);
      lock_up = new wxImage(string(WiredSettings->DataDir + string(MIXERLOCKUP)).c_str(), wxBITMAP_TYPE_PNG);
      lock_dn = new wxImage(string(WiredSettings->DataDir + string(MIXERLOCKDOWN)).c_str(), wxBITMAP_TYPE_PNG);
      MuteLeftButton = 	new DownButton(this, MuteLeftId, wxPoint(24, 105), 
				       wxSize(13, 13), hp_up, hp_dn, false);
      MuteRightButton = new DownButton(this, MuteRightId, wxPoint(63, 105), 
				       wxSize(13, 13), hp_up, hp_dn, false);
      LockButton = new DownButton(this, LockId, wxPoint(41, 99), 
				  wxSize(19, 19), lock_up, lock_dn, false);
      Lock = false;
    }
}

ChannelGui::~ChannelGui()
{
  delete MixerBmp;
  delete bg;
  delete hp_up;
  delete hp_dn;
  delete lock_up;
  delete lock_dn;
}

void				ChannelGui::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxMemoryDC			memDC;
  wxPaintDC			dc(this);
  wxRegionIterator		upd(GetUpdateRegion());
  memDC.SelectObject(*MixerBmp);

  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, 
	      upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }  
}

void				ChannelGui::OnFaderLeft(wxScrollEvent& WXUNUSED(e))
{
  float				res = static_cast<float>(FaderLeft->GetValue() / 100.f);
  wxString			s;
  
  s.Printf("%d", FaderLeft->GetValue());
  if (Lock) 
    {
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      Chan->VolumeRight = res;
      Chan->VolumeLeft = res;
      MixMutex.Unlock();
      FaderRight->SetValue(FaderLeft->GetValue());
      //VolumeRight->SetLabel(s);
    }
  else 
    {
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      Chan->VolumeLeft = res;
      MixMutex.Unlock();
    }
  //VolumeLeft->SetLabel(s);
}

void				ChannelGui::OnFaderRight(wxScrollEvent& WXUNUSED(e))
{ 
  float				res = static_cast<float>(FaderRight->GetValue() / 100.f);
  wxString			s;

  s.Printf("%d", FaderRight->GetValue());
  if (Lock) 
    {
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      Chan->VolumeRight = res;
      Chan->VolumeLeft = res;
      MixMutex.Unlock();
      FaderLeft->SetValue(FaderRight->GetValue());
      //VolumeLeft->SetLabel(s);
    }
  else 
    {
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      Chan->VolumeRight = res;
      MixMutex.Unlock();
    }
  //VolumeRight->SetLabel(s);
}

void				ChannelGui::SetLabel(const wxString& label)
{
  int				len = label.length();

  if (len && len > LABEL_MAXCHAR)
    {
      wxString s;
      s = label.Mid(0, LABEL_MAXCHAR - 2);
      s << '-' << label.Mid(len - 1);
      Label->SetLabel(s);
    }
  else
    Label->SetLabel(label);
}

void				ChannelGui::SetOpt(SeqTrack* st)
{
  ConnectedSeqTrack = st;
  //SetLabel(st->Text->GetValue());
}

void				ChannelGui::UpdateScreen()
{
  float				lrms, rrms;
  
  MixMutex.Lock();		//mutex used by Channel::PushBuffer()
  lrms= Chan->Lrms;
  rrms= Chan->Rrms;
  MixMutex.Unlock();
  VumLeft->SetValue((long)(((20.f * (float)log10(lrms) + 96.f) / 96.f ) * 100.f));
  VumRight->SetValue((long)(((20.f * (float)log10(rrms) + 96.f) / 96.f ) * 100.f));
  if (ConnectedSeqTrack)
    {
      ConnectedSeqTrack->SetVuValue(VumLeft->GetValue());
    }
}

void				ChannelGui::OnMuteLeft(wxCommandEvent& WXUNUSED(e))
{
  bool				m = MuteLeftButton->GetOn();

  if (Lock)
    {
      MixMutex.Lock();
      Chan->MuteLeft = m;
      Chan->MuteRight = m;
      MixMutex.Unlock();
      if (m)
	MuteRightButton->SetOn();
      else
	MuteRightButton->SetOff();
    }
  else
    {
      MixMutex.Lock();
      Chan->MuteLeft = m;
      MixMutex.Unlock();
    }
}

void				ChannelGui::OnMuteRight(wxCommandEvent& WXUNUSED(e))
{
  bool				m = MuteRightButton->GetOn();

  if (Lock)
    {
      MixMutex.Lock();
      Chan->MuteLeft = m;
      Chan->MuteRight = m;
      MixMutex.Unlock();
      if (m)
	MuteLeftButton->SetOn();
      else
	MuteLeftButton->SetOff();
    }
  else
    {
      MixMutex.Lock();
      Chan->MuteRight = m;
      MixMutex.Unlock();
    }
}

void				ChannelGui::OnLock(wxCommandEvent& WXUNUSED(e))
{
  Lock = !Lock;
  if (Lock)
    LockButton->SetOn();
  else
    LockButton->SetOff();
}

MasterChannelGui::MasterChannelGui( Channel *channel, wxImage *img_bg, 
				    wxImage *img_fg, wxWindow* parent, 
				    wxWindowID id, const wxPoint& pos, 
				    const wxSize& size )
  : ChannelGui(channel, img_bg, img_fg, parent, id, pos, size, "MASTER")
{
  //  Label = new wxStaticText(this, -1, "MASTER", wxPoint(20, 0));
}

MasterChannelGui::~MasterChannelGui()
{
  
}

void				MasterChannelGui::OnFaderLeft(wxScrollEvent &e)
{
  float				res = static_cast<float>(FaderLeft->GetValue() / 100.f);
  wxString			s;

  s.Printf("%d", FaderLeft->GetValue());
  if (Lock) 
    {
      MixMutex.Lock();			//mutex used by Mixer::Mixouput()
      Mix->VolumeRight = res;
      Mix->VolumeLeft = res;
      MixMutex.Unlock();
      FaderRight->SetValue(FaderLeft->GetValue());
      //VolumeRight->SetLabel(s);
    }
  else 
    {
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      Chan->VolumeLeft = res;
      MixMutex.Unlock();
    }
  //VolumeLeft->SetLabel(s); 
}

void				MasterChannelGui::OnFaderRight(wxScrollEvent &e)
{ 
  float				res = static_cast<float>(FaderRight->GetValue() / 100.f);
  wxString			s;

  s.Printf("%d", FaderRight->GetValue());
  if (Lock) 
    {
      MixMutex.Lock();			//mutex used by Mixer::Mixoutput()
      Mix->VolumeRight = res;
      Mix->VolumeLeft = res;
      MixMutex.Unlock();
      FaderLeft->SetValue(FaderRight->GetValue());
      //VolumeLeft->SetLabel(s);
    }
  else 
    {
      MixMutex.Lock();			//mutex used by Mixer::Mixoutput()
      Mix->VolumeRight = res;
      MixMutex.Unlock();
    }
  //  VolumeRight->SetLabel(s);
}


void				MasterChannelGui::OnMuteLeft(wxCommandEvent& WXUNUSED(e))
{
  bool				m = MuteLeftButton->GetOn();

  if (Lock)
    {
      MixMutex.Lock();
      Mix->MuteL = m;
      Mix->MuteR = m;
      MixMutex.Unlock();
      if (m)
	MuteRightButton->SetOn();
      else
	MuteRightButton->SetOff();
    }
  else
    {
      MixMutex.Lock();
      Mix->MuteL = m;
      MixMutex.Unlock();
    }
}

void				MasterChannelGui::OnMuteRight(wxCommandEvent& WXUNUSED(e))
{
  bool				m = MuteRightButton->GetOn();

  if (Lock)
    {
      MixMutex.Lock();
      Mix->MuteL = m;
      Mix->MuteR = m;
      MixMutex.Unlock();
      if (m)
	MuteLeftButton->SetOn();
      else
	MuteLeftButton->SetOff();
    }
  else
    {
      MixMutex.Lock();
      Mix->MuteR = m;
      MixMutex.Unlock();
    }
}

void				MasterChannelGui::OnLock(wxCommandEvent& WXUNUSED(e))
{
  Lock = !Lock;
  if (Lock)
    LockButton->SetOn();
  else
    LockButton->SetOff();
}

BEGIN_EVENT_TABLE(ChannelGui, wxPanel)
  EVT_COMMAND_SCROLL(FaderLeftId, ChannelGui::OnFaderLeft)
  EVT_COMMAND_SCROLL(FaderRightId, ChannelGui::OnFaderRight)
  EVT_BUTTON(MuteLeftId, ChannelGui::OnMuteLeft)
  EVT_BUTTON(MuteRightId, ChannelGui::OnMuteRight)
  EVT_BUTTON(LockId, ChannelGui::OnLock)
  EVT_PAINT(ChannelGui::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MasterChannelGui, wxPanel)
  EVT_COMMAND_SCROLL(FaderLeftId, MasterChannelGui::OnFaderLeft)
  EVT_COMMAND_SCROLL(FaderRightId, MasterChannelGui::OnFaderRight)
  EVT_BUTTON(MuteLeftId, ChannelGui::OnMuteLeft)
  EVT_BUTTON(MuteRightId, ChannelGui::OnMuteRight)
  EVT_BUTTON(LockId, ChannelGui::OnLock)
  EVT_PAINT(ChannelGui::OnPaint)
END_EVENT_TABLE()