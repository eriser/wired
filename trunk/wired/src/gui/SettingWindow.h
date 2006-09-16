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
  //void					OnGeneralClick(wxCommandEvent &event);
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

  // refresh list of audio systems avalaible
  void					RefreshSystems(wxChoice* choice);

  // refresh list of devices avalaible for selected audio system
  void					RefreshDevices(wxChoice* choice, int system_selected, int select);

  // refresh list of channels avalaible for selected device 
  void					RefreshChannels(wxCheckListBox* list, int system_selected,
							int device_selected, bool input);

  wxButton				*OkBtn;
  wxButton				*ApplyBtn;
  wxButton				*CancelBtn;
  wxCheckBox				*QuickWaveBox;
  wxCheckBox				*dBWaveBox;
  wxPanel				*AudioInputPanel;
  wxPanel				*AudioOutputPanel;
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
  int					*Latencies;
  wxTreeCtrl				*SettingsTree;

  wxPanel				*CurrentPanel;
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


