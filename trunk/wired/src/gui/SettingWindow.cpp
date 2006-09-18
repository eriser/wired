// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MainWindow.h"
#include "SettingWindow.h"
#include "AudioEngine.h"
#include "Settings.h"
#include "Colour.h"
#include "../midi/MidiThread.h"
#include "../midi/MidiDevice.h"

#include "../samplerate/WiredSampleRate.h"

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
  : wxDialog(0x0, -1, _("Wired Settings"), wxDefaultPosition, WIN_SIZE)
{
  wxTreeItemId	root;
  wxTreeItemId	id;

  MidiLoaded = false;
  AudioLoaded = false;
  Center();

  SettingsTree = new wxTreeCtrl(this, CATEGORY_ID, wxPoint(WIN_MARGIN, WIN_MARGIN), 
				wxSize(206, 400), wxSUNKEN_BORDER | wxTR_NO_LINES |
				wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_HIDE_ROOT |
				wxTR_FULL_ROW_HIGHLIGHT);

  // flags assigned to all sizer in all right panel
  BoxFlags.Left();
  BoxFlags.Expand();
  BoxFlags.Border(wxALL, 8);

  //SettingsTree->SetIndent(20);
  GeneralPanelView();
  AudioPanelView();
  AudioInputPanelView();
  AudioOutputPanelView();
  MidiPanelView();
  
  SettingsTree->AddRoot(wxT(""), -1, -1);
  root = SettingsTree->GetRootItem();
  SettingsTree->AppendItem(root, _("General"), -1, -1, (wxTreeItemData*)GeneralPanel);
  id = SettingsTree->AppendItem(root, _("Audio"), -1, -1, (wxTreeItemData*)AudioPanel);
  SettingsTree->AppendItem(id, _("Output"), -1, -1, (wxTreeItemData*)AudioOutputPanel);
  SettingsTree->AppendItem(id, _("Input"), -1, -1, (wxTreeItemData*)AudioInputPanel);
  SettingsTree->AppendItem(root, wxString(_("Midi"), *wxConvCurrent), -1, -1, (wxTreeItemData*)MidiPanel);

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
   GeneralPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);
   QuickWaveBox = new wxCheckBox(GeneralPanel, -1, _("Quickly render waveforms"), wxPoint(8, 8));
   dBWaveBox = new wxCheckBox(GeneralPanel, -1, _("Render waveforms in dB mode"), wxPoint(8, 28));
   undoRedoMaxDepthTextCtrl = new wxTextCtrl(GeneralPanel, -1, wxT(""), wxPoint(218, 50), wxSize(45, 25));
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

  // right panel
  AudioInputPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);

  // list of audio system
  InputSystemChoice = new wxChoice(AudioInputPanel, Setting_InputSystem);
  RefreshSystems(InputSystemChoice);

  // list of devices
  InputDeviceChoice = new wxChoice(AudioInputPanel, Setting_InputDev);
  RefreshDevices(InputDeviceChoice, 0, Audio->GetDefaultInputDevice()); // based on system selection

  // list of channels
  InputChannelList = new wxCheckListBox(AudioInputPanel, Setting_InputChan);
  RefreshChannels(InputChannelList, 0, 0, true);

  // sizer
  InputBox = new wxBoxSizer(wxVERTICAL);
  InputBox->Add(new wxStaticText(AudioInputPanel, -1, _("Avalaible sound system:")), BoxFlags);
  InputBox->Add(InputSystemChoice, BoxFlags);
  InputBox->Add(new wxStaticText(AudioInputPanel, -1, _("Select Input sound card:")), BoxFlags);
  InputBox->Add(InputDeviceChoice, BoxFlags);
  InputBox->Add(new wxStaticText(AudioInputPanel, -1, _("Select Input channels to use with this sound card:")),
		BoxFlags);
  InputBox->Add(InputChannelList, BoxFlags);

  AudioInputPanel->SetSizer(InputBox);
  InputBox->SetSizeHints(AudioInputPanel);
}

//
// Creates the panel for audio output settings
//

void				SettingWindow::AudioOutputPanelView()
{
  vector<Device*>::iterator		i;
  vector<AudioSystem*>::iterator	n;

  // right panel
  AudioOutputPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);

  // list of audio systems
  OutputSystemChoice = new wxChoice(AudioOutputPanel, Setting_OutputSystem);
  RefreshSystems(OutputSystemChoice);

  // list of devices
  OutputDeviceChoice = new wxChoice(AudioOutputPanel, Setting_OutputDev);
  RefreshDevices(OutputDeviceChoice, 0, Audio->GetDefaultOutputDevice());

  // list of channels
  OutputChannelList = new wxCheckListBox(AudioOutputPanel, Setting_OutputChan);
  RefreshChannels(OutputChannelList, 0, 0, false);

  // sizer
  OutputBox = new wxBoxSizer(wxVERTICAL);
  OutputBox->Add(new wxStaticText(AudioOutputPanel, -1, _("Avalaible sound system:")), BoxFlags);
  OutputBox->Add(OutputSystemChoice, BoxFlags);
  OutputBox->Add(new wxStaticText(AudioOutputPanel, -1, _("Select Output sound card:")), BoxFlags);
  OutputBox->Add(OutputDeviceChoice, BoxFlags);
  OutputBox->Add(new wxStaticText(AudioOutputPanel, -1, _("Select left and right Output channels for this sound card:")),
		 BoxFlags);
  OutputBox->Add(OutputChannelList, BoxFlags);

  AudioOutputPanel->SetSizer(OutputBox);
  OutputBox->SetSizeHints(AudioOutputPanel);
}

// Creates the panel for midi settings

void				SettingWindow::MidiPanelView()
{
  MidiPanel = new wxPanel(this, -1, PAN_POS, PAN_SIZE, wxSUNKEN_BORDER);
  new wxStaticText(MidiPanel, -1, _("Select MIDI In devices to use:"), 
		   wxPoint(8, 8));
  MidiInList = new wxCheckListBox(MidiPanel, Setting_MidiIn, wxPoint(8, 30), wxSize(368, 200), 0);
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

//
// We click on midi checkbox
//

void SettingWindow::OnMidiInClick(wxCommandEvent &event)
{
  vector<long>::iterator	i;
  MidiDeviceList::iterator	j;
  int				k;
  wxString			s;

  MidiLoaded = true;
  MidiInList->Clear();
  for (j = MidiEngine->DeviceList.begin(), k = 1; j != MidiEngine->DeviceList.end(); j++, k++)
    {      
      s.Printf(_("In %d: %s"), k, (const char *)(*j)->Name.mb_str(*wxConvCurrent));
      MidiInList->Append(s);
    }
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
    MainWin->AlertDialog(_("audio engine"), 
			 _("You may check for your audio settings if you want to use Wired.."));
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
      if ((*dev)->AudioSystem == system_selected - 1)
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

  LoadSampleFormat();
  LoadSampleRates();

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
  wxString strMaxUndoRedoDepth;
  int i;

  QuickWaveBox->SetValue(WiredSettings->QuickWaveRender);
  dBWaveBox->SetValue(WiredSettings->dbWaveRender);
  strMaxUndoRedoDepth << WiredSettings->maxUndoRedoDepth;
  undoRedoMaxDepthTextCtrl->SetValue(strMaxUndoRedoDepth);
  
  if (WiredSettings->OutputSystem > -1)
    {
      OutputSystemChoice->SetSelection(WiredSettings->OutputSystem);
      RefreshDevices(OutputDeviceChoice, OutputSystemChoice->GetSelection(),
		     Audio->GetDefaultOutputDevice());
    }
  if (WiredSettings->OutputDev > -1)
    {
      OutputDeviceChoice->SetSelection(WiredSettings->OutputDev);
      RefreshChannels(OutputChannelList, OutputSystemChoice->GetSelection(),
		      OutputDeviceChoice->GetSelection(), false);
      LoadChannels(OutputChannelList, WiredSettings->OutputChannels);
    }
  if (WiredSettings->InputSystem > -1)
    {
      InputSystemChoice->SetSelection(WiredSettings->InputSystem);
      RefreshDevices(InputDeviceChoice, InputSystemChoice->GetSelection(),
		     Audio->GetDefaultInputDevice());
    }
  if (WiredSettings->InputDev > -1)
    {
      InputDeviceChoice->SetSelection(WiredSettings->InputDev);
      RefreshChannels(InputChannelList, InputSystemChoice->GetSelection(),
		      InputDeviceChoice->GetSelection(), true);
      LoadChannels(InputChannelList, WiredSettings->InputChannels);
    }

  if (WiredSettings->SamplesPerBuffer > 0)
    {
      for (int i = 0; i < 9; i++)
	if (Latencies[i] == WiredSettings->SamplesPerBuffer)
	  LatencySlider->SetValue(i);
      UpdateLatency();
    }    
}

void SettingWindow::LoadChannels(wxCheckListBox* to, vector<long>& from)
{
  int	i;

  for (i = 0; i < to->GetCount(); i++)
    to->Check(i, false);
  for (i = 0; i < from.size(); i++)
    to->Check(from[i]);
}

void SettingWindow::SaveChannels(wxCheckListBox* from, vector<long>& to)
{
  int	i;

  to.clear();
  for (i = 0; i < from->GetCount(); i++)
    if (from->IsChecked(i))
      to.push_back(i);
}

void SettingWindow::Save()
{
  long i;

  WiredSettings->QuickWaveRender = QuickWaveBox->IsChecked();
  WiredSettings->dbWaveRender = dBWaveBox->IsChecked();

  WiredSettings->OutputSystem = OutputSystemChoice->GetSelection();
  WiredSettings->InputSystem = InputSystemChoice->GetSelection();
  WiredSettings->OutputDev = OutputDeviceChoice->GetSelection();
  WiredSettings->InputDev = InputDeviceChoice->GetSelection();

  undoRedoMaxDepthTextCtrl->GetValue().ToULong(&WiredSettings->maxUndoRedoDepth);

  // if we changed audio settings
  if (AudioLoaded)
    {
      cout << "[SETTINGS] Save Audio settings" << endl;

      // save output channels
      SaveChannels(OutputChannelList, WiredSettings->OutputChannels);
      
      // save input channels
      SaveChannels(InputChannelList, WiredSettings->InputChannels);

      WiredSettings->SampleRate = RateChoice->GetSelection();
      SetDefaultSampleFormat();		//forcing 32 bit floats
      WiredSettings->SampleFormat = BitsChoice->GetSelection();
      WiredSettings->SamplesPerBuffer = Latencies[LatencySlider->GetValue()];
    }

  // if we changed midi settings
  if (MidiLoaded)
    {
      cout << "[SETTINGS] Save Midi settings" << endl;
      SaveChannels(MidiInList, WiredSettings->MidiIn);
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
  
  if (WiredSettings->SampleFormat < BitsChoice->GetCount())
    BitsChoice->SetSelection(WiredSettings->SampleFormat);
}

void SettingWindow::LoadSampleRates()
{
  vector<double>::iterator	i;
  wxString			s;
  unsigned int			k = BitsChoice->GetSelection();
  Device *dev = Audio->GetDevice(OutputDeviceChoice->GetSelection(),
				 OutputSystemChoice->GetSelection());

  RateChoice->Clear();
  if (dev && k < dev->SupportedFormats.size())
    {
      DeviceFormat *f = dev->SupportedFormats.at(k);
      for (i = f->SampleRates.begin(); i != f->SampleRates.end(); i++)
	{
	  s.Printf(wxT("%.0f Hz"), *i);
	  k = RateChoice->Append(s);
	  if (*i == DEFAULT_SAMPLE_RATE)
	    RateChoice->SetSelection(k);
	}
    }
  if (WiredSettings->SampleRate < RateChoice->GetCount())
    RateChoice->SetSelection(WiredSettings->SampleRate);
}

void SettingWindow::SetDefaultSampleFormat(void)
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
