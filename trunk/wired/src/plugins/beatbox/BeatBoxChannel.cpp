#include <wx/progdlg.h>

#include "BeatBoxChannel.h"
#include "FileLoader.h"
#include <wx/filename.h>
#include <math.h>

BEGIN_EVENT_TABLE(BeatBoxChannel, wxWindow)
  EVT_PAINT(BeatBoxChannel::OnPaint)
  
  EVT_COMMAND_SCROLL(BC_Lev, BeatBoxChannel::OnLevChange)
  EVT_COMMAND_SCROLL(BC_Bal, BeatBoxChannel::OnBalChange)
  EVT_COMMAND_SCROLL(BC_Sta, BeatBoxChannel::OnStartChange)
  EVT_COMMAND_SCROLL(BC_End, BeatBoxChannel::OnEndChange)
  EVT_COMMAND_SCROLL(BC_Len, BeatBoxChannel::OnLenChange)
  EVT_COMMAND_SCROLL(BC_Pit, BeatBoxChannel::OnPitchChange)
  EVT_COMMAND_SCROLL(BC_Vel, BeatBoxChannel::OnVelChange)

  EVT_BUTTON(BC_Select, BeatBoxChannel::OnSelectChannel)
  EVT_BUTTON(BC_OnLoadSound, BeatBoxChannel::OnLoadSound)
  EVT_BUTTON(BC_ReverseSound, BeatBoxChannel::ReverseSound)
  EVT_BUTTON(BC_Mute, BeatBoxChannel::OnMute)
  EVT_BUTTON(BC_Solo, BeatBoxChannel::OnSolo)
  EVT_BUTTON(BC_OnPlaySound, BeatBoxChannel::OnPlaySound)
END_EVENT_TABLE()


inline BeatNoteToPlay::BeatNoteToPlay(int notenum, float vel, 
				      unsigned long delta,
				      BeatBoxChannel* c, float** b)
{
      NoteNum = notenum;
      NumChan = c->Id;
      Delta = delta;
      Lev = c->Lev;
      Vel = c->Vel * vel;
      Pitch = c->Pitch;
      Start = c->Start;
      End = c->End;
      Len = c->Len;
      Reversed = c->Reversed;
      Buffer = b;
      OffSet = 0;
      SEnd = 0;
      
      if (c->Wave)
	{
	  OffSet = static_cast<unsigned long>
	    (floor(c->Wave->GetNumberOfFrames() * Start));
	  SEnd = static_cast<unsigned long>
	    (floor(c->Wave->GetNumberOfFrames() * End));
	}
}

BeatNoteToPlay::BeatNoteToPlay(BeatBoxChannel* c, float** buf)
{
  NumChan = c->Id;
  NoteNum = 0;
  Start = c->Start;
  Len = c->Len;
  End = c->End;
  Pitch = c->Pitch;
  Vel = c->Vel;
  Lev = c->Lev;
  Reversed = c->Reversed;
  Buffer = buf;
  OffSet = 0;
  SEnd = 0;
  
  if (c->Wave)
    {
      OffSet = static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * Start));
      SEnd = static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * End));
    }
  
}


BeatBoxChannel::BeatBoxChannel( wxWindow *parent, wxWindowID id, 
				const wxPoint &pos, const wxSize &size, 
				unsigned int num, string datadir, wxMutex* m )
  : wxWindow( parent, id, pos, size )
{
  Data = new unsigned int[2];
  Id = num;
  Data[0] = num; /* Channel number 0-11 */
  Data[1] = 0;
  
  DataDir = datadir;
  PatternMutex = m;
  
  Start = 0.f;
  End = Len = 1.f;
  Lev = Vel = Pitch = 1.0f;
  Reversed = false;
  Muted = false;
  IsSolo = false;
  Selected = false;
  Wave = 0x0;
  
  Rythms = new list<BeatNote*>[8];
  
  /* Channel Background */
  wxImage* ch_bg = 
    new wxImage(string(DataDir + string(CHANNEL_BG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  if (ch_bg)
    BgBmp = new wxBitmap(ch_bg);
  
  /* Wave label */
  WaveLabel = 
    new wxStaticText(this, -1, "empty", wxPoint(8,5), wxSize(20, 10));
  WaveLabel->SetFont(wxFont(8, wxNORMAL, wxNORMAL, wxNORMAL));
  WaveLabel->SetForegroundColour(*wxWHITE);
  WaveLabel->Show();
  
  
  /* Channels Buttons */
  wxImage* bmp_su = 
    new wxImage(string(DataDir + string(SELECT_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  wxImage* bmp_sd = 
    new wxImage(string(DataDir + string(SELECT_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp_su && bmp_sd)
    SelectionButton = new DownButton( this, BC_Select, 
				      wxPoint(4,180), 
				      wxSize(39,21),
				      bmp_su, bmp_sd, false );
  
  wxImage *bmp_pu = 
    new wxImage(string(DataDir + string(CH_PLAY_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  wxImage *bmp_pd = 
    new wxImage(string(DataDir + string(CH_PLAY_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp_pu && bmp_pd)
    PlayButton = new DownButton( this, BC_OnPlaySound, 
				 wxPoint(21,30), 
				 wxSize(17,16),
				 bmp_pu, bmp_pd, true );
  
  
  
  
  wxImage *bmp1 = 
    new wxImage(string(DataDir + string(CH_LOAD_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  wxImage *bmp2 = 
    new wxImage(string(DataDir + string(CH_LOAD_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)
    new DownButton( this, BC_OnLoadSound, 
		    wxPoint(6,31), 
		    wxSize(14,15),
		    bmp1, bmp2, true );
  
  bmp1 = 
    new wxImage(string(DataDir + string(REV_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  bmp2 = 
    new wxImage(string(DataDir + string(REV_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)
    new DownButton( this, BC_ReverseSound, 
		    wxPoint(15,17), 
		    wxSize(12,14),
		    bmp1, bmp2, false );
  
  bmp1 =
    new wxImage(string(DataDir + string(SOLO_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  bmp2 =
    new wxImage(string(DataDir + string(SOLO_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)  
    SoloButton = 
      new DownButton( this, BC_Solo, wxPoint(26, 17), wxSize(14,14), 
		      bmp1, bmp2, false );

  bmp1 =
    new wxImage(string(DataDir + string(MUTE_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  bmp2 =
    new wxImage(string(DataDir + string(MUTE_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)  
    MuteButton = 
      new DownButton( this, BC_Mute, wxPoint(2,17), wxSize(14,14),
		      bmp1, bmp2, false );
  
  
  bmp1 = new wxImage(string(DataDir + string(CH_KNOB_CENTER)).c_str(),
		     wxBITMAP_TYPE_PNG);
  bmp2 = new wxImage(string(DataDir + string(CH_DOT)).c_str(),
		     wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)
    {
      KnobLev = 
	new HintedKnob(this, BC_Lev, this->GetParent(), bmp1, bmp2, 
		       0, 127, 100, 1,
		       wxPoint(17, 53), wxSize(8,8), 
		       GetPosition() + wxPoint(25,61));
      KnobBal = 
	new HintedKnob(this, BC_Bal, this->GetParent(), bmp1, bmp2, 
		       0, 100, 50, 1,
		       wxPoint(17, 76), wxSize(8,8), 
		       GetPosition() + wxPoint(25,84));
      KnobStart = 
	new HintedKnob(this, BC_Sta, this->GetParent(), bmp1, bmp2, 
		       0, 100, 0, 1,
		       wxPoint(8, 106), wxSize(8,8), 
		       GetPosition() + wxPoint(16,114));
      KnobEnd = 
	new HintedKnob(this, BC_End, this->GetParent(), bmp1, bmp2, 
		       0, 100, 100, 1,
		       wxPoint(24, 106), wxSize(8,8), 
		       GetPosition() + wxPoint(32,114));
      KnobLen = 
	new HintedKnob(this, BC_Len, this->GetParent(), bmp1, bmp2,
		       0, 100, 100, 1,
		       wxPoint(16, 127), wxSize(8,8), 
		       GetPosition() + wxPoint(24,135));
      KnobPitch = 
	new HintedKnob(this, BC_Pit, this->GetParent(), bmp1, bmp2, 
		       0, 127, 100, 1,
		       wxPoint(8, 160), wxSize(8,8), 
		       GetPosition() + wxPoint(16,168));
      KnobVel = 
	new HintedKnob(this, BC_Vel, this->GetParent(), bmp1, bmp2, 
		       0, 127, 100, 1,
		       wxPoint(25, 160), wxSize(8,8), 
		       GetPosition() + wxPoint(33,168));
    }
}

BeatBoxChannel::~BeatBoxChannel()
{
  for (unsigned char i = 0; i < 8; i++)
    {
      for (list<BeatNote*>::iterator bn = Rythms[i].begin();
	   bn != Rythms[i].end(); bn = Rythms[i].erase(bn))
	delete *bn;
      Rythms[i].clear();
    }
  delete [] Rythms;
  
  delete BgBmp;
  delete PlayButton;
  
  if (Wave)
    delete Wave;
  delete SelectionButton;
  delete [] Data;
}

void BeatBoxChannel::OnLevChange(wxScrollEvent& WXUNUSED(event))
{
  float mlevel = static_cast<float>(KnobLev->GetValue()/100.0f);
  PatternMutex->Lock();
  Lev = mlevel;
  PatternMutex->Unlock();
}

void BeatBoxChannel::OnBalChange(wxScrollEvent& WXUNUSED(event))
{
}


void BeatBoxChannel::OnStartChange(wxScrollEvent& WXUNUSED(event))
{
  float start = static_cast<float>(KnobStart->GetValue()/100.0f);
  PatternMutex->Lock();
  Start = start;
  PatternMutex->Unlock();

}

void BeatBoxChannel::OnEndChange(wxScrollEvent& WXUNUSED(event))
{
  float end = static_cast<float>(KnobEnd->GetValue()/100.0f);
  PatternMutex->Lock();
  End = end;
  PatternMutex->Unlock();

}

void BeatBoxChannel::OnLenChange(wxScrollEvent& WXUNUSED(event))
{
  float len = static_cast<float>(KnobLen->GetValue()/100.0f);
  PatternMutex->Lock();
  Len = len;
  PatternMutex->Unlock();
  
}

void BeatBoxChannel::OnPitchChange(wxScrollEvent& WXUNUSED(event))
{
  float pitch = static_cast<float>(KnobPitch->GetValue()/100.0f);
  PatternMutex->Lock();
  Pitch = pitch;
  PatternMutex->Unlock();

}

void BeatBoxChannel::OnVelChange(wxScrollEvent& WXUNUSED(event))
{
  float vel = static_cast<float>(KnobVel->GetValue()/100.0f);
  PatternMutex->Lock();
  Vel = vel;
  PatternMutex->Unlock();
}

void BeatBoxChannel::OnSelectChannel(wxCommandEvent& WXUNUSED(e))
{
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  Data[1] = ACT_SELECT;
  event.SetClientData((void*)Data);
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
}

void BeatBoxChannel::OnLoadSound(wxCommandEvent& WXUNUSED(e))
{
  FileLoader *dlg = 
    new FileLoader(this, -1, "Loading sound file", false, false, NULL);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();
      dlg->Destroy();
      WaveFile *w;
      
      wxProgressDialog *Progress = 
	new wxProgressDialog("Loading wave file", "Please wait...", 
			     100, this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT
			     | wxPD_REMAINING_TIME);
      Progress->Update(1);
      try
	{
	  w = new WaveFile(selfile, true);
          Progress->Update(60);
          SetWaveFile(w);
          Progress->Update(99);
	}
      catch (...)
	{
	  cout << "[BEATBOX] Cannot open wave file !" << endl;
	}
      delete Progress;
    }
  else
    dlg->Destroy();
  
}

void BeatBoxChannel::ReverseSound(wxCommandEvent& WXUNUSED(e))
{
  PatternMutex->Lock();
  Reversed = !Reversed;
  PatternMutex->Unlock();
}

void BeatBoxChannel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
 wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*BgBmp);    
  wxRegionIterator upd(GetUpdateRegion());
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, 
	      upd.GetX(), upd.GetY(), wxCOPY, FALSE);      
      upd++;
    }
}

void BeatBoxChannel::OnPlaySound(wxCommandEvent& WXUNUSED(e))
{
  PatternMutex->Lock();
  if (!Wave)
    {
      PatternMutex->Unlock();
      return;
    }
  PatternMutex->Unlock();
  
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  Data[1] = ACT_PLAY;
  event.SetClientData((void*)Data);
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
}

void BeatBoxChannel::OnMute(wxCommandEvent& WXUNUSED(e))
{
  PatternMutex->Lock();
  //bool Muted is used by BeatBox::Process
  Muted = !Muted;
  PatternMutex->Unlock();
}

void BeatBoxChannel::OnSolo(wxCommandEvent& WXUNUSED(e))
{
  PatternMutex->Lock();
  //bool IsSolo is used by BeatBox::Process
  if (IsSolo)
    {
      UnSolo();
      PatternMutex->Unlock();
      return;
    }
  PatternMutex->Unlock();
  
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  Data[1] = ACT_SOLO;
  event.SetClientData((void*)Data);
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
  
}

void BeatBoxChannel::Solo()
{
  SoloButton->SetOn();
  IsSolo = true;
}

void BeatBoxChannel::UnSolo()
{
  SoloButton->SetOff();
  IsSolo = false;
}

void BeatBoxChannel::Mute(void)
{
  MuteButton->SetOn();
  Muted = true;
}

void BeatBoxChannel::UnMute(void)
{
  MuteButton->SetOff();
  Muted = false;
}

void BeatBoxChannel::SetWaveFile(WaveFile* wave)
{
  wxFileName fn(wave->Filename.c_str());
  wxString st = fn.GetName();
  st.Truncate(5);
  WaveLabel->SetLabel(st);
  
  PatternMutex->Lock();
  if (Wave)
    delete Wave;
  Wave = wave;
  PatternMutex->Unlock();
  
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  Data[1] = ACT_SETWAVE;
  event.SetClientData((void*)Data);
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
}

void BeatBoxChannel::Select(void)
{
  SelectionButton->SetOn();
}

void BeatBoxChannel::DeSelect(void)
{
  SelectionButton->SetOff();
}

void BeatBoxChannel::Reset(void)
{
  for (unsigned char ps = 0; ps < 8; ps++)
    {
      for (list<BeatNote*>::iterator bn = Rythms[ps].begin(); 
	   bn != Rythms[ps].end();)
	{
	  delete *bn;
	  bn = Rythms[ps].erase(bn);
	}
      Rythms[ps].clear();
    }
  if (Wave)
    delete Wave;
  Wave = 0x0;
}
