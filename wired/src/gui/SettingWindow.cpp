// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "SettingWindow.h"
#include "AudioEngine.h"
#include "Settings.h"
#include "Colour.h"
#include "../midi/MidiThread.h"
#include "../midi/MidiDevice.h"

SettingWindow::SettingWindow()
  : wxDialog(0x0, -1, "Wired Settings", wxDefaultPosition, wxSize(400, 516))
{
  vector<Device*>::iterator i;

  MidiLoaded = false;
  AudioLoaded = false;
  Center();
  GeneralBtn = new wxToggleButton(this, Setting_General, "General", 
		       wxPoint(8, 8), wxSize(80, 22));
  AudioBtn = new wxToggleButton(this, Setting_Audio, "Audio", 
				wxPoint(92, 8), wxSize(80, 22));
  MidiBtn = new wxToggleButton(this, Setting_Midi, "MIDI", wxPoint(176, 8), wxSize(80, 22));

  // GENERAL panel
  GeneralPanel = new wxPanel(this, -1, wxPoint(8, 38), wxSize(384, 436), wxNO_BORDER);
  QuickWaveBox = new wxCheckBox(GeneralPanel, -1, "Quickly render waveforms", wxPoint(8, 8));
  dBWaveBox = new wxCheckBox(GeneralPanel, -1, "Render waveforms in dB mode", wxPoint(8, 28));

  // AUDIO panel
  AudioPanel = new wxPanel(this, -1, wxPoint(8, 38), wxSize(384, 436), wxNO_BORDER);

  // AUDIO/Output
  new wxStaticText(AudioPanel, -1, "Select Output sound card:", 
		   wxPoint(8, 8));
  OutputChoice = new wxChoice(AudioPanel, Setting_OutputDev, wxPoint(8, 30), wxSize(368, -1), 0, 0x0);
  OutputChoice->Append(wxString("None"));
  OutputChoice->SetSelection(0);

  for (i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++)
    {
      OutputChoice->Append(wxString((*i)->Name.c_str()));
    }

  new wxStaticText(AudioPanel, -1, "Select left and right Output channels for this sound card:", 
		   wxPoint(8, 70));
  OutputList = new wxCheckListBox(AudioPanel, Setting_OutputChan, wxPoint(8, 94), wxSize(368, 68), 0);

  wxStaticText *t = new wxStaticText(AudioPanel, -1, "Sample format:", wxPoint(8, 180));

  int x1, x2;
  t->GetSize(&x1, 0x0);

  BitsChoice = new wxChoice(AudioPanel, Setting_Bits, wxPoint(12 + x1, 172), wxSize(80, -1), 0, 0x0);  

  t = new wxStaticText(AudioPanel, -1, "Sample rate:", 
				     wxPoint(22 + x1 + BitsChoice->GetSize().x, 180));
  
  t->GetSize(&x2, 0x0);
  RateChoice = new wxChoice(AudioPanel, Setting_Rate, wxPoint(26 + x1 + BitsChoice->GetSize().x + x2, 172), 
			    wxSize(96, -1), 0, 0x0);

  Latency = new wxStaticText(AudioPanel, -1, "Latency:", wxPoint(8, 212));
  LatencySlider = new wxSlider(AudioPanel, Setting_Latency, 4096, 0, 65536, wxPoint(8, 232), wxSize(368, -1));
  LatencySlider->SetRange(0, 8);
  LatencySlider->SetPageSize(1);
  Latencies = new int [9];
  Latencies[0] = 16;
  Latencies[1] = 16*2;
  Latencies[2] = 16*2*2;
  Latencies[3] = 16*2*2*2;
  Latencies[4] = 16*2*2*2*2;
  Latencies[5] = 16*2*2*2*2*2;
  Latencies[6] = 16*2*2*2*2*2*2;
  Latencies[7] = 16*2*2*2*2*2*2*2;
  Latencies[8] = 16*2*2*2*2*2*2*2*2;
    
  // AUDIO/Input
  new wxStaticText(AudioPanel, -1, "Select Input sound card:", 
		   wxPoint(8, 270));
  InputChoice = new wxChoice(AudioPanel, Setting_InputDev, wxPoint(8, 290), wxSize(368, -1), 0, 0x0);
  InputChoice->Append(wxString("None"));
  InputChoice->SetSelection(0);

  for (i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++)
    {
      InputChoice->Append(wxString((*i)->Name.c_str()));
    }

  new wxStaticText(AudioPanel, -1, "Select Input channels to use with this sound card:", 
		   wxPoint(8, 330));
  InputList = new wxCheckListBox(AudioPanel, Setting_InputChan, wxPoint(8, 352), wxSize(368, 68), 0);
  /*  wxStaticText* restart = 
      new wxStaticText(AudioPanel, -1, "Please restart wired to store and apply changes", wxPoint(8, 420)); That's not true :) */

  
  // MIDI panel
  MidiPanel = new wxPanel(this, -1, wxPoint(8, 38), wxSize(384, 436), wxNO_BORDER);
  new wxStaticText(MidiPanel, -1, "Select MIDI In devices to use:", 
		   wxPoint(8, 8));
  MidiInList = new wxCheckListBox(MidiPanel, Setting_MidiIn, wxPoint(8, 30), wxSize(368, 200), 0);

  AudioPanel->SetBackgroundColour(CL_SEQ_BACKGROUND);
  MidiPanel->SetBackgroundColour(CL_SEQ_BACKGROUND);
  GeneralPanel->SetBackgroundColour(CL_SEQ_BACKGROUND);

  AudioPanel->Show(false);
  MidiPanel->Show(false);
  GeneralPanel->Show(true);
  GeneralBtn->SetValue(true);

  OkBtn = new wxButton(this, Setting_Ok, "OK", wxPoint(180, 484), wxSize(64, 22));
  ApplyBtn = new wxButton(this, Setting_Apply, "Apply", wxPoint(248, 484), wxSize(64, 22));
  CancelBtn = new wxButton(this, Setting_Cancel, "Cancel", wxPoint(316, 484), wxSize(64, 22));
  
  Load();
}

SettingWindow::~SettingWindow()
{
  delete [] Latencies;
}

void SettingWindow::OnGeneralClick(wxCommandEvent &event)
{
  GeneralBtn->SetValue(true);
  AudioBtn->SetValue(false);
  MidiBtn->SetValue(false);
  AudioPanel->Show(false);
  MidiPanel->Show(false);
  GeneralPanel->Show(true);
}

void SettingWindow::OnAudioClick(wxCommandEvent &event)
{
  GeneralBtn->SetValue(false);
  AudioBtn->SetValue(true);
  MidiBtn->SetValue(false);
  AudioPanel->Show(true);
  MidiPanel->Show(false);
  GeneralPanel->Show(false);

  if (!AudioLoaded)
    {
      vector<Device*>::iterator i;
      vector<long>::iterator k;
      int j, val;
      wxString s;

      AudioLoaded = true;
      OutputList->Clear();
      if ((val = OutputChoice->GetSelection()) > 0)
	for (j = 1, i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++, j++)
	  {
	    if (j == val)
	      {
		for (j = 1; j <= (*i)->MaxOutputChannels; j++)
		  {
		    s.Printf("Output %d", j);
		    OutputList->Append(s);
		  }
		break;
	    }
	}
      InputList->Clear();
      if ((val = InputChoice->GetSelection()) > 0)
	for (j = 1, i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++, j++)
	  {
	    if (j == val)
	      {
		for (j = 1; j <= (*i)->MaxInputChannels; j++)
		  {
		    s.Printf("Input %d", j);
		    InputList->Append(s);
		  }
		break;
	    }
	}
      for (k = WiredSettings->OutputChannels.begin(); k != WiredSettings->OutputChannels.end(); k++)
	if (*k < OutputList->GetCount())
	  OutputList->Check(*k);
      for (k = WiredSettings->InputChannels.begin(); k != WiredSettings->InputChannels.end(); k++)
	if (*k < InputList->GetCount())
	  InputList->Check(*k);

      LoadSampleFormat();

      if (WiredSettings->SampleRate < RateChoice->GetCount())
	RateChoice->SetSelection(WiredSettings->SampleRate);
      if (WiredSettings->SampleFormat < BitsChoice->GetCount())
	WiredSettings->SampleFormat = BitsChoice->GetSelection();      
      
      for (int i = 0; i < 9; i++)
	if (Latencies[i] == WiredSettings->SamplesPerBuffer)
	  LatencySlider->SetValue(i);
      UpdateLatency();
    }  

}

void SettingWindow::OnMidiClick(wxCommandEvent &event)
{
  GeneralBtn->SetValue(false);
  AudioBtn->SetValue(false);
  MidiBtn->SetValue(true);
  AudioPanel->Show(false);
  MidiPanel->Show(true);
  GeneralPanel->Show(false);

  if (!MidiLoaded)
    {
      vector<long>::iterator i;
      MidiDeviceList::iterator j;
      int k;
      wxString s;

      MidiLoaded = true;
      MidiInList->Clear();
      for (j = MidiEngine->DeviceList.begin(), k = 1; j != MidiEngine->DeviceList.end(); j++, k++)
	{      
	  s.Printf("In %d: %s", k, (*j)->Name.c_str());
	  MidiInList->Append(s);
	}
      for (i = WiredSettings->MidiIn.begin(); i != WiredSettings->MidiIn.end(); i++)
	if (*i < MidiInList->GetCount())
	  MidiInList->Check(*i);
    }
}

void SettingWindow::OnOkClick(wxCommandEvent &event)
{
  Save();
  EndModal(wxID_OK);
}

void SettingWindow::OnCancelClick(wxCommandEvent &event)
{
  EndModal(wxID_CANCEL);
}

void SettingWindow::OnApplyClick(wxCommandEvent &event)
{
  /*
    Audio->CloseStream();
  
  */
  Save();
  
  /*
  cout << "[CONF]" << endl;
  cout << "[CONF]" << endl;
  cout << "[CONF] OutputDev " << WiredSettings->OutputDev << endl;
  cout << "[CONF] InputDev " << WiredSettings->InputDev << endl;
  cout << "[CONF] OutputLatency " << WiredSettings->OutputLatency << endl;
  cout << "[CONF] InputLatency " << WiredSettings->InputLatency << endl;
  cout << "[CONF] SampleFormat " << WiredSettings->SampleFormat << endl;
  cout << "[CONF] SampleRate " << WiredSettings->SampleRate << endl;
  cout << "[CONF] SamplesPerBuffer " << WiredSettings->SamplesPerBuffer << endl;
  cout << "[CONF]" << endl;
  cout << "[CONF]" << endl;
  
  Audio->GetDeviceSettings();
  Audio->OpenStream();
  */
}

void SettingWindow::OnInputDevClick(wxCommandEvent &event)
{
  vector<Device*>::iterator i;
  int j, val;
  wxString s;

  InputList->Clear();
  if ((val = InputChoice->GetSelection()) == 0)
    return;
  for (j = 1, i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++, j++)
    {
      if (j == val)
	{
	  for (j = 1; j <= (*i)->MaxInputChannels; j++)
	    {
	      s.Printf("Input %d", j);
	      InputList->Append(s);
	    }
	  return;
	}
    }  
}

void SettingWindow::OnOutputDevClick(wxCommandEvent &event)
{
  vector<Device*>::iterator i;
  int j, val;
  wxString s;

  OutputList->Clear();
  if ((val = OutputChoice->GetSelection()) != 0)
    {
      for (j = 1, i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++, j++)
	{
	  if (j == val)
	    {
	      for (j = 1; j <= (*i)->MaxOutputChannels; j++)
		{
		  s.Printf("Output %d", j);
		  OutputList->Append(s);
		  // on check les premieres sorties l/r
		  if (j < 3)
		    OutputList->Check(j - 1);
		}
	      break;
	    }
	}  
    }
  LoadSampleFormat();
}

void SettingWindow::OnOutputChanClick(wxCommandEvent &event)
{
  int i, j;

  j = 0;
  for (i = 0; i < OutputList->GetCount() - 1; i++)
    if (OutputList->IsChecked(i))
      j++;
  // pas plus de 2 channel de sortie
  if (j > 2)
    {
      OutputList->Check(event.GetInt(), false);
      wxMessageDialog msg(this, "No more than 2 output channels can be selected", "Wired", 
			  wxOK | wxICON_EXCLAMATION);
      msg.ShowModal();
    }
}

void SettingWindow::Load()
{
  QuickWaveBox->SetValue(WiredSettings->QuickWaveRender);
  dBWaveBox->SetValue(WiredSettings->dbWaveRender);  
  if (WiredSettings->OutputDev > -1)
    OutputChoice->SetSelection(WiredSettings->OutputDev + 1);
  if (WiredSettings->InputDev > -1)
    InputChoice->SetSelection(WiredSettings->InputDev + 1);
  if (WiredSettings->SamplesPerBuffer > 0)
    {
      for (int i = 0; i < 9; i++)
	if (Latencies[i] == WiredSettings->SamplesPerBuffer)
	  LatencySlider->SetValue(i);
    }
}

void SettingWindow::Save()
{
  long i;

  WiredSettings->QuickWaveRender = QuickWaveBox->IsChecked();
  WiredSettings->dbWaveRender = dBWaveBox->IsChecked();
  WiredSettings->OutputDev = OutputChoice->GetSelection() - 1;
  WiredSettings->InputDev = InputChoice->GetSelection() - 1;

  if (AudioLoaded)
    {
      WiredSettings->OutputChannels.clear();
      for (i = 0; i < OutputList->GetCount(); i++)
	if (OutputList->IsChecked(i))
	  WiredSettings->OutputChannels.push_back(i);
      
      WiredSettings->InputChannels.clear();
      for (i = 0; i < InputList->GetCount(); i++)
	if (InputList->IsChecked(i))
	  WiredSettings->InputChannels.push_back(i);
      WiredSettings->SampleRate = RateChoice->GetSelection();
      
      SetDefaultSampleFormat();		//forcing 32 bit floats
      WiredSettings->SampleFormat = BitsChoice->GetSelection();
      
      WiredSettings->SamplesPerBuffer = Latencies[LatencySlider->GetValue()];
    }
  if (MidiLoaded)
    {
      WiredSettings->MidiIn.clear();
      for (i = 0; i < MidiInList->GetCount(); i++)
	if (MidiInList->IsChecked(i))
	  WiredSettings->MidiIn.push_back(i);
      
      WiredSettings->Save();
    }
}

void SettingWindow::LoadSampleFormat()
{
  vector<DeviceFormat *>::iterator i;
  unsigned int k = OutputChoice->GetSelection() - 1;
  
  BitsChoice->Clear();
  if (k < Audio->DeviceList.size())
    {
      Device *dev = Audio->DeviceList.at(k);
      for (i = dev->SupportedFormats.begin(); i != dev->SupportedFormats.end(); 
	   i++)
	if ((*i)->SampleRates.size() > 0)
	  {
	    switch ((*i)->SampleFormat)
	      {
	      case paInt8 : 
		{
		  BitsChoice->Append(wxString("8 bits[currently unsupported]"));
		  break;
		}
	      case paUInt8 : 
		{
		  BitsChoice->Append(wxString("8 bits (unsigned)[currently unsupported]"));
		  break;
		}
	      case paInt16 : 
		{
		  BitsChoice->SetSelection(
		    BitsChoice->Append(wxString("16 bits[currently unsupported]")));
		  break;
		}
	      case paInt24 : 
		{
		  BitsChoice->Append(wxString("24 bits[currently unsupported]"));
		  break;
		}
	      case paInt32 : 
		{
		  BitsChoice->Append(wxString("32 bits[currently unsupported]"));
		  break;
		}
	      case paFloat32 : 
		{
		  BitsChoice->Append(wxString("32 bits (float)"));
		  break;
		}
	      }
	  }     
    }
  if (WiredSettings->SampleFormat < BitsChoice->GetCount())
    BitsChoice->SetSelection(WiredSettings->SampleFormat);
  LoadSampleRates();
}

void SettingWindow::LoadSampleRates()
{
  vector<double>::iterator i;
  wxString s;
  unsigned int k = BitsChoice->GetSelection();
  unsigned int j = OutputChoice->GetSelection() - 1;
  
  RateChoice->Clear();	 
  if (j < Audio->DeviceList.size())
    {
      Device *dev = Audio->DeviceList.at(j);
      if (k < dev->SupportedFormats.size())
	{
	  DeviceFormat *f = dev->SupportedFormats.at(k);
	  for (i = f->SampleRates.begin(); i != f->SampleRates.end(); i++)
	    {
	      s.Printf("%.0f Hz", *i);
	      k = RateChoice->Append(s);
	      if (*i == 44100.f)
		RateChoice->SetSelection(k);
	    }
	}
    }
  if (WiredSettings->SampleRate < RateChoice->GetCount())
    RateChoice->SetSelection(WiredSettings->SampleRate);
}

void SettingWindow::SetDefaultSampleFormat(void)
{
  unsigned int j = OutputChoice->GetSelection() - 1;
  
  if (j < Audio->DeviceList.size())
    {
      Device *dev = Audio->DeviceList.at(j);
      for (long k = 0; k < dev->SupportedFormats.size(); k++)
	if (dev->SupportedFormats[k]->SampleFormat == paFloat32)
	  BitsChoice->SetSelection(k);
    }
}

void SettingWindow::OnSampleFormatClick(wxCommandEvent &event)
{
  
  SetDefaultSampleFormat();	//forcing 32bits floats
  LoadSampleRates();
}

void SettingWindow::OnSampleRateClick(wxCommandEvent &event)
{
  UpdateLatency();
}

void SettingWindow::OnLatencyChange(wxCommandEvent &event)
{
  UpdateLatency();
}

void SettingWindow::UpdateLatency()
{
  wxString s;

  unsigned int j = OutputChoice->GetSelection() - 1;
  unsigned int k = BitsChoice->GetSelection();
  unsigned int l = RateChoice->GetSelection();
  double res = 0;
  
  if (j < Audio->DeviceList.size())
    {
      Device *dev = Audio->DeviceList.at(j);
      if (k < dev->SupportedFormats.size())
	{
	  DeviceFormat *f = dev->SupportedFormats[k];
	  if (l < f->SampleRates.size())
	    res = f->SampleRates[l];
	}
    }
  
  s.Printf("Latency: %d samples per buffer, %.2f msec", 
	   Latencies[LatencySlider->GetValue()], 
	   static_cast<float>( static_cast<double>(
	   (Latencies[LatencySlider->GetValue()]/res)*1000.0) ) );
  Latency->SetLabel(s);
}

BEGIN_EVENT_TABLE(SettingWindow, wxDialog)
  EVT_TOGGLEBUTTON(Setting_General, SettingWindow::OnGeneralClick)
  EVT_TOGGLEBUTTON(Setting_Audio, SettingWindow::OnAudioClick)
  EVT_TOGGLEBUTTON(Setting_Midi, SettingWindow::OnMidiClick)
  EVT_BUTTON(Setting_Ok, SettingWindow::OnOkClick)
  EVT_BUTTON(Setting_Cancel,SettingWindow:: OnCancelClick)
  EVT_BUTTON(Setting_Apply, SettingWindow::OnApplyClick)
  EVT_CHOICE(Setting_OutputDev, SettingWindow::OnOutputDevClick)
  EVT_CHOICE(Setting_InputDev, SettingWindow::OnInputDevClick)
  EVT_CHOICE(Setting_Bits, SettingWindow::OnSampleFormatClick)
  EVT_CHOICE(Setting_Rate, SettingWindow::OnSampleRateClick)
  EVT_CHECKLISTBOX(Setting_OutputChan, SettingWindow::OnOutputChanClick)
  EVT_SLIDER(Setting_Latency, SettingWindow::OnLatencyChange)
END_EVENT_TABLE()
