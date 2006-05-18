// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "SynthPlug.h"

/******** SynthPlugin Implementation *********/

BEGIN_EVENT_TABLE(SynthPlugin, wxWindow)
  EVT_PAINT(SynthPlugin::OnPaint)
  EVT_COMMAND_SCROLL(Synth_Osc1_Phase, SynthPlugin::OnOsc1Phase)
  EVT_COMMAND_SCROLL(Synth_Osc1_Transpose, SynthPlugin::OnOsc1Transpose)
  EVT_COMMAND_SCROLL(Synth_Osc1_Fine, SynthPlugin::OnOsc1Fine)
  EVT_COMMAND_SCROLL(Synth_Osc2_Phase, SynthPlugin::OnOsc2Phase)
  EVT_COMMAND_SCROLL(Synth_Osc2_Transpose, SynthPlugin::OnOsc2Transpose)
  EVT_COMMAND_SCROLL(Synth_Osc2_Fine, SynthPlugin::OnOsc2Fine)
  EVT_COMMAND_SCROLL(Synth_Adsr1_Attack, SynthPlugin::OnAdsr1Attack)
  EVT_COMMAND_SCROLL(Synth_Adsr1_Decay, SynthPlugin::OnAdsr1Decay)
  EVT_COMMAND_SCROLL(Synth_Adsr1_Sustain, SynthPlugin::OnAdsr1Sustain)
  EVT_COMMAND_SCROLL(Synth_Adsr1_Release, SynthPlugin::OnAdsr1Release)
  EVT_COMMAND_SCROLL(Synth_Adsr2_Attack, SynthPlugin::OnAdsr2Attack)
  EVT_COMMAND_SCROLL(Synth_Adsr2_Decay, SynthPlugin::OnAdsr2Decay)
  EVT_COMMAND_SCROLL(Synth_Adsr2_Sustain, SynthPlugin::OnAdsr2Sustain)
  EVT_COMMAND_SCROLL(Synth_Adsr2_Release, SynthPlugin::OnAdsr2Release)
  EVT_COMMAND_SCROLL(Synth_Lfo1_Rate, SynthPlugin::OnLfo1Rate)
  EVT_COMMAND_SCROLL(Synth_Lfo1_Amount, SynthPlugin::OnLfo1Amount)
  EVT_COMMAND_SCROLL(Synth_Lfo1_Delay, SynthPlugin::OnLfo1Delay)
  EVT_COMMAND_SCROLL(Synth_Lfo2_Rate, SynthPlugin::OnLfo2Rate)
  EVT_COMMAND_SCROLL(Synth_Lfo2_Amount, SynthPlugin::OnLfo2Amount)
  EVT_COMMAND_SCROLL(Synth_Lfo2_Delay, SynthPlugin::OnLfo2Delay)
END_EVENT_TABLE()

SynthPlugin::SynthPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo), NoteIsOn(false), Freq(0)
{
  cout << "[SYNTH] Host is " << GetHostProductName()
       << " version " << GetHostProductVersion() << endl;

  png_tmp = new wxImage("data/synth.png", wxBITMAP_TYPE_PNG);
  BgBmp = new wxBitmap(png_tmp);
  
  fader_bg = new wxImage("data/Fader_bg.png",wxBITMAP_TYPE_PNG);
  fader_fg = new wxImage("data/fader_fg.png",wxBITMAP_TYPE_PNG);
  knob_bg = new wxImage("data/knob_bg.png",wxBITMAP_TYPE_PNG);
  knob_fg = new wxImage("data/knob_fg.png",wxBITMAP_TYPE_PNG);

  // FIXME add a volume fader
  Osc1Phase = new KnobCtrl(this, Synth_Osc1_Phase, knob_bg, knob_fg, 0, 127, 63, 1,
			   wxPoint(95, 35), wxSize(32, 32));
  Osc1Transpose = new KnobCtrl(this, Synth_Osc1_Transpose, knob_bg, knob_fg, 0, 127, 63, 1,
			   wxPoint(95, 75), wxSize(32, 32));
  Osc1Fine = new KnobCtrl(this, Synth_Osc1_Fine, knob_bg, knob_fg, 0, 127, 63, 1,
			   wxPoint(95, 115), wxSize(32, 32));

  Osc2Phase = new KnobCtrl(this, Synth_Osc2_Phase, knob_bg, knob_fg, 0, 127, 63, 1,
			   wxPoint(95, 195), wxSize(32, 32));
  Osc2Transpose = new KnobCtrl(this, Synth_Osc2_Transpose, knob_bg, knob_fg, 0, 127, 63, 1,
			   wxPoint(95, 235), wxSize(32, 32));
  Osc2Fine = new KnobCtrl(this, Synth_Osc2_Fine, knob_bg, knob_fg, 0, 127, 63, 1,
			   wxPoint(95, 275), wxSize(32, 32));

  Adsr1Attack = new KnobCtrl(this, Synth_Adsr1_Attack, knob_bg, knob_fg, 0, 127, 63, 1,
			     wxPoint(249, 40), wxSize(32, 32));
  Adsr1Decay = new KnobCtrl(this, Synth_Adsr1_Decay, knob_bg, knob_fg, 0, 127, 63, 1,
			    wxPoint(292, 40), wxSize(32, 32));
  Adsr1Sustain = new KnobCtrl(this, Synth_Adsr1_Sustain, knob_bg, knob_fg, 0, 127, 63, 1,
			      wxPoint(335, 40), wxSize(32, 32));
  Adsr1Release = new KnobCtrl(this, Synth_Adsr1_Release, knob_bg, knob_fg, 0, 127, 63, 1,
			      wxPoint(380, 40), wxSize(32, 32));
  
  Adsr2Attack = new KnobCtrl(this, Synth_Adsr2_Attack, knob_bg, knob_fg, 0, 127, 63, 1,
			     wxPoint(249, 139), wxSize(32, 32));
  Adsr2Decay = new KnobCtrl(this, Synth_Adsr2_Decay, knob_bg, knob_fg, 0, 127, 63, 1,
			    wxPoint(292, 139), wxSize(32, 32));
  Adsr2Sustain = new KnobCtrl(this, Synth_Adsr2_Sustain, knob_bg, knob_fg, 0, 127, 63, 1,
			      wxPoint(335, 139), wxSize(32, 32));
  Adsr2Release = new KnobCtrl(this, Synth_Adsr2_Release, knob_bg, knob_fg, 0, 127, 63, 1,
			      wxPoint(380, 139), wxSize(32, 32));
  
  Lfo1Rate = new KnobCtrl(this, Synth_Lfo1_Rate, knob_bg, knob_fg, 0, 127, 63, 1,
			  wxPoint(23, 358), wxSize(32, 32));
  Lfo1Amount = new KnobCtrl(this, Synth_Lfo1_Amount, knob_bg, knob_fg, 0, 127, 63, 1,
			  wxPoint(65, 358), wxSize(32, 32));
  Lfo1Delay = new KnobCtrl(this, Synth_Lfo1_Delay, knob_bg, knob_fg, 0, 127, 63, 1,
			  wxPoint(108, 358), wxSize(32, 32));

  Lfo2Rate = new KnobCtrl(this, Synth_Lfo2_Rate, knob_bg, knob_fg, 0, 127, 63, 1,
			  wxPoint(253, 358), wxSize(32, 32));
  Lfo2Amount = new KnobCtrl(this, Synth_Lfo2_Amount, knob_bg, knob_fg, 0, 127, 63, 1,
			    wxPoint(293, 358), wxSize(32, 32));
  Lfo2Delay = new KnobCtrl(this, Synth_Lfo2_Delay, knob_bg, knob_fg, 0, 127, 63, 1,
			   wxPoint(336, 358), wxSize(32, 32));
  
  // FIXME make a list of oscillators and generate them dynamically & graphically
  Osc1 = new SineOscillator();
  Osc2 = new NoisePseudoOscillator();
  Adsr1 = new Adsr();
  Adsr2 = new Adsr();
  Lfo1 = new Lfo();
  Lfo2 = new Lfo();
  SetBackgroundColour(wxColour(200, 200, 200));
}

void SynthPlugin::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*BgBmp);    
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(), wxCOPY, FALSE);      
      upd++;
    }
  Plugin::OnPaintEvent(event);
}

void SynthPlugin::Process(float **input, float **output, long sample_length)
{
  if (NoteIsOn)
    {
      Mutex.Lock();
      bzero(output[0], sample_length * sizeof(float));
      bzero(output[1], sample_length * sizeof(float));
      Osc1->Accumulate(output, sample_length);
      Osc2->Accumulate(output, sample_length);
      Mutex.Unlock();
    }
}

void	 SynthPlugin::SetSamplingRate(double rate) 
{ 
  SampleRate = rate;
  Osc1->SetSampleRate(rate);
  Osc2->SetSampleRate(rate);
  cout << "Synth Sample Rate set to : " << rate << endl;
}

void SynthPlugin::ProcessEvent(WiredEvent &event)
{
  if ((event.MidiData[0] == M_NOTEON1) || (event.MidiData[0] == M_NOTEON2))
    {
      // FIXME Ew ..
      Freq = 440.f * static_cast<float>(pow(2, static_cast<float>(event.MidiData[1] - 69)/12.0f));
      // make that ^ a function ffs
      if (!event.MidiData[2]) 
	NoteIsOn = false; // FIXME handle polyphony and set this when seq->stop too
      else
	NoteIsOn = true;
      cout << "Note frequency : " << Freq << endl;
      // FIXME too ..
      Osc1->SetFreq(Freq);
      Osc2->SetFreq(Freq);
    }
  cout << "[SYNTH] Got midi in : " << event.MidiData[0] << " " 
       << event.MidiData[1] << " " << event.MidiData[2] << endl;
}

SynthPlugin::~SynthPlugin()
{
  delete BgBmp;
}

void SynthPlugin::Stop()
{
  NoteIsOn = false;
}

bool SynthPlugin::IsInstrument()
{
  return true;
}

bool SynthPlugin::IsAudio()
{
  return true;
}

bool SynthPlugin::IsMidi()
{
  return true;
}

wxBitmap *SynthPlugin::GetBitmap()
{
  return BgBmp;
}

void SynthPlugin::OnButtonClick(wxCommandEvent &e)
{
  SetBackgroundColour(*wxBLUE);
  Refresh();
}

void SynthPlugin::OnOsc1Phase(wxScrollEvent &event)
{
  int phase;
  
  Mutex.Lock();
  phase = Osc1Phase->GetValue();
  Mutex.Unlock();
  Osc1->SetPhase(phase); // FIXME float ? int ?
  cout << "Osc1 Phase set to : " << Osc1->GetPhase() << endl;
}

void SynthPlugin::OnOsc1Transpose(wxScrollEvent &event)
{
  int transpose;
  
  Mutex.Lock();
  transpose = Osc1Transpose->GetValue();
  Mutex.Unlock();
  Osc1->SetTranspose(transpose);
  cout << "Osc1 Transpose set to : " << Osc1->GetTranspose() << endl;
}

void SynthPlugin::OnOsc1Fine(wxScrollEvent &event)
{
  int fine;
  
  Mutex.Lock();
  fine = Osc1Fine->GetValue();
  Mutex.Unlock();
  Osc1->SetFine(fine);
  cout << "Osc1 Fineset to : " << Osc1->GetFine() << endl;
}

void SynthPlugin::OnOsc2Phase(wxScrollEvent &event)
{
  int phase;
  
  Mutex.Lock();
  phase = Osc2Phase->GetValue();
  Mutex.Unlock();
  Osc2->SetPhase(phase); // FIXME float ? int ?
  cout << "Osc2 Phase set to : " << Osc2->GetPhase() << endl;
}

void SynthPlugin::OnOsc2Transpose(wxScrollEvent &event)
{
  int transpose;
  
  Mutex.Lock();
  transpose = Osc2Transpose->GetValue();
  Mutex.Unlock();
  Osc2->SetTranspose(transpose);
  cout << "Osc2 Transpose set to : " << Osc2->GetTranspose() << endl;
}

void SynthPlugin::OnOsc2Fine(wxScrollEvent &event)
{
  int fine;
  
  Mutex.Lock();
  fine = Osc2Fine->GetValue();
  Mutex.Unlock();
  Osc2->SetFine(fine);
  cout << "Osc2 Fine set to : " << Osc2->GetFine() << endl;
}

void SynthPlugin::OnAdsr1Attack(wxScrollEvent &event)
{
  int attack;

  Mutex.Lock();
  attack = Adsr1Attack->GetValue();
  Mutex.Unlock();
  Adsr1->SetAttack(attack);
  cout << "Adsr1 Attack set to : " << Adsr1->GetAttack() << endl;
}

void SynthPlugin::OnAdsr1Decay(wxScrollEvent &event)
{
  int decay;

  Mutex.Lock();
  decay = Adsr1Decay->GetValue();
  Mutex.Unlock();
  Adsr1->SetDecay(decay);
  cout << "Adsr1 Decay set to : " << Adsr1->GetDecay() << endl;
}

void SynthPlugin::OnAdsr1Sustain(wxScrollEvent &event)
{
  int sustain;

  Mutex.Lock();
  sustain = Adsr1Sustain->GetValue();
  Mutex.Unlock();
  Adsr1->SetSustain(sustain);
  cout << "Adsr1 Sustain set to : " << Adsr1->GetSustain() << endl;
}

void SynthPlugin::OnAdsr1Release(wxScrollEvent &event)
{
  int release;

  Mutex.Lock();
  release = Adsr1Release->GetValue();
  Mutex.Unlock();
  Adsr1->SetRelease(release);
  cout << "Adsr1 Release set to : " << Adsr1->GetRelease() << endl;
}

void SynthPlugin::OnAdsr2Attack(wxScrollEvent &event)
{
  int attack;

  Mutex.Lock();
  attack = Adsr2Attack->GetValue();
  Mutex.Unlock();
  Adsr2->SetAttack(attack);
  cout << "Adsr2 Attack set to : " << Adsr2->GetAttack() << endl;
}

void SynthPlugin::OnAdsr2Decay(wxScrollEvent &event)
{
  int decay;

  Mutex.Lock();
  decay = Adsr2Decay->GetValue();
  Mutex.Unlock();
  Adsr2->SetDecay(decay);
  cout << "Adsr2 Decay set to : " << Adsr2->GetDecay() << endl;
}

void SynthPlugin::OnAdsr2Sustain(wxScrollEvent &event)
{
  int sustain;

  Mutex.Lock();
  sustain = Adsr2Sustain->GetValue();
  Mutex.Unlock();
  Adsr2->SetSustain(sustain);
  cout << "Adsr2 Sustain set to : " << Adsr2->GetSustain() << endl;
}

void SynthPlugin::OnAdsr2Release(wxScrollEvent &event)
{
  int release;

  Mutex.Lock();
  release = Adsr2Release->GetValue();
  Mutex.Unlock();
  Adsr2->SetRelease(release);
  cout << "Adsr2 Release set to : " << Adsr2->GetRelease() << endl;
}

void SynthPlugin::OnLfo1Rate(wxScrollEvent &event)
{
  int rate;

  Mutex.Lock();
  rate = Lfo1Rate->GetValue();
  Mutex.Unlock();
  Lfo1->SetRate(rate);
  cout << "Lfo1 Rate set to : " << Lfo1->GetRate() << endl;
}

void SynthPlugin::OnLfo1Amount(wxScrollEvent &event)
{
  int amount;

  Mutex.Lock();
  amount = Lfo1Amount->GetValue();
  Mutex.Unlock();
  Lfo1->SetAmount(amount);
  cout << "Lfo1 Amount set to : " << Lfo1->GetAmount() << endl;
}

void SynthPlugin::OnLfo1Delay(wxScrollEvent &event)
{
  int delay;

  Mutex.Lock();
  delay = Lfo1Delay->GetValue();
  Mutex.Unlock();
  Lfo1->SetDelay(delay);
  cout << "Lfo1 Delay set to : " << Lfo1->GetDelay() << endl;
}

void SynthPlugin::OnLfo2Rate(wxScrollEvent &event)
{
  int rate;

  Mutex.Lock();
  rate = Lfo2Rate->GetValue();
  Mutex.Unlock();
  Lfo2->SetRate(rate);
  cout << "Lfo2 Rate set to : " << Lfo2->GetRate() << endl;
}

void SynthPlugin::OnLfo2Amount(wxScrollEvent &event)
{
  int amount;

  Mutex.Lock();
  amount = Lfo2Amount->GetValue();
  Mutex.Unlock();
  Lfo2->SetAmount(amount);
  cout << "Lfo2 Amount set to : " << Lfo2->GetAmount() << endl;
}

void SynthPlugin::OnLfo2Delay(wxScrollEvent &event)
{
  int delay;

  Mutex.Lock();
  delay = Lfo2Delay->GetValue();
  Mutex.Unlock();
  Lfo2->SetDelay(delay);
  cout << "Lfo2 Delay set to : " << Lfo2->GetDelay() << endl;
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "SY31");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_INSTR;  
    info.UnitsX = 3;
    info.UnitsY = 5;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    return new SynthPlugin(*startinfo, &info);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








