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
  void					OnAudioClick(wxCommandEvent &event);
  void					OnMidiClick(wxCommandEvent &event);
  void					OnOkClick(wxCommandEvent &event);
  void					OnCancelClick(wxCommandEvent &event);
  void					OnApplyClick(wxCommandEvent &event);
  void					OnInputDevClick(wxCommandEvent &event);
  void					OnOutputDevClick(wxCommandEvent &event);
  void					OnOutputChanClick(wxCommandEvent &event);
  void					OnSampleFormatClick(wxCommandEvent &event);
  void					OnSampleRateClick(wxCommandEvent &event);
  void					OnLatencyChange(wxCommandEvent &event);
  void					OnSelPrefCategory(wxTreeEvent &event);

  bool					MidiLoaded; // a cause d'un bug wx...
  bool					AudioLoaded; // la meme mais pour l'audio...

 protected:
  void					LoadSampleFormat();
  void					LoadSampleRates();
  void					UpdateLatency();
  void					SetDefaultSampleFormat(void);
  void                                  RefreshOutputDev();
  void                                  RefreshInputDev();

  wxButton				*OkBtn;
  wxButton				*ApplyBtn;
  wxButton				*CancelBtn;
  wxCheckBox				*QuickWaveBox;
  wxCheckBox				*dBWaveBox;
  wxPanel				*AudioInputPanel;
  wxPanel				*AudioOutputPanel;
  wxChoice				*OutputChoice;
  wxChoice				*InputChoice;
  wxCheckListBox			*OutputList;
  wxCheckListBox			*InputList;
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
  Setting_Latency
};

#endif/*__SETTINGWINDOW_H__*/


