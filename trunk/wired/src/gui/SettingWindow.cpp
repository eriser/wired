// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "SettingWindow.h"
#include "AudioEngine.h"
#include "Settings.h"
#include "Colour.h"
#include "../midi/MidiThread.h"
#include "../midi/MidiDevice.h"
#include <sstream>

#define	CATEGORY_ID	45001


#define	WIN_WIDTH	650
#define WIN_HEIGHT	500
#define WIN_SIZE	wxSize(WIN_WIDTH, WIN_HEIGHT)

#define WIN_MARGIN	12

#define	BTN_WIDTH	88
#define BTN_HEIGHT	32
#define	BTN_SIZE	wxSize(BTN_WIDTH, BTN_HEIGHT)
#define	BTN_POS_Y	(WIN_HEIGHT - WIN_MARGIN - BTN_HEIGHT)
#define BTN_CANCEL_POS	wxPoint(WIN_WIDTH - 3 * (BTN_WIDTH + 6) + 6 - WIN_MARGIN, BTN_POS_Y)
#define BTN_OK_POS	wxPoint(WIN_WIDTH - BTN_WIDTH - WIN_MARGIN, BTN_POS_Y)
#define BTN_APPLY_POS	wxPoint(WIN_WIDTH - 2 * BTN_WIDTH - 6 - WIN_MARGIN, BTN_POS_Y)

#define PAN_SIZE	wxSize(584, 436)
#define PAN_POS		wxPoint(222, WIN_MARGIN)

SettingWindow::SettingWindow()
  : wxDialog(0x0, -1, "Wired Settings", wxDefaultPosition, WIN_SIZE)
{
  wxTreeItemId	root;
  wxTreeItemId	id;

  MidiLoaded = false;
  AudioLoaded = false;
  Center();
  
  // GeneralBtn = new wxToggleButton(this, Setting_General, _("General"), 
// 				  wxPoint(8, 8), wxSize(80, 28));
//   AudioBtn = new wxToggleButton(this, Setting_Audio, _("Audio"), 
// 				wxPoint(92, 8), wxSize(80, 28));
//   MidiBtn = new wxToggleButton(this, Setting_Midi, _("MIDI"), wxPoint(176, 8), wxSize(80, 28));
  
  SettingsTree = new wxTreeCtrl(this, CATEGORY_ID, wxPoint(WIN_MARGIN, WIN_MARGIN), 
				wxSize(206, 400), wxSUNKEN_BORDER | wxTR_NO_LINES |
				wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_HIDE_ROOT |
				wxTR_FULL_ROW_HIGHLIGHT);
  //SettingsTree->SetIndent(20);
  GeneralPanelView();
  AudioPanelView();
  AudioInputPanelView();
  AudioOutputPanelView();
  MidiPanelView();
  
  SettingsTree->AddRoot("", -1, -1);
  root = SettingsTree->GetRootItem();
  SettingsTree->AppendItem(root, "General", -1, -1, (wxTreeItemData*)GeneralPanel);
  id = SettingsTree->AppendItem(root, "Audio", -1, -1, (wxTreeItemData*)AudioPanel);
  SettingsTree->AppendItem(id, "Output", -1, -1, (wxTreeItemData*)AudioOutputPanel);
  SettingsTree->AppendItem(id, "Input", -1, -1, (wxTreeItemData*)AudioInputPanel);
  SettingsTree->AppendItem(root, wxString("Midi"), -1, -1, (wxTreeItemData*)MidiPanel);

  AudioPanel->Show(false);
  MidiPanel->Show(false);
  AudioInputPanel->Show(false);
  AudioOutputPanel->Show(false);
  GeneralPanel->Show(true);
  CurrentPanel = GeneralPanel;

  OkBtn = new wxButton(this, wxID_OK, _("OK"), BTN_OK_POS, BTN_SIZE);
  ApplyBtn = new wxButton(this, wxID_APPLY, _("Apply"), BTN_APPLY_POS, BTN_SIZE);
  CancelBtn = new wxButton(this, wxID_CANCEL, _("Cancel"), BTN_CANCEL_POS, BTN_SIZE);
  
  Load();
}

//
// Creates the general panel prefs
//

void				SettingWindow::GeneralPanelView()
{
   GeneralPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);
   QuickWaveBox = new wxCheckBox(GeneralPanel, -1, _("Quickly render waveforms"), wxPoint(8, 8));
   dBWaveBox = new wxCheckBox(GeneralPanel, -1, _("Render waveforms in dB mode"), wxPoint(8, 28));
   undoRedoMaxDepthTextCtrl = new wxTextCtrl(GeneralPanel, -1, "", wxPoint(218, 50), wxSize(45, 25));
   undoRedoMaxDepthStaticText = new wxStaticText(GeneralPanel, -1, _("Undo redo maximum depth"), wxPoint(10, 50));
}

// Creates the panel for global audio settings

void				SettingWindow::AudioPanelView()
{
  AudioPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);
  wxStaticText *t = new wxStaticText(AudioPanel, -1, _("Sample format:"), wxPoint(8, 10));

  int x1, x2;
  t->GetSize(&x1, 0x0);
  BitsChoice = new wxChoice(AudioPanel, Setting_Bits, wxPoint(12 + x1, 2), wxSize(80, -1), 0, 0x0);  
  t = new wxStaticText(AudioPanel, -1, _("Sample rate:"), 
				     wxPoint(22 + x1 + BitsChoice->GetSize().x, 10));
  t->GetSize(&x2, 0x0);
  RateChoice = new wxChoice(AudioPanel, Setting_Rate, wxPoint(26 + x1 + BitsChoice->GetSize().x + x2, 2), 
			    wxSize(96, -1), 0, 0x0);

  Latency = new wxStaticText(AudioPanel, -1, _("Latency:"), wxPoint(8, 50));
  LatencySlider = new wxSlider(AudioPanel, Setting_Latency, 4096, 0, 65536, wxPoint(8, 70), wxSize(368, -1));
  LatencySlider->SetRange(0, 8);
  LatencySlider->SetPageSize(1);
  Latencies = new int [9];
  for (int i = 0; i < 9; i++)
    Latencies[i] = 16 << i;
}

//
// Creates the panel for audio input settings
//

void				SettingWindow::AudioInputPanelView()
{
  vector<Device*>::iterator	i;

  AudioInputPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);
  new wxStaticText(AudioInputPanel, -1, _("Select Input sound card:"),
		   wxPoint(8, 8));
  InputChoice = new wxChoice(AudioInputPanel, Setting_InputDev, wxPoint(8, 30), wxSize(368, -1), 0, 0x0);
  InputChoice->Append(wxString(_("None")));
  InputChoice->SetSelection(0);
  for (i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++)
    InputChoice->Append(wxString((*i)->Name.c_str()));
  new wxStaticText(AudioInputPanel, -1, _("Select Input channels to use with this sound card:"), 
		   wxPoint(8, 70));
  InputList = new wxCheckListBox(AudioInputPanel, Setting_InputChan, wxPoint(8, 94), wxSize(368, 68), 0);
}

//
// Creates the panel for audio output settings
//

void				SettingWindow::AudioOutputPanelView()
{
  vector<Device*>::iterator	i;

  AudioOutputPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);
  new wxStaticText(AudioOutputPanel, -1, _("Select Output sound card:"), 
		   wxPoint(8, 8));
  OutputChoice = new wxChoice(AudioOutputPanel, Setting_OutputDev, wxPoint(8, 30), wxSize(368, -1), 0, 0x0);
  OutputChoice->Append(wxString(_("None")));
  OutputChoice->SetSelection(0);
  for (i = Audio->DeviceList.begin(); i != Audio->DeviceList.end(); i++)
    OutputChoice->Append(wxString((*i)->Name.c_str()));
  new wxStaticText(AudioOutputPanel, -1, _("Select left and right Output channels for this sound card:"), 
		   wxPoint(8, 70));
  OutputList = new wxCheckListBox(AudioOutputPanel, Setting_OutputChan, wxPoint(8, 94), wxSize(368, 68), 0);
}

// Creates the panel for midi settings

void				SettingWindow::MidiPanelView()
{
  MidiPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);
  new wxStaticText(MidiPanel, -1, _("Select MIDI In devices to use:"), 
		   wxPoint(8, 8));
  MidiInList = new wxCheckListBox(MidiPanel, Setting_MidiIn, wxPoint(8, 30), wxSize(368, 200), 0);

  // wxListCtrl *list = new wxListCtrl(MidiPanel, -1, wxPoint(10, 300), wxSize(368, 50), wxSUNKEN_BORDER);
//   list->InsertColumn(0, _("Item"), wxLIST_FORMAT_LEFT, 200);
//   list->InsertColumn(1, _("Value"), wxLIST_FORMAT_LEFT, 80);

}

SettingWindow::~SettingWindow()
{
  delete [] Latencies;
}

//
// Callback when user selects a settings' category
//

void			SettingWindow::OnSelPrefCategory(wxTreeEvent &e)
{
  wxTreeItemId		item = e.GetItem();
  wxPanel		*tmp = (wxPanel *)SettingsTree->GetItemData(item);
  
  if (tmp)
    {
      CurrentPanel->Show(false);
      CurrentPanel = tmp;
      tmp->Show(true);
    }
}

void SettingWindow::OnAudioClick(wxCommandEvent &event)
{
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
		    s.Printf(_("Output %d"), j);
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
		    s.Printf(_("Input %d"), j);
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
	  s.Printf(_("In %d: %s"), k, (*j)->Name.c_str());
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
	      s.Printf(_("Input %d"), j);
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
		  s.Printf(_("Output %d"), j);
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
      wxMessageDialog msg(this, _("No more than 2 output channels can be selected"), "Wired", 
			  wxOK | wxICON_EXCLAMATION);
      msg.ShowModal();
    }
}

void SettingWindow::Load()
{
	ostringstream	oss;
	
  QuickWaveBox->SetValue(WiredSettings->QuickWaveRender);
  dBWaveBox->SetValue(WiredSettings->dbWaveRender);
  oss << WiredSettings->maxUndoRedoDepth;
  undoRedoMaxDepthTextCtrl->SetValue(oss.str());
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
	
//	AudioMutex.Lock();
//	MidiMutex.Lock();
  long i;
  istringstream	iss((string)undoRedoMaxDepthTextCtrl->GetValue());

  WiredSettings->QuickWaveRender = QuickWaveBox->IsChecked();
  WiredSettings->dbWaveRender = dBWaveBox->IsChecked();
  WiredSettings->OutputDev = OutputChoice->GetSelection() - 1;
  WiredSettings->InputDev = InputChoice->GetSelection() - 1;
  iss >> WiredSettings->maxUndoRedoDepth;

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
//	AudioMutex.Unlock();
//	MidiMutex.Unlock();
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
		  BitsChoice->Append(wxString(_("8 bits[currently unsupported]")));
		  break;
		}
	      case paUInt8 : 
		{
		  BitsChoice->Append(wxString(_("8 bits (unsigned)[currently unsupported]")));
		  break;
		}
	      case paInt16 : 
		{
		  BitsChoice->SetSelection(
		    BitsChoice->Append(wxString(_("16 bits[currently unsupported]"))));
		  break;
		}
	      case paInt24 : 
		{
		  BitsChoice->Append(wxString(_("24 bits[currently unsupported]")));
		  break;
		}
	      case paInt32 : 
		{
		  BitsChoice->Append(wxString(_("32 bits[currently unsupported]")));
		  break;
		}
	      case paFloat32 : 
		{
		  BitsChoice->Append(wxString(_("32 bits (float)")));
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
  
  s.Printf(_("Latency: %d samples per buffer, %.2f msec"), 
	   Latencies[LatencySlider->GetValue()], 
	   static_cast<float>( static_cast<double>(
	   (Latencies[LatencySlider->GetValue()]/res)*1000.0) ) );
  Latency->SetLabel(s);
}

BEGIN_EVENT_TABLE(SettingWindow, wxDialog)
  // EVT_TOGGLEBUTTON(Setting_General, SettingWindow::OnGeneralClick)
//   EVT_TOGGLEBUTTON(Setting_Audio, SettingWindow::OnAudioClick)
//   EVT_TOGGLEBUTTON(Setting_Midi, SettingWindow::OnMidiClick)
  EVT_BUTTON(wxID_OK, SettingWindow::OnOkClick)
  EVT_BUTTON(wxID_CANCEL,SettingWindow:: OnCancelClick)
  EVT_BUTTON(wxID_APPLY, SettingWindow::OnApplyClick)
  EVT_CHOICE(Setting_OutputDev, SettingWindow::OnOutputDevClick)
  EVT_CHOICE(Setting_InputDev, SettingWindow::OnInputDevClick)
  EVT_CHOICE(Setting_Bits, SettingWindow::OnSampleFormatClick)
  EVT_CHOICE(Setting_Rate, SettingWindow::OnSampleRateClick)
  EVT_CHECKLISTBOX(Setting_OutputChan, SettingWindow::OnOutputChanClick)
  EVT_SLIDER(Setting_Latency, SettingWindow::OnLatencyChange)
  EVT_TREE_SEL_CHANGED(CATEGORY_ID, SettingWindow::OnSelPrefCategory)
END_EVENT_TABLE()
