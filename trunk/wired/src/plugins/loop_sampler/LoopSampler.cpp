// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "LoopSampler.h"
#include "midi.h"
#include "BeatDialog.h"
#include <stdio.h>
#include <wx/progdlg.h>
#include <math.h>

static PlugInitInfo info;

BEGIN_EVENT_TABLE(LoopSampler, wxWindow)
  EVT_PAINT(LoopSampler::OnPaint)
  EVT_BUTTON(LoopSampler_ShowOpt, LoopSampler::OnShowView)
  EVT_BUTTON(LoopSampler_Open, LoopSampler::OnOpenFile)
  EVT_BUTTON(LoopSampler_Save, LoopSampler::OnSaveFile)
  EVT_BUTTON(LoopSampler_ToSeqTrack, LoopSampler::OnToSeqTrack)
  EVT_BUTTON(LoopSampler_Play, LoopSampler::OnPlay)
  EVT_BUTTON(LoopSampler_MesUp, LoopSampler::OnMesUp)
  EVT_BUTTON(LoopSampler_MesDown, LoopSampler::OnMesDown)
  EVT_BUTTON(LoopSampler_PolyUp, LoopSampler::OnPolyUp)
  EVT_BUTTON(LoopSampler_PolyDown, LoopSampler::OnPolyDown)
  EVT_BUTTON(LoopSampler_Invert, LoopSampler::OnInvert)
  EVT_BUTTON(LoopSampler_Tempo, LoopSampler::OnTempo)
  EVT_BUTTON(LoopSampler_Octave, LoopSampler::OnOctave)
  EVT_COMMAND_SCROLL(LoopSampler_Pitch, LoopSampler::OnPitch)
  EVT_COMMAND_SCROLL(LoopSampler_Volume, LoopSampler::OnVolume)
  EVT_COMMAND_SCROLL(LoopSampler_Attack, LoopSampler::OnAttack)
  EVT_ENTER_WINDOW(LoopSampler::OnHelp)
END_EVENT_TABLE()


LoopSampler::LoopSampler(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)    
{
  HelpMode = false;
  BeatCount = GetSigNumerator();
  BarCount = ((double)BeatCount / (double)GetSigNumerator());
  PolyphonyCount = 7;
  Playing = false;
  AutoPlaying = false;
  View = 0x0;
  Wave = 0x0;
  Octave = 1.f;
  Pitch = 1.f;  
  Invert = false;
  Tempo = false;
  Volume = 1.f;
  Attack = 1.f;
  Decay = 1.f;
  Sustain = 1.f;
  Release = 1.f;

  AttackMs = 0.f;
  AttackCoef = 0.f;
  AttackLen = 0;

  ReleaseCoef = expf(logf(0.01f) / (0.1 * 44100.f * 0.001f));
  Envelope = 0.0f;

  read_buf = 0x0;

  LoopInfo.Start = 0;
  LoopInfo.End = 0;

  /* Graphic control initialization */

  bmp = new wxBitmap(string(GetDataDir() + string(IMG_LS_BMP)).c_str(), wxBITMAP_TYPE_BMP);  
  SetBackgroundColour(wxColour(200, 200, 200));

  ls_bg = new wxImage(string(GetDataDir() + string(IMG_LS_BG)).c_str(), wxBITMAP_TYPE_PNG);
  if (ls_bg)
    BgBmp = new wxBitmap(ls_bg);

  play_up = new wxImage(string(GetDataDir() + string(IMG_LS_PLAY_UP)).c_str(), wxBITMAP_TYPE_PNG);
  play_down = new wxImage(string(GetDataDir() + string(IMG_LS_PLAY_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  open_up = new wxImage(string(GetDataDir() + string(IMG_LS_OPEN_UP)).c_str(), wxBITMAP_TYPE_PNG);
  open_down = new wxImage(string(GetDataDir() + string(IMG_LS_OPEN_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  save_up = new wxImage(string(GetDataDir() + string(IMG_LS_SAVE_UP)).c_str(), wxBITMAP_TYPE_PNG);
  save_down = new wxImage(string(GetDataDir() + string(IMG_LS_SAVE_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  seq_up =  new wxImage(string(GetDataDir() + string(IMG_LS_SEQ_UP)).c_str(), wxBITMAP_TYPE_PNG);
  seq_down =  new wxImage(string(GetDataDir() + string(IMG_LS_SEQ_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  opt_up =  new wxImage(string(GetDataDir() + string(IMG_LS_OPT_UP)).c_str(), wxBITMAP_TYPE_PNG);
  opt_down =  new wxImage(string(GetDataDir() + string(IMG_LS_OPT_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  up_up = new wxImage(string(GetDataDir() + string(IMG_LS_UPUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  up_down = new wxImage(string(GetDataDir() + string(IMG_LS_UPDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  down_up = new wxImage(string(GetDataDir() + string(IMG_LS_DOWNUP_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  down_down = new wxImage(string(GetDataDir() + string(IMG_LS_DOWNDO_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  btn_up = new wxImage(string(GetDataDir() + string(IMG_LS_BTN_ON_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  btn_down = new wxImage(string(GetDataDir() + string(IMG_LS_BTN_DOWN_IMG)).c_str(), wxBITMAP_TYPE_PNG);
  fader_bg = new wxImage(string(GetDataDir() + string(IMG_LS_FADER_BG)).c_str(), wxBITMAP_TYPE_PNG);
  fader_fg = new wxImage(string(GetDataDir() + string(IMG_LS_FADER_FG)).c_str(), wxBITMAP_TYPE_PNG);
  fader_vol_bg = new wxImage(string(GetDataDir() + string(IMG_LS_FADER_VOL_BG)).c_str(), wxBITMAP_TYPE_PNG);
  fader_vol_fg = new wxImage(string(GetDataDir() + string(IMG_LS_FADER_VOL_FG)).c_str(), wxBITMAP_TYPE_PNG);
  knob_bg = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_BG)).c_str(), wxBITMAP_TYPE_PNG);
  knob_fg = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_FG)).c_str(), wxBITMAP_TYPE_PNG);
  
  /* Toolbar Gauche */
    
  ShowOptBtn = new DownButton(this, LoopSampler_ShowOpt, 
			      wxPoint(346, 39), wxSize(27, 28), opt_up, opt_down, true);
  PlayBtn = new DownButton(this, LoopSampler_Play, 
			   wxPoint(219, 39), wxSize(27, 28), play_up, play_down, false);
  ToSeqTrackBtn = new DownButton(this, LoopSampler_ToSeqTrack, 
				 wxPoint(314, 39), wxSize(27, 28), seq_up, seq_down, true);
  SaveBtn = new DownButton(this, LoopSampler_Save, 
			   wxPoint(283, 39), wxSize(27, 28), save_up, save_down, true);
  OpenBtn = new DownButton(this, LoopSampler_Open, 
			   wxPoint(251, 39), wxSize(27, 28), open_up, open_down, true);

  /* Toolbar Haut */
  

  wxString s;

  s.Printf("%d", BeatCount);

  MesCountLabel = new wxStaticText(this, -1, s, wxPoint(288, 87), wxSize(-1, 12));
  MesCountLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));

  MesUpBtn = new HoldButton(this, LoopSampler_MesUp, wxPoint(268, 86), wxSize(13, 9), 
			    up_up, up_down);
  MesDownBtn = new HoldButton(this, LoopSampler_MesDown, wxPoint(268, 96), wxSize(13, 9), 
			      down_up, down_down);

  s.Printf("%d", PolyphonyCount);
  PolyCountLabel = new wxStaticText(this, -1, s, wxPoint(288, 111), wxSize(-1, 12));
  PolyCountLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  PolyCountLabel->SetLabel(s);

  PolyUpBtn = new HoldButton(this, LoopSampler_PolyUp, wxPoint(268, 110), wxSize(13, 9), 
			    up_up, up_down);
  PolyDownBtn = new HoldButton(this, LoopSampler_PolyDown, wxPoint(268, 120), wxSize(13, 9), 
			      down_up, down_down);

  LedOff = new wxBitmap(wxImage(string(GetDataDir() + string(IMG_LS_LED_OFF_IMG)).c_str(), wxBITMAP_TYPE_PNG));
  LedOn = new wxBitmap(wxImage(string(GetDataDir() + string(IMG_LS_LED_ON_IMG)).c_str(), wxBITMAP_TYPE_PNG));

  MidiInBmp = new wxStaticBitmap(this, -1, *LedOff, wxPoint(368, 6));

  /* Envelope */

  VolumeFader = new FaderCtrl(this, LoopSampler_Volume, fader_vol_bg, fader_vol_fg, 0, 127, &Volume, true, 
                             wxPoint(13, 56), wxSize(23, 132));

  AttackFader = new FaderCtrl(this, LoopSampler_Attack, fader_bg, fader_fg, 0, 1000, &Attack, true,
			      wxPoint(51, 56), wxSize(26, 132));

  DecayFader = new FaderCtrl(this, LoopSampler_Decay, fader_bg, fader_fg, 0, 127, &Decay, true, 
                             wxPoint(89, 56), wxSize(26, 132));

  SustainFader = new FaderCtrl(this, LoopSampler_Sustain, fader_bg, fader_fg, 0, 127, &Sustain, true,
                             wxPoint(127, 56), wxSize(26, 132));


  ReleaseFader = new FaderCtrl(this, LoopSampler_Release, fader_bg, fader_fg, 0, 127, &Release, true, 
                             wxPoint(166, 56), wxSize(26, 132));

  /* Global tuning */

  wxImage *imgs[9];
  imgs[0] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_D)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[1] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_C)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[2] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_B)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[3] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_A)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[4] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_0)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[5] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_1)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[6] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_2)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[7] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_3)).c_str(), wxBITMAP_TYPE_PNG);
  imgs[8] = new wxImage(string(GetDataDir() + string(IMG_LS_KNOB_4)).c_str(), wxBITMAP_TYPE_PNG);
  
  OctaveKnob = new CycleKnob(this, LoopSampler_Octave, 9, imgs, 30, 0, 8, 4,
			    wxPoint(228, 131), wxSize(60, 51));//new KnobCtrl(this, LoopSampler_Octave, knob_bg, knob_fg, 0, 8, 4, 1,

  for (int i = 0; i < 9; i++)
    delete imgs[i];
    //    wxPoint(233, 133), wxSize(23, 23));
  
  PitchKnob = new KnobCtrl(this, LoopSampler_Pitch, knob_bg, knob_fg, 1, 200, 100, 1,
			   wxPoint(311, 134), wxSize(53, 52));

  /* Modes */

  InvertBtn = new DownButton(this, LoopSampler_Invert, wxPoint(321, 90), wxSize(19, 15), btn_down, btn_up, false);
  TempoBtn = new DownButton(this, LoopSampler_Tempo, wxPoint(321, 113), wxSize(19, 15), btn_down, btn_up, false);

  /* Help events */

  Connect(LoopSampler_Octave, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnOctaveHelp); 
  Connect(LoopSampler_Pitch, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnPitchHelp);  
  Connect(LoopSampler_Tempo, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnAutoStretchHelp);  
  Connect(LoopSampler_Invert, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnInvertHelp);  
  Connect(LoopSampler_Play, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnPlayHelp);  
  Connect(LoopSampler_Open, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnOpenHelp);  
  Connect(LoopSampler_Save, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnSaveHelp);  
  Connect(LoopSampler_ToSeqTrack, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnSendSeqHelp);  
  Connect(LoopSampler_ShowOpt, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnOptViewHelp);  

  /* Default MIDI automation */

  Connect(LoopSampler_Volume, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnVolumeController);    
  Connect(LoopSampler_Attack, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnAttackController);    
  Connect(LoopSampler_Decay, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnDecayController);    
  Connect(LoopSampler_Sustain, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnSustainController);    
  Connect(LoopSampler_Release, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnReleaseController);    
  Connect(LoopSampler_Octave, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnOctaveController);  
  Connect(LoopSampler_Pitch, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnPitchController);  
  Connect(LoopSampler_Tempo, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnTempoController);  
  Connect(LoopSampler_Invert, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&LoopSampler::OnInvertController);  

  MidiVolume[0] = M_CONTROL;
  MidiVolume[1] = 0x7;
  MidiAttack[0] = M_CONTROL;
  MidiAttack[1] = 0xA;
  MidiDecay[0] = M_CONTROL;
  MidiDecay[1] = 0xB;
  MidiSustain[0] = M_CONTROL;
  MidiSustain[1] = 0xC;
  MidiRelease[0] = M_CONTROL;
  MidiRelease[1] = 0xD;
  MidiOctave[0] = M_CONTROL;
  MidiOctave[1] = 0xE;
  MidiPitch[0] = M_CONTROL;
  MidiPitch[1] = 0xF;
  MidiTempo[0] = -1;
  MidiTempo[1] = -1;
  MidiInvert[0] = -1;
  MidiInvert[1] = -1;
  MidiSliceNote[0] = M_CONTROL;
  MidiSliceNote[1] = 0x10;
  MidiSlicePitch[0] = M_CONTROL;
  MidiSlicePitch[1] = 0x11;
  MidiSliceVol[0] = -1;
  MidiSliceVol[1] = -1;;
  MidiSliceAffect[0] = -1;
  MidiSliceAffect[1] = -1;
  MidiSliceInvert[0] = -1;
  MidiSliceInvert[1] = -1;

  MidiLed = false;

  UpdateMidiLed = false;
  UpdateVolume = false;
  UpdateAttack = false;
  UpdateDecay = false;
  UpdateSustain = false;
  UpdateRelease = false;
  UpdateOctave = false;
  UpdatePitch = false;
  UpdateTempo = false;
  UpdateInvert = false;
  UpdateSliceNote = false;
  UpdateSlicePitch = false;
  UpdateSliceVol = false;
  UpdateSliceAffect = false;
  UpdateSliceInvert = false;
}

LoopSampler::~LoopSampler()
{
  delete BgBmp;
  delete bmp;  
  delete ls_bg;
  delete play_up;
  delete play_down;
  delete open_up;
  delete open_down;
  delete save_up;
  delete save_down;
  delete seq_up;
  delete seq_down;
  delete opt_up;
  delete opt_down;
  delete up_up;
  delete up_down;
  delete down_up;
  delete down_down;
  delete fader_bg;
  delete fader_fg;
  delete fader_vol_bg;
  delete fader_vol_fg;
  delete knob_bg;
  delete knob_fg;

  list<Slice *>::iterator k;
  for (k = Slices.begin(); k != Slices.end(); k++)
    delete *k;

  //  if (View)
  // View->Destroy();
  if (Wave)
    delete Wave;
  list<LoopNote *>::iterator i;
  for (i = Notes.begin(); i != Notes.end(); i++)
    delete *i;

  if (read_buf)
    {
      delete read_buf[0];
      delete read_buf[1];
      delete read_buf;
    }
}


void LoopSampler::OnHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("This is Wired's Loop Sampler. It enables you to load loops and modify them, by cutting them into slices for automatic loop scaling, with pitch shifting and time stretching."));
}

void LoopSampler::OnOctaveHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("This knob sets the octave of the loaded sample. It will use a pitch shifting algorithm to achieve that effect. Right click on it to set the MIDI automation controller."));
}

void LoopSampler::OnPitchHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Changes the pitch (speed) of the whole loaded sample. Right click on it to set the MIDI automation controller."));
}

void LoopSampler::OnAutoStretchHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Stretch the whole sample that it synchronizes with the sequencer BPM Right click on it to set the MIDI automation controller."));
}

void LoopSampler::OnInvertHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Inverts the sample audio data. Right click on it to set the MIDI automation controller."));
}


void LoopSampler::OnPlayHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Plays the sample, synchronized with the sequencer."));
}

void LoopSampler::OnOpenHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Opens an audio file or Loop Sampler patch."));
}

void LoopSampler::OnSaveHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Saves current configuration to a Loop Sampler patch file."));
}

void LoopSampler::OnSendSeqHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Send current slices configuration to the selected MIDI tracks, as MIDI notes."));
}

void LoopSampler::OnOptViewHelp(wxMouseEvent &event)
{
  if (HelpMode)
    SendHelp(_("Shows Loop Sampler's optional view."));
}

void LoopSampler::SetBufferSize(long size) 
{
  Mutex.Lock();

  cout << "[LOOPSAMPLER] Buffer size is now: " << size << endl;
  Workshop.SetBufferSize(size);
  Workshop.SetPolyphony(PolyphonyCount);

  if (read_buf)
    {
      delete read_buf[0];
      delete read_buf[1];
      delete read_buf;
    }

  read_buf = new float *[2];
  read_buf[0] = new float[size];
  read_buf[1] = new float[size];

  Mutex.Unlock();
} 

void LoopSampler::SetSamplingRate(double rate) 
{ 
  Mutex.Lock();

  cout << "[LOOPSAMPLER] Sampling rate is now: " << rate << endl;

  SamplingRate = rate; 
  if (AttackMs)
    {
      AttackLen = long(AttackMs * SamplingRate);
      AttackCoef = 1.f / (float)AttackLen;
    }

  if (View)
    View->SetSamplingRate((int)rate);

  list<Slice *>::iterator k;
  for (k = Slices.begin(); k != Slices.end(); k++)  
    {
      (*k)->LeftTouch->setSampleRate((int)SamplingRate);
      (*k)->RightTouch->setSampleRate((int)SamplingRate);
    }

  Mutex.Unlock();
}

void LoopSampler::Process(float **input, float **output, long sample_length)
{
  Mutex.Lock();
  if (!Wave)
    {
      Mutex.Unlock();
      return;
    }
  if (Playing)
    {      
      double MesCur, MesBufCur;
      list<Slice *>::iterator i;

      MesCur = fmod(GetBarPos(), BarCount);
      MesBufCur = fmod(MesCur + sample_length * GetBarsPerSample(), BarCount);

      if (MesBufCur < MesCur)
	MesCur = 0.0;
      for (i = Slices.begin(); i != Slices.end(); i++)
	{
	  if (Notes.size() >= PolyphonyCount)
	    break;

	  if (((*i)->Bar >= MesCur) && ((*i)->Bar < MesBufCur))
	    {	      
	      LoopNote *note = new LoopNote((*i)->AffectMidi, (*i)->Volume, *i, 
					    (long)(((*i)->Bar - MesCur) * GetSamplesPerBar()),
					    Workshop.GetFreeBuffer(), 0, true);

	      list<LoopNote *>::iterator n;
	      // Delete the old played note
	      for (n = Notes.begin(); n != Notes.end();)
		{  
		  if ((*n)->PlayMode)
		    {
		      Workshop.SetFreeBuffer((*n)->Buffer);
		      delete *n;	 
		      n = Notes.erase(n);
		      break;
		    }
		  else
		    n++;
		}
	      Notes.push_back(note);
	    }
	}
    }

  list<LoopNote *>::iterator i;
  LoopNote *n;
  long length, end, idx, chan; 

  // Processing des notes
  for (i = Notes.begin(); i != Notes.end(); i++)
    {
      n = *i;

      //      cout << "pos: " << n->Position << "; start: " << LoopInfo.Start << "; end: " 
      //   << LoopInfo.End << endl;

      if (n->Position > LoopInfo.End)
	n->End = true;
      if (n->Position < LoopInfo.Start)
	{
	  if (!(n->SliceNote->EndPosition < 
		LoopInfo.Start))
	    n->Position = LoopInfo.Start;
	  else
	    n->End = true;
	}
      
      if (!(n->End))
	{
	  length = sample_length - n->Delta;

	  /*end = (long)((n->SliceNote->EndPosition - n->Position) / n->SliceNote->Pitch);
	  if (end < (length))
	    length = end - n->Delta;
	  else
	  end = 0;*/

	  /*if (length <= 0)
	  {
	    cout << "length < 0!!!" << endl;
	    n->End = true;
	    }*/
	 
	  Wave->SetPitch((n->SliceNote->Pitch + Pitch) / 2.f);
	  Wave->SetInvert(n->SliceNote->Invert);

	  curL = 0;
	  curR = 0;
	  retTouchL = 0;
	  retTouchR = 0;

	  while (!(n->End) && ((curL < length) || (curR < length)))
	    {
	      do
		{
		  retTouchL = n->SliceNote->LeftTouch->receiveSamples(n->Buffer[0] + curL +
								      n->Delta, 
								      length - curL);
		  retTouchR = n->SliceNote->RightTouch->receiveSamples(n->Buffer[1] + curR 
								       + n->Delta, 
								       length - curR);
		  curL += retTouchL;
		  curR += retTouchR;
		}
	      while ((retTouchL || retTouchR) && ((curL < length) || (curR < length)));

	      //	      cout << "NOTE : " << n << ", pos: " << n->Position << ", curL: " << curL	      
	      //   << "; retL: " << retTouchL << "; retR: " << retTouchR << endl;

	      if (n->Position < n->SliceNote->EndPosition)
		{
		  //		  cout << "POS: " << n->Position << "; ENDPOS: " << n->SliceNote->EndPosition << endl;
		  //cout << "read pos: " << n->Position << endl;
		  Wave->Read(read_buf, n->Position, length, 0, &(n->Position));
		  n->SliceNote->LeftTouch->putSamples(read_buf[0], length);
		  n->SliceNote->RightTouch->putSamples(read_buf[1], length);
		}
	      // "else" doesn't work here, very strange, probably a compiler optimisation bug
	      if (n->Position >= n->SliceNote->EndPosition) 
		{
		  if ((retTouchL == 0/* < sample_length*/) && (retTouchR == 0/*< sample_length*/))
		    {
		      n->End = true;
		      n->SliceNote->LeftTouch->clear();
		      n->SliceNote->RightTouch->clear();
		      break;
		    }
		}
	    }

	  if (n->Volume != 1.f)
	    for (chan = 0; chan < 2; chan++)	    
	      for (idx = n->Delta; idx < length; idx++)
		n->Buffer[chan][idx] *= n->Volume;
	  
	  //cout << "begin pos: " << n->BeginPosition << ", pos: " << n->Position << ", length: " << length << endl;

	  if (n->Start)
	    {
	      n->Start = false;
	      float f;
	      float inc = 1.f / (float)length;

	      for (chan = 0; chan < 2; chan++)	    
		for (idx = 0, f = 0.f; idx < length; idx++, f += inc)
		  n->Buffer[chan][idx] *= f;  
	    }
 
	  /*end = (long)((n->SliceNote->EndPosition - n->Position) / n->SliceNote->Pitch);

	  if (end <= length)
	    {
	      float f;
	      float inc = 1.f / (float)length;

	      for (chan = 0; chan < 2; chan++)	    
		for (idx = 0, f = 1.f; idx < length; idx++, f -= inc)
		  n->Buffer[chan][idx] *= f;  
		  }*/	  
	  
	  /*
	  if ((n->Position - n->BeginPosition) < AttackLen)
	    {	      
	      float f;
	      f = n->Attack;

	      if (length > (AttackLen - (n->Position - n->BeginPosition)))
		end = AttackLen - (n->Position - n->BeginPosition);
	      else
		end = length;
	      for (chan = 0; chan < 2; chan++)
		for (idx = n->Delta, n->Attack = f; idx < end; 
		     idx++, n->Attack += AttackCoef)
		  {   
		    n->Buffer[chan][idx] *= n->Attack;
		  }
	    }
	  */

	  if (n->Delta)
	    n->Delta = 0;
	}
    }

  Workshop.GetMix(output);

  // Suppression des notes termin?es
  for (i = Notes.begin(); i != Notes.end();)
    {  
      //if ((*i)->Position >= (*i)->SliceNote->EndPosition)      
      if ((*i)->End)
	{
	  Workshop.SetFreeBuffer((*i)->Buffer);
	  delete *i;	 
	  i = Notes.erase(i);
	}
      else
	i++;
    }
  Mutex.Unlock();
 }

void LoopSampler::ProcessEvent(WiredEvent &event)
{
  if ((event.MidiData[0] == M_NOTEON1) || (event.MidiData[0] == M_NOTEON2))
    {
      if (!event.MidiData[2]) 
	{
	  Mutex.Lock();

	  // Suppression des notes termin?es
	  list<LoopNote *>::iterator i;
	  for (i = Notes.begin(); i != Notes.end(); i++)
	    {  
	      if ((*i)->Note == event.MidiData[1])      
		{
		  Workshop.SetFreeBuffer((*i)->Buffer);
		  delete *i;	 
		  Notes.erase(i);
		  break;
		}
	    }	  

	  UpdateMidiLed = true;
	  MidiLed = false;
	  AskUpdate();
	  //	  MidiInBmp->SetBitmap(*LedOff);

	  Mutex.Unlock();
	}
      else
	{
	  Mutex.Lock();

	  UpdateMidiLed = true;
	  MidiLed = true;
	  AskUpdate();
	  //MidiInBmp->SetBitmap(*LedOn);	  

	  if (Notes.size() < PolyphonyCount)	    
	    {
	      list<Slice *>::iterator i;
	      list<LoopNote *>::iterator j;

	      for (i = Slices.begin(); i != Slices.end(); i++)
		{
		  if ((*i)->AffectMidi == event.MidiData[1])
		    {
		      /*** Temporary fix for not making it possible to play 2 times the same 
			   note. We need to create new SoundTouch objects for each note, not 
			   slice ***/
		      for (j = Notes.begin(); j != Notes.end(); j++)
			if ((*j)->Note == event.MidiData[1])
			  {
			    Workshop.SetFreeBuffer((*j)->Buffer);
			    delete *j;	 
			    Notes.erase(j);
			    break;
			  }  

		      LoopNote *n = new LoopNote(event.MidiData[1], 
						 event.MidiData[2] / 100.f,
						 *i, event.DeltaFrames, 
						 Workshop.GetFreeBuffer(), event.NoteLength);
		      Notes.push_back(n);
		      //printf("[LOOPSAMPLER] Note added: %2x\n", n->Note);

		      break;
		    }		
		}			       
	    }
	  else
	    cout << "[LOOPSAMPLER] Max polyphony reached" << endl;
	  
	  Mutex.Unlock();
	}
    }
  else
    ProcessMidiControls(event.MidiData);
//  printf("[LOOPSAMPLER] Got midi in : %2x %2x %2x\n", event.MidiData[0], event.MidiData[1], event.MidiData[2]);
}

void LoopSampler::ProcessMidiControls(int MidiData[3])
{
  Mutex.Lock();

  // else if (event.MidiData[0] == M_CONTROL)

  if ((MidiVolume[0] == MidiData[0]) && (MidiVolume[1] == MidiData[1]))
    {
      Volume = MidiData[2] / 100.f;
      UpdateVolume = true;
      AskUpdate();
      //VolumeFader->SetValue(MidiData[2]);
      Workshop.SetVolume(Volume);
    }
  else if ((MidiAttack[0] == MidiData[0]) && (MidiAttack[1] == MidiData[1]))
    {
      AttackCoef = MidiData[2] / 100.f;
      UpdateAttack = true;
      AskUpdate();
      //AttackFader->SetValue(MidiData[2] * 10);
    }
  else if ((MidiOctave[0] == MidiData[0]) && (MidiOctave[1] == MidiData[1]))
    {
      //OctaveKnob->SetValue(MidiData[2] / 10 - 1);
      float oct = (MidiData[2] / 15) - 4;
      if (oct != Octave)
	{
	  Octave = oct;
	  cout << "Octave: " << Octave << endl;
	  
	  list<Slice *>::iterator k;
	  for (k = Slices.begin(); k != Slices.end(); k++)  
	    (*k)->SetOctave(Octave);     
	  
	  UpdateOctave =  true;
	  AskUpdate();
	}
    }
  else if ((MidiPitch[0] == MidiData[0]) && (MidiPitch[1] == MidiData[1]))
    {
      Pitch = (MidiData[2] * 1.574f) / 100.f; // 1.574 = 200 / 127

      UpdatePitch = true;
      AskUpdate();
      //PitchKnob->SetValue((int)(MidiData[2] * 1.574f));
      //Pitch = PitchKnob->GetValue() / 100.f;
    }
  else if ((MidiTempo[0] == MidiData[0]) && (MidiTempo[1] == MidiData[1]))
    {
      if (MidiData[2])
	TempoBtn->SetOn();
      else
	TempoBtn->SetOff();
      SetTempo();
    }
  else if ((MidiInvert[0] == MidiData[0]) && (MidiInvert[1] == MidiData[1]))
    {
      if (MidiData[2])
	InvertBtn->SetOn();
      else
	InvertBtn->SetOff();
      list<Slice *>::iterator i;
      for (i = Slices.begin(); i != Slices.end(); i++)
	(*i)->Invert = !(*i)->Invert;
    }

  Mutex.Unlock();
}

void	LoopSampler::Update()
{
  Mutex.Lock();

  if (UpdateMidiLed)
    {
      UpdateMidiLed = false;      
      if (MidiLed)
	MidiInBmp->SetBitmap(*LedOn);
      else
	MidiInBmp->SetBitmap(*LedOff);
    }
  else if (UpdateVolume)
    {
      UpdateVolume = false;
      VolumeFader->SetValue((int)(Volume * 100));
    }
  else if (UpdateAttack)
    {
      UpdateAttack = false;
      AttackFader->SetValue((int)(Attack * 1000));
    }
  else if (UpdateOctave)
    {
      UpdateOctave = false;
      OctaveKnob->SetValue((int)(Octave) + 4);
    }
  else if (UpdatePitch)
    {
      UpdatePitch = false;
      PitchKnob->SetValue((int)(Pitch * 100));
    }

  Mutex.Unlock();
}

wxWindow *LoopSampler::CreateView(wxWindow *zone, wxPoint &pos, wxSize &size)
{
  View = new LoopSamplerView(&Mutex, zone, pos, size, GetDataDir(), &LoopInfo);
  if (Wave)
    {
      View->SetWaveFile(Wave);
      View->SetSlices(&Slices);
      SetBarCoeff();
    }
  return (View);
}

void LoopSampler::Play()
{
  Mutex.Lock();

  SeqPlaying = true;
  if (AutoPlaying)
    Playing = true;

  Mutex.Unlock();
}

void LoopSampler::Stop()
{
  Mutex.Lock();

  SeqPlaying = false;
  Playing = false;
  //PlayBtn->SetOff();

  Mutex.Unlock();
}

void LoopSampler::DestroyView()
{
  View->Destroy();
  View = 0x0;
}

bool LoopSampler::IsInstrument()
{
  return (true);
}

bool LoopSampler::IsAudio()
{
  return (false);
}

bool LoopSampler::IsMidi()
{
  return (true);
}

void LoopSampler::Load(int fd, long size)
{
  long len;

  Mutex.Lock();

  read(fd, &len, sizeof (len));
  
  char s[len + 1];
  read(fd, s, len);
  s[len] = 0;
  read(fd, &BarCount, sizeof (BarCount));
  read(fd, &BeatCount, sizeof (BeatCount));
  read(fd, &Volume, sizeof (Volume));
  read(fd, &Attack, sizeof (Attack));
  read(fd, &Decay, sizeof (Decay));
  read(fd, &Sustain, sizeof (Sustain));
  read(fd, &Release, sizeof (Release));
  read(fd, &Octave, sizeof (Octave));
  read(fd, &Pitch, sizeof (Pitch));
  read(fd, &Invert, sizeof (Invert));
  read(fd, &Tempo, sizeof (Tempo));

  Octave--;

  wxString str;

  str.Printf("%d", BeatCount);
  MesCountLabel->SetLabel(str);
  VolumeFader->SetValue((long)(Volume * 100.f));
  AttackFader->SetValue((long)(Attack * 100.f));
  DecayFader->SetValue((long)(Decay * 100.f));
  SustainFader->SetValue((long)(Sustain * 100.f));
  ReleaseFader->SetValue((long)(Release * 100.f));
  OctaveKnob->SetValue((long)(Octave + 4));
  PitchKnob->SetValue((long)(Pitch * 100.f));

  if (Invert)
    InvertBtn->SetOn();
  else
    InvertBtn->SetOff();
  
  WaveFile *w;
  try
    {
      w = new WaveFile(s, true);

      Mutex.Unlock();

      SetWaveFile(w);
      ShowOptionalView();  

      Mutex.Lock();
    }
  catch (...)
    {
      cout << "[LOOPSAMPLER] Cannot open wave file !" << endl;      
    }

  read(fd, &(LoopInfo.Start), sizeof (LoopInfo.Start));
  read(fd, &(LoopInfo.End), sizeof (LoopInfo.End));

  if (read(fd, &len, sizeof (len)) < sizeof (len))
    return;
  
  Slices.clear();
  for (int i = 0; i < len; i++)
    {      
      Slice *s = new Slice(0, 0.0, (int)SamplingRate);
      read(fd, &(s->Position), sizeof (s->Position));
      read(fd, &(s->EndPosition), sizeof (s->EndPosition));
      read(fd, &(s->Bar), sizeof (s->Bar));
      read(fd, &(s->Note), sizeof (s->Note));
      read(fd, &(s->Pitch), sizeof (s->Pitch));
      read(fd, &(s->Volume), sizeof (s->Volume));
      read(fd, &(s->AffectMidi), sizeof (s->AffectMidi));
      read(fd, &(s->Invert), sizeof (s->Invert));

      s->SetOctave(Octave);
      s->SetNote(s->Note);
 
      Slices.push_back(s);
    }  

  read(fd, MidiVolume, sizeof (int[2]));
  read(fd, MidiAttack, sizeof (int[2]));
  read(fd, MidiDecay, sizeof (int[2]));
  read(fd, MidiSustain, sizeof (int[2]));
  read(fd, MidiRelease, sizeof (int[2]));
  read(fd, MidiOctave, sizeof (int[2]));
  read(fd, MidiPitch, sizeof (int[2]));
  read(fd, MidiTempo, sizeof (int[2]));
  read(fd, MidiInvert, sizeof (int[2]));
  read(fd, MidiSliceNote, sizeof (int[2]));
  read(fd, MidiSlicePitch, sizeof (int[2]));
  read(fd, MidiSliceVol, sizeof (int[2]));
  read(fd, MidiSliceAffect, sizeof (int[2]));
  read(fd, MidiSliceInvert, sizeof (int[2]));

  if (Tempo)
    {
      TempoBtn->SetOn();

      SetTempo();
    }
  else
    TempoBtn->SetOff();

  if (View)
    {
      View->SetSlices(&Slices);
      View->SetLoopCursors();
    }

  Mutex.Unlock();
}

long LoopSampler::Save(int fd)
{
  long len, size = 0;

  if (Wave)
    {
      len = Wave->Filename.size();
      size = write(fd, &len, sizeof (len));
      size += write(fd, Wave->Filename.c_str(), len);
    }
  else
    {
      len = 0;
      size = write(fd, &len, sizeof (len));
    }
  size += write(fd, &BarCount, sizeof (BarCount));
  size += write(fd, &BeatCount, sizeof (BeatCount));
  size += write(fd, &Volume, sizeof (Volume));
  size += write(fd, &Attack, sizeof (Attack));
  size += write(fd, &Decay, sizeof (Decay));
  size += write(fd, &Sustain, sizeof (Sustain));
  size += write(fd, &Release, sizeof (Release));
  size += write(fd, &Octave - 1, sizeof (Octave));
  size += write(fd, &Pitch, sizeof (Pitch));
  size += write(fd, &Invert, sizeof (Invert));

  size += write(fd, &Tempo, sizeof (Tempo));
  size += write(fd, &(LoopInfo.Start), sizeof (LoopInfo.Start));
  size += write(fd, &(LoopInfo.End), sizeof (LoopInfo.End));      
  
  len = Slices.size();
  size += write(fd, &len, sizeof (len));
  list<Slice *>::iterator i;
  for (i = Slices.begin(); i != Slices.end(); i++)
    {
      size += write(fd, &(*i)->Position, sizeof ((*i)->Position));
      size += write(fd, &(*i)->EndPosition, sizeof ((*i)->EndPosition));
      size += write(fd, &(*i)->Bar, sizeof ((*i)->Bar));
      size += write(fd, &(*i)->Note, sizeof ((*i)->Note));
      size += write(fd, &(*i)->Pitch, sizeof ((*i)->Pitch));
      size += write(fd, &(*i)->Volume, sizeof ((*i)->Volume));
      size += write(fd, &(*i)->AffectMidi, sizeof ((*i)->AffectMidi));
      size += write(fd, &(*i)->Invert, sizeof ((*i)->Invert));
    }
  
  // Midi automation
  size += write(fd, MidiVolume, sizeof (int[2]));
  size += write(fd, MidiAttack, sizeof (int[2]));
  size += write(fd, MidiDecay, sizeof (int[2]));
  size += write(fd, MidiSustain, sizeof (int[2]));
  size += write(fd, MidiRelease, sizeof (int[2]));
  size += write(fd, MidiOctave, sizeof (int[2]));
  size += write(fd, MidiPitch, sizeof (int[2]));
  size += write(fd, MidiTempo, sizeof (int[2]));
  size += write(fd, MidiInvert, sizeof (int[2]));
  size += write(fd, MidiSliceNote, sizeof (int[2]));
  size += write(fd, MidiSlicePitch, sizeof (int[2]));
  size += write(fd, MidiSliceVol, sizeof (int[2]));
  size += write(fd, MidiSliceAffect, sizeof (int[2]));
  size += write(fd, MidiSliceInvert, sizeof (int[2]));

  return (size);
}

void LoopSampler::SetBPM(float bpm)
{
  Mutex.Lock();

  if (Tempo)
    SetTempo();

  Mutex.Unlock();
}

void LoopSampler::SetSignature(int num, int den)
{
  Mutex.Lock();

  SetBarCoeff();
  if (Tempo)
    SetTempo();

  Mutex.Unlock();
}

void LoopSampler::SetTempo()
{
  list<Slice *>::iterator i;

  Tempo = TempoBtn->GetOn();
  if (!Wave)
    {
      return;
    }

  if (!Tempo)
    {
      for (i = Slices.begin(); i != Slices.end(); i++)
	(*i)->SetTempo(1.f);
    }
  else
    {
      float f;

      f = (float)(Wave->GetNumberOfFrames() / (GetSamplesPerBar() * BarCount));
      for (i = Slices.begin(); i != Slices.end(); i++)
	(*i)->SetTempo(f);
    }
}

void LoopSampler::SetBarCoeff()
{
  double coeff;
  list<Slice *>::iterator i;

  if (!Wave)
    return;
  BarCount = ((double)BeatCount / (double)GetSigNumerator());
  coeff = BarCount / (double)Wave->GetNumberOfFrames();
  for (i = Slices.begin(); i != Slices.end(); i++)
    (*i)->Bar = (*i)->Position * coeff;
  if (View)    
    View->SetBarCoeff(coeff);    
}

void LoopSampler:: SetWaveFile(WaveFile *w)
{
  WaveFile *tmpw = Wave;

  Mutex.Lock();

  Slices.clear();

  Wave = w;

  Mutex.Unlock();

  if (tmpw)
    {
      delete tmpw;
      /*
      delete tmpl;
      delete tmpr;
      */
    }

  if (View)
    {
      View->SetWaveFile(w);
      View->SetSamplingRate((int)SamplingRate);

      Mutex.Lock();

      View->SetSlices(&Slices);	      

      Mutex.Unlock();
    }
  Mutex.Lock();

  SetBarCoeff();

  LoopInfo.Start = 0;
  LoopInfo.End = w->GetNumberOfFrames();

  Mutex.Unlock();
}

wxBitmap *LoopSampler::GetBitmap()
{
  return (bmp);
}

void LoopSampler::OnOpenFile(wxCommandEvent &event)
{
  vector<string> exts;  

  exts.push_back("wav\tMicrosoft WAV format (*.wav)");
  exts.push_back("wls\tLoop Sampler patch (*.wls)");
  exts.push_back("aif\tApple/SGI AIFF format (*.aif)");
  exts.push_back("au;snd\tSun/NeXT AU format (*.au; *.snd)");
  exts.push_back("svx\tAmiga IFF / SVX8 / SV16 format (*.svx)");
  exts.push_back("paf;fap\tEnsoniq PARIS format (*.paf; *.fap)");
  exts.push_back("nist\tSphere NIST format (*.nist)");
  exts.push_back("ircam;sf\tBerkeley/IRCAM/CARL format (*.ircam; *.sf)");
  exts.push_back("voc\tCreative Labs VOC format (*.voc)");
  exts.push_back("w64\tSonic Foundry's 64 bit RIFF/WAV format (*.w64)");
  exts.push_back("raw\tRAW PCM data format (*.raw)");
  exts.push_back("mat4;mat\tMatlab (tm) V4.2 / GNU Octave 2.0 format (*.mat4; *.mat)");
  exts.push_back("mat5\tMatlab (tm) V5 / GNU Octave 2.1 format (*.mat5)");
  exts.push_back("pvf\tPortable Voice Format (*.pvf)");
  exts.push_back("xi\tFastracker 2 format (*.xi)");

  string s = OpenFileLoader(_("Loading sound file"), &exts);
  if (!s.empty())
    {
      string selfile = s;

      wxProgressDialog *Progress = new wxProgressDialog(_("Loading wave file"), _("Please wait..."), 
							100, this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT
                                                        | wxPD_REMAINING_TIME);
      Progress->Update(1);
      
      wxFileName f(s.c_str());
      if (f.GetExt().CompareTo("wls"))
	{
	  WaveFile *w;
	  try
	    {
	      w = new WaveFile(selfile, true);
	      if (View)
		CloseOptionalView();
	      Progress->Update(60);
	      SetWaveFile(w);
	      Progress->Update(99);
	      ShowOptionalView();
	    }
	  catch (...)
	    {
	      cout << "[LOOPSAMPLER] Cannot open wave file !" << endl;
	    }

	  BeatDialog *dlg = new BeatDialog(this);
	  if (dlg->ShowModal() == wxID_OK)
	    {
	      BeatCount = dlg->BeatCtrl->GetValue();
	      wxString s;      
	      s.Printf("%d", BeatCount);
	      MesCountLabel->SetLabel(s);
	      SetBarCoeff();
	      if (View)
		View->SetBeats(GetSigNumerator(), BeatCount);
	      if (Tempo)
		SetTempo();
	    }
	  dlg->Destroy();
	}
      else 
	{
	  int fd;

	  cout << "[LOOPSAMPLER] Opening patch" << endl;
	  if ((fd = open(s.c_str(), O_RDONLY)) > -1)
	    {
	      struct stat st;
	      fstat(fd, &st);
	      cout << "file length: " << st.st_size << endl;	      
	      Load(fd, st.st_size);
	    }
	  else 
	    cout << "[LOOPSAMPLER] Could not open file: " << s << endl;
	}
      delete Progress;
    }
}

void LoopSampler::OnSaveFile(wxCommandEvent &event)
{
  int fd;
  vector<string> exts;
  string s;

  exts.push_back("wls\tLoop Sampler patch (*.wls)");
  s = SaveFileLoader(_("Save Loop Sampler patch"), &exts);

  if (!s.empty())
    {
      wxFileName f(s.c_str());
      if (!f.HasExt())
	s = s + ".wls";
      if ((fd = open(s.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) > -1)
	{
	  cout << "[LOOPSAMPLER] Patch saved: " << Save(fd) << " bytes" << endl;
	  close(fd);
	}
    }
}

void LoopSampler::OnOctave(wxCommandEvent &event)
{
  Mutex.Lock();

  Octave = OctaveKnob->GetValue() - 4;

  list<Slice *>::iterator k;
  for (k = Slices.begin(); k != Slices.end(); k++)  
    (*k)->SetOctave(Octave);      

  Mutex.Unlock();
}

void LoopSampler::OnPitch(wxScrollEvent &event)
{
  Mutex.Lock();

  Pitch = PitchKnob->GetValue() / 100.f;

  Mutex.Unlock();
}

void LoopSampler::OnVolume(wxScrollEvent &event)
{
  Mutex.Lock();

  Volume = VolumeFader->GetValue() / 100.f;
  Workshop.SetVolume(Volume);

  Mutex.Unlock();
}

void LoopSampler::OnAttack(wxScrollEvent &event)
{
  Mutex.Lock();

  AttackMs = AttackFader->GetValue() / 1000.f;

  if (AttackMs)
    {
      AttackLen = (long)(AttackMs * SamplingRate);
      AttackCoef = 1.f / (float)AttackLen;
    }
  //  cout << "ms: " << AttackMs << "; len: " << AttackLen << "; coef: " << AttackCoef << endl;

  Mutex.Unlock();
}

void LoopSampler::OnMesDown(wxCommandEvent &event)
{
  if (BeatCount > 1)
    {
      Mutex.Lock();

      BeatCount--;
      wxString s;      
      s.Printf("%d", BeatCount);
      MesCountLabel->SetLabel(s);
      SetBarCoeff();
      if (View)
	View->SetBeats(GetSigNumerator(), BeatCount);
      if (Tempo)
	SetTempo();

      Mutex.Unlock();
    }
}

void LoopSampler::OnMesUp(wxCommandEvent &event)
{
  if (BeatCount < 100)
    {
      Mutex.Lock();

      BeatCount++;
      wxString s;      
      s.Printf("%d", BeatCount);
      MesCountLabel->SetLabel(s);
      SetBarCoeff();
      if (View)
	View->SetBeats(GetSigNumerator(), BeatCount);
      if (Tempo)
	SetTempo();

      Mutex.Unlock();
    }
}

void LoopSampler::OnPolyUp(wxCommandEvent &event)
{
  wxString s;      

  if (PolyphonyCount < 256)
    {
      Mutex.Lock();

      PolyphonyCount++;
      Workshop.SetPolyphony(PolyphonyCount);
      s.Printf("%d", PolyphonyCount);
      PolyCountLabel->SetLabel(s);      

      Mutex.Unlock();  
    }
}

void LoopSampler::OnPolyDown(wxCommandEvent &event)
{
  wxString s;      

  if (PolyphonyCount > 1)
    {
      Mutex.Lock();

      PolyphonyCount--;
      Workshop.SetPolyphony(PolyphonyCount);
      s.Printf("%d", PolyphonyCount);
      PolyCountLabel->SetLabel(s);      

      Mutex.Unlock();
    }
}

void LoopSampler::OnPlay(wxCommandEvent &event)
{
  Mutex.Lock();

  if (PlayBtn->GetOn())
    {
      AutoPlaying = true;
      if (SeqPlaying)
	Playing = true;
    }
  else
    {
      AutoPlaying = false;
      if (SeqPlaying)
	Playing = false;
    }

  Mutex.Unlock();
}

void LoopSampler::OnToSeqTrack(wxCommandEvent &event)
{
  if (Wave)
    {
      list<SeqCreateEvent *> l;
      SeqCreateEvent *e, *f;
      list<Slice *>::iterator i;
      list<SeqCreateEvent *>::iterator j;

      double d = GetBarsPerSample();
      
      for (i = Slices.begin(); i != Slices.end();)
	{
	  // Note on
	  e = new SeqCreateEvent;
	  e->Position = (*i)->Bar;
	  e->EndPosition = (*i)->EndPosition * d;;
	  e->MidiMsg[0] = 0x90;
	  //e->MidiMsg[0] &= 0xF0;
	  e->MidiMsg[1] = (*i)->AffectMidi;
	  e->MidiMsg[2] = (int)((*i)->Volume * 100.f);
	  l.push_back(e);

	  // Note off
	  f = new SeqCreateEvent;
	  f->Position = e->EndPosition;
	  f->EndPosition = f->Position;
	  f->MidiMsg[0] = 0x80;
	  f->MidiMsg[1] = e->MidiMsg[1];
	  f->MidiMsg[2] = 0;
	  l.push_back(f);
	  i++;

	  if ((i == Slices.end()) && Tempo)
	    {
	      e->EndPosition = BarCount;
	      f->Position = BarCount;
	      f->EndPosition = BarCount;
	    }
	}      
      CreateMidiPattern(&l);
      for (j = l.begin(); j != l.end(); j++)
	delete *j;
    }
}

void LoopSampler::OnShowView(wxCommandEvent &event)
{
  ShowOptionalView();
}

void LoopSampler::OnInvert(wxCommandEvent &event)
{
  list<Slice *>::iterator i;

  Mutex.Lock();

  Invert = !Invert;
  for (i = Slices.begin(); i != Slices.end(); i++)
    (*i)->Invert = !(*i)->Invert;

  Mutex.Unlock();
}

void LoopSampler::OnTempo(wxCommandEvent &event)
{
  Mutex.Lock();

  SetTempo();

  Mutex.Unlock();
}

void LoopSampler::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*BgBmp);    
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }

  Plugin::OnPaintEvent(event);
}

void LoopSampler::CheckExistingControllerData(int MidiData[3])
{
  if ((MidiVolume[0] == MidiData[0]) && (MidiVolume[1] == MidiData[1]))
    MidiVolume[0] = -1;
  else if ((MidiAttack[0] == MidiData[0]) && (MidiAttack[1] == MidiData[1]))
    MidiAttack[0] = -1;
  else if ((MidiDecay[0] == MidiData[0]) && (MidiDecay[1] == MidiData[1]))
    MidiDecay[0] = -1;
  else if ((MidiSustain[0] == MidiData[0]) && (MidiSustain[1] == MidiData[1]))
    MidiSustain[0] = -1;
  else if ((MidiRelease[0] == MidiData[0]) && (MidiRelease[1] == MidiData[1]))
    MidiRelease[0] = -1;
  else if ((MidiOctave[0] == MidiData[0]) && (MidiOctave[1] == MidiData[1]))
    MidiOctave[0] = -1;
  else if ((MidiPitch[0] == MidiData[0]) && (MidiPitch[1] == MidiData[1]))
    MidiPitch[0] = -1;
  else if ((MidiTempo[0] == MidiData[0]) && (MidiTempo[1] == MidiData[1]))
    MidiTempo[0] = -1;
  else if ((MidiInvert[0] == MidiData[0]) && (MidiInvert[1] == MidiData[1]))
    MidiInvert[0] = -1;
  else if ((MidiSliceNote[0] == MidiData[0]) && (MidiSliceNote[1] == MidiData[1]))
    MidiSliceNote[0] = -1;
  else if ((MidiSlicePitch[0] == MidiData[0]) && (MidiSlicePitch[1] == MidiData[1]))
    MidiSlicePitch[0] = -1;
  else if ((MidiSliceVol[0] == MidiData[0]) && (MidiSliceVol[1] == MidiData[1]))
    MidiSliceVol[0] = -1;
  else if ((MidiSliceAffect[0] == MidiData[0]) && (MidiSliceAffect[1] == MidiData[1]))
    MidiSliceAffect[0] = -1;
  else if ((MidiSliceInvert[0] == MidiData[0]) && (MidiSliceInvert[1] == MidiData[1]))
    MidiSliceInvert[0] = -1;
}

void LoopSampler::OnVolumeController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiVolume[0] = midi_data[0];
      MidiVolume[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnAttackController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiAttack[0] = midi_data[0];
      MidiAttack[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnDecayController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiDecay[0] = midi_data[0];
      MidiDecay[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnSustainController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiSustain[0] = midi_data[0];
      MidiSustain[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnReleaseController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiRelease[0] = midi_data[0];
      MidiRelease[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnOctaveController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiOctave[0] = midi_data[0];
      MidiOctave[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnPitchController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiPitch[0] = midi_data[0];
      MidiPitch[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnTempoController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiTempo[0] = midi_data[0];
      MidiTempo[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void LoopSampler::OnInvertController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);
      MidiInvert[0] = midi_data[0];
      MidiInvert[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "PLLS");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_INSTR;  
    info.UnitsX = 2;
    info.UnitsY = 2;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new LoopSampler(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








