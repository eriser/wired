// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
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
#include "Settings.h"
#include "Channel.h"
#include "Mixer.h"

ChannelGui::ChannelGui(Channel* channel, wxImage* img_bg, wxImage* img_fg,
		       wxWindow* parent, wxWindowID id,
		       const wxPoint& pos, const wxSize& size,
		       const wxString& label, WiredDocument* docParent)
  : wxPanel(parent, id, pos, size), WiredDocument(wxT("ChannelGui"), docParent)
{
  ConnectedSeqTrack = 0x0;
  SetBackgroundColour(*wxBLACK);//CL_RULER_BACKGROUND);

  wxImage	bg(wxString(WiredSettings->DataDir + wxString(BG)), wxBITMAP_TYPE_PNG);
  MixerBmp = new wxBitmap(bg);

  Chan = channel;
  ImgFaderBg = img_bg;
  ImgFaderFg = img_fg;
  if (Chan && Chan->Stereo)
    {
      FaderLeft  = new FaderCtrl(this, FaderLeftId, ImgFaderBg, ImgFaderFg, 0,
				 127, &Chan->VolumeLeft, true, wxPoint(17, 10), wxDefaultSize,
				 this, GetPosition() + wxPoint(5,15));
      wxImage *green = new wxImage(wxString(WiredSettings->DataDir + wxString(VUM_GREEN)).c_str(), wxBITMAP_TYPE_PNG);
      wxImage *orange = new wxImage(wxString(WiredSettings->DataDir + wxString(VUM_ORANGE)).c_str(), wxBITMAP_TYPE_PNG);
      wxImage *red = new wxImage(wxString(WiredSettings->DataDir + wxString(VUM_RED)).c_str(), wxBITMAP_TYPE_PNG);
      VumLeft  = new VUMCtrl(this, -1, 100, green, orange, red, wxPoint(8, 23), wxSize(4, 65), wxNO_BORDER);
      VumRight = new VUMCtrl(this, -1, 100, green, orange, red, wxPoint(60, 23), wxSize(4, 65), wxNO_BORDER);
      FaderRight = new FaderCtrl(this, FaderRightId, ImgFaderBg, ImgFaderFg, 0,
				 127, &Chan->VolumeRight, true, wxPoint(69, 10), wxDefaultSize,
				 this, GetPosition() + wxPoint(75,15));
      /*VolumeLeft = new wxStaticText(this, -1, "100", wxPoint(5, 90));
      VolumeLeft->SetFont(wxFont(7, wxBOLD, wxBOLD, wxBOLD));
      VolumeRight = new wxStaticText(this, -1, "100", wxPoint(75, 90));
      VolumeRight->SetFont(wxFont(7, wxBOLD, wxBOLD, wxBOLD));*/
      Label = new wxStaticText(this, -1, label, wxPoint(25, 0));
      Label->SetForegroundColour(*wxWHITE);
      Label->SetForegroundColour(*wxBLACK);
      Label->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT,
			    wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
      //cout << "Chan->VolumeGui : " << Chan->VolumeLeft << endl;
      FaderLeft->SetValue((int)(Chan->VolumeLeft * 100));
      FaderRight->SetValue((int)(Chan->VolumeRight * 100));
      VumLeft->SetValue(0);
      VumRight->SetValue(0);
      hp_up = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERHPUP)).c_str(), wxBITMAP_TYPE_PNG);
      hp_dn = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERHPDOWN)).c_str(), wxBITMAP_TYPE_PNG);
      lock_up = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERLOCKUP)).c_str(), wxBITMAP_TYPE_PNG);
      lock_dn = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERLOCKDOWN)).c_str(), wxBITMAP_TYPE_PNG);
      MuteLeftButton = 	new DownButton(this, MuteLeftId, wxPoint(24, 105),
				       wxSize(13, 13), hp_up, hp_dn, false);
      MuteRightButton = new DownButton(this, MuteRightId, wxPoint(63, 105),
				       wxSize(13, 13), hp_up, hp_dn, false);
      LockButton = new DownButton(this, LockId, wxPoint(41, 99),
				  wxSize(19, 19), lock_up, lock_dn, false);
      Lock = true;
      LockButton->SetOn();
      //cout << "constructor" << endl;
    }
}

ChannelGui::~ChannelGui()
{
  delete MixerBmp;
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
  //wxString			s;

  //s.Printf("%d", FaderLeft->GetValue());
  if (Lock)
    {
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      FaderRight->SetValue(FaderLeft->GetValue());
      MixMutex.Unlock();
    }
  //else
  //{
  //  MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      //Chan->VolumeLeft = res;
  //  MixMutex.Unlock();
  //}
  //VolumeLeft->SetLabel(s);
  //cout << Mix->VolumeLeft << " | " << Mix->VolumeRight << endl;
}

void				ChannelGui::OnFaderRight(wxScrollEvent& WXUNUSED(e))
{
  //float				res = static_cast<float>(FaderRight->GetValue() / 100.f);
  //wxString			s;

  //s.Printf("%d", FaderRight->GetValue());
  if (Lock)
    {

      //Chan->VolumeRight = res;
      //Chan->VolumeLeft = res;
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      FaderLeft->SetValue(FaderRight->GetValue());
      MixMutex.Unlock();
      //cout << "in lock" << endl;
      //VolumeLeft->SetLabel(s);
    }
  //else
  //{
      //MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      //Chan->VolumeRight = res;
      //MixMutex.Unlock();
  //}
  //VolumeRight->SetLabel(s);
  //cout << Mix->VolumeLeft << " | " << Mix->VolumeRight << endl;
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


void				ChannelGui::SetStereo(bool stereo)
{
  this->Stereo = stereo;
}

void				ChannelGui::SetLock(bool lock)
{
  this->Lock = lock;
}

void				ChannelGui::SetMuteLeftButton(bool isDown)
{
  if (isDown)
    this->MuteLeftButton->SetOn();
  else
    this->MuteLeftButton->SetOff();
}

void				ChannelGui::SetMuteRightButton(bool isDown)
{
  if (isDown)
    this->MuteRightButton->SetOn();
  else
    this->MuteRightButton->SetOff();
}

void				ChannelGui::SetLockButton(bool isDown)
{
  if (isDown)
    this->LockButton->SetOn();
  else
    this->LockButton->SetOff();
}


void				ChannelGui::OnMuteLeft(wxCommandEvent& WXUNUSED(e))
{
  bool				m = MuteLeftButton->GetOn();

  if (Lock)
    {
      MixMutex.Lock();
      Chan->SetMuteLeft(m);
      Chan->SetMuteRight(m);
      MixMutex.Unlock();
      if (m)
	MuteRightButton->SetOn();
      else
	MuteRightButton->SetOff();
    }
  else
    {
      MixMutex.Lock();
      Chan->SetMuteLeft(m);
      MixMutex.Unlock();
    }
}

void				ChannelGui::OnMuteRight(wxCommandEvent& WXUNUSED(e))
{
  bool				m = MuteRightButton->GetOn();

  if (Lock)
    {
      MixMutex.Lock();
      Chan->SetMuteLeft(m);
      Chan->SetMuteRight(m);
      MixMutex.Unlock();
      if (m)
	MuteLeftButton->SetOn();
      else
	MuteLeftButton->SetOff();
    }
  else
    {
      MixMutex.Lock();
      Chan->SetMuteRight(m);
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

void				ChannelGui::Load(SaveElementArray data)
{
  int		dataCompt;

#ifdef __DEBUG__
  std::cerr << "ChannelGui::Load : " << this << std::endl;
#endif
  for (dataCompt = 0; dataCompt < data.GetCount(); dataCompt++)
    {
#ifdef __DEBUG__
      std::cerr << "ChannelGui->key = " << data[dataCompt]->getKey().mb_str() << std::endl;
      std::cerr << "ChannelGui->value = " << data[dataCompt]->getValue().mb_str() << std::endl;
#endif

      if (data[dataCompt]->getKey() == wxT("stereo"))
	{
	  if (!data[dataCompt]->getValueInt())
	    SetStereo(false);
	  else
	    SetStereo(true);
	}
      else if (data[dataCompt]->getKey() == wxT("volumeLeft"))
	{
	  FaderLeft->SetValue((int)(data[dataCompt]->getValueFloat() * 100));
	  //	  VolumeLeft->SetLabel(data[dataCompt]->getValueFloat());
	}
      else if (data[dataCompt]->getKey() == wxT("volumeRight"))
	{
	  FaderRight->SetValue((int)(data[dataCompt]->getValueFloat() * 100));
	  //	  VolumeRight->SetLabel(data[dataCompt]->getValueFloat());
	}
      else if (data[dataCompt]->getKey() == wxT("muteLeftButton"))
	{
	  if (!data[dataCompt]->getValueInt())
	    SetMuteLeftButton(false);
	  else
	    SetMuteLeftButton(true);
	}
      else if (data[dataCompt]->getKey() == wxT("muteRightButton"))
	{
	  if (!data[dataCompt]->getValueInt())
	    SetMuteRightButton(false);
	  else
	    SetMuteRightButton(true);
	}
      else if (data[dataCompt]->getKey() == wxT("lockButton"))
	{
	  if (!data[dataCompt]->getValueInt()) {
	    Lock = false;
	    SetLockButton(false);
	  }
	  else {
	    Lock = true;
	    SetLockButton(true);
	  }
	}
      else if (data[dataCompt]->getKey() == wxT("label"))
	{
	  Label->SetLabel(data[dataCompt]->getValue());
	}

    }
}

void 	ChannelGui::Save()
{
  SaveElement	*savedElem;

  if (Stereo)
    savedElem = new SaveElement(wxT("stereo"), wxT("1"));
  else
    savedElem = new SaveElement(wxT("stereo"), wxT("0"));
  saveDocData(savedElem);

  //VolumeLeft
  savedElem = new SaveElement(wxT("volumeLeft"), FaderLeft->GetValue() / 100);
  //FaderLeft->SetValue((int)(data[dataCompt]->getValueFloat() * 100));
  //  savedElem = new SaveElement(wxT("volumeLeft"), VolumeLeft->GetLabel());
  saveDocData(savedElem);

  //VolumeRight
  savedElem = new SaveElement(wxT("volumeRight"), FaderRight->GetValue() / 100);
  // savedElem = new SaveElement(wxT("volumeRight"), VolumeRight->GetLabel());
  saveDocData(savedElem);

  //MuteLeft
  if (MuteLeftButton->GetOn())
    savedElem = new SaveElement(wxT("muteLeftButton"), wxT("1"));
  else
    savedElem = new SaveElement(wxT("muteLeftButton"), wxT("0"));
  saveDocData(savedElem);

  //MuteRight
  if (MuteRightButton->GetOn())
    savedElem = new SaveElement(wxT("muteRightButton"), wxT("1"));
  else
    savedElem = new SaveElement(wxT("muteRightButton"), wxT("0"));
  saveDocData(savedElem);

  //Lock
  if (LockButton->GetOn())
    savedElem = new SaveElement(wxT("lockButton"), wxT("1"));
  else
    savedElem = new SaveElement(wxT("lockButton"), wxT("0"));
  saveDocData(savedElem);

  //Label
  savedElem = new SaveElement(wxT("label"), Label->GetLabel());
  saveDocData(savedElem);

}


MasterChannelGui::MasterChannelGui(Channel* channel, wxImage* img_bg,
				   wxImage* img_fg, wxWindow* parent,
				   wxWindowID id, const wxPoint& pos,
				   const wxSize& size, WiredDocument* parentDoc)
  : ChannelGui(channel, img_bg, img_fg, parent, id, pos, size, _("MASTER"), parentDoc)
{
  //  Label = new wxStaticText(this, -1, "MASTER", wxPoint(20, 0));
}

MasterChannelGui::~MasterChannelGui()
{

}

void				MasterChannelGui::OnFaderLeft(wxScrollEvent& e)
{
  //float		res = static_cast<float>(FaderLeft->GetValue() / 100.f);
  //wxString		s;

  //s.Printf("%d", FaderLeft->GetValue());
  if (Lock)
    {
      MixMutex.Lock();			//mutex used by Mixer::Mixouput()
      Mix->VolumeRight = Chan->VolumeRight / 100.f;
      Mix->VolumeLeft = Chan->VolumeLeft / 100.f;
      MixMutex.Unlock();
      FaderRight->SetValue(FaderLeft->GetValue());
      //VolumeRight->SetLabel(s);
    }
  else
    {
      MixMutex.Lock();			//mutex used by Channel::PushBuffer()
      Mix->VolumeLeft = Chan->VolumeLeft / 100.f;
      MixMutex.Unlock();
    }
  //cout << Mix->VolumeLeft << " | " << Mix->VolumeRight << endl;
  //VolumeLeft->SetLabel(s);
}

void				MasterChannelGui::OnFaderRight(wxScrollEvent& e)
{
  //float				res = static_cast<float>(FaderRight->GetValue() / 100.f);
  //wxString			s;

  //s.Printf("%d", FaderRight->GetValue());
  if (Lock)
    {
      MixMutex.Lock();			//mutex used by Mixer::Mixoutput()
      Mix->VolumeRight = Chan->VolumeRight / 100.f;
      Mix->VolumeLeft = Chan->VolumeLeft / 100.f;
      FaderLeft->SetValue(FaderRight->GetValue());
      MixMutex.Unlock();
    }
  else
    {
      MixMutex.Lock();			//mutex used by Mixer::Mixoutput()
      Mix->VolumeRight = Chan->VolumeRight / 100.f;
      MixMutex.Unlock();
    }
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
EVT_BUTTON(MuteLeftId, MasterChannelGui::OnMuteLeft)
  EVT_BUTTON(MuteRightId, MasterChannelGui::OnMuteRight)
  EVT_BUTTON(LockId, ChannelGui::OnLock)
  EVT_PAINT(ChannelGui::OnPaint)
END_EVENT_TABLE()
