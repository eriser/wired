// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <vector>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/textfile.h>
#include <wx/splitter.h>
#include <wx/cmdproc.h>

#define PLUG_MENU_INDEX_START		(50000)
#define INDEX_MENUITEM_UNDO	        2
#define INDEX_MENUITEM_REDO		3
// TODO : this should be created regarding to $PREFIX/etc/wired/wired_exts.conf
#define WIRED_SUPPORTED_SNDFILES	wxT("Microsoft WAV format (*.wav)|*.wav|Apple/SGI AIFF format (*.aif)|*.aif|Sun/NeXT AU format (*.au; *.snd)|*.au;*.snd|Amiga IFF / SVX8 / SV16 format (*.svx)|*.svx|Ensoniq PARIS format (*.paf; *.fap)|*.paf;*.fap|Sphere NIST format (*.nist)|*.nist|Berkeley/IRCAM/CARL format (*.ircam; *.sf)|*.ircam;*.sf|Creative Labs VOC format (*.voc)|*.voc|Sonic Foundry's 64 bit RIFF/WAV format (*.w64)|*.w64|RAW PCM data format (*.raw)|*.raw|Matlab (tm) V4.2 / GNU Octave 2.0 format (*.mat4; *.mat)|*.mat4;*.mat|Matlab (tm) V5 / GNU Octave 2.1 format (*.mat5)|*.mat5|Portable Voice Format (*.pvf)|*.pvf|Fastracker 2 format (*.xi)|*.xi|All supported files|*.wav;*.aif;*.au;*.snd;*.svx;*.paf;*.fap;*.nist;*.ircam;*.sf;*.voc;*.w64;*.raw;*.mat;*.mat4;*.mat5;*.pvf;*.xi;*.*")

#define WIRED_SUPPORTED_CONFFILE	wxT("XML Patch (*.xml)|*.xml")

#include "Plugin.h"
#include "WiredVideo.h"
#include "FloatingFrame.h"
#include "WiredDocument.h"

typedef	struct s_PlugStartInfo		PlugStartInfo;
class					PluginLoader;
class					MainWindow;

extern MainWindow		*MainWin;
extern std::vector<PluginLoader *>	LoadedPluginsList;
extern PlugStartInfo		StartInfo;
extern wxMutex		        AudioMutex;
extern wxCondition*	        SeqStopped;
extern wxCommandProcessor	*UndoRedo;

class					MainWindow: public wxFrame, public WiredDocument
{
 public:
  MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size, WiredDocument *parent);
  void					OnClose(wxCloseEvent &event);
  void					OnQuit(wxCommandEvent &event);
  void					OnOpen(wxCommandEvent &event);
  void					OnNew(wxCommandEvent &event);
  void					OnSave(wxCommandEvent &event);
  void					OnSaveAs(wxCommandEvent &event);
  void					OnImportDir(wxCommandEvent &event);
  void					OnImportWave(wxCommandEvent &event);
  void					OnImportMIDI(wxCommandEvent &event);
  void					OnImportAKAI(wxCommandEvent &event);
  void					OnExportWave(wxCommandEvent &event);
  void					OnExportMIDI(wxCommandEvent &event);
  void					OnSettings(wxCommandEvent &event);
  void					OnDeleteRack(wxCommandEvent &event);
  void					OnAddTrackAudio(wxCommandEvent &event);
  void					OnAddTrackMidi(wxCommandEvent &event);
  void					OnDeleteTrack(wxCommandEvent &event);
  void					SetSelectedSolo(wxCommandEvent &event);
  void					OnCreateRackClick(wxCommandEvent &event);
  void					OnCreateEffectClick(wxCommandEvent &event);
  void					OnFloatTransport(wxCommandEvent &event);
  wxFrame				*FloatTransport();
  void					OnFloatSequencer(wxCommandEvent &event);
  wxFrame				*FloatSequencer();
  void					OnFloatRack(wxCommandEvent &event);
  wxFrame				*FloatRack();
  void					OnFloatMediaLibrary(wxCommandEvent &event);

  void					OnSwitchRackOptViewEvent(wxCommandEvent &event);
  void					OnSwitchSeqOptViewEvent(wxCommandEvent &event);
  void					OnFullScreen(wxCommandEvent &event);
  void					OnAbout(wxCommandEvent &event);
  void					OnIntegratedHelp(wxCommandEvent &event);
  void					OnTimer(wxTimerEvent &event);
  void					OnKillTimer(wxTimerEvent &event);

  bool					NewSession();
  void					LoadPlugins();
  void					StartStream(wxCommandEvent &event);
  void					StopStream(wxCommandEvent &event);

  void					OnEditUndo(wxCommandEvent &event);
  void					OnEditRedo(wxCommandEvent &event);
  void					OnUndo(wxCommandEvent &event);
  void					OnRedo(wxCommandEvent &event);
  void					OnHistory(wxCommandEvent &event);
  void					OnCut(wxCommandEvent &event);
  void					OnCopy(wxCommandEvent &event);
  void					OnPaste(wxCommandEvent &event);
  void					OnDelete(wxCommandEvent &event);
  void					OnSelectAll(wxCommandEvent &event);

  void					MediaLibraryShow(wxCommandEvent &event);
  void					OnSaveML(wxCommandEvent &e);
  void					OnLoadML(wxCommandEvent &e);

  void			                OnShowDebug(wxCommandEvent &event);

  void					OnOpenVideo(wxCommandEvent &event);
  void					OnCloseVideo(wxCommandEvent &event);
  void					OnSeekVideo(wxCommandEvent &event);

  void					OnKey(wxKeyEvent& event);
  void					OnSpaceKey();
  void					SwitchRackOptView();
  void					SwitchSeqOptView();
  void					AddUpdatePlugin(Plugin *p);

  void					SwitchDockedFloat(bool isCurrentlyFloating, int mustBeFloating,
							  wxCommandEvent evt, wxPoint pos, wxSize size,
							  int checkBox, wxFrame *frame,
							  wxFrame *(MainWindow::*floatfunc)());
  void					Save();
  void					Load(SaveElementArray data);
  void					CleanChildren();

  /* init func */
  int					Init();

  /* can be called from SettingWindow */
  int					InitAudio(bool restart = false);

  /* Plugin creation */
  void					CreatePluginFromUniqueId(wxString UniqueId,
								 wxString name);

  wxString				m_FrameTitle;
 protected:
  friend class				MediaLibrary;

 private:
  int					PluginMenuIndexCount;
  bool					RackModeView;
  bool					SeqModeView;
  wxSize				WindowSize;
  wxPoint				WindowPos;
  void					OnIdle(wxIdleEvent &event);

  /**
   * Enumerate states of a panel
   * \param panelShow, show the panel
   * \param panelShowInWindow, show the panel in a detached window
   * \parem panelHide, hide the panel
   */
  enum panelState
  {
    panelShow = 0,
    panelShowInWindow,
    panelHide,
    panelHideFromWindow
  }	;

  /**
   * Provide feature to show or to hide MediaLibrary
   * \param show a panelState, define the state of the MediaLibrary
   */
  void					ShowMediaLibrary(panelState show);

  /* DSSI & LADSPA Plugins Menus */
  void					LoadExternalPlugins();
  int					AddPluginMenuItem(int Type, bool IsEffect, const wxString& MenuName);
  void					OnCreateExternalPlugin(wxCommandEvent &event);

  /* Undo Redo Menus */
  void					InitUndoRedoMenuItems();
  void					CreateUndoRedoMenus(wxMenu *callingMenu);
  void					removeAllMenuItems(wxMenu *menu);

  /* Video Menu */
  void					InitVideoMenuItems();

  void					InitFileConverter();

  /* Config files */
  wxTextFile				PluginsConfFile;

  /* Visible controls */
  wxSplitterWindow			*split;
  wxSplitterWindow			*splitVert;
  wxMenuBar				*MenuBar;
  wxMenu				*FileMenu;
  wxMenu				*EditMenu;
  wxMenu				*VideoMenu;
  wxMenu				*UndoMenu;
  wxMenu				*RedoMenu;
  wxMenu				*HistoryMenu;
  wxMenu				*ViewMenu;
  wxMenu				*SequencerMenu;
  wxMenu				*RacksMenu;
  wxMenu				*CreateInstrMenu;
  wxMenu				*CreateDSSIInstrMenu;
  wxMenu				*CreateLADSPAInstrMenu;
  wxMenu				*CreateEffectMenu;
  wxMenu				*CreateDSSIEffectMenu;
  wxMenu				*CreateLADSPAEffectMenu;
  wxMenu				*HelpMenu;
  wxMenu				*WindowMenu;
  wxMenu				*MediaLibraryMenu;

  wxMenuItem				*ItemFloatingTrans;
  wxMenuItem				*ItemFloatingSeq;
  wxMenuItem				*ItemFloatingRacks;
  wxMenuItem				*ItemFloatingMediaLibrary;
  wxMenuItem				*ItemShowMediaLibrary;
  wxMenuItem				*ItemHideMediaLibrary;

  wxMenuItem				*ItemFullscreenToggle;

  /* Sizers */
  wxBoxSizer				*BottomSizer;
  wxBoxSizer				*TopSizer;
  wxBoxSizer				*TopLeftSizer;
  wxBoxSizer				*TopRightSizer;

  /* Frame pour detacher les objets */
  FloatingFrame				*TransportFrame;
  wxFrame				*OptFrame;
  wxFrame				*SequencerFrame;
  wxFrame				*RackFrame;
  wxFrame				*MediaLibraryFrame;

  wxTimer				*SeqTimer;

  /* List of plugins that need to be updated for their gui */
  list<Plugin *>			UpdatePlugins;

  wxLogWindow                           *LogWin;
  wxLogStderr                           *LogTarget;
  FILE                                  *LogFile;

  DECLARE_EVENT_TABLE()
};

// menu IDs
enum
{
  MainWin_Quit = 42,
  MainWin_New,
  MainWin_Open,
  MainWin_Save,
  MainWin_SaveAs,
  MainWin_ImportDir,
  MainWin_ImportWave,
  MainWin_ImportMIDI,
  MainWin_ImportAKAI,
  MainWin_ExportWave,
  MainWin_ExportMIDI,
  MainWin_DeleteRack,
  MainWin_AddTrackAudio,
  MainWin_AddTrackMidi,
  MainWin_DeleteTrack,
  MainWin_SoloTrack,
  MainWin_Copy,
  MainWin_Cut,
  MainWin_Paste,
  MainWin_Settings,
  MainWin_About,
  MainWin_StopStream,
  MainWin_StartStream,
  MainWin_CloseStream,
  MainWin_OpenStream,
  MainWin_FloatTransport,
  MainWin_FloatSequencer,
  MainWin_FloatRacks,
  MainWin_FloatView,
  MainWin_FloatMediaLibrary,
  MainWin_UndoRedoBeta,
  MainWin_Undo,
  MainWin_Redo,
  MainWin_UndoRedo,
  MainWin_History,
  MainWin_Delete,
  MainWin_SelectAll,
  MainWin_FullScreen,
  MainWin_SeqTimer,
  MainWin_KillTimer,
  MainWin_FileLoader,
  MainWin_IntHelp,
  MainWin_SwitchRack,
  MainWin_SwitchSeq,
  MainWin_VideoBeta,
  MainWin_OpenVideo,
  MainWin_CloseVideo,
  MainWin_SeekVideo,
  MainWin_MediaLibraryBeta,
  MainWin_MediaLibraryShow,
  MainWin_MediaLibraryHide,
  MainWin_SaveML,
  MainWin_LoadML,
  MainWin_ShowLog
};

#endif

