// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

//a enlever
#include <sndfile.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/colour.h>
#include <wx/textfile.h>
#include <wx/timer.h>
#include <wx/splash.h>
#include <wx/splitter.h>

#define WIRED_VERSION			(0.2f)
#define PLUG_MENU_INDEX_START		(50000)
#define INDEX_MENUITEM_UNDO		0
#define INDEX_MENUITEM_REDO		1

#include "Plugin.h"

typedef	struct s_PlugStartInfo		PlugStartInfo;
class					PluginLoader;
class					WiredSession;
class					WiredSessionXml;

class					MainWindow: public wxFrame
{
 public:
  
  MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
  void					OnClose(wxCloseEvent &event);
  void					OnQuit(wxCommandEvent &event);
  void					OnOpen(wxCommandEvent &event);
  void					OnNew(wxCommandEvent &event);
  void					OnSave(wxCommandEvent &event);
  void					OnSaveAs(wxCommandEvent &event);
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
  void					OnChangeAudioDir(wxCommandEvent &event);
  void					OnCreateRackClick(wxCommandEvent &event);
  void					OnCreateEffectClick(wxCommandEvent &event);
  void					OnFloatTransport(wxCommandEvent &event);
  void					OnFloatSequencer(wxCommandEvent &event);
  void					OnFloatRack(wxCommandEvent &event);

  void					OnSwitchRackOptViewEvent(wxCommandEvent &event);
  void					OnSwitchSeqOptViewEvent(wxCommandEvent &event);
  void					OnFullScreen(wxCommandEvent &event);
  void					OnAbout(wxCommandEvent &event);
  void					OnIntegratedHelp(wxCommandEvent &event);
  void					OnTimer(wxTimerEvent &event);
  void					OnFileLoaderStart(wxCommandEvent &event);
  void					OnFileLoaderStop(wxCommandEvent &event);
  bool					NewSession();
  void					LoadPlugins();
  void					AlertDialog(const wxString& from, const wxString& msg);
  void					StartStream(wxCommandEvent &event);
  void					StopStream(wxCommandEvent &event);

  void					OnUndo(wxCommandEvent &event);
  void					OnRedo(wxCommandEvent &event);
  void					OnHistory(wxCommandEvent &event);
  void					OnCut(wxCommandEvent &event);
  void					OnCopy(wxCommandEvent &event);
  void					OnPaste(wxCommandEvent &event);
  void					OnDelete(wxCommandEvent &event);
  void					OnSelectAll(wxCommandEvent &event);

  void					OnSpaceKey();
  void					SwitchRackOptView();
  void					SwitchSeqOptView();  
  void					AddUpdatePlugin(Plugin *p);

 private:
  int					PluginMenuIndexCount;			
  bool					RackModeView;
  bool					SeqModeView;

  /* DSSI & LADSPA Plugins Menus */
  void					LoadExternalPlugins();
  int					AddPluginMenuItem(int Type, bool IsEffect, const string& MenuName);
  void					OnCreateExternalPlugin(wxCommandEvent &event);

  /* Undo Redo Menus */
  void					InitUndoRedoMenuItems();
  void					CreateUndoRedoMenus(wxMenu *callingMenu);
  void					removeAllMenuItems(wxMenu *menu);
  
  void					InitFileConverter();
  
  /* Config files */
  wxTextFile				PluginsConfFile;
  
  /* Visible controls */
  wxSplitterWindow		*split;
  wxMenuBar				*MenuBar;
  wxMenu				*FileMenu;
  wxMenu				*EditMenu;
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

  /* Sizers */
  wxBoxSizer				*BottomSizer;
  wxBoxSizer				*TopSizer;

  /* Frame pour detacher les objets */
  wxFrame				*TransportFrame;
  wxFrame				*OptFrame;
  wxFrame				*SequencerFrame;
  wxFrame				*RackFrame;

  wxTimer				*SeqTimer;

  /* List of plugins that need to be updated for their gui */
  list<Plugin *>			UpdatePlugins;

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
  MainWin_ImportWave,
  MainWin_ImportMIDI,
  MainWin_ImportAKAI,
  MainWin_ExportWave,
  MainWin_ExportMIDI,
  MainWin_DeleteRack,
  MainWin_AddTrackAudio,
  MainWin_AddTrackMidi,
  MainWin_DeleteTrack,
  MainWin_ChangeAudioDir,
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
  MainWin_Undo,
  MainWin_Redo,
  MainWin_History, 
  MainWin_Delete,
  MainWin_SelectAll,
  MainWin_FullScreen,
  MainWin_SeqTimer,
  MainWin_FileLoader,
  MainWin_IntHelp,
  MainWin_SwitchRack,
  MainWin_SwitchSeq 
};

extern MainWindow		*MainWin;
extern vector<PluginLoader *>	LoadedPluginsList;
extern PlugStartInfo		StartInfo;
extern WiredSession		*CurrentSession;

#endif

