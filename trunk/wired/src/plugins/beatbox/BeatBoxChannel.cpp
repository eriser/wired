#include <wx/progdlg.h>

#include "BeatBoxChannel.h"
#include "BeatBox.h"
//#include "FileLoader.h"
#include <wx/filename.h>
#include <math.h>

BEGIN_EVENT_TABLE(BeatBoxChannel, wxWindow)
  EVT_PAINT(BeatBoxChannel::OnPaint)
  
  EVT_LEFT_DOWN(BeatBoxChannel::OnMouseEvent)
  EVT_MOTION(BeatBoxChannel::OnMouseEvent)
  EVT_ENTER_WINDOW(BeatBoxChannel::OnEnterWindow)
  EVT_LEAVE_WINDOW(BeatBoxChannel::OnLeave)
  
  EVT_COMMAND_SCROLL(BC_Lev, BeatBoxChannel::OnLevChange)
  EVT_COMMAND_SCROLL(BC_Pan, BeatBoxChannel::OnPanChange)
  EVT_COMMAND_SCROLL(BC_Sta, BeatBoxChannel::OnStartChange)
  EVT_COMMAND_SCROLL(BC_End, BeatBoxChannel::OnEndChange)
  EVT_COMMAND_SCROLL(BC_Pit, BeatBoxChannel::OnPitchChange)
  EVT_COMMAND_SCROLL(BC_Vel, BeatBoxChannel::OnVelChange)
  
  EVT_BUTTON(BC_Pol, BeatBoxChannel::OnPolyphonyChange)
  EVT_BUTTON(BC_Select, BeatBoxChannel::OnSelectChannel)
  EVT_BUTTON(BC_LoadSound, BeatBoxChannel::OnLoadSound)
  EVT_BUTTON(BC_ReverseSound, BeatBoxChannel::ReverseSound)
  EVT_BUTTON(BC_Mute, BeatBoxChannel::OnMute)
  EVT_BUTTON(BC_Solo, BeatBoxChannel::OnSolo)
  EVT_BUTTON(BC_PlaySound, BeatBoxChannel::OnPlaySound)
END_EVENT_TABLE()


BeatBoxChannel::BeatBoxChannel( wxWindow *parent, wxWindowID id, 
				const wxPoint &pos, const wxSize &size, 
				unsigned int num, WiredBeatBox* drm31)//string datadir, wxMutex* m )
  : wxWindow( parent, id, pos, size )
{
  Data = new unsigned int[2];
  Id = num;
  
  Data[0] = num; /* Channel number 0-11 */
  Data[1] = 0;
  
  DataDir = drm31->GetDataDir();
  PatternMutex = drm31->GetMutexPtr();
  DRM31 = drm31;
  
  Params[LEV] = 1.0f;
  Params[VEL] = 1.0f;
  Params[PIT] = 1.0f;
  Params[PAN] = 0.5f;
  Params[STA] = 0.0f;
  Params[END] = 1.0f;
  
  //Start = 0.f;
  //End = 1.f;
  //Lev = Vel = Pitch = 1.0f;
  //Pan[0] = Pan[1] = 1.0f;
  Reversed = false;
  Muted = false;
  IsSolo = false;
  Selected = false;
  Wave = 0x0;
  
  Rythms = new list<BeatNote*>*[5];
  Rythms[0] = new list<BeatNote*>[8];
  Rythms[1] = new list<BeatNote*>[8];
  Rythms[2] = new list<BeatNote*>[8];
  Rythms[3] = new list<BeatNote*>[8];
  Rythms[4] = new list<BeatNote*>[8];
  
  /* Channel Background */
  wxImage* ch_bg = 
    new wxImage(string(DataDir + string(CHANNEL_BG)).c_str(), 
		wxBITMAP_TYPE_PNG);
  if (ch_bg)
    BgBmp = new wxBitmap(ch_bg);
  
  /* Wave label */
  WaveLabel = 
    new wxStaticText(this, -1, "empty", wxPoint(10,5), wxSize(25, 10), 
		     wxALIGN_RIGHT);
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
				      wxPoint(4,251), 
				      wxSize(39,21),
				      bmp_su, bmp_sd, false );
  
  wxImage *bmp_pu = 
    new wxImage(string(DataDir + string(CH_PLAY_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  wxImage *bmp_pd = 
    new wxImage(string(DataDir + string(CH_PLAY_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp_pu && bmp_pd)
    PlayButton = new DownButton( this, BC_PlaySound, 
				 wxPoint(28,34), 
				 wxSize(21,18),
				 bmp_pu, bmp_pd, true );
  
  wxImage *bmp1 = 
    new wxImage(string(DataDir + string(CH_LOAD_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  wxImage *bmp2 = 
    new wxImage(string(DataDir + string(CH_LOAD_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)
    new DownButton( this, BC_LoadSound, 
		    wxPoint(2,34), 
		    wxSize(14,15),
		    bmp1, bmp2, true );
  
  bmp1 = 
    new wxImage(string(DataDir + string(REV_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  bmp2 = 
    new wxImage(string(DataDir + string(REV_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)
    ReverseButton =
      new DownButton( this, BC_ReverseSound, 
		      wxPoint(18,18), 
		      wxSize(15,15),
		      bmp1, bmp2, false );
  
  bmp1 =
    new wxImage(string(DataDir + string(SOLO_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  bmp2 =
    new wxImage(string(DataDir + string(SOLO_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)  
    SoloButton = 
      new DownButton( this, BC_Solo, wxPoint(33, 18), wxSize(14,14), 
		      bmp1, bmp2, false );
  
  bmp1 =
    new wxImage(string(DataDir + string(MUTE_UP)).c_str(),
		wxBITMAP_TYPE_PNG);
  bmp2 =
    new wxImage(string(DataDir + string(MUTE_DO)).c_str(),
		wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)  
    MuteButton = 
      new DownButton( this, BC_Mute, wxPoint(1,18), wxSize(17,16),
		      bmp1, bmp2, false );
  
  bmp1 = new wxImage(string(DataDir + string(CH_KNOB_LEV_CENTER)).c_str(),
		     wxBITMAP_TYPE_PNG);
  bmp2 = new wxImage(string(DataDir + string(CH_LEV_DOT)).c_str(),
		     wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)
    KnobLev = 
      new KnobCtrl(this, BC_Lev, bmp1, bmp2, 
		   0, 127, 100, 1,
		   wxPoint(17,59), wxSize(16,16), 
		   this->GetParent(), GetPosition());
  
  bmp1 = new wxImage(string(DataDir + string(CH_KNOB_PAN_CENTER)).c_str(),
		     wxBITMAP_TYPE_PNG);
  
  if (bmp1 && bmp2)
    KnobPan = 
      new KnobCtrl(this, BC_Pan, bmp1, bmp2, 
		   0, 100, 50, 1,
		   wxPoint(18,92), wxSize(15,16), 
		   this->GetParent(), GetPosition());
  
  bmp1 = new wxImage(string(DataDir + string(CH_KNOB_CENTER)).c_str(),
		     wxBITMAP_TYPE_PNG);
  if (bmp1 && bmp2)
    {
      KnobStart = 
	new KnobCtrl(this, BC_Sta, bmp1, bmp2, 
		     0, 100, 0, 1,
		     wxPoint(8, 138), wxSize(9,9), 
		     this->GetParent(), GetPosition());
      KnobStart->SetValue(0);
      
      KnobEnd = 
	new KnobCtrl(this, BC_End, bmp1, bmp2, 
		     0, 100, 100, 1,
		     wxPoint(34,138), wxSize(9,9), 
		     this->GetParent(), GetPosition());
      KnobPitch = 
	new KnobCtrl(this, BC_Pit, bmp1, bmp2, 
		       0, 127, 100, 1,
		       wxPoint(35, 215), wxSize(9,9), 
		     this->GetParent(), GetPosition());
      KnobVel = 
	new KnobCtrl(this, BC_Vel, bmp1, bmp2, 
		     0, 127, 100, 1,
		     wxPoint(6, 235), wxSize(9,9), 
		     this->GetParent(), GetPosition());
    }
  
  wxImage** imgs_;
  imgs_ = new wxImage*[3];
  imgs_[0] = new wxImage(_T(string(DataDir + string(CH_POLY1)).c_str()));
  imgs_[1] = new wxImage(_T(string(DataDir + string(CH_POLY2)).c_str()));
  imgs_[2] = new wxImage(_T(string(DataDir + string(CH_POLY3)).c_str()));
  

  //Voices = new Polyphony();
  Voices = 8;
  PolyKnob = new CycleKnob(this, BC_Pol, 3, imgs_, 10, 1, 99, 8,
			   wxPoint(4, 175), wxDefaultSize);
  VoicesLabel = new wxStaticText(this, -1, "8", 
				 wxPoint(25,180), wxSize(16,8), wxALIGN_RIGHT);
  VoicesLabel->SetFont(wxFont(8, wxBOLD, wxBOLD, wxBOLD));
  VoicesLabel->SetForegroundColour(*wxWHITE);
  VoicesLabel->Show();

  /* Help */
  
  /*
    Connect(GetId(), wxEVT_ENTER_WINDOW,
    (wxObjectEventFunction)(wxEventFunction) 
    (wxMouseEventFunction)&BeatBoxChannel::OnEnterWindow);
  */
  
  Connect(BC_Select, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnSelectHelp);
  Connect(BC_LoadSound, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnLoadHelp);
  Connect(BC_ReverseSound, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnReverseHelp);
  Connect(BC_Mute, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnMuteHelp);
  Connect(BC_Solo, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnSoloHelp);
  Connect(BC_PlaySound, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnPlayHelp);
  Connect(BC_Lev, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnLevHelp);
  Connect(BC_Pan, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnPanHelp);
  Connect(BC_Sta, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnStaHelp);
  Connect(BC_End, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnEndHelp);
  Connect(BC_Pit, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnPitHelp);
  Connect(BC_Vel, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnVelHelp);
  Connect(BC_Pol, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnPolHelp);
  
  // Gui updating relatives
  AskUpdateChannel	= false;
  AskUpdatePlay		= false;
  AskUpdateMute		= false;
  AskUpdateSolo		= false;
  AskUpdateLevel	= false;
  AskUpdatePan		= false;
  AskUpdateVel		= false;
  AskUpdatePitch	= false;
  AskUpdateStart	= false;
  AskUpdateEnd		= false;
  
  

  // Midi controls
  
  MidiVolume[0] = -1;
  MidiVolume[1] = 100;
  MidiPan[0] = -1;
  MidiPan[1] = 0;
  MidiStart[0] = -1;
  MidiStart[1] = 0;
  MidiEnd[0] = -1;
  MidiEnd[1] = 0;
  MidiPoly[0] = -1;
  MidiPoly[1] = 0;
  MidiPitch[0] = -1;
  MidiPitch[1] = 100;
  MidiVel[0] = -1;
  MidiVel[1] = 100;
  
  Connect(BC_Lev, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnLevController);
  Connect(BC_Pan, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnPanController);
  Connect(BC_Sta, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnStartController);
  Connect(BC_End, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnEndController);
  Connect(BC_Pit, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnPitchController);
  Connect(BC_Vel, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnVelController);
  Connect(BC_Pol, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&BeatBoxChannel::OnPolyController);
}

void BeatBoxChannel::OnLevController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (DRM31->ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      DRM31->CheckExistingControllerData(midi_data);      
      MidiVolume[0] = midi_data[0];
      MidiVolume[1] = midi_data[1];

      //Mutex.Unlock();
    }
  delete midi_data;
}

void BeatBoxChannel::OnPanController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (DRM31->ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      DRM31->CheckExistingControllerData(midi_data);      
      MidiPan[0] = midi_data[0];
      MidiPan[1] = midi_data[1];

      //Mutex.Unlock();
    }
  delete midi_data;
}

void BeatBoxChannel::OnStartController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (DRM31->ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      DRM31->CheckExistingControllerData(midi_data);      
      MidiStart[0] = midi_data[0];
      MidiStart[1] = midi_data[1];

      //Mutex.Unlock();
    }
  delete midi_data;
}

void BeatBoxChannel::OnEndController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (DRM31->ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      DRM31->CheckExistingControllerData(midi_data);      
      MidiEnd[0] = midi_data[0];
      MidiEnd[1] = midi_data[1];

      //Mutex.Unlock();
    }
  delete midi_data;
}

void BeatBoxChannel::OnPitchController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (DRM31->ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      DRM31->CheckExistingControllerData(midi_data);      
      MidiPitch[0] = midi_data[0];
      MidiPitch[1] = midi_data[1];
      //Mutex.Unlock();
    }
  delete midi_data;
}

void BeatBoxChannel::OnVelController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (DRM31->ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      DRM31->CheckExistingControllerData(midi_data);      
      MidiVel[0] = midi_data[0];
      MidiVel[1] = midi_data[1];

      //Mutex.Unlock();
    }
  delete midi_data;
}

void BeatBoxChannel::OnPolyController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (DRM31->ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      DRM31->CheckExistingControllerData(midi_data);      
      MidiPoly[0] = midi_data[0];
      MidiPoly[1] = midi_data[1];
      //Mutex.Unlock();
    }
  delete midi_data;
}


BeatBoxChannel::~BeatBoxChannel()
{
  for (unsigned char p = 5; p < 5; p++)
    {
      for (unsigned char i = 0; i < 8; i++)
	{
	  for (list<BeatNote*>::iterator bn = Rythms[p][i].begin();
	       bn != Rythms[p][i].end(); bn = Rythms[p][i].erase(bn))
	    delete *bn;
	  Rythms[p][i].clear();
	}
      delete [] Rythms[p];
    }
  delete [] Rythms;
  
  delete BgBmp;
  delete PlayButton;
  
  if (Wave)
    delete Wave;
  delete SelectionButton;
  delete [] Data;
  
  //  delete Voices;
}

void BeatBoxChannel::OnLevChange(wxScrollEvent& WXUNUSED(event))
{
  float mlevel = static_cast<float>(KnobLev->GetValue()/100.0f);
  //PatternMutex->Lock();
  //Lev = mlevel;
  Params[LEV] = mlevel;
  //PatternMutex->Unlock();
}

void BeatBoxChannel::OnPanChange(wxScrollEvent& WXUNUSED(event))
{
  int val = KnobPan->GetValue();
  float panl;
  float panr;
  float pan = KnobPan->GetValue() / 100.0f;
  /*
  if (val == 50)
    panl = panr = 1.f;
  else if (val < 50)
    {
      panl = 1.f;
      panr = val * 2 / 100.f;
    }
  else
    {
      panr = 1.f;
      panl = (100 - ((val-50)*2)) / 100.f;
    }
  */
  //PatternMutex->Lock();
  Params[PAN] = pan;
  //Pan[0] = panl;
  //Pan[1] = panr;
  //PatternMutex->Unlock();
}


void BeatBoxChannel::OnStartChange(wxScrollEvent& WXUNUSED(event))
{
  float start = static_cast<float>(KnobStart->GetValue()/100.0f);
  //PatternMutex->Lock();
  Params[STA] = start;
  //Start = start;
  //PatternMutex->Unlock();

}

void BeatBoxChannel::OnEndChange(wxScrollEvent& WXUNUSED(event))
{
  float end = static_cast<float>(KnobEnd->GetValue()/100.0f);
  //PatternMutex->Lock();
  Params[END] = end;
  //  PatternMutex->Unlock();

}

void BeatBoxChannel::OnPitchChange(wxScrollEvent& WXUNUSED(event))
{
  float pitch = static_cast<float>(KnobPitch->GetValue()/100.0f);
  //PatternMutex->Lock();
  Params[PIT] = pitch;
  //Pitch = pitch;
  //PatternMutex->Unlock();

}

void BeatBoxChannel::OnVelChange(wxScrollEvent& WXUNUSED(event))
{
  float vel = static_cast<float>(KnobVel->GetValue()/100.0f);
  //PatternMutex->Lock();
  Params[VEL] = vel;
  //Vel = vel;
  //PatternMutex->Unlock();
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
  string selfile = DRM31->OpenFileLoader("Loading sound file", 0x0);
  if (!selfile.empty())
    {
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
    {
      cout << "[BEATBOX] Cannot open wave file !" << endl;
    }
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
  //PatternMutex->Lock();
  if (!Wave)
    {
      //PatternMutex->Unlock();
      return;
    }
  //PatternMutex->Unlock();
  
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
  //bool IsSolo is no more used by BeatBox::Process
  IsSolo = !IsSolo;
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
  WaveFile* tmp = 0x0;
  wxFileName fn(wave->Filename.c_str());
  wxString st = fn.GetName();
  st.Truncate(5);
  WaveLabel->SetLabel(st);
  
  
  PatternMutex->Lock();
  tmp = Wave;//delete Wave;
  Wave = wave;
  PatternMutex->Unlock();
  
  if (tmp)
    delete tmp;

  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  Data[1] = ACT_SETWAVE;
  event.SetClientData((void*)Data);
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
}

void BeatBoxChannel::OnPolyphonyChange(wxCommandEvent& WXUNUSED(e))
{
  unsigned int n = PolyKnob->GetValue();
  wxString s;
  s.Printf("%d", n);
  VoicesLabel->SetLabel(s);
  PatternMutex->Lock();
  Voices = n;
  DRM31->SetVoices();
  PatternMutex->Unlock();
}

void BeatBoxChannel::SetMute()
{
  //PatternMutex->Lock();
  Muted = true;
  //PatternMutex->Unlock();
  
  AskUpdateChannel = true;
  AskUpdateMute = true;
  DRM31->AskUpdate();
}

void BeatBoxChannel::SetUnMute()
{
  //PatternMutex->Lock();
  Muted = false;
  //PatternMutex->Unlock();
  
  AskUpdateChannel = true;
  AskUpdateMute = true;
  DRM31->AskUpdate();
}


void BeatBoxChannel::SetPlay()
{
  AskUpdateChannel = true;
  AskUpdatePlay = true;
  DRM31->AskUpdate();
}

void BeatBoxChannel::SetSolo()
{
  PatternMutex->Lock();
  IsSolo != IsSolo;
  if (!IsSolo)
    for (int i = 0; i < NB_CHAN; i++)
      DRM31->Channels[i]->SetUnMute();
  else 
    {
      for (int i = 0; i < NB_CHAN; i++)
	DRM31->Channels[i]->SetMute();
      SetUnMute();
    }
  PatternMutex->Unlock();
  
  
  /*
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  Data[1] = ACT_SOLO;
  event.SetClientData((void*)Data);
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
  */
  AskUpdateChannel = true;
  AskUpdateSolo = true;
  DRM31->AskUpdate();
}

void BeatBoxChannel::SetLev(int lev)
{
  float mlevel = lev / 100.0f;
  PatternMutex->Lock();
  Params[LEV] = mlevel;
  PatternMutex->Unlock();
  MidiVolume[2] = lev;
  
  AskUpdateChannel = true;
  AskUpdateLevel = true;
  DRM31->AskUpdate();
  
  //KnobLev->SetValue(lev);
}

void BeatBoxChannel::SetPan(int pan)
{
  float fpan = pan / 100.0f;
  PatternMutex->Lock();
  Params[PAN] = fpan;
  PatternMutex->Unlock();
  
  AskUpdateChannel = true;
  AskUpdatePan = true;
  DRM31->AskUpdate();

  //KnobPan->SetValue(pan);
}

void BeatBoxChannel::SetStart(int start)
{
  float s = start / 100.f;
  PatternMutex->Lock();
  Params[STA] = s;
  PatternMutex->Unlock();
  
  AskUpdateChannel = true;
  AskUpdateStart = true;
  DRM31->AskUpdate();
  
  //KnobStart->SetValue(start);
}


void BeatBoxChannel::SetEnd(int end)
{
  float e = end/100.f;
  PatternMutex->Lock();
  Params[END] = e;
  PatternMutex->Unlock();
  
  AskUpdateChannel = true;
  AskUpdateEnd = true;
  DRM31->AskUpdate();
  //KnobEnd->SetValue(end);
}

void BeatBoxChannel::SetPitch(int pitch)
{
  float p = pitch/100.f;
  PatternMutex->Lock();
  Params[PIT] = p;
  PatternMutex->Unlock();
  
  AskUpdateChannel = true;
  AskUpdatePitch = true;
  DRM31->AskUpdate();

  //KnobPitch->SetValue(pitch);
}

void BeatBoxChannel::SetVel(int vel)
{
  float v = vel/100.f;
  PatternMutex->Lock();
  Params[VEL] = v;
  PatternMutex->Unlock();
  
  AskUpdateChannel = true;
  AskUpdateVel = true;
  DRM31->AskUpdate();

  //KnobVel->SetValue(vel);
}

void BeatBoxChannel::SetPolyphony(int voices)
{
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
  for (unsigned char b = 0; b < 5; b++)
    {
      for (unsigned char ps = 0; ps < 8; ps++)
	{
	  for (list<BeatNote*>::iterator bn = Rythms[b][ps].begin(); 
	       bn != Rythms[b][ps].end();)
	    {
	      delete *bn;
	      bn = Rythms[b][ps].erase(bn);
	    }
	  Rythms[b][ps].clear();
	}
    }
  if (Wave)
    delete Wave;
  Wave = 0x0;
}

void BeatBoxChannel::Update()
{
  AskUpdateChannel = false;
  
  if (AskUpdatePlay)
    ;
  if (AskUpdateMute)
    {
      AskUpdateMute = false;
      if (Muted)
	MuteButton->SetOn();
      else
	MuteButton->SetOff();
    }
  if (AskUpdateSolo)
    {
      AskUpdateSolo = false;
      if (IsSolo)
	SoloButton->SetOn();
      else
	SoloButton->SetOff();
    }
  if (AskUpdateLevel)
    {
      KnobLev->SetValue(MidiVolume[2]);
    }
  if (AskUpdatePan)
    {
      KnobPan->SetValue(MidiPan[2]);
    }
  if (AskUpdateVel)
    {
      KnobVel->SetValue(MidiVel[2]);
    }
  if (AskUpdatePitch)
    {
      KnobPitch->SetValue(MidiPitch[2]);
    }
  if (AskUpdateStart)
    {
      KnobStart->SetValue(MidiStart[2]);
    }
  if (AskUpdateEnd)
    {
      KnobEnd->SetValue(MidiEnd[2]);
    }
}

void BeatBoxChannel::UpdateGui()
{
  if (IsSolo)
    SoloButton->SetOn();
  else
    SoloButton->SetOff();
  if (Muted)
    SoloButton->SetOn();
  else
    SoloButton->SetOff();
  if (Reversed)
    ReverseButton->SetOn();
  else
    ReverseButton->SetOff();
  
  KnobLev->SetValue((int)floor(Params[LEV] * 100));
  KnobVel->SetValue((int)floor(Params[VEL] * 100));
  KnobPitch->SetValue((int)floor(Params[PIT] * 100));
  KnobVel->SetValue((int)floor(Params[PAN] * 100));
  KnobVel->SetValue((int)floor(Params[STA] * 100));
  KnobVel->SetValue((int)floor(Params[END] * 100));
  KnobVel->SetValue((int)floor(Params[VEL] * 100));
  
  PolyKnob->SetValue(Voices);
  wxString s;
  s.Printf("%d", Voices);
  VoicesLabel->SetLabel(s);
  
  DeSelect();
}

void BeatBoxChannel::OnMouseEvent(wxMouseEvent& event)
{
  wxPostEvent(GetParent(), event);
}

void BeatBoxChannel::OnEnterWindow(wxMouseEvent& event)
{ 
  wxPostEvent(GetParent(), event);
}

void BeatBoxChannel::OnLeave(wxMouseEvent& event)
{
  wxPostEvent(GetParent(), event);
}

void BeatBoxChannel::OnSelectHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Use this button to display and edit the tracker associated to this channel");
}

void BeatBoxChannel::OnLoadHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Use this button to open the sound file browser");
}

void BeatBoxChannel::OnReverseHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Invert playing order of samples");
}

void BeatBoxChannel::OnMuteHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Mute the channel");
}

void BeatBoxChannel::OnSoloHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Mute all other channels");
}

void BeatBoxChannel::OnPlayHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Play the sound instantaneously");
}

void BeatBoxChannel::OnLevHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Set level of the channel");
}

void BeatBoxChannel::OnPanHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Pan left - right");
}

void BeatBoxChannel::OnStaHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Set the starting point of the playing sound");
}

void BeatBoxChannel::OnEndHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Set the ending point of the playing sound");
}

void BeatBoxChannel::OnPitHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Set channel's pitch");
}

void BeatBoxChannel::OnVelHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Set channel's velocity");
}

void BeatBoxChannel::OnPolHelp(wxMouseEvent& event)
{
  if (DRM31->HelpMode)
    DRM31->SendHelp("Set the number of voices for this channel");
}
