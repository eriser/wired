// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MainWindow.h"

#ifdef __LINUX__
# include  <unistd.h>
#endif
#include <wx/splitter.h>
#include <wx/progdlg.h>
#include <wx/utils.h>
#include <algorithm>
#include "SequencerGui.h"
#include "HostCallback.h"
#include "WaveFile.h"
#include "SettingWindow.h"
#include "AudioPattern.h"
#include "AudioCenter.h"
#include "EditMidi.h"
#include "MidiFile.h"
#include "cAddTrackAction.h"
#include "cImportMidiAction.h"
#include "Transport.h"
#include "OptionPanel.h"
#include "Rack.h"
#include "SeqTrack.h"
#include "MixerGui.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "FaderCtrl.h"
#include "StaticLabel.h"
#include "VUMCtrl.h"
#include "FloatingFrame.h"
#include "AudioEngine.h"
#include "Settings.h"
#include "EngineError.h"
#include "Sequencer.h"
#include "Track.h"
#include "Mixer.h"
#include "MidiThread.h"
#include "PluginLoader.h"
#include "WiredExternalPluginMgr.h"
#include "FileConversion.h"
#include <config.h>
#include "Threads.h"
#include "MediaLibrary.h"
#include "MLTree.h"
#include "SaveCenter.h"
#include "debug.h"
#include "Wizard.h"

#ifdef DEBUG_MAINWINDOW
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : "  << __LINE__ << " : " << __FUNCTION__  << endl; }
#else
#define LOG
#endif

#define WIZ_WIDTH 200
#define WIZ_HEIGHT 200
#define WIZ_WIN_SIZE wxSize(WIZ_WIDTH, WIZ_HEIGHT)

//Isn't it bullshit to declare things here ?
Rack			*RackPanel = NULL;
SequencerGui		*SeqPanel = NULL;
Sequencer		*Seq = NULL;
WiredVideo		*WiredVideoObject = NULL;
AudioEngine		*Audio = NULL;
Mixer			*Mix = NULL;
AudioCenter		WaveCenter;
Transport		*TransportPanel = NULL;
PlugStartInfo		StartInfo;
vector<PluginLoader *>	LoadedPluginsList;
//SaveCenter		*saveCenter;
//SaveCenter		*saveCenter = NULL;
WiredExternalPluginMgr	*LoadedExternalPlugins = NULL;
MediaLibrary		*MediaLibraryPanel = NULL;
FileConversion		*FileConverter = NULL;
SettingWindow		*SettingsWin = NULL;

wxMutex			AudioMutex(wxMUTEX_RECURSIVE);
wxCondition		*SeqStopped = NULL;

MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size, WiredDocument *parent)
  : wxFrame((wxFrame *) NULL, wxID_ANY, title, pos, size,
	    wxDEFAULT_FRAME_STYLE | wxWS_EX_PROCESS_IDLE | wxMAXIMIZE, wxT("wired")),
    WiredDocument(wxT("MainWindow"), parent)
{
  LOG;
  wxFileName	path;

  m_FrameTitle = wxT("wired");
  SeqTimer = NULL;
  InitLocale();

#if wxUSE_STATUSBAR
  CreateStatusBar(2);
#endif
  WiredSettings = new Settings();

  //saveCenter = new SaveCenter(wxGetCwd());
  LoadedExternalPlugins = new WiredExternalPluginMgr();
  LogWin = new wxLogWindow(this, wxT("Wired log"), false);

  try
    {
      Audio = new AudioEngine();
    }
  catch (Error::InitFailure &f)
    {
      cout << "[MAINWIN] Portaudio Failure :" << f.getMsg().mb_str() << endl;
      // FIXME: find a cleaner solution
      cout << "Critical error" << endl;
      exit(1);
    }

  // Mixer must be declared after AudioEngine
  Mix = new Mixer();
  Seq = new Sequencer(NULL);
  MidiEngine = new MidiThread();
  MidiEngine->OpenDefaultDevices();
  SettingsWin = new SettingWindow();

  /* Creation Menu */

  FileConverter = NULL;

  TransportFrame = NULL;
  OptFrame = NULL;
  SequencerFrame = NULL;
  RackFrame = NULL;
  MediaLibraryFrame = NULL;

  MenuBar = new wxMenuBar;
  FileMenu = new wxMenu;
  EditMenu = new wxMenu;
  VideoMenu = new wxMenu;
  UndoMenu = new wxMenu;
  RedoMenu = new wxMenu;
  SequencerMenu = new wxMenu;
  RacksMenu = new wxMenu;
  CreateInstrMenu = new wxMenu;
  CreateEffectMenu = new wxMenu;
  HelpMenu = new wxMenu;
  WindowMenu = new wxMenu;

  //calls exec... will *not* work on windows
#ifdef __LINUX__
  FileMenu->Append(MainWin_New, _("&Open Wizard\tCtrl-N"));
  FileMenu->AppendSeparator();
#endif
#ifndef DEBUG_DISABLE_CLEAN_MENUS
  FileMenu->Append(MainWin_New, _("&New\tCtrl-N"));
  FileMenu->Append(MainWin_Open, _("&Open...\tCtrl-O"));
  FileMenu->AppendSeparator();
#endif
  FileMenu->Append(MainWin_ImportWave, _("&Import Wave file..."));
  FileMenu->Append(MainWin_ImportMIDI, _("Import &MIDI file..."));
  FileMenu->Append(MainWin_ImportAKAI, _("Import A&KAI sample..."));
  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_Save, _("&Save\tCtrl-S"));
  FileMenu->Append(MainWin_SaveAs, _("Save &as...\tF12"));
  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_ExportWave, _("E&xport Wave file..."));
  FileMenu->Append(MainWin_ExportMIDI, _("Ex&port MIDI file..."));
  FileMenu->AppendSeparator();
#ifndef FEATURE_DISABLE_SAVE_ML
  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_SaveML, _("Save Media Library"));
  FileMenu->Append(MainWin_LoadML, _("Load Media Library"));
  FileMenu->AppendSeparator();
#endif
  FileMenu->Append(MainWin_Quit, _("&Quit\tCtrl-Q"));

  //EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Cut, _("C&ut\tCtrl+X"));
  EditMenu->Append(MainWin_Copy, _("&Copy\tCtrl+C"));
  EditMenu->Append(MainWin_Paste, _("&Paste\tCtrl+V"));
  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Delete, _("&Delete\tDel"));
  EditMenu->Append(MainWin_SelectAll, _("Select &all\tCtrl+A"));
  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Settings, _("&Settings..."));

  SequencerMenu->Append(MainWin_AddTrackAudio, _("&Add Audio Track"));
  SequencerMenu->Append(MainWin_AddTrackMidi, _("Add &MIDI Track"));
  // Added by Julien Eres
  SequencerMenu->Append(MainWin_AddTrackAutomation, _("Add A&utomation Track"));
  SequencerMenu->Append(MainWin_SoloTrack, _("&Toggle solo (Enter)"));
  SequencerMenu->Append(MainWin_DeleteTrack, _("&Delete Track (Backspace)"));

  RacksMenu->Append(MainWin_DeleteRack, _("D&elete Rack"));

  HelpMenu->Append(MainWin_IntHelp, _("&Show Integrated Help"));
  HelpMenu->Append(MainWin_About, _("&About..."));

  /* XXX
  MediaLibraryMenu->Append(MainWin_MediaLibraryBeta, _("This feature is currently in alpha stage"))->Enable(false);
  MediaLibraryMenu->Append(MainWin_SaveML, _("Save Media Library"));
  MediaLibraryMenu->Append(MainWin_LoadML, _("Load Media Library"));
  MediaLibraryMenu->AppendSeparator();

  ItemShowMediaLibrary = MediaLibraryMenu->AppendCheckItem(MainWin_MediaLibraryShow, _("&Show/Hide\tCtrl-M"));
  ItemFloatingMediaLibrary = MediaLibraryMenu->AppendCheckItem(MainWin_FloatMediaLibrary, _("&Floating"));
  */

  WindowMenu->Append(MainWin_SwitchRack, _("Switch &Rack/Optional view\tTAB"));
  WindowMenu->Append(MainWin_SwitchSeq, _("Switch &Sequencer/Optional view\tCtrl+TAB"));
  WindowMenu->AppendSeparator();
  ItemFloatingTrans = WindowMenu->AppendCheckItem(MainWin_FloatTransport, _("Floating &Transport"));
  ItemShowMediaLibrary = WindowMenu->AppendCheckItem(MainWin_MediaLibraryShow, _("Show/&Hide MediaLibrary\tCtrl-M"));
  WindowMenu->AppendSeparator();
  ItemFloatingMediaLibrary = WindowMenu->AppendCheckItem(MainWin_FloatMediaLibrary, _("Floating &MediaLibrary"));
  ItemFloatingTrans = WindowMenu->AppendCheckItem(MainWin_FloatTransport, _("Floating &Transport"));
  ItemFloatingSeq = WindowMenu->AppendCheckItem(MainWin_FloatSequencer,_("Floating S&equencer"));
  ItemFloatingRacks = WindowMenu->AppendCheckItem(MainWin_FloatRacks, _("Floating R&acks"));
  //   ItemFloatingOptView = WindowMenu->AppendCheckItem(MainWin_FloatView, _("Floating Optional View"));
  WindowMenu->AppendSeparator();
  ItemFullscreenToggle = WindowMenu->AppendCheckItem(MainWin_FullScreen,
						     _("&Fullscreen"));
  WindowMenu->AppendSeparator();
  WindowMenu->AppendCheckItem(MainWin_ShowLog, _("&Log window"));

  MenuBar->Append(FileMenu, _("&File"));
  MenuBar->Append(EditMenu, _("&Edit"));
  MenuBar->Append(SequencerMenu, _("&Sequencer"));
  MenuBar->Append(RacksMenu, _("&Racks"));
  MenuBar->Append(CreateInstrMenu, _("&Instruments"));
  MenuBar->Append(CreateEffectMenu, _("Effec&ts"));
  // Video menu is empty... and not finished
  //  MenuBar->Append(VideoMenu, _("&Video"));
  MenuBar->Append(WindowMenu, _("&Window"));
  MenuBar->Append(HelpMenu, _("&Help"));

  SetMenuBar(MenuBar);

  splitVert = new wxSplitterWindow(this);
  split = new wxSplitterWindow(splitVert);
  split->SetMinimumPaneSize(2);
  splitVert->SetMinimumPaneSize(2);

  /* Creation Panel */
  cout << "[MAINWIN] Start creation of GUI objects";
  RackPanel = new Rack(split, -1, wxPoint(0, 0), wxSize(800, 250));
  cout << ". ";
  SeqPanel = new SequencerGui(split, wxPoint(0, 0), wxSize(800, 200), this, (WiredDocument*)Seq);
  cout << ". ";
  OptPanel = new OptionPanel(this, wxPoint(306, 452), wxSize(470, 120), wxSIMPLE_BORDER, NULL);
  cout << ". ";
 TransportPanel = new Transport(this, wxPoint(0, 452), wxSize(300, 150), wxNO_BORDER, NULL);
  cout << ". ";
  MediaLibraryPanel = new MediaLibrary(splitVert);
  cout << " done" << endl;

  splitVert->SplitVertically(MediaLibraryPanel, split);
  split->SplitHorizontally(RackPanel, SeqPanel, 200);

  /* Placement Panel */
  BottomSizer = new wxBoxSizer(wxHORIZONTAL);
  BottomSizer->Add(TransportPanel, 0, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2);
  BottomSizer->Add(OptPanel, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2);

  TopSizer = new wxBoxSizer(wxVERTICAL);
  TopSizer->Add(splitVert, 1, wxEXPAND | wxALL, 2);
  TopSizer->Add(BottomSizer, 0, wxEXPAND | wxALL, 0);
  SetSizer(TopSizer);

  RackPanel->SetBackgroundColour(*wxBLACK);
  SeqPanel->SetBackgroundColour(*wxWHITE);
  OptPanel->SetBackgroundColour(*wxLIGHT_GREY);
  MediaLibraryPanel->SetBackgroundColour(*wxWHITE);

  // media lib start hidden until its state is stable
  ShowMediaLibrary(panelHideFromWindow);
  RackPanel->Show();
  SeqPanel->Show();
  OptPanel->Show();
  TransportPanel->Show();

  StartInfo.HostCallback = HostCallback;
  StartInfo.Version = WIRED_VERSION;
  StartInfo.Rack = RackPanel;

  LoadPlugins();

  RackPanel->AddPlugToMenu();

  LoadExternalPlugins();

  RackModeView = true;
  SeqModeView = true;

  // Minimum size of the window
  SetSizeHints(400, 300);

  Connect(MainWin_ImportWave, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&MainWindow::OnImportWave);

  //Funky icon.
  wxIcon wiredIcon;
  wxString iconPath;

  iconPath << WiredSettings->DataDir << wxT("ihm/splash/icon-small.gif");

  if(wiredIcon.LoadFile(iconPath, wxBITMAP_TYPE_GIF))
    SetIcon(wiredIcon);

#if wxUSE_STATUSBAR
  Connect(wxID_ANY, wxEVT_IDLE, (wxObjectEventFunction) &MainWindow::OnIdle);
#endif
}

// basicaly launch actions which are non-graphical related
int			MainWindow::Init()
{
  LOG;
  wxThreadError		err;

  // start midi thread
  if ((err = MidiEngine->Create()) != wxTHREAD_NO_ERROR)
    {
      cout << "[MAINWIN] Create MidiEngine thread failed ! (error:"
	   << err << ")" << endl;
      return (-1);
    }
  if ((err = MidiEngine->Run()) != wxTHREAD_NO_ERROR)
    {
      cout << "[MAINWIN] Run MidiEngine thread failed ! (error:"
	   << err << ")" << endl;
      return (-1);
    }
  else
    wxGetApp().m_threads.Add(MidiEngine);

  // creation of condition needed for InitAudio and Seq communication
  SeqStopped = new wxCondition(AudioMutex);
  if (!SeqStopped->IsOk())
    {
      cout << "[MAINWIN] Condition creation failed.. critical error" << endl;
      return (-1);
    }

  // init audio
  if (InitAudio() < 0)
    {
      // avoid to flood user, he already knows he has a deprecated config!
      if (!WiredSettings->ConfIsDeprecated())
	AlertDialog(_("audio engine"),
		    _("Could not open audio device : check that the device is not busy (used by another application) and that your audio settings are correct."));
    }
  InitFileConverter();

  // start sequencer thread (after InitAudio is a good option)
  if ((err = Seq->Create()) != wxTHREAD_NO_ERROR)
    {
      cout << "[MAINWIN] Create sequencer thread failed ! (error:"
	   << err << ")" << endl;
      return (-1);
    }
  Seq->SetPriority(WXTHREAD_MAX_PRIORITY);
  if ((err = Seq->Run()) != wxTHREAD_NO_ERROR)
    {
      cout << "[MAINWIN] Run sequencer thread failed ! (error:"
	   << err << ")" << endl;
      return (-1);
    }
  else
    wxGetApp().m_threads.Add(Seq);

  //InitUndoRedoMenuItems();
  //  InitVideoMenuItems();

  SeqTimer = new wxTimer(this, MainWin_SeqTimer);
  SeqTimer->Start(40);

  WindowSize = MainWin->GetSize();
  WindowPos = MainWin->GetPosition();

  return (0);
}

int			MainWindow::InitAudio(bool restart)
{
  LOG;
  // midi stuff
    list<RackTrack*>::iterator	rtit;
    RackTrack			*rt;
    list<Plugin*>::iterator	pit;
    Plugin			*p;
    int				i;
    int				j;
    vector<RackTrack*>		vrt;
    vector<Plugin*>		vp;
    vrt.clear();
    vp.clear();
  // midi stuff end

  wxMutexLocker audioLock(AudioMutex);
  if (Audio->IsOk)
    {
      Audio->IsOk = false;
      // wait sequencer thread until he finish his cycle
      if (SeqStopped->WaitTimeout(3000) == wxCOND_TIMEOUT)
	{
	  cout << "[MAINWIN] Sequencer thread stuck ?" << endl;
	  return (-1);
	}
    }
  wxMutexLocker	seqLock(SeqMutex);

  // save settings
  if (restart)
    SettingsWin->Save();

  // stop all racks that have something to do with midi
  if (RackPanel)
  {
    i = 0;
    for (rtit = RackPanel->RackTracks.begin(); rtit != RackPanel->RackTracks.end(); rtit++)
    {
      rt = *rtit;
      pit = rt->Racks.begin();
      for (pit = rt->Racks.begin() ; pit != rt->Racks.end(); pit++)
      {
	p = *pit;
	if (p)
	{
	  cout << "Plugin uniq id = '";
	  for (i = 0; i < 4 ; i++)
	    cout << p->InitInfo->UniqueId[i];
	  cout << "'" << endl;
	  if (p->IsMidi())
	  {

	    cout << " ...added to the 'do something' list" << endl;
	    vrt.push_back(rt);
	    vp.push_back(p);
	  }
	}
      }
    }

    for (i = 0 ; i < vp.size() ; i++)
    {
      /*
       *cout << "saving number '" << i << "'" << endl;
       *vp[i]->StartInfo.parent->Save();
       * //will not write the file :
       * //we need to call saveCenter->SaveOneDocument
       * //but for this we need a filename...
       * //the best way to do this would be NOT to apply changes
       * //but SAVE them and warn that it won't apply until next exec
       */
      cout << "deleting number '" << i << "'" << endl;
      vrt[i]->Parent->DeleteRack(vp[i]);
    }

  }

  // change settings
  if (!Audio->CloseStream())
    {
      cout
	<< "[MAINWIN] Could not close audio stream, you may restart Wired"
	<< endl;
      return (-1);
    }
  try
    {
      Audio->IsOk = false;

      Audio->GetDeviceSettings();
      if (restart == true)
	Mix->InitBuffers();
      Audio->OpenStream();
      if (Audio->StartStream())
	Audio->IsOk = true;
      else
	cout << "[MAINWIN] AudioMutex still locked"<< endl;
    }
  // from GetDeviceSettings
  catch (Error::InvalidDeviceSettings)
    {
      cout << "[MAINWIN] Invalid Device Settings" << endl;
    }
  // from SetChannels
  catch (Error::ChannelsNotSet)
    {
      cout << "[MAINWIN] Channels Not Set" << endl;
    }
  // from OpenStream
  catch (Error::StreamNotOpen)
    {
      cout << "[MAINWIN] Stream Not Opened" << endl;
    }
  // from nowhere
  catch (Error::AudioEngineError)
    {
      cout << "[MAINWIN] General AudioEngine Error" << endl;
    }
  catch (std::bad_alloc)
    {
      cout << "[MAINWIN] oom" << endl;
    }
  catch (std::exception &e)
    {
      cout << "[MAINWIN] Stdlib failure (" << e.what() <<
	")during AudioEngine init, check your code" << endl;
    }
  catch (...)
    {
      cout << "[MAINWIN] Unknown AudioEngine error" << endl;
    }

  if ( Audio->IsOk )
    {
      vector<Track *>::iterator	i;

      // Sequencer refill its own vars
      Seq->AudioConfig();

      // Refill tracks connections
      if (SettingsWin->AudioLoaded || SettingsWin->MidiLoaded)
	for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
	  (*i)->GetTrackOpt()->FillChoices();
      // Sends sample rate and buffer size modifications to plugins
      if (SettingsWin->AudioLoaded)
	RackPanel->SetAudioConfig(Audio->SamplesPerBuffer, Audio->SampleRate);
      if (restart)
	{
	  FileConverter->SetFormat((PaSampleFormat)Audio->SampleFormat);
	  FileConverter->SetBufferSize(Audio->SamplesPerBuffer);
	  FileConverter->SetSampleRate((long unsigned int)Audio->SampleRate);
	}

    }
  else
    cout << "Could not open audio device : check that the device is not busy (used by another application) and that your audio settings are correct." << endl;

  // dialog can't be  before mutex unlocking (outside this function is better)
  if (!Audio->IsOk)
    return (-1);

  if (SettingsWin->MidiLoaded)
    {
      wxMutexLocker lock(MidiDeviceMutex);

      // Reopen midi devices
      MidiEngine->OpenDefaultDevices();
    }
  // reinit SettingsWin var
  SettingsWin->AudioLoaded = false;
  SettingsWin->MidiLoaded = false;
  return (0);
}

void                MainWindow::InitLocale()
{
  LOG;
  // disable extra output of wx
  wxLog		log(wxLogNull);
  wxString	prefix = wxT(PACKAGE_LOCALE_DIR);

  mLocale = new wxLocale();
  mLocale->AddCatalogLookupPathPrefix(prefix);

  //try to set default language (is it really useful ? it seems to never work)
  if (mLocale->Init(wxLANGUAGE_DEFAULT) == true)
    cout << "[MAINWIN][InitLocale] locale initialized to wxLANGUAGE_DEFAULT" << endl;;

  // add wx basic translation (File, Window, About, ..) (It seems to never return true ...)
  if(mLocale->AddCatalog(wxT("wxstd")))
    cout << "[MAINWIN][Initlocale] wxstd catalog added" << endl;;

  // add our translations
  if(mLocale->AddCatalog(wxT("wired")))
    cout << "[MAINWIN][InitLocale] wired catalog added" << endl;;
}

void					MainWindow::InitFileConverter()
{
  LOG;
  //USES WIRESESSION
  FileConverter = new FileConversion();
  t_samplerate_info info;
  // 	int i;
  // 	if (Audio->UserData->Sets->WorkingDir.empty())
  // 	  {
  // 	    wxDirDialog dir(this, _("Choose the audio working directory"), wxFileName::GetCwd(), wxDD_NEW_DIR_BUTTON | wxCAPTION | wxSUNKEN_BORDER);
  // 	    if (dir.ShowModal() == wxID_OK)
  // 	      CurrentXmlSession->GetAudioDir() = dir.GetPath();
  // 	    else
  // 	      CurrentXmlSession->GetAudioDir() = wxFileName::GetCwd();

  // 	    Audio->UserData->Sets->WorkingDir = CurrentXmlSession->GetAudioDir();
  // 	  }
  // 	else
  // 	  {
  // 	    CurrentXmlSession->GetAudioDir() = Audio->UserData->Sets->WorkingDir;

  // 	  }
  // 	info.WorkingDirectory = CurrentXmlSession->GetAudioDir();

  info.WorkingDirectory = saveCenter->getAudioDir();
  info.SampleRate = (unsigned long) Audio->SampleRate;
  info.SamplesPerBuffer = (unsigned long) Audio->SamplesPerBuffer;
  // init FileConverter with 4Mo of cache
  if (FileConverter->Init(&info, info.WorkingDirectory, (unsigned long)1024 * 1024 * 4, this) == false)
    cout << "[MAINWIN] Create file converter thread failed !" << endl;
  MediaLibraryPanel->SetFileConverter(FileConverter);
}

void					MainWindow::InitUndoRedoMenuItems()
{
  LOG;
  EditMenu->Insert(INDEX_MENUITEM_UNDO, MainWin_Undo, _("U&ndo"), UndoMenu);
  EditMenu->Insert(INDEX_MENUITEM_REDO, MainWin_Redo, _("&Redo"), RedoMenu);
  EditMenu->Enable(MainWin_Undo, false);
  EditMenu->Enable(MainWin_Redo, false);
}

void					MainWindow::InitVideoMenuItems()
{
  LOG;
  VideoMenu->Append(MainWin_OpenVideo, _("&Open video"));
  VideoMenu->Append(MainWin_CloseVideo, _("&Close video"));
  VideoMenu->AppendCheckItem(MainWin_SeekVideo, _("&Seek with video playing"));
  cout << "new wiredvideo"<< endl;
  WiredVideoObject = new WiredVideo();
  VideoMenu->Enable(MainWin_OpenVideo, true);
  VideoMenu->Enable(MainWin_CloseVideo, false);
}

void					MainWindow::OnClose(wxCloseEvent &event)
{
  LOG;
  vector<RackTrack *>::iterator		i;
  vector<Plugin *>::iterator		j;
  vector<PluginLoader *>::iterator	k;
  int					res;

  wxMessageDialog *msg = new wxMessageDialog(this, _("Save current session ?"), wxT("Wired"),
					     wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE);
  res = msg->ShowModal();
  msg->Hide();
  msg->Destroy();

  if (res == wxID_YES)
    {
      wxCommandEvent evt;
      OnSave(evt);
    }
  else if (res == wxID_CANCEL)
    {
      if (event.CanVeto())
	{
	  event.Veto();
	  return;
	}
    }

  this->Hide();
  ::wxSafeYield();

#if wxUSE_STATUSBAR
  Disconnect(wxEVT_IDLE, (wxObjectEventFunction) &MainWindow::OnIdle);
#endif
  cout << "[MAINWIN] Stopping threads..."<< endl;
  wxThread *thread;

  wxGetApp().m_mutex.Lock();
  const wxArrayThread& threads = wxGetApp().m_threads;
  size_t count = threads.GetCount();

  for (size_t i = 0; i < count; i++)
    threads.Item(i)->Delete();

  if (count > 0)
    {
      cout << "[MAINWIN] Waiting for Threads to stop..." << endl;
      if (wxGetApp().m_condAllDone->WaitTimeout(5000) == wxCOND_TIMEOUT)
	cout << "[MAINWIN] Threads are stuck !"<< endl;
    }
  wxGetApp().m_mutex.Unlock();

  cout << "[MAINWIN] Done !"<< endl;

  cout << "[MAINWIN] Stopping graphic things..."<< endl;
  SeqTimer->Stop();
  delete SeqTimer;
  SeqTimer = NULL;

  cout << "[MAINWIN] Unloading shared libraries..."<< endl;
  for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
    delete *k;

  if (FileConverter)
    delete FileConverter;

  cout << "[MAINWIN] Closing all audio devices and streams..." << endl;
  // if we have to do something with already dead threads, it's here.
  if (Audio)
    delete Audio;
  Audio = NULL; // for handling event of Transport::OnIdle

  delete Mix;

  cout << "[MAINWIN] Unloading logging manager..." << endl;
  delete LogWin;

  cout << "[MAINWIN] Unloading external plugins..." << endl;
  if (LoadedExternalPlugins)
    delete LoadedExternalPlugins;

  cout << "[MAINWIN] Closing Media Library" << endl;
  if(MediaLibraryPanel)
    delete MediaLibraryPanel;

  cout << "[MAINWIN] Unloading session manager..." << endl;
  delete saveCenter;

  cout << "[MAINWIN] Unloading user settings manager..." << endl;
  if (WiredSettings)
    delete WiredSettings;

  cout << "[MAINWIN] Closing..." << endl;
  // all gui windows (and this one) are destroyed in MainApp::OnExit
  // but exit prevent wired to segfault... :(
  exit(0);
}

void					MainWindow::OnQuit(wxCommandEvent &WXUNUSED(event))
{
  LOG;
  Close(false);
}

void					MainWindow::OnNew(wxCommandEvent &event)
{
  LOG;
  NewSession();
}

bool					MainWindow::NewSession()
{
  LOG;
  // an existing session is opened, we'll ask for confirmation
  wxMessageDialog			msg(this, _("Save current session ?"), wxT("Wired"),
					    wxYES_NO | wxCANCEL | wxICON_QUESTION);
  int					res;

  res = msg.ShowModal();
  if (res == wxID_YES)
    {
      wxCommandEvent evt;
      OnSave(evt);    //CurrentXmlSession->Save();
    }
  else if (res == wxID_CANCEL)
    return (false);

  Seq->Stop();
#ifdef __LINUX__
  //haha //will *not* work on windows
  cout << "[MAINWIN] Closing sound stream..." << endl;
  if (Audio)
	Audio->CloseStream();
  cout << "[MAINWIN] Closing Media Library" << endl;
  if(MediaLibraryPanel)
    delete MediaLibraryPanel;

  cout << "[MAINWIN] Unloading session manager..." << endl;
  delete saveCenter;

  cout << "[MAINWIN] Unloading user settings manager..." << endl;
  if (WiredSettings)
    delete WiredSettings;

  execlp("wired", "wired-nosplash", (char*)NULL);
  exit(0);
#else
  // this needs serious debugging...
  /*
   *saveCenter->CleanTree();
   *delete saveCenter;
   */
  //OpenWizard();
#endif
  return (true);
}

void					MainWindow::CleanChildren()
{
  LOG;
  UpdatePlugins.clear();

  // AudioCenter
  WaveCenter.Clear();
}

void					MainWindow::OnOpen(wxCommandEvent &event)
{
  LOG;
  wxDirDialog	dirDialog(NULL, _("Select a project folder"),
			  saveCenter->getProjectPath().GetPath());

  if (dirDialog.ShowModal() == wxID_OK)
    {
      wxString selfile = dirDialog.GetPath();

      cout << "[MAINWIN] User opens " << selfile.mb_str() << endl;

      saveCenter->setProjectPath(selfile);
    }
}

void					MainWindow::OnSave(wxCommandEvent &event)
{
  LOG;
  saveCenter->SaveProject();
}

void					MainWindow::OnSaveAs(wxCommandEvent &event)
{
  LOG;
  wxDirDialog	dirDialog(NULL, _("Select a project folder"),
			  saveCenter->getProjectPath().GetPath());

  if(dirDialog.ShowModal() == wxID_OK)
    saveCenter->setProjectPath(dirDialog.GetPath());
  else
    return ;

  saveCenter->SaveProject();

}

void MainWindow::OnImportWave(wxCommandEvent &event)
{
  LOG;
  wxFileDialog dlg(this, _("Loading sound file"), wxT(""), wxT(""), WIRED_SUPPORTED_SNDFILES, wxMULTIPLE);
  if (dlg.ShowModal() == wxID_OK)
  {
    wxArrayString paths;
    dlg.GetPaths(paths);

    MidiMutex.Lock();
    MidiDeviceMutex.Lock();
    SeqMutex.Unlock();

    for (wxArrayString::iterator i = paths.begin(); i != paths.end(); i++)
      FileConverter->ImportFile((*i));

    MidiMutex.Unlock();
    MidiDeviceMutex.Unlock();
  }
}

void MainWindow::OnImportMIDI(wxCommandEvent &event)
{
  LOG;
  wxFileDialog dlg(this, _("Import MIDI file"), wxT(""), wxT(""), _("Midi file (*.mid)|*.mid"));
  if (dlg.ShowModal() == wxID_OK)
  {
    wxString selfile = dlg.GetPath();

    cout << "[MAINWIN] Users imports MIDI file : " << selfile.mb_str() << endl;
    wxProgressDialog Progress(_("Loading midi file"), _("Please wait..."), 100,
      this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
    Progress.Update(1);
    cImportMidiAction* action = new cImportMidiAction(selfile, eMidiTrack);
    action->Do();
    Progress.Update(99);
  }
}

void					MainWindow::OnImportAKAI(wxCommandEvent &event)
{
  LOG;
  //TransportPanel->OnStop(event);
  wxFileDialog dlg(this, _("Import AKAI samples"));
  if (dlg.ShowModal() == wxID_OK)
  {
    wxString selfile = dlg.GetPath();
    wxProgressDialog Progress(_("Loading midi file"), _("Please wait..."), 100,
      this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
    Progress.Update(1);
    cImportAkaiAction* action = new cImportAkaiAction(selfile, eAudioTrack);
    action->Do();
    Progress.Update(99);
// 	cout << "[MAINWIN] Users imports AKAI sample : " << selfile << endl;
// 	wstring dev = selfile.substr(0, selfile.find(":", 0));
// 	selfile = selfile.substr(selfile.find(":", 0) + 1, selfile.size() - selfile.find(":", 0));
// 	wstring path = selfile.substr(10, selfile.size() - 10);
// 	unsigned int pos = path.find("/", 0);
// 	int part = path.substr(0, pos).c_str()[0] - 64;
// 	path = path.substr(pos, path.size() - pos);
// 	int opos = 0;
// 	while ((pos = path.find("/", opos)) != wstring::npos)
// 	opos = pos + 1;
// 	wstring name = path.substr(opos, path.size() - opos);
// 	path = path.substr(1, opos - 2);
// 	wxProgressDialog *Progress = new wxProgressDialog("Loading wave file", "Please wait...",
// 	100, this,
// 	wxPD_AUTO_HIDE | wxPD_CAN_ABORT |
// 	wxPD_REMAINING_TIME);
// 	t_akaiSample *sample = akaiGetSampleByName((char *)dev.c_str(), part, (char *)path.c_str(), (char *)name.c_str());
// 	if (sample != NULL)
// 	{
// 	Progress->Update(1);
// 	cout << "[MAINWIN] sample channel : " << sample->channels << endl;
// 	WaveFile *w = new WaveFile(sample->buffer, sample->size, 2, sample->rate);

// 	if (!w->Error)
// 	{
// 	Track *t = SeqPanel->CreateTrack(eAudioTrack);
// 	Progress->Update(90);
// 	t->CreateAudioPattern(w);
// 	Progress->Update(99);
// 	}
// 	else
// 	cout << "[MAINWIN] Cannot import AKAI wave file !" << endl;
// 	delete Progress;
// 	}
  }
}

void MainWindow::OnExportWave(wxCommandEvent &event)
{
  LOG;
  //  TransportPanel->OnStop(event);
  double total = Seq->EndLoopPos - Seq->BeginLoopPos;

  if (total <= 0)
  {
    wxMessageDialog msg(this, _("Please correctly place the Left and Right markers"), wxT("Wired"), wxOK | wxICON_EXCLAMATION | wxCENTRE);
    msg.ShowModal();
    return;
  }
  wxFileDialog dlg(this, _("Exporting sound file"), wxT(""), wxT(""), wxT(""), wxSAVE);
  if (dlg.ShowModal() == wxID_OK)
  {
    wxString selfile = dlg.GetPath();
    wxFileName f(selfile);
    if (f.GetExt().IsEmpty())
    {
      f.SetExt(wxT("wav"));
      selfile = f.GetFullPath();
    }
    cout << "[MAINWIN] User exports " << selfile.mb_str() << endl;
    if (Seq->ExportToWave(selfile) == false)
    {
      cout << "[MAINWIN] Export canceled by user " << endl;
      return;
    }
    wxProgressDialog Progress(_("Exporting mix"), _("Please wait..."), (int)Seq->EndLoopPos * 1000, this,
      wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_AUTO_HIDE |
      wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME);
    bool done = false;
    while (!done)
    {
      if (Progress.Update((int) Seq->CurrentPos * 1000) == false)
          break;
      wxMilliSleep(50);
      if (Seq->CurrentPos >= Seq->EndLoopPos)
        done = true;
    }
  }
}

void MainWindow::OnExportMIDI(wxCommandEvent &event)
{
  LOG;
  wxFileDialog dlg(this, _("Export MIDI file"), wxT(""), wxT(""), _("Midi file (*.mid)|*.mid"), wxSAVE);
  if (dlg.ShowModal() == wxID_OK)
  {
    wxString selfile = dlg.GetPath();
    cout << "[MAINWIN] Users exports MIDI file : " << selfile.mb_str() << endl;
  }
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
}

void					MainWindow::LoadPlugins()
{
  LOG;
  wxString				str;
  PluginLoader				*p;

  if (!PluginsConfFile.Open(WiredSettings->PlugConfFile))
    {
      cerr << "[MAINWIN] Could not load " << WiredSettings->PlugConfFile.mb_str() << endl;
      return;
    }
  PluginMenuIndexCount = PLUG_MENU_INDEX_START;
  for (str = PluginsConfFile.GetFirstLine(); !PluginsConfFile.Eof();
       str = PluginsConfFile.GetNextLine())
    {
      if ((str.length() > 0) && (str.at(0) != '#'))
	{
	  p = new PluginLoader(str);
	  if (p && p->IsLoaded())
	    {
	      LoadedPluginsList.push_back(p);

	      p->Id = PluginMenuIndexCount++;
	      if (p->InitInfo.Type == ePlugTypeInstrument)
		{
		  CreateInstrMenu->Append(p->Id, p->InitInfo.Name);
		  Connect(p->Id, wxEVT_COMMAND_MENU_SELECTED,
			  (wxObjectEventFunction)(wxEventFunction)
			  (wxCommandEventFunction)&MainWindow::OnCreateRackClick);
		}
	      else if (p->InitInfo.Type == ePlugTypeEffect)
		{
		  CreateEffectMenu->Append(p->Id, p->InitInfo.Name);
		  Connect(p->Id, wxEVT_COMMAND_MENU_SELECTED,
			  (wxObjectEventFunction)(wxEventFunction)
			  (wxCommandEventFunction)&MainWindow::OnCreateEffectClick);
		}
	      else
		cout << "[MAINWIN] Plugin type unknown" << endl;
#ifdef __DEBUG__
	      cout << "[MAINWIN] Plugin " << p->InitInfo.Name.mb_str() << " is working" << endl;
#endif
	    }
	  else
	    delete p;
	}
    }
  cout << "[MAINWIN] Loaded " << LoadedPluginsList.size() << " internal plugins" << endl;
}

void					MainWindow::LoadExternalPlugins()
{
  LOG;
  //  map<int, wstring>				PluginsList;
  list<wxString>					PluginsList;
  //  map<int, wstring>::iterator	IterPluginsList;
  list<wxString>::iterator		IterPluginsList;
  int							PluginInfo;
  int							PluginId;
  long							LongPluginId;
  wxString						PluginName;
  wxString						Sep(wxT("#"));

  CreateDSSIInstrMenu = NULL;
  CreateLADSPAInstrMenu = NULL;
  CreateDSSIEffectMenu = NULL;
  CreateLADSPAEffectMenu = NULL;
  LoadedExternalPlugins->LoadPLugins(TYPE_PLUGINS_DSSI | TYPE_PLUGINS_LADSPA);
  LoadedExternalPlugins->SetStartInfo(StartInfo);
  PluginsList = LoadedExternalPlugins->GetSortedPluginsList(Sep);

  for (IterPluginsList = PluginsList.begin(); IterPluginsList != PluginsList.end(); IterPluginsList++)
    {
      if ((*IterPluginsList).find_last_of(Sep) > 0)
  	{
	  PluginName = (*IterPluginsList).substr(0, (*IterPluginsList).find_last_of(Sep));
	  wxString((*IterPluginsList).substr((*IterPluginsList).find_last_of(Sep) + 1)).ToLong(&LongPluginId);
	  PluginId = (int)LongPluginId;
  	}
      PluginInfo = LoadedExternalPlugins->GetPluginType(PluginId);
      LoadedExternalPlugins->SetMenuItemId(PluginId,
					   AddPluginMenuItem(PluginInfo, PluginInfo & TYPE_PLUGINS_EFFECT, PluginName));
    }
  cout << "[MAINWIN] Loaded " << PluginsList.size() << " external plugins" << endl;
}

int						MainWindow::AddPluginMenuItem(int Type, bool IsEffect, const wxString& MenuName)
{
  LOG;
  int					Id = PluginMenuIndexCount++;
  wxMenuItem			*NewItem;

  if (IsEffect == true)
    {
      if (Type & TYPE_PLUGINS_DSSI)
	{
	  if (!CreateDSSIEffectMenu)
	    {
	      CreateDSSIEffectMenu = new wxMenu();
	      CreateEffectMenu->Append(Id, wxT("DSSI"), CreateDSSIEffectMenu);
	      Id = PluginMenuIndexCount++;
	    }
	  NewItem = CreateDSSIEffectMenu->Append(Id, MenuName);
	}
      else if (Type & TYPE_PLUGINS_LADSPA)
	{
	  if (!CreateLADSPAEffectMenu)
	    {
	      CreateLADSPAEffectMenu = new wxMenu();
	      CreateEffectMenu->Append(Id, wxT("LADSPA"), CreateLADSPAEffectMenu);
	      Id = PluginMenuIndexCount++;
	    }
	  NewItem = CreateLADSPAEffectMenu->Append(Id, MenuName);
	}
    }
  else
    {
      if (Type & TYPE_PLUGINS_DSSI)
	{
	  if (!CreateDSSIInstrMenu)
	    {
	      CreateDSSIInstrMenu = new wxMenu();
	      CreateInstrMenu->Append(Id, wxT("DSSI"), CreateDSSIInstrMenu);
	      Id = PluginMenuIndexCount++;
	    }
	  NewItem = CreateDSSIInstrMenu->Append(Id, MenuName);
	}
      else if (Type & TYPE_PLUGINS_LADSPA)
	{
	  if (!CreateLADSPAInstrMenu)
	    {
	      CreateLADSPAInstrMenu = new wxMenu();
	      CreateInstrMenu->Append(Id, wxT("LADSPA"), CreateLADSPAInstrMenu);
	      Id = PluginMenuIndexCount++;
	    }
	  NewItem = CreateLADSPAInstrMenu->Append(Id, MenuName);
	}
    }
  if (NewItem)
    Connect(Id, wxEVT_COMMAND_MENU_SELECTED,
	    (wxObjectEventFunction)(wxEventFunction)
	    (wxCommandEventFunction)&MainWindow::OnCreateExternalPlugin);
  return Id;
}

void					MainWindow::CreatePluginFromUniqueId(wxString UniqueId,
									     wxString name)
{
  LOG;
  char					Uniq[4];
  vector<PluginLoader *>::iterator	it;

  // load a rack from a known UniqueId
  Uniq[0] = UniqueId[0];
  Uniq[1] = UniqueId[1];
  Uniq[2] = UniqueId[2];
  Uniq[3] = UniqueId[3];
  for (it = LoadedPluginsList.begin(); it != LoadedPluginsList.end(); it++)
    if (COMPARE_IDS((*it)->InitInfo.UniqueId,Uniq))
      {
	RackPanel->AddToSelectedTrack(StartInfo, *it);
	return;
      }

  // if it's a bad UniqueId, we search from its name
  // basically it will be an external plugin
  list<wxString>::iterator		IterPluginsList;
  list<wxString>			PluginsList;
  wxString				Sep(wxT("#"));
  wxString				PluginName;
  long					PluginId;

  PluginsList = LoadedExternalPlugins->GetSortedPluginsList(Sep);
  for (IterPluginsList = PluginsList.begin(); IterPluginsList != PluginsList.end(); IterPluginsList++)
    {
      if ((*IterPluginsList).find_last_of(Sep) > 0)
  	{
	  PluginName = (*IterPluginsList).substr(0, (*IterPluginsList).find_last_of(Sep));
	  if (PluginName == name)
	    {
	      wxString((*IterPluginsList).substr((*IterPluginsList).find_last_of(Sep) + 1)).ToLong((long*)&PluginId);
	      cout << "id : " << PluginId << endl;
	      RackPanel->AddToSelectedTrack(StartInfo, new PluginLoader(LoadedExternalPlugins, StartInfo, PluginId));
	      return;
	    }
	}
    }
}

void					MainWindow::OnCreateExternalPlugin(wxCommandEvent &event)
{
  LOG;
  if (LoadedExternalPlugins)
    {
      cout << "[MAINWIN] Creating rack for plugin id :" << event.GetId() << endl;
      PluginLoader 	*NewPlugin = new PluginLoader(LoadedExternalPlugins, event.GetId(), StartInfo);

      LoadedPluginsList.push_back(NewPlugin);
      cActionManager::Global().AddEffectAction(&StartInfo, NewPlugin, true);
    }
}

void					MainWindow::OnCreateRackClick(wxCommandEvent &event)
{
  LOG;
  int					id = event.GetId();
  vector<PluginLoader *>::iterator	i;
  PluginLoader				*p = 0x0;

  for (i = LoadedPluginsList.begin(); i != LoadedPluginsList.end(); i++)
    if ((*i)->Id == id)
      {
	p = *i;
	break;
      }
  if (p)
    {
      cout << "[MAINWIN] Creating rack for plugin: " << p->InitInfo.Name.mb_str() << endl;
      cCreateRackAction* action = new cCreateRackAction(&StartInfo,  p);
      action->Do();
    }
}

void					MainWindow::OnCreateEffectClick(wxCommandEvent &event)
{
  LOG;
  int					id = event.GetId();
  vector<PluginLoader *>::iterator	i;
  PluginLoader				*p = 0x0;

  for (i = LoadedPluginsList.begin(); i != LoadedPluginsList.end(); i++)
    if ((*i)->Id == id)
      {
	p = *i;
	break;
      }
  if (p)
    {
      cout << "[MAINWIN] Creating rack for plugin: " << p->InitInfo.Name.mb_str() << endl;
      cActionManager::Global().AddEffectAction(&StartInfo, p, true);
      //CreateUndoRedoMenus(EditMenu);
    }
}

void					MainWindow::OnDeleteRack(wxCommandEvent &event)
{
  LOG;
  RackPanel->DeleteSelectedRack();
}

void					MainWindow::OnAddTrackAudio(wxCommandEvent &event)
{
  LOG;
  SeqPanel->CreateTrack(eAudioTrack);
}

void					MainWindow::OnAddTrackMidi(wxCommandEvent &event)
{
  LOG;
  Track *newTrack = SeqPanel->CreateTrack(eMidiTrack);
}

// Added by Julien Eres
void					MainWindow::OnAddTrackAutomation(wxCommandEvent &event)
{
  LOG;
  Track *newTrack = SeqPanel->CreateTrack(eAutomationTrack);
  newTrack->CreateMidiPattern(new MidiTrack(0, NULL, 96, wxT(""), 1));
  cout << __FUNCTION__ << '@' << __LINE__ << ": TODO In progress." << endl;
}

void					MainWindow::OnFloatTransport(wxCommandEvent &event)
{
  LOG;
  FloatTransport();
}

wxFrame					*MainWindow::FloatTransport()
{
  LOG;
  if (WindowMenu->IsChecked(MainWin_FloatTransport))
    {
      TransportPanel->Hide();
      BottomSizer->Detach(TransportPanel);
      BottomSizer->Layout();

      TransportFrame = new FloatingFrame(0x0, -1, _("Transport"), TransportPanel->GetPosition(),
					 TransportPanel->GetSize(), TransportPanel->GetParent(),
					 ItemFloatingTrans, MainWin_FloatTransport);
      TransportPanel->Reparent(TransportFrame);
      TransportPanel->Show();
      TransportFrame->Show();
    }
  else
    {
      TransportPanel->Reparent(this);
      TransportPanel->SetSize(wxSize(300, 150));
      BottomSizer->Insert(0, TransportPanel, 0, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2);
      BottomSizer->Layout();

      delete TransportFrame;
      TransportFrame = 0x0;
    }
  return (TransportFrame);
}

void					MainWindow::OnFloatSequencer(wxCommandEvent &event)
{
  LOG;
  FloatSequencer();
}

wxFrame					*MainWindow::FloatSequencer()
{
  LOG;
  if (WindowMenu->IsChecked(MainWin_FloatSequencer))
    {
      if (SeqModeView)
	split->Unsplit(SeqPanel);
      else
	BottomSizer->Detach(SeqPanel);

      SequencerFrame = new FloatingFrame(0x0, -1, _("Sequencer"), SeqPanel->GetPosition(),
					 SeqPanel->GetSize(), SeqPanel->GetParent(),
					 ItemFloatingSeq, MainWin_FloatSequencer);
      SeqPanel->Reparent(SequencerFrame);
      SeqPanel->Show();
      SequencerFrame->Show();

      // disable the floating mode for Rack
      ItemFloatingRacks->Enable(false);
    }
  else
    {
      // enable the floating mode for Rack
      ItemFloatingRacks->Enable(true);

      // if optview is already here
      if (split->IsSplit())
	{
	  SeqPanel->Reparent(this);
	  SeqPanel->SetSize(wxSize(470, 150));
	  BottomSizer->Add(SeqPanel, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2);
	  BottomSizer->Layout();
	  SeqPanel->Show();
	}
      else
	{
	  SeqPanel->Reparent(split);
	  split->SplitHorizontally(split->GetWindow1(), SeqPanel);
	}
      delete SequencerFrame;
      SequencerFrame = 0x0;
    }
  return (SequencerFrame);
}

void					MainWindow::OnFloatRack(wxCommandEvent &event)
{
  LOG;
  FloatRack();
}

wxFrame					*MainWindow::FloatRack()
{
  LOG;
  if (WindowMenu->IsChecked(MainWin_FloatRacks))
    {
      if (RackModeView)
	split->Unsplit(RackPanel);
      else
	BottomSizer->Detach(RackPanel);

      RackFrame = new FloatingFrame(0x0, -1, _("Racks"), RackPanel->GetPosition(),
				    RackPanel->GetSize(), RackPanel->GetParent(),
				    ItemFloatingRacks, MainWin_FloatRacks);
      RackPanel->Reparent(RackFrame);
      RackPanel->Show();
      RackFrame->Show();

      // disable the floating mode for Sequencer
      ItemFloatingSeq->Enable(false);
    }
  else
    {
      // enable the floating mode for Sequencer
      ItemFloatingSeq->Enable(true);

      // if optview is already here
      if (split->IsSplit())
	{
	  RackPanel->Reparent(this);
	  RackPanel->SetSize(wxSize(470, 150));
	  BottomSizer->Add(RackPanel, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2);
	  BottomSizer->Layout();
	  RackPanel->Show();
	}
      else
	{
	  RackPanel->Reparent(split);
	  split->SplitHorizontally(RackPanel, split->GetWindow1());
	}
      delete RackFrame;
      RackFrame = 0x0;
    }
  return (RackFrame);
}

void					MainWindow::ShowMediaLibrary(panelState show)
{
  LOG;
  if (show == panelShowInWindow || show == panelHide)
    {
      if (show == panelShowInWindow)
	{
	  wxSize		size;

	  // force size to be large enough
	  size.SetWidth(200);
	  size.SetHeight(400);
	  MediaLibraryFrame = new FloatingFrame(0x0, -1, _("MediaLibrary"),
						MediaLibraryPanel->GetPosition(),
						size, MediaLibraryPanel->GetParent(),
						ItemFloatingMediaLibrary, MainWin_FloatMediaLibrary);
	}

      // detach media library from vertical splitter
      splitVert->Unsplit(MediaLibraryPanel);

      if (show == panelShowInWindow)
	{
	  // show panel
	  MediaLibraryPanel->Reparent(MediaLibraryFrame);
	  MediaLibraryPanel->Show();
	  MediaLibraryPanel->SetVisible();
	  MediaLibraryPanel->SetFloating();

	  // show frame
	  MediaLibraryFrame->Show();

	  // disable Show and Hide menu actions
	  ItemShowMediaLibrary->Enable(false);
	}

      if (show == panelHide)
	MediaLibraryPanel->SetInvisible();
    }
  else if (show == panelHideFromWindow || show == panelShow)
    {
      if (show == panelHideFromWindow)
	{
	  // re-attach media library
	  MediaLibraryPanel->Reparent(splitVert);

	  // delete frame of media library
	  delete MediaLibraryFrame;
	  MediaLibraryFrame = 0x0;
	}

      splitVert->SplitVertically(MediaLibraryPanel, split);
      splitVert->SetSashPosition(200);

      if (show == panelShow)
	{
	  // show panel
	  MediaLibraryPanel->Show();
	  MediaLibraryPanel->SetVisible();
	}

      // enable Show and Hide menu actions
      ItemShowMediaLibrary->Enable();
      MediaLibraryPanel->SetDocked();
    }
}

void					MainWindow::OnFloatMediaLibrary(wxCommandEvent &event)
{
  LOG;
  //  if (MediaLibraryMenu->IsChecked(MainWin_FloatMediaLibrary))
  if (!MediaLibraryPanel->IsFloating())
    {
      if (!WindowMenu->IsChecked(MainWin_MediaLibraryShow))
	ItemShowMediaLibrary->Check(true);
      ShowMediaLibrary(panelShowInWindow);
    }
  else
    ShowMediaLibrary(panelHideFromWindow);
}

// must be called only when medialibrary is docked
void					MainWindow::MediaLibraryShow(wxCommandEvent &event)
{
  LOG;
  //  if (MediaLibraryMenu->IsChecked(MainWin_MediaLibraryShow))
  if (!MediaLibraryPanel->IsVisible())
    ShowMediaLibrary(panelShow);
  else
    ShowMediaLibrary(panelHide);
}

// must be called only when medialibrary is docked
//void					MainWindow::MediaLibraryHide(wxCommandEvent &event)
//{
//  ShowMediaLibrary(panelHide);
//}

void					MainWindow::OnSwitchRackOptViewEvent(wxCommandEvent &event)
{
  LOG;
  SwitchRackOptView();
}

void					MainWindow::OnSwitchSeqOptViewEvent(wxCommandEvent &event)
{
  LOG;
  SwitchSeqOptView();
}

void					MainWindow::SwitchRackOptView()
{
  LOG;
  // if optview is already switched with sequencer, reswitch
  if (!SeqModeView)
    SwitchSeqOptView();

  // if optview is not on top (rackview)
  if (RackModeView)
    {
      BottomSizer->Detach(OptPanel);
      OptPanel->Reparent(split);

      // if Rack is already on top
      if (split->GetWindow1() == RackPanel)
	{
	  split->ReplaceWindow(RackPanel, OptPanel);
	  RackPanel->Reparent(this);
	  RackPanel->SetSize(wxSize(470, 150));
	  BottomSizer->Add(RackPanel, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2);
	  BottomSizer->Layout();
	}
      else if (split->GetWindow1() == SeqPanel) // if sequencer is alone
	split->SplitHorizontally(OptPanel, SeqPanel);

    }
  else // if optview is on top (rackview)
    {
      // if Rack is already on bottom
      if (BottomSizer->Detach(RackPanel))
	{
	  RackPanel->Reparent(split);
	  split->ReplaceWindow(OptPanel, RackPanel);
	}
      else // else we leave sequencer alone
	split->Unsplit(OptPanel);

      OptPanel->Hide();
      OptPanel->SetSize(wxSize(470, 150));
      OptPanel->Reparent(this);
      BottomSizer->Add(OptPanel, 1, wxEXPAND | wxALL, 2);
      BottomSizer->Layout();
      OptPanel->Show();

    }
  RackModeView = !RackModeView;
}

void					MainWindow::SwitchSeqOptView()
{
  LOG;
  // if optview is already switched with rack, reswitch
  if (!RackModeView)
    SwitchRackOptView();

  // if optview is not on middle (seqview)
  if (SeqModeView)
    {
      OptPanel->Hide();
      BottomSizer->Detach(OptPanel);
      OptPanel->Reparent(split);
      // if sequencer is not in a floating frame
      if (!SequencerFrame)
	{
	  SeqPanel->Hide();
	  SeqPanel->Reparent(this);
	  split->ReplaceWindow(SeqPanel, OptPanel);

	  SeqPanel->SetSize(wxSize(470, 150));
	  BottomSizer->Add(SeqPanel, 1, wxEXPAND | wxALL, 2);
	  BottomSizer->Layout();
	  SeqPanel->Show();
	}
      else
	{
	  // if rack is not in a floating frame
	  if (!RackFrame)
	    split->SplitHorizontally(RackPanel, OptPanel);
	}
      OptPanel->Show();
    }
  else // if optview is on middle (seqview)
    {
      // if sequencer is on bottom
      if (!SequencerFrame)
	{
	  SeqPanel->Hide();
	  BottomSizer->Detach(SeqPanel);
	  SeqPanel->Reparent(split);
	  split->ReplaceWindow(OptPanel, SeqPanel);
	  SeqPanel->Show();
	}
      else
	split->Unsplit(OptPanel);

      OptPanel->Hide();
      OptPanel->SetSize(wxSize(470, 150));
      OptPanel->Reparent(this);

      BottomSizer->Add(OptPanel, 1, wxEXPAND | wxALL, 2);
      BottomSizer->Layout();
      OptPanel->Show();
    }
  SeqModeView = !SeqModeView;
}

void					MainWindow::OnSettings(wxCommandEvent &event)
{
  LOG;
  vector<Track *>::iterator		i;

  // settings window can't be on top of fullscreen application
  if (IsFullScreen())
    {
      // simulate same action
      ItemFullscreenToggle->Check(false);
      OnFullScreen(event);
    }
  if (SettingsWin->ShowModal() == wxID_OK &&
      (SettingsWin->AudioLoaded || SettingsWin->MidiLoaded))
    {
      if (InitAudio(true) < 0)
	{
	  // avoid to flood user, he already knows he has a deprecated config!
	  if (!WiredSettings->ConfIsDeprecated())
	    AlertDialog(_("audio engine"),
			_("Could not open audio device : check that the device is not busy (used by another application) and that your audio settings are correct."));
	}
    }
}

void					MainWindow::AlertDialog(const wxString& from, const wxString& msg)
{
  LOG;
  wxMessageDialog			mdialog(MainWin, msg, from, wxICON_INFORMATION, wxDefaultPosition);

  mdialog.ShowModal();
}

void					MainWindow::OnOpenVideo(wxCommandEvent &event)
{
  LOG;
  WiredVideoObject->OpenFile();
  if (WiredVideoObject->asFile)
    {
      VideoMenu->Enable(MainWin_OpenVideo, false);
      VideoMenu->Enable(MainWin_CloseVideo, true);
    }
}

void					MainWindow::OnCloseVideo(wxCommandEvent &event)
{
  LOG;
  WiredVideoObject->CloseFile();
  VideoMenu->Enable(MainWin_OpenVideo, true);
  VideoMenu->Enable(MainWin_CloseVideo, false);
}

void					MainWindow::OnSeekVideo(wxCommandEvent &event)
{
  LOG;
  //  WiredVideoObject->SetSeek(VideoMenu->IsChecked(MainWin_SeekVideo));
}

void					MainWindow::SetSelectedSolo(wxCommandEvent &event)
{
  LOG;
  SeqPanel->SetSelectedSolo();
}

void					MainWindow::OnDeleteTrack(wxCommandEvent &event)
{
  LOG;
  SeqPanel->DeleteSelectedTrack();
  /* Needs path in AudioPattern */
  //cActionManager::Global().AddImportWaveAction(selfile, true, false);
}

void					MainWindow::OnUndo(wxCommandEvent &event)
{
  LOG;
  wxMenuItemList					listItems;
  wxMenuItemList::const_iterator	iter;

  listItems = UndoMenu->GetMenuItems();
  for (iter = listItems.begin(); iter != listItems.end(); iter++)
    {
      cActionManager::Global().Undo();
      cout << "Undo" << endl;
      if ((*iter)->GetId() == event.GetId())
	break;
    }
  CreateUndoRedoMenus(EditMenu);
}

void					MainWindow::OnRedo(wxCommandEvent &event)
{
  LOG;
  wxMenuItemList					listItems;
  wxMenuItemList::const_iterator	iter;

  listItems = RedoMenu->GetMenuItems();
  for (iter = listItems.begin(); iter != listItems.end(); iter++)
    {
      cActionManager::Global().Redo();
      if ((*iter)->GetId() == event.GetId())
	break;
    }
  CreateUndoRedoMenus(EditMenu);
}

void					MainWindow::removeAllMenuItems(wxMenu *menu)
{
  LOG;
  wxMenuItemList			menuItemList;
  wxMenuItemList::const_iterator	itermenuItems;

  if (menu)
    {
      menuItemList = menu->GetMenuItems();
      for (itermenuItems = menuItemList.begin(); itermenuItems != menuItemList.end(); itermenuItems++)
	menu->Delete(*itermenuItems);
    }
}

void					MainWindow::CreateUndoRedoMenus(wxMenu *callingMenu)
{
  LOG;
  std::list<t_menuInfo*>					historyList;
  std::list<t_menuInfo*>::const_iterator	iter;
  wxMenu									*undoMenu;
  wxMenu									*redoMenu;
  int										separatorIndex;
  int										count;

  undoMenu = callingMenu->FindItemByPosition(INDEX_MENUITEM_UNDO)->GetSubMenu();
  redoMenu = callingMenu->FindItemByPosition(INDEX_MENUITEM_REDO)->GetSubMenu();
  if (!undoMenu || !redoMenu)
    return;

  removeAllMenuItems(undoMenu);
  removeAllMenuItems(redoMenu);
  historyList = cActionManager::Global().getListActions(&separatorIndex);
  for (count = 0, iter = historyList.begin(); iter != historyList.end(); iter++, separatorIndex--, count++)
    {
      wxMenuItem	*insertedMenuItem;
      wxString		tmpString;

      if (separatorIndex > 0)
	{
	  tmpString = wxString((*iter)->label);
	  if (count == 0)
	    tmpString += wxT("\tCtrl+Z");
	  insertedMenuItem = undoMenu->Append((*iter)->id, tmpString);
	  Connect((*iter)->id, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)(wxEventFunction)
		  (wxCommandEventFunction)&MainWindow::OnUndo);
	}
      else
	{
	  tmpString = wxString((*iter)->label);
	  if (separatorIndex == 0)
	    tmpString += wxT("\tCtrl+Shift+Z");
	  insertedMenuItem = redoMenu->Append((*iter)->id, tmpString);
	  Connect((*iter)->id, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)(wxEventFunction)
		  (wxCommandEventFunction)&MainWindow::OnRedo);
	}
    }
  callingMenu->Enable(MainWin_Undo, undoMenu->GetMenuItemCount() > 0);
  callingMenu->Enable(MainWin_Redo, redoMenu->GetMenuItemCount() > 0);
}

void					MainWindow::OnCut(wxCommandEvent &event)
{
  LOG;
  SeqPanel->OnCut(event);
}

void					MainWindow::OnCopy(wxCommandEvent &event)
{
  LOG;
  SeqPanel->OnCopy(event);
}

void					MainWindow::OnPaste(wxCommandEvent &event)
{
  LOG;
  SeqPanel->OnPaste(event);
}

void					MainWindow::OnDelete(wxCommandEvent &event)
{
  LOG;
  SeqPanel->OnDeleteClick(event);
}

void					MainWindow::OnSelectAll(wxCommandEvent &event)
{
  LOG;
  SeqPanel->OnSelectAll(event);
}

void					MainWindow::OnFullScreen(wxCommandEvent &event)
{
  LOG;
  WindowSize = MainWin->GetSize();
  WindowPos = MainWin->GetPosition();
  ShowFullScreen(!IsFullScreen(), wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION );
}

void					MainWindow::OnAbout(wxCommandEvent &event)
{
  LOG;
  wxBitmap aboutbtm;
  if (aboutbtm.LoadFile(wxString(WiredSettings->DataDir +
				 wxString(wxT("ihm/splash/about.png"))),
			wxBITMAP_TYPE_PNG))
    {
      wxSplashScreen* splash = new wxSplashScreen(aboutbtm,
						  wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
						  6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
						  wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    }
  //wxYield();
}

void					MainWindow::OnSpaceKey()
{
  LOG;
  wxCommandEvent			e(-1, -1);

  if (Seq->Playing)
    {
      TransportPanel->OnStop(e);
    }
  else
    {
      TransportPanel->PlayBtn->SetOn();
      TransportPanel->OnPlay(e);
    }
}

void					MainWindow::OnTimer(wxTimerEvent &event)
{
//  LOG;
  wxCommandEvent			commandEvt;
  CursorEvent				cursorEvt;
  list<Pattern *>::iterator		patternIt;
  list<MidiPattern *>::iterator		midiPatternIt;
  list<Plugin *>::iterator		pluginIt;
  list<Track *>::iterator		trackIt;
  wxMutexLocker				locker(SeqMutex);

  if (MixerPanel != NULL)
      MixerPanel->OnMasterChange(commandEvt);
  if (Seq->Playing)
    {
      SeqPanel->OnSetPosition(cursorEvt);

      if (Seq->Recording)
	{
	  for (patternIt = Seq->PatternsToResize.begin(); patternIt != Seq->PatternsToResize.end(); patternIt++)
	    {
	      commandEvt.SetEventObject((wxObject *)*patternIt);
	      SeqPanel->OnResizePattern(commandEvt);
	    }
	  Seq->PatternsToResize.clear();
	  for (midiPatternIt = Seq->PatternsToRefresh.begin(); midiPatternIt != Seq->PatternsToRefresh.end(); midiPatternIt++)
	    {
	      commandEvt.SetEventObject((wxObject *)*midiPatternIt);
	      SeqPanel->OnDrawMidi(commandEvt);
	    }
	  Seq->PatternsToRefresh.clear();
	}
    }

  UpdatePlugins.clear();

  for (trackIt = Seq->TracksToRefresh.begin(); trackIt != Seq->TracksToRefresh.end(); trackIt++)
    (*trackIt)->GetTrackOpt()->SetVuValue();
  Seq->TracksToRefresh.clear();
}

void					MainWindow::AddUpdatePlugin(Plugin *p)
{
  LOG;
  list<Plugin *>::iterator		i;

  for (i = UpdatePlugins.begin(); i != UpdatePlugins.end(); i++)
    if (*i == p)
      return;
  UpdatePlugins.push_back(p);
}
/*
void					MainWindow::OnFileLoaderStart(wxCommandEvent &event)
{
  FileLoader				*f = (FileLoader *)event.GetEventObject();
  wxString                              selfile;

  selfile = f->GetSelectedFile();
  FileConverter->ConvertSamplerateNoGraph(selfile);
  if (wxFileExists(saveCenter->getAudioDir() + selfile.AfterLast('/')))
    Seq->PlayFile(saveCenter->getAudioDir() + selfile.AfterLast('/'), f->IsAkai());
  else
     Seq->PlayFile(selfile, f->IsAkai());
}

void					MainWindow::OnFileLoaderStop(wxCommandEvent &event)
{
  Seq->StopFile();
}
*/
void					MainWindow::OnIntegratedHelp(wxCommandEvent &event)
{
  LOG;
  OptPanel->ShowHelp();
}

void                  MainWindow::OnShowDebug(wxCommandEvent &event)
{
  LOG;
  if(WindowMenu->IsChecked(MainWin_ShowLog))
    {
      LogWin->Show(true);
    }
  else
    {
      LogWin->Show(false);
    }
}

void					MainWindow::OnKillTimer(wxTimerEvent &WXUNUSED(event))
{
  LOG;
  cout << "[MAINWIN] Killing Threads" << endl;
  exit (0);
}

void					MainWindow::OnIdle(wxIdleEvent &WXUNUSED(event))
{
//  LOG;
#if wxUSE_STATUSBAR
  wxLogNull NoLog;
  if (SeqTimer)
    {
      wxString			LeftSpace(_("Left space on drive : "));
      wxLongLong		Total, Free;

/*    Does not work everywhere : kernel version problem
 *    wxString			LeftMemory(_("Free memory : "));
 *    wxLongLong Size = wxGetFreeMemory();
 *    if (Size > 0)
 *      LeftMemory += Size.ToString();
 *    else
 *      LeftMemory += _("Unknown");
*/
      wxGetDiskSpace(saveCenter->getAudioDir(), &Total, &Free);

//      LeftSpace += FileLoader::FormatSize((off_t) Free.GetValue()) + wxT("/") + FileLoader::FormatSize((off_t)Total.GetValue());
      SetStatusText(LeftSpace, 0);
      //SetStatusText(LeftMemory, 1);
    }
#endif
}

void		MainWindow::OnKey(wxKeyEvent& event)
{
  LOG;
  if (event.GetKeyCode() == WXK_SPACE)
    OnSpaceKey();
  else if (event.GetKeyCode() == WXK_TAB)
    {
      if (event.ShiftDown())
	SwitchSeqOptView();
      else
	SwitchRackOptView();
    }
  else
    event.Skip();
}

void		MainWindow::Save()
{
  LOG;
  SaveElement	*saveElem;
  wxSize	size;
  wxPoint	pos;

  saveElem = new SaveElement(wxT("MainWindow"), wxT(""));
  size = MainWin->GetSize();
  pos = MainWin->GetPosition();
  if (IsFullScreen())
    {
      saveElem->addAttribute(wxT("Width"), WindowSize.GetWidth());
      saveElem->addAttribute(wxT("Height"), WindowSize.GetHeight());
      saveElem->addAttribute(wxT("Pos_x"), WindowPos.x);
      saveElem->addAttribute(wxT("Pos_y"), WindowPos.y);
    }
  else
    {
      saveElem->addAttribute(wxT("Width"), size.GetWidth());
      saveElem->addAttribute(wxT("Height"), size.GetHeight());
      saveElem->addAttribute(wxT("Pos_x"), pos.x);
      saveElem->addAttribute(wxT("Pos_y"), pos.y);
    }
  saveDocData(saveElem);

  saveElem = new SaveElement(wxT("SwitchView"), wxT(""));
  saveElem->addAttribute(wxT("RackModeView"), RackModeView);
  saveElem->addAttribute(wxT("SeqModeView"), SeqModeView);
  saveDocData(saveElem);

  saveElem = new SaveElement(wxT("FullScreen"), IsFullScreen());
  saveDocData(saveElem);

  size = MediaLibraryPanel->GetSize();
  if (MediaLibraryFrame)
    pos = MediaLibraryFrame->GetPosition();
  saveElem = new SaveElement(wxT("MediaLibrary"), wxT(""));
  saveElem->addAttribute(wxT("Show"), MediaLibraryPanel->IsVisible());
  saveElem->addAttribute(wxT("floating"), MediaLibraryPanel->IsFloating());
  saveElem->addAttribute(wxT("Width"), size.GetWidth());
  saveElem->addAttribute(wxT("Height"), size.GetHeight());
  saveElem->addAttribute(wxT("Pos_x"), pos.x);
  saveElem->addAttribute(wxT("Pos_y"), pos.y);
  saveElem->addAttribute(wxT("SashPos"), splitVert->GetSashPosition());
  saveDocData(saveElem);

  size = TransportPanel->GetSize();
  if (TransportFrame)
    pos = TransportFrame->GetPosition();
  saveElem = new SaveElement(wxT("Transport"), wxT(""));
  saveElem->addAttribute(wxT("Floating"), (WindowMenu->IsChecked(MainWin_FloatTransport)));
  saveElem->addAttribute(wxT("Width"), size.GetWidth());
  saveElem->addAttribute(wxT("Height"), size.GetHeight());
  saveElem->addAttribute(wxT("Pos_x"), pos.x);
  saveElem->addAttribute(wxT("Pos_y"), pos.y);
  saveDocData(saveElem);

  size = SeqPanel->GetSize();
  if (SequencerFrame)
    pos = SequencerFrame->GetPosition();
  saveElem = new SaveElement(wxT("Sequencer"), wxT(""));
  saveElem->addAttribute(wxT("Floating"), (WindowMenu->IsChecked(MainWin_FloatSequencer)));
  saveElem->addAttribute(wxT("Width"), size.GetWidth());
  saveElem->addAttribute(wxT("Height"), size.GetHeight());
  saveElem->addAttribute(wxT("Pos_x"), pos.x);
  saveElem->addAttribute(wxT("Pos_y"), pos.y);
  saveElem->addAttribute(wxT("SashPos"), split->GetSashPosition());
  saveDocData(saveElem);

  size = RackPanel->GetSize();
  if (RackFrame)
    pos = RackFrame->GetPosition();
  saveElem = new SaveElement(wxT("Rack"), wxT(""));
  saveElem->addAttribute(wxT("Floating"), (WindowMenu->IsChecked(MainWin_FloatRacks)));
  saveElem->addAttribute(wxT("Width"), size.GetWidth());
  saveElem->addAttribute(wxT("Height"), size.GetHeight());
  saveElem->addAttribute(wxT("Pos_x"), pos.x);
  saveElem->addAttribute(wxT("Pos_y"), pos.y);
  saveElem->addAttribute(wxT("SashPos"), split->GetSashPosition());
  saveDocData(saveElem);
}

typedef wxFrame *(MainWindow::*floatfunc)(void);

void		MainWindow::SwitchDockedFloat(bool isCurrentlyFloating, int mustBeFloating,
					      wxCommandEvent evt, wxPoint pos, wxSize size,
					      int checkBox, wxFrame *frame, floatfunc function)
{
  LOG;
  if (mustBeFloating)
    {
      if (!isCurrentlyFloating)
	{
	  WindowMenu->Check(checkBox, true);
	  frame = (*this.*function)();
	}
      frame->SetPosition(pos);
      frame->SetSize(size);
    }
  else
    {
      if (isCurrentlyFloating)
	{
	  WindowMenu->Check(checkBox, false);
	  this->ProcessEvent(evt);
	}
    }
}

void		MainWindow::Load(SaveElementArray data)
{
  LOG;
   int			i;
   wxSize		size;
   wxPoint		pos;
   bool			isFloating;

   for (i = 0; i < data.GetCount(); i++)
     {
       if (data[i]->getKey() == wxT("MainWindow"))
	 {
	   pos.x = data[i]->getAttributeInt(wxT("Pos_x"));
	   pos.y = data[i]->getAttributeInt(wxT("Pos_y"));
	   MainWin->SetSize(data[i]->getAttributeInt(wxT("Width")),
			    data[i]->getAttributeInt(wxT("Height")));
           MainWin->SetPosition(pos);
         }

       else if (data[i]->getKey() == wxT("SwitchView"))
	 {
	   if (!(data[i]->getAttributeInt(wxT("RackModeView")) == RackModeView))
	     MainWin->SwitchRackOptView();
	   if (!(data[i]->getAttributeInt(wxT("SeqModeView")) == SeqModeView))
	     MainWin->SwitchSeqOptView();
	 }

       else if (data[i]->getKey() == wxT("FullScreen"))
	 {
	   if (data[i]->getValueInt() == 1)
	     ShowFullScreen(!IsFullScreen(), wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION );
	 }

       else if (data[i]->getKey() == wxT("MediaLibrary"))
	 {
	   isFloating = MediaLibraryPanel->IsFloating();
           if (data[i]->getAttributeInt(wxT("Show")) == 1)
	     {
	       if (!MediaLibraryPanel->IsVisible())
		 {
		   ShowMediaLibrary(panelShow);
		   ItemShowMediaLibrary->Check(true);
		 }
	       splitVert->SetSashPosition(data[i]->getAttributeInt(wxT("SashPos")));
	     }
	   else
	     {
	       if (MediaLibraryPanel->IsVisible())
		 {
		   ShowMediaLibrary(panelHide);
		   ItemShowMediaLibrary->Check(false);
		 }
	     }
	   if (data[i]->getAttributeInt(wxT("floating")) == 1)
	     {
	       if (!isFloating)
		 {
		   ShowMediaLibrary(panelShowInWindow);
		   ItemFloatingMediaLibrary->Check(true);
		 }
	       pos.x = data[i]->getAttributeInt(wxT("Pos_x"));
	       pos.y = data[i]->getAttributeInt(wxT("Pos_y"));
	       MediaLibraryFrame->SetPosition(pos);
	       MediaLibraryFrame->SetSize(data[i]->getAttributeInt(wxT("Width")),
					  data[i]->getAttributeInt(wxT("Height")));
	     }
	   else
	     {
	       if (isFloating)
		 {
		   ShowMediaLibrary(panelHideFromWindow);
		   ItemFloatingMediaLibrary->Check(false);
		   splitVert->SetSashPosition(data[i]->getAttributeInt(wxT("SashPos")));
		 }
	     }
	 }

       else if (data[i]->getKey() == wxT("Transport"))
	 {
	   wxCommandEvent evtFloatTrans(wxEVT_COMMAND_MENU_SELECTED, MainWin_FloatTransport);
	   SwitchDockedFloat(WindowMenu->IsChecked(MainWin_FloatTransport), data[i]->getAttributeInt(wxT("Floating")),
			     evtFloatTrans,
			     wxPoint(data[i]->getAttributeInt(wxT("Pos_x")), data[i]->getAttributeInt(wxT("Pos_y"))),
			     wxSize(data[i]->getAttributeInt(wxT("Width")), data[i]->getAttributeInt(wxT("Height"))),
			     MainWin_FloatTransport, (wxFrame *) TransportFrame, &MainWindow::FloatTransport);
	 }

       else if (data[i]->getKey() == wxT("Sequencer"))
	 {
	   wxCommandEvent evtFloatSeq(wxEVT_COMMAND_MENU_SELECTED, MainWin_FloatSequencer);
	   SwitchDockedFloat(WindowMenu->IsChecked(MainWin_FloatSequencer), data[i]->getAttributeInt(wxT("Floating")),
                             evtFloatSeq,
                             wxPoint(data[i]->getAttributeInt(wxT("Pos_x")), data[i]->getAttributeInt(wxT("Pos_y"))),
                             wxSize(data[i]->getAttributeInt(wxT("Width")), data[i]->getAttributeInt(wxT("Height"))),
                             MainWin_FloatSequencer, (wxFrame *) SequencerFrame, &MainWindow::FloatSequencer);
	   if (!data[i]->getAttributeInt(wxT("Floating")) == 1)
	     split->SetSashPosition(data[i]->getAttributeInt(wxT("SashPos")));
	 }

       else if (data[i]->getKey() == wxT("Rack"))
	 {
	   wxCommandEvent evtFloatRack(wxEVT_COMMAND_MENU_SELECTED, MainWin_FloatRacks);
	   SwitchDockedFloat(WindowMenu->IsChecked(MainWin_FloatRacks), data[i]->getAttributeInt(wxT("Floating")),
                             evtFloatRack,
                             wxPoint(data[i]->getAttributeInt(wxT("Pos_x")), data[i]->getAttributeInt(wxT("Pos_y"))),
                             wxSize(data[i]->getAttributeInt(wxT("Width")), data[i]->getAttributeInt(wxT("Height"))),
                             MainWin_FloatRacks, (wxFrame *) RackFrame, &MainWindow::FloatRack);
	   if (!data[i]->getAttributeInt(wxT("Floating")) == 1)
	     split->SetSashPosition(data[i]->getAttributeInt(wxT("SashPos")));
	 }
     }
}

void MainWindow::OnLoadML(wxCommandEvent &WXUNUSED(event))
{
  LOG;
    wxFileDialog dlg(this, _("Load Media Library"), wxT(""), wxT(""), _("Media Library file (*.xml)|*.xml"));
    if (dlg.ShowModal() == wxID_OK)
        MediaLibraryPanel->MLTreeView->LoadPatch(dlg.GetPath());
}

void MainWindow::OnSaveML(wxCommandEvent &WXUNUSED(event))
{
  LOG;
    wxFileDialog dlg(this, _("Save Media Library"), wxT(""), wxT(""), _("Media Library file (*.xml)|*.xml"), wxSAVE);
    if (dlg.ShowModal() == wxID_OK)
        MediaLibraryPanel->MLTreeView->OnSave(dlg.GetPath());
}

void		MainWindow::OpenWizard()
{
  LOG;


  Wizard	wiz;

  wiz.ShowModal();
  if (wxFileName::DirExists(wiz.GetDir()))
	WiredStartSession(wiz.GetDir());
  else
	WiredStartSession(ChooseSessionDir());
}

wxString		MainWindow::ChooseSessionDir()
{
  LOG;

  wxDirDialog	dirDialog(NULL, _("Select a project folder"), wxGetCwd());

  while(dirDialog.ShowModal() != wxID_OK)
    AlertDialog(_("Warning"),
			 _("You have to select a project folder."));

  return (dirDialog.GetPath());
}

void		MainWindow::WiredStartSession(wxString sessionDir)
{
  wxFileName	path;

  WiredSettings->AddDirToRecent(sessionDir);
  path.AssignDir(sessionDir);
  path.MakeAbsolute();
  wxFileName::SetCwd(path.GetPath());
  saveCenter->setProjectPath(path);

  if (saveCenter->IsProject(path))
     saveCenter->LoadProject();
}

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxSetCursorPos, 313131)
  END_DECLARE_EVENT_TYPES()

  BEGIN_EVENT_TABLE(MainWindow, wxFrame)
  // key binding
  EVT_KEY_UP(MainWindow::OnKey) // DOWN is bugged

  // menu
  EVT_MENU(MainWin_Quit, MainWindow::OnQuit)
  EVT_MENU(MainWin_New, MainWindow::OnNew)
  EVT_MENU(MainWin_Save, MainWindow::OnSave)
  EVT_MENU(MainWin_SaveAs, MainWindow::OnSaveAs)
  //EVT_MENU(MainWin_ImportWave, MainWindow::OnImportWave)
  EVT_MENU(MainWin_ImportMIDI, MainWindow::OnImportMIDI)
  EVT_MENU(MainWin_ImportAKAI, MainWindow::OnImportAKAI)
  EVT_MENU(MainWin_ExportWave, MainWindow::OnExportWave)
  EVT_MENU(MainWin_Settings, MainWindow::OnSettings)
  EVT_MENU(MainWin_Open, MainWindow::OnOpen)
  EVT_MENU(MainWin_DeleteRack, MainWindow::OnDeleteRack)
  EVT_MENU(MainWin_AddTrackAudio, MainWindow::OnAddTrackAudio)
  EVT_MENU(MainWin_AddTrackMidi, MainWindow::OnAddTrackMidi)
  // Added by Julien Eres
  EVT_MENU(MainWin_AddTrackAutomation, MainWindow::OnAddTrackAutomation)
  EVT_MENU(MainWin_DeleteTrack, MainWindow::OnDeleteTrack)
  EVT_MENU(MainWin_SoloTrack, MainWindow::SetSelectedSolo)
  EVT_MENU(MainWin_FloatTransport, MainWindow::OnFloatTransport)
  EVT_MENU(MainWin_FloatSequencer, MainWindow::OnFloatSequencer)
  EVT_MENU(MainWin_FloatRacks, MainWindow::OnFloatRack)
  EVT_MENU(MainWin_FloatMediaLibrary, MainWindow::OnFloatMediaLibrary)
  EVT_MENU(MainWin_MediaLibraryShow, MainWindow::MediaLibraryShow)
#ifndef FEATURE_DISABLE_SAVE_ML
  EVT_MENU(MainWin_SaveML, MainWindow::OnSaveML)
  EVT_MENU(MainWin_LoadML, MainWindow::OnLoadML)
#endif
  EVT_MENU(MainWin_Undo, MainWindow::OnUndo)
  EVT_MENU(MainWin_Redo, MainWindow::OnRedo)
  //EVT_MENU(MainWin_History, MainWindow::OnHistory)
  EVT_MENU(MainWin_Copy, MainWindow::OnCopy)
  EVT_MENU(MainWin_Cut, MainWindow::OnCut)
  EVT_MENU(MainWin_Paste, MainWindow::OnPaste)
  EVT_MENU(MainWin_Delete, MainWindow::OnDelete)
  EVT_MENU(MainWin_SelectAll, MainWindow::OnSelectAll)
  EVT_MENU(MainWin_SwitchRack,  MainWindow::OnSwitchRackOptViewEvent)
  EVT_MENU(MainWin_SwitchSeq,  MainWindow::OnSwitchSeqOptViewEvent)
  EVT_MENU(MainWin_FullScreen,  MainWindow::OnFullScreen)
  EVT_MENU(MainWin_About, MainWindow::OnAbout)
  EVT_MENU(MainWin_IntHelp, MainWindow::OnIntegratedHelp)
  EVT_MENU(MainWin_ShowLog, MainWindow::OnShowDebug)

  // event
  EVT_CLOSE(MainWindow::OnClose)
  EVT_TIMER(MainWin_SeqTimer, MainWindow::OnTimer)
  EVT_TIMER(MainWin_KillTimer, MainWindow::OnKillTimer)
/*
  // button
  EVT_BUTTON(FileLoader_Start, MainWindow::OnFileLoaderStart)
  EVT_BUTTON(FileLoader_Stop, MainWindow::OnFileLoaderStop)
*/
  //  EVT_MENU(MainWin_OpenVideo, MainWindow::OnOpenVideo)
  //  EVT_MENU(MainWin_CloseVideo, MainWindow::OnCloseVideo)
  //  EVT_MENU(MainWin_SeekVideo, MainWindow::OnSeekVideo)
  //EVT_TEXT_MAXLEN(101010, MainWindow::OnSetPosition)
  //EVT_PLAYPOSITION(313131, MainWindow::OnSetPosition)
END_EVENT_TABLE()
