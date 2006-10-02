// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __SETTINGWINDOW_H__
#define __SETTINGWINDOW_H__

#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/treectrl.h>

class					SettingWindow : public wxDialog
{
 public:
  SettingWindow();
  ~SettingWindow();

  void					GeneralPanelView();
  void					AudioPanelView();
  void					AudioInputPanelView();
  void					AudioOutputPanelView();
  void					MidiPanelView();

  void					Load();
  void					Save();
  void					OnOkClick(wxCommandEvent &event);
  void					OnCancelClick(wxCommandEvent &event);
  void					OnApplyClick(wxCommandEvent &event);
  void					OnInputDevClick(wxCommandEvent &event);
  void					OnOutputDevClick(wxCommandEvent &event);
  void					OnInputSystemClick(wxCommandEvent &event);
  void					OnOutputSystemClick(wxCommandEvent &event);
  void					OnOutputChanClick(wxCommandEvent &event);
  void					OnSampleFormatClick(wxCommandEvent &event);
  void					OnSampleRateClick(wxCommandEvent &event);
  void					OnLatencyChange(wxCommandEvent &event);
  void					OnSelPrefCategory(wxTreeEvent &event);
  void					OnMidiInClick(wxCommandEvent &event);

  bool					MidiLoaded; // true if midi settings has changed
  bool					AudioLoaded; // true if audio settings has changed

 protected:
  void					LoadSampleFormat();
  void					LoadSampleRates();
  void					UpdateLatency();
  void					SetDefaultSampleFormat(void);
  void                                  RefreshOutputDev();
  void                                  RefreshInputDev();

  // refresh list of audio systems available
  void					RefreshSystems(wxChoice* choice);

  // refresh list of devices available for selected audio system
  void					RefreshDevices(wxChoice* choice, int system_selected, int select);

  // refresh list of channels available for selected device 
  void					RefreshChannels(wxCheckListBox* list, int system_selected,
							int device_selected, bool input);

  // Populates the Midi Panel
  void					PopulateMidiIn(wxCheckListBox* list);

  wxButton				*OkBtn;
  wxButton				*ApplyBtn;
  wxButton				*CancelBtn;
  wxCheckBox				*QuickWaveBox;
  wxCheckBox				*dBWaveBox;
  wxChoice				*OutputDeviceChoice;
  wxChoice				*OutputSystemChoice;
  wxChoice				*InputDeviceChoice;
  wxChoice				*InputSystemChoice;
  wxCheckListBox			*OutputChannelList;
  wxCheckListBox			*InputChannelList;
  wxChoice				*RateChoice;
  wxChoice				*BitsChoice;
  wxStaticText				*Latency;
  wxSlider				*LatencySlider;
  wxTextCtrl				*undoRedoMaxDepthTextCtrl;
  wxStaticText				*undoRedoMaxDepthStaticText;
  wxStaticText				*SampleRateText;
  wxStaticText				*BitRateText;
  wxStaticText				*LatencyText;
  int					*Latencies;
  wxTreeCtrl				*SettingsTree;

  //link to the current panel
  wxPanel				*CurrentPanel;

  //different panels
  wxPanel				*AudioInputPanel;
  wxPanel				*AudioOutputPanel;
  wxPanel				*MidiPanel;
  wxPanel				*AudioPanel;
  wxPanel				*GeneralPanel;
  wxCheckListBox			*MidiInList;

  wxBoxSizer				*OutputBox;
  wxBoxSizer				*InputBox;
  wxSizerFlags				BoxFlags;

 private:
  void					SaveChannels(wxCheckListBox* from, vector<long>& to);
  void					LoadChannels(wxCheckListBox* to, vector<long>& from);


  DECLARE_EVENT_TABLE()
};

enum
{
  Setting_Ok = 1,
  Setting_Cancel,
  Setting_Apply,
  Setting_General,
  Setting_Audio,
  Setting_Midi,
  Setting_OutputDev,
  Setting_OutputChan,
  Setting_InputDev,
  Setting_InputChan,
  Setting_MidiIn,
  Setting_Bits,
  Setting_Rate,
  Setting_Latency,
  Setting_OutputSystem,
  Setting_InputSystem
};

#endif/*__SETTINGWINDOW_H__*/


