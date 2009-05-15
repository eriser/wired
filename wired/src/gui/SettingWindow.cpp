// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <vector>

#include <wx/imaglist.h>
#include <wx/image.h>

#include "MainApp.h"
#include "MainWindow.h"
#include "SettingWindow.h"
#include "AudioEngine.h"
#include "Settings.h"
#include "Colour.h"
#include "MidiThread.h"
#include "MidiDevice.h"

#include "WiredSampleRate.h"

#include "general.xpm"
#include "audio.xpm"
#include "output.xpm"
#include "input.xpm"
#include "midi.xpm"

#define	CATEGORY_ID	25001

#define WIN_SIZE	wxSize(650, 520)

#define MENUICON_SIZE   (20)
#ifndef RESIZE_ICON
# define RESIZE_ICON(ico, w, h)	(wxBitmap(ico).ConvertToImage().Rescale(w, h))
#endif

using namespace std;

SettingWindow::SettingWindow()
  : wxDialog(NULL, -1, _("Wired Settings"), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER)
{
  wxTreeItemId	root;
  wxTreeItemId	AudioItem;
  wxBoxSizer*	mainSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer*	horiSizer = new wxBoxSizer(wxHORIZONTAL);

  // set a minimal size to the window
  mainSizer->SetMinSize(WIN_SIZE);

  MidiLoaded = false;
  AudioLoaded = false;

  SettingsTree = new wxTreeCtrl(this, CATEGORY_ID, wxDefaultPosition, wxDefaultSize,
				wxSUNKEN_BORDER | wxTR_NO_LINES |
				wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_HIDE_ROOT |
				wxTR_FULL_ROW_HIGHLIGHT | wxTR_EXTENDED);

  wxImageList   *imagelist = new wxImageList(MENUICON_SIZE, MENUICON_SIZE, true);

  imagelist->Add(RESIZE_ICON(wxIcon(general_xpm), MENUICON_SIZE, MENUICON_SIZE));
  imagelist->Add(RESIZE_ICON(wxIcon(audio_xpm), MENUICON_SIZE, MENUICON_SIZE));
  imagelist->Add(RESIZE_ICON(wxIcon(output_xpm), MENUICON_SIZE, MENUICON_SIZE));
  imagelist->Add(RESIZE_ICON(wxIcon(input_xpm), MENUICON_SIZE, MENUICON_SIZE));
  imagelist->Add(RESIZE_ICON(wxIcon(midi_xpm), MENUICON_SIZE, MENUICON_SIZE));
  SettingsTree->AssignImageList(imagelist);

  // flags assigned to all sizer in all right panel
  BoxFlags.Left();
  BoxFlags.Expand();
  BoxFlags.Border();

  // initialize all view/panel
  GeneralPanelView();
  AudioPanelView();
  AudioInputPanelView();
  AudioOutputPanelView();
  MidiPanelView();

  SettingsTree->AddRoot(wxT(""), -1, -1);
  root = SettingsTree->GetRootItem();
  SettingsTree->AppendItem(root, _("General"), 0, -1, (wxTreeItemData*)GeneralPanel);
  AudioItem = SettingsTree->AppendItem(root, _("Audio"), 1, -1, (wxTreeItemData*)AudioPanel);
  SettingsTree->AppendItem(AudioItem, _("Output"), 2, -1, (wxTreeItemData*)AudioOutputPanel);
  SettingsTree->AppendItem(AudioItem, _("Input"), 3, -1, (wxTreeItemData*)AudioInputPanel);
  SettingsTree->AppendItem(root, _("Midi"), 4, -1, (wxTreeItemData*)MidiPanel);

  // start with audio item already expanded
  SettingsTree->Expand(AudioItem);

  // show only the current "general" panel
  AudioPanel->Show(false);
  MidiPanel->Show(false);
  AudioInputPanel->Show(false);
  AudioOutputPanel->Show(false);
  GeneralPanel->Show(true);
  CurrentPanel = GeneralPanel;

  // add colomns of the first "line" to the sizer
  horiSizer->Add(SettingsTree, wxSizerFlags().Proportion(40).Expand().Right().Border());
  horiSizer->Add(CurrentPanel, wxSizerFlags().Proportion(100).Expand().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Border());

  // create the standardized way to create dialog buttons
  wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();

  buttonSizer->AddButton(new wxButton(this, wxID_OK, _("OK")));
  buttonSizer->AddButton(new wxButton(this, wxID_APPLY, _("Apply")));
  buttonSizer->AddButton(new wxButton(this, wxID_CANCEL, _("Cancel")));
  // re-order buttons accordling to the theme and/or system
  buttonSizer->Realize();

  // add first and second line to the main sizer
  mainSizer->Add(horiSizer, wxSizerFlags().Proportion(100).Expand().Border());
  mainSizer->Add(buttonSizer, wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_BOTTOM).Border());

  // resize items and window
  SetSizer(mainSizer);
  mainSizer->SetSizeHints(this);

  // center the window on the screen
  Center();

  Load();
}

SettingWindow::~SettingWindow()
{
  // created in AudioPanelView
  delete [] Latencies;
}

//
// Creates the general panel prefs
//

void				SettingWindow::GeneralPanelView()
{
  wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* undoSizer = new wxBoxSizer(wxHORIZONTAL);

  GeneralPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

  QuickWaveBox = new wxCheckBox(GeneralPanel, -1, _("Quickly render waveforms"));
  dBWaveBox = new wxCheckBox(GeneralPanel, -1, _("Render waveforms in dB mode"));

  undoRedoMaxDepthStaticText = new wxStaticText(GeneralPanel, -1, _("Undo redo maximum depth :"));
  undoRedoMaxDepthTextCtrl = new wxTextCtrl(GeneralPanel, -1);

  undoSizer->Add(undoRedoMaxDepthStaticText, BoxFlags);
  undoSizer->Add(undoRedoMaxDepthTextCtrl, BoxFlags);

  panelSizer->Add(QuickWaveBox, BoxFlags);
  panelSizer->Add(dBWaveBox, BoxFlags);
  panelSizer->AddSpacer( 5 );
  panelSizer->Add(undoSizer, BoxFlags);

  GeneralPanel->SetSizer(panelSizer);
  panelSizer->SetSizeHints(this);
}

// Creates the panel for global audio settings

void				SettingWindow::AudioPanelView()
{
  AudioPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
}

//
// Creates the panel for audio input settings
//

void				SettingWindow::AudioInputPanelView()
{
  // right panel
  AudioInputPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

  // list of audio system
  InputSystemChoice = new wxChoice(AudioInputPanel, Setting_InputSystem);
  RefreshSystems(InputSystemChoice);

  // list of devices
  InputDeviceChoice = new wxChoice(AudioInputPanel, Setting_InputDev);
  RefreshDevices(InputDeviceChoice, InputSystemChoice->GetSelection(),
		 Audio->GetDefaultInputDevice()); // based on system selection

  // list of channels
  InputChannelList = new wxCheckListBox(AudioInputPanel, Setting_InputChan);
  RefreshChannels(InputChannelList,
		  InputSystemChoice->GetSelection(),
		  InputDeviceChoice->GetSelection(), true);

  // sizer
  InputBox = new wxBoxSizer(wxVERTICAL);
  InputBox->Add(new wxStaticText(AudioInputPanel, -1, _("Available sound system:")), BoxFlags);
  InputBox->Add(InputSystemChoice, BoxFlags);
  InputBox->Add(new wxStaticText(AudioInputPanel, -1, _("Select Input sound card:")), BoxFlags);
  InputBox->Add(InputDeviceChoice, BoxFlags);
  InputBox->Add(new wxStaticText(AudioInputPanel, -1, _("Select Input channels to use with this sound card:")),
		BoxFlags);
  InputBox->Add(InputChannelList, BoxFlags);

  AudioInputPanel->SetSizer(InputBox);
  InputBox->SetSizeHints(this);
}

//
// Creates the panel for audio output settings
//

void				SettingWindow::AudioOutputPanelView()
{
  // right panel
  AudioOutputPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

  //samplerate and bitrate choice
  BitRateText = new wxStaticText(AudioOutputPanel, -1, _("Sample format:"));
  BitsChoice = new wxChoice(AudioOutputPanel, Setting_Bits);
  SampleRateText = new wxStaticText(AudioOutputPanel, -1, _("Sample rate:"));
  RateChoice = new wxChoice(AudioOutputPanel, Setting_Rate);
  Latency = new wxStaticText(AudioOutputPanel, -1, _("Latency:"));
  LatencySlider = new wxSlider(AudioOutputPanel, Setting_Latency, 4096, 0, 65536);
  LatencySlider->SetRange(0, 8);
  LatencySlider->SetPageSize(1);
  Latencies = new int [9];

  for (int t = 0; t < 9; t++)
    Latencies[t] = 16 << t;

  // list of audio systems
  OutputSystemChoice = new wxChoice(AudioOutputPanel, Setting_OutputSystem);
  RefreshSystems(OutputSystemChoice);

  // list of devices
  OutputDeviceChoice = new wxChoice(AudioOutputPanel, Setting_OutputDev);
  RefreshDevices(OutputDeviceChoice, OutputSystemChoice->GetSelection(),
		 Audio->GetDefaultOutputDevice());

  // list of channels
  OutputChannelList = new wxCheckListBox(AudioOutputPanel, Setting_OutputChan);
  RefreshChannels(OutputChannelList,
		  OutputSystemChoice->GetSelection(),
		  OutputDeviceChoice->GetSelection(), false);

  // sizer
  OutputBox = new wxBoxSizer(wxVERTICAL);
  OutputBox->Add(new wxStaticText(AudioOutputPanel, -1, _("Available sound system:")), BoxFlags);
  OutputBox->Add(OutputSystemChoice, BoxFlags);
  OutputBox->Add(new wxStaticText(AudioOutputPanel, -1, _("Select Output sound card:")), BoxFlags);
  OutputBox->Add(OutputDeviceChoice, BoxFlags);
  OutputBox->Add(new wxStaticText(AudioOutputPanel, -1, _("Select left and right Output channels for this sound card:")),
		 BoxFlags);
  OutputBox->Add(OutputChannelList, BoxFlags);
  //add samplerate and bitrate choice
  wxBoxSizer *BitRateBox =  new wxBoxSizer(wxHORIZONTAL);
  BitRateBox->Add(BitRateText, BoxFlags);
  BitRateBox->Add(BitsChoice, BoxFlags);

  OutputBox->Add(BitRateBox, BoxFlags);

  wxBoxSizer *SampleRateBox =  new wxBoxSizer(wxHORIZONTAL);
  SampleRateBox->Add(SampleRateText, BoxFlags);
  SampleRateBox->Add(RateChoice, BoxFlags);
  OutputBox->Add(SampleRateBox, BoxFlags);

  OutputBox->Add(Latency, BoxFlags);
  OutputBox->Add(LatencySlider, BoxFlags);

  AudioOutputPanel->SetSizer(OutputBox);
  OutputBox->SetSizeHints(this);
}

// Creates the panel for midi settings

void				SettingWindow::MidiPanelView()
{
  MidiPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

  // fill midi devices
  MidiInList = new wxCheckListBox(MidiPanel, Setting_MidiIn);
  PopulateMidiIn(MidiInList);

  // add panel sizer
  wxBoxSizer*	midiSizer = new wxBoxSizer(wxVERTICAL);
  midiSizer->Add(new wxStaticText(MidiPanel, -1, _("Select MIDI In devices to use:")),
		 BoxFlags);
  midiSizer->Add(MidiInList, BoxFlags);

  MidiPanel->SetSizer(midiSizer);
  midiSizer->SetSizeHints(this);
}


//Populates the Midi panel with the devices.
void SettingWindow::PopulateMidiIn(wxCheckListBox* list)
{
  MidiDeviceList::iterator	deviceIt;
  vector<wxString>::iterator	strIt;
  int				i;

  list->Clear();
  for (i = 0, deviceIt = MidiEngine->DeviceList.begin();
      deviceIt != MidiEngine->DeviceList.end(); deviceIt++, i++)
  {
    MidiInList->Append((*deviceIt)->Name);
    for (strIt = WiredSettings->MidiInStr.begin();
	strIt != WiredSettings->MidiInStr.end(); strIt++)
      if ((*strIt).Cmp((*deviceIt)->Name) == 0)
	MidiInList->Check(i);
  }

  std::cerr << "[Setting Window] Midi In list populated" << std::endl;

}


//
// Callback when user selects a settings' category
//

void			SettingWindow::OnSelPrefCategory(wxTreeEvent &e)
{
  wxTreeItemId		item = e.GetItem();
  wxPanel		*newPanel = (wxPanel *)SettingsTree->GetItemData(item);


  //Hide old panel
  if (newPanel)
    {
      CurrentPanel->Show(false);

      // resize dialog if necessary
      if ( GetSizer()->Replace(CurrentPanel, newPanel, true) == false )
	{
	  cout << "[SETTINGWINDOW] Spank developers: panel not found" << endl;
	}

      CurrentPanel = newPanel;
    }

  //Refresh panel content
  if (CurrentPanel == MidiPanel)
    PopulateMidiIn(MidiInList);

  //Show new panel
  CurrentPanel->Show(true);
  GetSizer()->Layout();
  GetSizer()->SetSizeHints(this);
}

//
// We click on midi checkbox
//

void SettingWindow::OnMidiInClick(wxCommandEvent &event)
{
  vector<int>::iterator	i;

  MidiLoaded = true;

  for (i = WiredSettings->MidiIn.begin(); i != WiredSettings->MidiIn.end(); i++)
    if (*i < MidiInList->GetCount())
      MidiInList->Check(*i);
}

void SettingWindow::OnOkClick(wxCommandEvent &event)
{
  EndModal(wxID_OK);
}

void SettingWindow::OnCancelClick(wxCommandEvent &event)
{
  AudioLoaded = false;
  MidiLoaded = false;
  EndModal(wxID_CANCEL);
}

void SettingWindow::OnApplyClick(wxCommandEvent &event)
{
    if (MainWin->InitAudio(true) < 0)
	wxGetApp().AlertDialog(_("audio engine"),
			       _("Could not open audio device : check that the device is not busy (used by another application) and that your audio settings are correct."));
  AudioLoaded = false;
  MidiLoaded = false;
}

void SettingWindow::OnInputSystemClick(wxCommandEvent &event)
{
  AudioLoaded = true;
  RefreshDevices(InputDeviceChoice, InputSystemChoice->GetSelection(), Audio->GetDefaultInputDevice());
  OnInputDevClick(event);
}

void SettingWindow::OnOutputSystemClick(wxCommandEvent &event)
{
  AudioLoaded = true;
  RefreshDevices(OutputDeviceChoice, OutputSystemChoice->GetSelection(), Audio->GetDefaultOutputDevice());
  LoadSampleFormat();
  LoadSampleRates();
  OnOutputDevClick(event);
}

void SettingWindow::OnInputDevClick(wxCommandEvent &event)
{
  AudioLoaded = true;
  RefreshChannels(InputChannelList, InputSystemChoice->GetSelection(),
		  InputDeviceChoice->GetSelection(), true);
}

void SettingWindow::OnOutputDevClick(wxCommandEvent &event)
{
  AudioLoaded = true;
  RefreshChannels(OutputChannelList, OutputSystemChoice->GetSelection(),
		  OutputDeviceChoice->GetSelection(), false);
  LoadSampleFormat();
  LoadSampleRates();
}

void SettingWindow::RefreshSystems(wxChoice* choice)
{
  vector<AudioSystem*>::iterator	sys;

  choice->Clear();
  choice->Append(_("None"));

  for (sys = Audio->SystemList.begin(); sys != Audio->SystemList.end(); sys++)
    choice->Append((*sys)->GetName());
  choice->SetSelection(Audio->GetDefaultAudioSystem());
}

void SettingWindow::RefreshDevices(wxChoice* choice, int system_selected, int selected)
{
  vector<Device*>::iterator	dev;

  choice->Clear();
  choice->Append(_("None"));
  choice->SetSelection(0);

  // system is "None"
  if (!system_selected)
    return;

  for (dev = Audio->DeviceList.begin(); dev != Audio->DeviceList.end(); dev++)
    {
      if ((*dev)->Host->GetId() == system_selected - 1)
	choice->Append((*dev)->Name);
    }
  choice->SetSelection(selected);
}

void SettingWindow::RefreshChannels(wxCheckListBox* list, int system_selected,
				    int device_selected, bool input)
{
  Device*	dev;
  wxString	str;
  int		i;

  list->Clear();

  dev = Audio->GetDevice(device_selected, system_selected);
  // device is "None"
  if (!dev)
    return;

  for (i = 1; (input) ? (i <= dev->MaxInputChannels) :
	 (i <= dev->MaxOutputChannels); i++)
    {
      str.Clear();
      if (input)
	str << _("Input ");
      else
	str << _("Output ");
      str << i;
      list->Append(str);
      if (i < 3)
	list->Check(i - 1);
    }

  // load only if it's Output
  if (!input)
    {
      LoadSampleFormat();
      LoadSampleRates();
    }
}

void SettingWindow::OnOutputChanClick(wxCommandEvent &event)
{
  int i, j;

  AudioLoaded = true;
  j = 0;
  for (i = 0; i < OutputChannelList->GetCount() - 1; i++)
    if (OutputChannelList->IsChecked(i))
      j++;
  // pas plus de 2 channel de sortie
  if (j > 2)
    {
      OutputChannelList->Check(event.GetInt(), false);
      wxMessageDialog msg(this, _("No more than 2 output channels can be selected"), wxT("Wired"),
			  wxOK | wxICON_EXCLAMATION);
      msg.ShowModal();
    }
}

void SettingWindow::Load()
{
  Device*	inCurrentDevice;
  Device*	outCurrentDevice;
  AudioSystem*	inCurrentSystem;
  AudioSystem*	outCurrentSystem;
  wxString	strMaxUndoRedoDepth;
  int		i;
  wxCommandEvent	fakeevent;

  // load misc settings
  QuickWaveBox->SetValue(WiredSettings->QuickWaveRender);
  dBWaveBox->SetValue(WiredSettings->dbWaveRender);
  strMaxUndoRedoDepth << WiredSettings->maxUndoRedoDepth;
  undoRedoMaxDepthTextCtrl->SetValue(strMaxUndoRedoDepth);

  // init vars from conf file
  if (WiredSettings->InputSystemStr.length())
    {
      // try to assign AudioSystem id from AudioSystem name
      inCurrentSystem = Audio->GetAudioSystemByName(WiredSettings->InputSystemStr);
      if (inCurrentSystem)
	{
	  WiredSettings->InputSystemId = inCurrentSystem->GetId() + 1;

	  // try to assign device id from device name
	  if (WiredSettings->InputDeviceStr.length())
	    {
	      inCurrentDevice = Audio->GetDeviceByName(WiredSettings->InputDeviceStr);
	      if (inCurrentDevice)
		WiredSettings->InputDeviceId = Audio->GetDeviceIdByTrueId(inCurrentDevice);
	      else
		WiredSettings->InputDeviceId = 0;
	    }
	}
      else
	{
	  // user has selected "none"
	  WiredSettings->InputSystemId = 0;
	  WiredSettings->InputDeviceId = 0;
	}
    }
  else
    {
      // If there are no value in conf file, we set default
      WiredSettings->InputSystemId = Audio->GetDefaultAudioSystem();
      WiredSettings->InputDeviceId = Audio->GetDefaultInputDevice();
    }


  // init vars from conf file
  if (WiredSettings->OutputSystemStr.length())
    {
      // try to assign AudioSystem id from AudioSystem name
      inCurrentSystem = Audio->GetAudioSystemByName(WiredSettings->OutputSystemStr);
      if (inCurrentSystem)
	{
	  WiredSettings->OutputSystemId = inCurrentSystem->GetId() + 1;

	  // try to assign device id from device name
	  if (WiredSettings->OutputDeviceStr.length())
	    {
	      inCurrentDevice = Audio->GetDeviceByName(WiredSettings->OutputDeviceStr);
	      if (inCurrentDevice)
		WiredSettings->OutputDeviceId = Audio->GetDeviceIdByTrueId(inCurrentDevice);
	      else
		WiredSettings->OutputDeviceId = 0;
	    }
	}
      else
	{
	  // user has selected "none"
	  WiredSettings->OutputSystemId = 0;
	  WiredSettings->OutputDeviceId = 0;
	}
    }
  else
    {
      // If there are no value in conf file, we set default
      WiredSettings->OutputSystemId = Audio->GetDefaultAudioSystem();
      WiredSettings->OutputDeviceId = Audio->GetDefaultOutputDevice();
    }


  // apply changes in controls for INPUT
  InputSystemChoice->SetSelection(WiredSettings->InputSystemId);
  OnInputSystemClick(fakeevent);
  InputDeviceChoice->SetSelection(WiredSettings->InputDeviceId);
  OnInputDevClick(fakeevent);
  LoadChannels(InputChannelList, WiredSettings->InputChannels);

  // apply changes in controls for OUTPUT
  OutputSystemChoice->SetSelection(WiredSettings->OutputSystemId);
  OnOutputSystemClick(fakeevent);
  OutputDeviceChoice->SetSelection(WiredSettings->OutputDeviceId);
  OnOutputDevClick(fakeevent);
  LoadChannels(OutputChannelList, WiredSettings->OutputChannels);

  // changes audio settings related to output device
  if (WiredSettings->SampleRate >= 0 &&
      WiredSettings->SampleRate < RateChoice->GetCount())
    RateChoice->SetSelection(WiredSettings->SampleRate);

  if (WiredSettings->SamplesPerBuffer > 0)
    {
      for (i = 0; i < 9; i++)
	if (Latencies[i] == WiredSettings->SamplesPerBuffer)
	  LatencySlider->SetValue(i);
      UpdateLatency();
    }

  
  LoadMidiIn(MidiInList, WiredSettings->MidiIn);
}

void	SettingWindow::LoadMidiIn(wxCheckListBox *to, vector<int>&from)
{
  int	i, j;
  MidiDeviceList::iterator	deviceIt;

  for (i = 0, deviceIt = MidiEngine->DeviceList.begin();
      deviceIt != MidiEngine->DeviceList.end(); deviceIt++, i++)
    for (j = 0 ; j < from.size() ; j++)
      if (i == j)
      {
	to->Check(i);
	WiredSettings->MidiInStr.push_back((*deviceIt)->Name);
      }
}

void SettingWindow::LoadChannels(wxCheckListBox* to, vector<int>& from)
{
  int	max;
  int	i;

  max = to->GetCount();
  for (i = 0; i < max; i++)
    to->Check(i, false);
  for (i = 0; i < from.size(); i++)
    if (from[i] >= 0 && from[i] < max)
      to->Check(from[i]);
}

void SettingWindow::SaveChannels(wxCheckListBox* from, vector<int>& to)
{
  int	i;

  to.clear();
  for (i = 0; i < from->GetCount(); i++)
    if (from->IsChecked(i))
      to.push_back(i);
}

void SettingWindow::Save()
{
  WiredSettings->QuickWaveRender = QuickWaveBox->IsChecked();
  WiredSettings->dbWaveRender = dBWaveBox->IsChecked();
  undoRedoMaxDepthTextCtrl->GetValue().ToLong(&WiredSettings->maxUndoRedoDepth);

  // if we changed audio settings
  if (AudioLoaded)
    {
      cout << "[SETTINGS] Save Audio settings" << endl;

      // we need to save system and device names instead their id
      Device*	output;
      Device*	input;

      WiredSettings->OutputSystemId = OutputSystemChoice->GetSelection();
      WiredSettings->InputSystemId = InputSystemChoice->GetSelection();
      WiredSettings->OutputDeviceId = OutputDeviceChoice->GetSelection();
      WiredSettings->InputDeviceId = InputDeviceChoice->GetSelection();

      output = Audio->GetDevice(WiredSettings->OutputDeviceId, WiredSettings->OutputSystemId);
      input = Audio->GetDevice(WiredSettings->InputDeviceId, WiredSettings->InputSystemId);
      if (output)
	{
	  WiredSettings->OutputSystemStr = output->Host->GetName();
	  WiredSettings->OutputDeviceStr = output->Name;
	}
      else
	{
	  WiredSettings->OutputSystemStr = wxT("None");
	  WiredSettings->OutputDeviceStr = wxT("None");
	}
      if (input)
	{
	  WiredSettings->InputSystemStr = input->Host->GetName();
	  WiredSettings->InputDeviceStr = input->Name;
	}
      else
	{
	  WiredSettings->InputSystemStr = wxT("None");
	  WiredSettings->InputDeviceStr = wxT("None");
	}

      // save output channels
      SaveChannels(OutputChannelList, WiredSettings->OutputChannels);

      // save input channels
      SaveChannels(InputChannelList, WiredSettings->InputChannels);

      WiredSettings->SampleRate = RateChoice->GetSelection();
      WiredSettings->SampleFormat = BitsChoice->GetSelection();
      WiredSettings->SamplesPerBuffer = Latencies[LatencySlider->GetValue()];
    }

  // if we changed midi settings
  if (MidiLoaded)
    {
      cout << "[SETTINGS] Save Midi settings" << endl;
      SaveChannels(MidiInList, WiredSettings->MidiIn);

      // MidiInStr things
      int	i;

      WiredSettings->MidiInStr.clear();
      for (i = 0; i < MidiInList->GetCount(); i++)
	if (MidiInList->IsChecked(i))
	  WiredSettings->MidiInStr.push_back(MidiInList->GetString(i));

    }
  if (AudioLoaded || MidiLoaded)
    WiredSettings->Save();
}

void SettingWindow::LoadSampleFormat()
{
  int					n;
  vector<DeviceFormat *>::iterator	i;
  Device *dev = Audio->GetDevice(OutputDeviceChoice->GetSelection(),
				 OutputSystemChoice->GetSelection());

  BitsChoice->Clear();

  if (dev)
    for (i = dev->SupportedFormats.begin(); i != dev->SupportedFormats.end(); i++)
      if ((*i)->SampleRates.size() > 0)
	// we check all known format
	for (n = 0; _FormatTypes[n].PaFormat; n++)
	  if ((*i)->SampleFormat == _FormatTypes[n].PaFormat)
	    // we support only our default
	    if ((*i)->SampleFormat == DEFAULT_SAMPLE_FORMAT)
	      BitsChoice->SetSelection(BitsChoice->Append(_FormatTypes[n].FormatName));
	    else
	      BitsChoice->Append(wxString(_FormatTypes[n].FormatName) +
				 _("[currently unsupported]"));

 if (BitsChoice->GetCount() == 0)
    {
      BitRateText->Hide();
      BitsChoice->Hide();
    }
  else
    {
      BitRateText->Show();
      BitsChoice->Show();
    }
}

void SettingWindow::LoadSampleRates()
{
  vector<double>::iterator	i;
  wxString			s;
  wxString			defaultSampleRate;
  unsigned int			k = BitsChoice->GetSelection();
  Device *dev = Audio->GetDevice(OutputDeviceChoice->GetSelection(),
				 OutputSystemChoice->GetSelection());

  // put default sample rate into a string (same format like below)
  s.Printf(wxT("%d Hz"), DEFAULT_SAMPLE_RATE_INT);
  defaultSampleRate = s;

  RateChoice->Clear();
  RateChoice->SetSelection(0);
  if (dev && k < dev->SupportedFormats.size())
    {
      DeviceFormat *f = dev->SupportedFormats.at(k);
      for (i = f->SampleRates.begin(); i != f->SampleRates.end(); i++)
	{
	  s.Printf(wxT("%.0f Hz"), *i);
	  k = RateChoice->Append(s);

	  // check the default value
	  if (s == defaultSampleRate || i == f->SampleRates.begin())
	    RateChoice->SetSelection(k);
	}
    }

  // hide or show controls
  if (RateChoice->GetCount() == 0)
    {
      SampleRateText->Hide();
      Latency->Hide();
      RateChoice->Hide();
      LatencySlider->Hide();
    }
  else
    {
      UpdateLatency();
      SampleRateText->Show();
      Latency->Show();
      RateChoice->Show();
      LatencySlider->Show();
    }
}

void SettingWindow::SetDefaultSampleFormat()
{
  Device *dev = Audio->GetDevice(OutputDeviceChoice->GetSelection(),
				 OutputSystemChoice->GetSelection());

  if (dev)
    for (long k = 0; k < dev->SupportedFormats.size(); k++)
      if (dev->SupportedFormats[k]->SampleFormat == DEFAULT_SAMPLE_FORMAT)
	BitsChoice->SetSelection(k);
}

void SettingWindow::OnSampleFormatClick(wxCommandEvent &event)
{
  AudioLoaded = true;
  SetDefaultSampleFormat();	//forcing 32bits floats
  LoadSampleFormat();
  LoadSampleRates();
}

void SettingWindow::OnSampleRateClick(wxCommandEvent &event)
{
  AudioLoaded = true;
  UpdateLatency();
}

void SettingWindow::OnLatencyChange(wxCommandEvent &event)
{
  AudioLoaded = true;
  UpdateLatency();
}

void SettingWindow::UpdateLatency()
{
  wxString	s;
  unsigned int	k = BitsChoice->GetSelection();
  unsigned int	l = RateChoice->GetSelection();
  double	res = 0;
  Device	*dev = Audio->GetDevice(OutputDeviceChoice->GetSelection(),
					OutputSystemChoice->GetSelection());

  if (dev && k < dev->SupportedFormats.size())
    {
      DeviceFormat *f = dev->SupportedFormats[k];
      if (l < f->SampleRates.size())
	res = f->SampleRates[l];
    }

  // FIXME : sth really weird delete all things after %.2f
  s.Printf(_("Latency: %d samples per buffer, %.2f msec"),
	   Latencies[LatencySlider->GetValue()],
	   static_cast<float>( static_cast<double>(
	   (Latencies[LatencySlider->GetValue()]/res)*1000.0) ) );
  Latency->SetLabel(s);
}

BEGIN_EVENT_TABLE(SettingWindow, wxDialog)
  // EVT_TOGGLEBUTTON(Setting_General, SettingWindow::OnGeneralClick)
  // EVT_TOGGLEBUTTON(Setting_Audio, SettingWindow::OnAudioClick)
//   EVT_TOGGLEBUTTON(Setting_Midi, SettingWindow::OnMidiClick)
  EVT_BUTTON(wxID_OK, SettingWindow::OnOkClick)
  EVT_BUTTON(wxID_CANCEL,SettingWindow:: OnCancelClick)
  EVT_BUTTON(wxID_APPLY, SettingWindow::OnApplyClick)
  EVT_CHOICE(Setting_OutputSystem, SettingWindow::OnOutputSystemClick)
  EVT_CHOICE(Setting_InputSystem, SettingWindow::OnInputSystemClick)
  EVT_CHOICE(Setting_OutputDev, SettingWindow::OnOutputDevClick)
  EVT_CHOICE(Setting_InputDev, SettingWindow::OnInputDevClick)
  EVT_CHOICE(Setting_Bits, SettingWindow::OnSampleFormatClick)
  EVT_CHOICE(Setting_Rate, SettingWindow::OnSampleRateClick)
  EVT_CHECKLISTBOX(Setting_OutputChan, SettingWindow::OnOutputChanClick)
  EVT_CHECKLISTBOX(Setting_MidiIn, SettingWindow::OnMidiInClick)
  EVT_SLIDER(Setting_Latency, SettingWindow::OnLatencyChange)
  EVT_TREE_SEL_CHANGED(CATEGORY_ID, SettingWindow::OnSelPrefCategory)
END_EVENT_TABLE()
