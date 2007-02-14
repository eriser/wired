// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __SETTINGWINDOW_H__
#define __SETTINGWINDOW_H__

#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/treectrl.h>

#include <vector>

/** This class handles the Settings Window.
 * It is instanciated once at startup, and then shown or hidden.
 */
class					SettingWindow : public wxDialog
{
 public:
  /** Default constructor.
   * Instanciate every element of the window and calls the Load() method.
   * \see Load()
   */
  SettingWindow();
  
  /** Default destructor. */
  ~SettingWindow();

  /** Instanciates the general panel. */
  void					GeneralPanelView();

  /** Instanciates the audio panel. */
  void					AudioPanelView();

  /** Instanciates the audio input panel. */
  void					AudioInputPanelView();

  /** Instanciates the audio output panel. */
  void					AudioOutputPanelView();

  /** Instanciates the midi input panel. */
  void					MidiPanelView();

  /** Loads the settings.
   * It takes infos from the WiredSetting object. 
   */
  void					Load();

  /** Saves the settings
   * It fills a WiredSetting object and then calls its Save method. 
   */
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

  /** Flag that indicates if the MIDI settings have changed. */
  bool					MidiLoaded;

  /** Flag that indicates if the Audio settings have changed. */
  bool					AudioLoaded;

 protected:
  /** Fills the sample format list. */
  void					LoadSampleFormat();

  /** Fills the samplerate list */
  void					LoadSampleRates();

  /** Updates the latency infos, based on the slider's value */
  void					UpdateLatency();

  /** Sets the default sample format. */
  void					SetDefaultSampleFormat();

  /** Refreshes the list of output devices. */
  void                                  RefreshOutputDev();

  /** Refreshes the list of input devices. */
  void                                  RefreshInputDev();

  /** Refreshes the list of audio systems available.
   * \param choice The wxChoice object containing the different systems.
   */
  void					RefreshSystems(wxChoice* choice);

  /** Refreshes the list of devices available for the selected audio system. 
   * \param choice The wxChoice object containing the different devices.
   * \param system_selected The index of the selected system in the wxChoice of the systems.
   * \param selected The index of the selected device.
   */
  void					RefreshDevices(wxChoice* choice, 
						       int system_selected, 
						       int selected);

  /** Refreshes the list of channels available for the selected device.
   * \param list the wxCheckListBox containing the different channels available.
   * \param system_selected the index of the selected system in its wxChoice object.
   * \param device_selected the index of the selected device in its wxChoice object.
   * \param input true if we are talking about input channels.
   */ 
  void					RefreshChannels(wxCheckListBox* list, 
							int system_selected,
							int device_selected, 
							bool input);


  /** Populates the wxCheckListBox containing the MIDI device.
   * \param list the wxCheckListBox containing the MIDI device.
   */
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

  /** A pointer to the current panel.
   * It does not correspond to a specific panel, and should be considered as a link
   * to the current panel.
   */
  wxPanel				*CurrentPanel;

  // The different panels.
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
  /** Saves the selected index of a wxCheckListBox into a vector<int>.
   * \param from the wxCheckListBox where item are selected.
   * \param to the vector<int> where indexes are saved.
   */
  void					SaveChannels(wxCheckListBox* from, 
						     std::vector<int>& to);
  /** Selects into a wxCheckListBox indexes stored into a vector<int>.
   * \param to the wxCheckListBox to fill.
   * \param from the vector<int> containing the indexes.
   */ 
  void					LoadChannels(wxCheckListBox* to, 
						     std::vector<int>& from);


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


