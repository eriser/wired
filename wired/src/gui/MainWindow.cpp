// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "MainWindow.h"

#include <wx/splitter.h>
#include <wx/progdlg.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <algorithm>
#include "SequencerGui.h"
#include "HostCallback.h"
#include "FileLoader.h"
#include "WaveFile.h"
#include "SettingWindow.h"
#include "AudioPattern.h"
#include "AudioCenter.h"
#include "EditMidi.h"
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
#include "../engine/AudioEngine.h"
#include "../engine/Settings.h"
#include "../engine/EngineError.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/Track.h"
#include "../mixer/Mixer.h"
#include "../engine/WiredSession.h"
#include "../midi/MidiThread.h"
#include "../plugins/PluginLoader.h"
#include "../xml/WiredSessionXml.h"
#include "../dssi/WiredExternalPluginMgr.h"
#include "FileConversion.h"
#include "config.h"
#include "Threads.h"
#include "MediaLibrary.h"
#include "MLTree.h"

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
WiredSession		*CurrentSession = NULL;
WiredSessionXml		*CurrentXmlSession = NULL;
WiredExternalPluginMgr	*LoadedExternalPlugins = NULL;
MediaLibrary		*MediaLibraryPanel = NULL;
FileConversion		*FileConverter = NULL;
SettingWindow		*SettingsWin = NULL;

wxMutex			AudioMutex;
wxCondition		*SeqStopped = NULL;

MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size)
  : wxFrame((wxFrame *) NULL, -1, title, pos, size, 
	    wxDEFAULT_FRAME_STYLE | wxWS_EX_PROCESS_IDLE)
{	
  SeqTimer = NULL;
  InitLocale();
  
#if wxUSE_STATUSBAR
  CreateStatusBar(2);
#endif
  WiredSettings = new Settings();
  CurrentSession = new WiredSession(wxString(wxT(""), *wxConvCurrent));
  CurrentXmlSession = new WiredSessionXml(wxString(wxT(""), *wxConvCurrent));
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
  // from GetDevices
  catch (Error::NoDevice)
    {
      cout << "[MAINWIN] No Device :\nPlease check you have a soundcard and Alsa installed" 
	   << endl;
      AlertDialog(_("Critical error"), _("You have no soundcard, Wired will exit."));
      exit(1);
    }
  
  // Mixer must be declared after AudioEngine 
  Mix = new Mixer();
  Seq = new Sequencer();
  MidiEngine = new MidiThread();
  MidiEngine->OpenDefaultDevices();
  SettingsWin = new SettingWindow();

  /* Creation Menu */

  FileConverter = NULL;
  
  TransportFrame = 0x0;
  OptFrame = 0x0;
  SequencerFrame = 0x0;
  RackFrame = 0x0; 

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
  MediaLibraryMenu = new wxMenu;
  
  FileMenu->Append(MainWin_New, _("&New\tCtrl-N"));
  FileMenu->Append(MainWin_Open, _("&Open...\tCtrl-O"));
  FileMenu->Append(MainWin_Save, _("&Save\tCtrl-S"));
  FileMenu->Append(MainWin_SaveAs, _("Save &as...\tF12"));
  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_ImportWave, _("&Import Wave file..."));
  FileMenu->Append(MainWin_ImportMIDI, _("Import &MIDI file..."));
  FileMenu->Append(MainWin_ImportAKAI, _("Import A&KAI sample..."));
  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_ExportWave, _("E&xport Wave file..."));
  FileMenu->Append(MainWin_ExportMIDI, _("Ex&port MIDI file..."));

  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_Quit, _("&Quit\tCtrl-Q"));


  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Cut, _("C&ut\tCtrl+X"));
  EditMenu->Append(MainWin_Copy, _("&Copy\tCtrl+C"));
  EditMenu->Append(MainWin_Paste, _("&Paste\tCtrl+V"));
  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Delete, _("&Delete\tDel"));
  EditMenu->Append(MainWin_SelectAll, _("&Select all\tCtrl+A"));
  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Settings, _("&Settings..."));
  
  SequencerMenu->Append(MainWin_AddTrackAudio, _("&Add Audio Track"));
  SequencerMenu->Append(MainWin_AddTrackMidi, _("&Add MIDI Track"));
  SequencerMenu->Append(MainWin_DeleteTrack, _("&Delete Track"));
  SequencerMenu->AppendSeparator();
  SequencerMenu->Append(MainWin_ChangeAudioDir, _("&Change Audio directory..."));
  
  RacksMenu->Append(MainWin_DeleteRack, _("D&elete Rack"));

  HelpMenu->Append(MainWin_IntHelp, _("&Show Integrated Help"));
  HelpMenu->Append(MainWin_About, _("&About..."));
  
  MediaLibraryMenu->Append(MainWin_MediaLibraryShow, _("&Show\tCtrl-M"));
  MediaLibraryMenu->Append(MainWin_MediaLibraryHide, _("&Hide\tCtrl-M"));
  ItemFloatingMediaLibrary = MediaLibraryMenu->AppendCheckItem(MainWin_FloatMediaLibrary, _("Floating"));
  
  WindowMenu->Append(MainWin_SwitchRack, _("Switch &Rack/Optional view\tTAB"));
  WindowMenu->Append(MainWin_SwitchSeq, _("Switch &Sequencer/Optional view\tCtrl+TAB"));
  WindowMenu->AppendSeparator();
  ItemFloatingTrans = WindowMenu->AppendCheckItem(MainWin_FloatTransport, _("Floating Transport"));
  WindowMenu->AppendSeparator();
  ItemFloatingSeq = WindowMenu->AppendCheckItem(MainWin_FloatSequencer,_("Floating Sequencer"));
  ItemFloatingRacks = WindowMenu->AppendCheckItem(MainWin_FloatRacks, _("Floating Racks"));
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
  MenuBar->Append(VideoMenu, _("&Video"));
  MenuBar->Append(MediaLibraryMenu, _("&MediaLibrary"));
  MenuBar->Append(WindowMenu, _("&Window"));
  MenuBar->Append(HelpMenu, _("&Help"));
    
  SetMenuBar(MenuBar);

  splitVert = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(0, 454));
  split = new wxSplitterWindow(splitVert);  
  split->SetMinimumPaneSize(1);
  splitVert->SetMinimumPaneSize(1);

  /* Creation Panel */
  RackPanel = new Rack(split, -1, wxPoint(0, 0), wxSize(800, 250));
  SeqPanel = new SequencerGui(split, wxPoint(0, 0), wxSize(800, 200), this);
  OptPanel = new OptionPanel(this, wxPoint(306, 452), wxSize(470, 120), wxSIMPLE_BORDER);
  TransportPanel = new Transport(this, wxPoint(0, 452), wxSize(300, 150), wxNO_BORDER);

  MediaLibraryPanel = new MediaLibrary(splitVert, wxPoint(0, 0), wxSize(0, 400), wxSIMPLE_BORDER);
  MediaLibraryPanel->SetSizeHints(2, 0);

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

  splitVert->SetSashPosition(200);
  RackPanel->SetBackgroundColour(*wxBLACK);
  SeqPanel->SetBackgroundColour(*wxWHITE);
  OptPanel->SetBackgroundColour(*wxLIGHT_GREY);
  MediaLibraryPanel->SetBackgroundColour(*wxWHITE);
  
  MediaLibraryPanel->Show();
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

  // Taille minimum de la fenetre
  SetSizeHints(400, 300);

  Connect(MainWin_ImportWave, wxEVT_COMMAND_MENU_SELECTED, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&MainWindow::OnImportWave);

#if wxUSE_STATUSBAR
  Connect(wxID_ANY, wxEVT_IDLE, (wxObjectEventFunction) &MainWindow::OnIdle);
#endif
}

// basicaly launch actions which are non-graphical related
int			MainWindow::Init()
{
  // start midi thread
  if (MidiEngine->Create() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Create MidiEngine thread failed !" << endl;
  if (MidiEngine->Run() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Run MidiEngine thread failed !" << endl;  
  else
    wxGetApp().m_threads.Add(MidiEngine);
  
  // creation of condition needed for InitAudio and Seq communication
  SeqStopped = new wxCondition(AudioMutex);
  if (!SeqStopped->IsOk())
    {
      cout << "[MAINWIN] Condition creation failed.. critical error" << endl;
      exit(1);
    }

  // init audio
  if (InitAudio() < 0)
    AlertDialog(_("audio engine"), 
		_("You may check for your audio settings if you want to use Wired.."));
  InitFileConverter();

  // start sequencer thread (after InitAudio is a good option)
  if (Seq->Create() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Create sequencer thread failed !" << endl;
  Seq->SetPriority(WXTHREAD_MAX_PRIORITY);
  if (Seq->Run() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Run sequencer thread failed !" << endl; 
  else
    wxGetApp().m_threads.Add(Seq);

  InitUndoRedoMenuItems();
  //  InitVideoMenuItems();

  SeqTimer = new wxTimer(this, MainWin_SeqTimer);
  SeqTimer->Start(40);
  
  return (0);
}

int			MainWindow::InitAudio(bool restart)
{
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
	Mix->InitOutputBuffers();
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

      // Refill tracks connections
      if (SettingsWin->AudioLoaded || SettingsWin->MidiLoaded)
	for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
	  (*i)->TrackOpt->FillChoices();      
      // Sends sample rate and buffer size modifications to plugins
      if (SettingsWin->AudioLoaded)
	{
	  list<RackTrack *>::iterator k;
	  list<Plugin *>::iterator j;
	  
	  for (k = RackPanel->RackTracks.begin(); 
	       k != RackPanel->RackTracks.end(); k++)
	    for (j = (*k)->Racks.begin(); j != (*k)->Racks.end(); j++)
	      {
		(*j)->SetBufferSize(Audio->SamplesPerBuffer);
		(*j)->SetSamplingRate(Audio->SampleRate);
	      }
	}

      if (restart)
	{
	  FileConverter->SetFormat((PaSampleFormat)Audio->SampleFormat);
	  FileConverter->SetBufferSize(Audio->SamplesPerBuffer);
	  FileConverter->SetSampleRate((long unsigned int)Audio->SampleRate);
	}
      
    }
  else
    cout << "You may check for your audio settings if you want to use Wired.." << endl;
  
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
  // disable extra output of wx
  wxLog		log(wxLogNull);
  wxString	prefix = wxString(wxT(INSTALL_PREFIX)) + wxString(wxT("share/locale/"));
  
  mLocale = new wxLocale();
  mLocale->AddCatalogLookupPathPrefix(prefix);
  if (mLocale->Init(wxLANGUAGE_DEFAULT) == true)
    {
      // add wx basic translation (File, Window, About, ..)
      mLocale->AddCatalog(wxT("wxstd"));
      // add our translations
      mLocale->AddCatalog(wxT("wired"));
    }
  else
    cout << "[MAINWIN] Could not initialize locale, falling down on default" << endl;
}

void					MainWindow::InitFileConverter()
{
	FileConverter = new FileConversion();
	t_samplerate_info info;
	int i; 
	if (Audio->UserData->Sets->WorkingDir.empty())
	  {
	    wxDirDialog dir(this, _("Choose the audio working directory"), wxFileName::GetCwd(), wxDD_NEW_DIR_BUTTON | wxCAPTION | wxSUNKEN_BORDER);
	    if (dir.ShowModal() == wxID_OK)
	      CurrentXmlSession->GetAudioDir() = dir.GetPath();
	    else
	      CurrentXmlSession->GetAudioDir() = wxFileName::GetCwd();
	    
	    Audio->UserData->Sets->WorkingDir = CurrentXmlSession->GetAudioDir();
	  }
	else
	  {
	    CurrentXmlSession->GetAudioDir() = Audio->UserData->Sets->WorkingDir;
	    
	  }
	info.WorkingDirectory = CurrentXmlSession->GetAudioDir();
	info.SampleRate = (unsigned long) Audio->SampleRate;
	info.SamplesPerBuffer = (unsigned long) Audio->SamplesPerBuffer;
	// init FileConverter with 4Mo of cache
	if (FileConverter->Init(&info, CurrentXmlSession->GetAudioDir(), (unsigned long)1024 * 1024 * 4, this) == false)
	  cout << "[MAINWIN] Create file converter thread failed !" << endl;
	MediaLibraryPanel->SetFileConverter(FileConverter);
}

void					MainWindow::InitUndoRedoMenuItems()
{
  EditMenu->Insert(INDEX_MENUITEM_UNDO, MainWin_Undo, _("U&ndo"), UndoMenu);
  EditMenu->Insert(INDEX_MENUITEM_REDO, MainWin_Redo, _("&Redo"), RedoMenu);
  EditMenu->Enable(MainWin_Undo, false);
  EditMenu->Enable(MainWin_Redo, false);
}

void					MainWindow::InitVideoMenuItems()
{
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
  //WiredVideoObject->CloseFile();
  /* for (i = RackPanel->RackTracks.begin(); i != RackPanel->RackTracks.end(); i++)  
     for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
     for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
     if ((*k)->InitInfo.Id == (*j)->InitInfo->Id)
     {
     cout << "[MAINWIN] Destroying plugin: " << (*j)->Name << endl;
     RackPanel->RemoveChild((*j)->Gui);
     (*k)->Destroy(*j);
     break;
     }*/
  cout << "[MAINWIN] Stopping threads..."<< endl;
  wxThread *thread;
  
  wxGetApp().m_mutex.Lock();
  const wxArrayThread& threads = wxGetApp().m_threads;
  size_t count = threads.GetCount();
  
  for (int i = 0; i < count; i++)
    threads.Item(i)->Delete();
  
  if (count > 0)
    {
      cout << "[MAINWIN] Waiting for Threads to stop..." << endl;
      if (wxGetApp().m_condAllDone->WaitTimeout(5000) == wxCOND_TIMEOUT)
	cout << "[MAINWIN] Threads are stuck !"<< endl;
    }
  wxGetApp().m_mutex.Unlock();
  
  cout << "[MAINWIN] Done !"<< endl;
  
  cout << "[MAINWIN] Stopping graphics things..."<< endl;
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
  delete Mix;

  if (Audio)
    delete Audio;
  Audio = NULL; // for handling event of Transport::OnIdle

  cout << "[MAINWIN] Unloading logging manager..." << endl;
  delete LogWin;

  cout << "[MAINWIN] Unloading external plugins..." << endl;
  if (LoadedExternalPlugins)
    delete LoadedExternalPlugins;

  cout << "[MAINWIN] Unloading session manager..." << endl;
  delete CurrentSession;
  delete CurrentXmlSession;

  cout << "[MAINWIN] Unloading user settings manager..." << endl;
  if (WiredSettings)
    delete WiredSettings;

  cout << "[MAINWIN] Closing..." << endl;
  // all gui windows (and this one) are destroyed in MainApp::OnExit
  // but exit prevent wired to segfault... :(
  exit(1);
}

void					MainWindow::OnQuit(wxCommandEvent &WXUNUSED(event))
{
  Close(false);
}

void					MainWindow::OnNew(wxCommandEvent &event)
{
  NewSession();
}

bool					MainWindow::NewSession()
{
  // une session existe d?ja, demande de confirmation d'enregistrement

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
  delete CurrentXmlSession;
  CurrentXmlSession = new WiredSessionXml(wxT(""));

  Seq->Stop();
  
  SeqMutex.Lock();
  
  WaveCenter.Clear();

  UpdatePlugins.clear();
  Seq->PatternsToRefresh.clear();
  Seq->PatternsToResize.clear();
  Seq->TracksToRefresh.clear();
  SeqPanel->DeleteAllTracks();
  RackPanel->DeleteAllRacks();
  OptPanel->DeleteTools();
  SeqMutex.Unlock();
  return (true);
}

void					MainWindow::OnOpen(wxCommandEvent &event)
{
  vector<wxString>			exts;
  
  exts.insert(exts.begin(), _("wrd\tWired session file (*.wrd)"));
  exts.insert(exts.begin(), _("xml\tWired session file (*.xml)"));
  FileLoader				dlg(this, MainWin_FileLoader, _("Open session"), false, false, &exts);
  if (dlg.ShowModal() == wxID_OK)
    {
      wxString selfile = dlg.GetSelectedFile();    
      
      cout << "[MAINWIN] User opens " << selfile.mb_str() << endl;
      if (!NewSession())
	{
	  //dlg->Destroy();
	  return;
	}
      if (selfile.size() > 4)
	{
	  transform(selfile.begin(), selfile.end(), selfile.begin(), (int(*)(int))tolower);
	  if (!selfile.substr(selfile.find_last_of('.')).compare(XML_EXTENSION))
	    {
	      if (CurrentXmlSession)				
		delete CurrentXmlSession;
	      CurrentXmlSession = new WiredSessionXml(selfile);
	      CurrentXmlSession->Load(selfile);
	      cout << "[MAINWIN] New session loaded" << endl;
	    }
	  else
	    {
	      cout << "[MAINWIN] Warning! Opening old format" << endl;
	      CurrentSession = new WiredSession(selfile);
	      CurrentSession->Load();
	    }
	}
      else
	cout << "[MAINWIN] Invalid Filename" << endl;
      
    }
}

void					MainWindow::OnSave(wxCommandEvent &event)
{
  if (CurrentXmlSession)
    {
      wxString			DocumentName(CurrentXmlSession->GetDocumentName());
      if (!DocumentName.empty())
	CurrentXmlSession->Save();
      else
	OnSaveAs(event);
    }
  else
    OnSaveAs(event);
}

void					MainWindow::OnSaveAs(wxCommandEvent &event)
{
  vector<wxString>			exts;
  
  exts.insert(exts.begin(), _("xml\tWired session file (*.xml)"));
  FileLoader				dlg(this, MainWin_FileLoader, _("Save session"), false, true, &exts);
  if (dlg.ShowModal() == wxID_OK)
    {
      wxString selfile = dlg.GetSelectedFile();    
      
      wxFileName f(selfile);
      if (!f.HasExt())
	selfile = selfile + XML_EXTENSION;
      cout << "[MAINWIN] User saves to " << selfile.mb_str() << endl;

      wxString audiodir;
      
      if (CurrentXmlSession)
	{
	  audiodir = wxString(CurrentXmlSession->GetAudioDir());
	  if (audiodir.size() == 0)
	    {
	      char	buffer[2048];
	      
	      getcwd(buffer, 2048);
	      audiodir = wxString(buffer, *wxConvCurrent);
	    }  	
	  delete CurrentXmlSession;
	}
      CurrentXmlSession = new WiredSessionXml(selfile, audiodir);
      CurrentXmlSession->Save();
    }
}

void					MainWindow::OnImportWave(wxCommandEvent &event)
{
  FileLoader				dlg(this, MainWin_FileLoader, _("Loading sound file"), false, false, FileConverter->GetCodecsExtensions(), true);
  int						res;
  
  if (dlg.ShowModal() == wxID_OK)
    {
      wxString 	selfile = dlg.GetSelectedFile();
      
      if (CurrentXmlSession->GetAudioDir().empty() == false)
	res = wxID_OK;
      else	     
	{
	  wxDirDialog dir(this, _("Choose the Audio file directory"), wxFileName::GetCwd());
	  if (dir.ShowModal() == wxID_OK)
	    {
	      CurrentXmlSession->GetAudioDir() = dir.GetPath();
	      res = wxID_OK;
	    }
	  else
	    res = wxID_CANCEL;
	}
      if (res != wxID_CANCEL)
	{
	  MidiMutex.Lock();
	  MidiDeviceMutex.Lock();
	  SeqMutex.Unlock();
	  
	  // convert and import file 
	  FileConverter->ImportFile(selfile);
	  
	  MidiMutex.Unlock();  
	  MidiDeviceMutex.Unlock();
	}
    }
}

void					MainWindow::OnImportMIDI(wxCommandEvent &event)
{
  vector<wxString>			exts;
  
  exts.insert(exts.begin(), _("mid\tMidi file (*.mid)"));
  FileLoader				dlg(this, MainWin_FileLoader, _("Import MIDI file"), false, false, &exts);
  if (dlg.ShowModal() == wxID_OK)
    {
      wxString selfile = dlg.GetSelectedFile();
      
      cout << "[MAINWIN] Users imports MIDI file : " << selfile.mb_str() << endl;
      wxProgressDialog Progress(_("Loading midi file"), _("Please wait..."), 100, 
				this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT 
				| wxPD_REMAINING_TIME);
      Progress.Update(1);
      cImportMidiAction* action = new cImportMidiAction(selfile, false);
      action->Do();
      Progress.Update(99);	
      //delete Progress;
      /*
	MidiFile *m;
	m = new MidiFile(selfile);
	
	if (m)
	{
	  Progress->Update(90);
	  for (int i = 0; i < m->GetNumberOfTracks(); i++)
	  {
	  if (m->GetTrack(i)->GetMaxPos() > 0)
	  {
	  Track *t = SeqPanel->AddTrack(false);
	  t->AddPattern(m->GetTrack(i));
	  }
	  }
	  Progress->Update(99);
	  }
	  else
	  cout << "[MAINWIN] Cannot import midi file !" << endl;
	  delete Progress;
      */
    }
}

void					MainWindow::OnImportAKAI(wxCommandEvent &event)
{
  //TransportPanel->OnStop(event);
  FileLoader				dlg(this, MainWin_FileLoader, _("Import AKAI samples"), true, false, NULL);
  
  if (dlg.ShowModal() == wxID_OK)
    {
      wxString selfile = dlg.GetSelectedFile();
      
      wxProgressDialog Progress(_("Loading midi file"), _("Please wait..."), 100, 
				this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT 
				| wxPD_REMAINING_TIME);
      Progress.Update(1);
      cImportAkaiAction* action = new cImportAkaiAction(selfile, true);
      action->Do();
      Progress.Update(99);	
      //delete Progress;
      /*
	cout << "[MAINWIN] Users imports AKAI sample : " << selfile << endl;
	wstring dev = selfile.substr(0, selfile.find(":", 0));
	selfile = selfile.substr(selfile.find(":", 0) + 1, selfile.size() - selfile.find(":", 0));
	wstring path = selfile.substr(10, selfile.size() - 10);
	unsigned int pos = path.find("/", 0);
	int part = path.substr(0, pos).c_str()[0] - 64;
	path = path.substr(pos, path.size() - pos);
	int opos = 0;
	while ((pos = path.find("/", opos)) != wstring::npos)
	opos = pos + 1;
	wstring name = path.substr(opos, path.size() - opos);
	path = path.substr(1, opos - 2);
	wxProgressDialog *Progress = new wxProgressDialog("Loading wave file", "Please wait...", 
	100, this, 
	wxPD_AUTO_HIDE | wxPD_CAN_ABORT | 
	wxPD_REMAINING_TIME);
	t_akaiSample *sample = akaiGetSampleByName((char *)dev.c_str(), part, (char *)path.c_str(), (char *)name.c_str());
	if (sample != NULL)
	{
	Progress->Update(1);
	cout << "[MAINWIN] sample channel : " << sample->channels << endl;
	  WaveFile *w = new WaveFile(sample->buffer, sample->size, 2, sample->rate);
	  
	  if (!w->Error)
	  {
		Track *t = SeqPanel->AddTrack(true);
		Progress->Update(90);
		t->AddPattern(w);
		Progress->Update(99);
	  }
	  else
		cout << "[MAINWIN] Cannot import AKAI wave file !" << endl;
	  delete Progress;
	  }
      */
    }
}

void					MainWindow::OnExportWave(wxCommandEvent &event)
{
  //  TransportPanel->OnStop(event);
  double total = Seq->EndLoopPos - Seq->BeginLoopPos; 
  
  if (total <= 0)
    {
      wxMessageDialog msg(this, _("Please correctly place the Left and Right markers"), wxT("Wired"), 
			  wxOK | wxICON_EXCLAMATION | wxCENTRE);
      msg.ShowModal();
      return;
    }
  FileLoader				dlg(this, MainWin_FileLoader, _("Exporting sound file"), false, true, NULL);
  
  if (dlg.ShowModal() == wxID_OK)
    {
      wxString          selfile = dlg.GetSelectedFile();    
      wxFileName	f(selfile);
      
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

      wxProgressDialog Progress(_("Exporting mix"), _("Please wait..."), 
				(int)Seq->EndLoopPos * 1000, this, 
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

void					MainWindow::OnExportMIDI(wxCommandEvent &event)
{
  vector<wxString>			exts;
  
  exts.insert(exts.begin(), _("mid\tMidi file (*.mid)"));
  FileLoader				dlg(this, MainWin_FileLoader, 
					    _("Export MIDI file"), false, true, &exts);
  
  if (dlg.ShowModal() == wxID_OK)
    {
      wxString selfile = dlg.GetSelectedFile();
      cout << "[MAINWIN] Users exports MIDI file : " << selfile.mb_str() << endl; 
    }
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  //dlg->Destroy();
}

void					MainWindow::LoadPlugins()
{
  wxString				str;
  PluginLoader				*p;
  
  if (!PluginsConfFile.Open(WiredSettings->PlugConfFile))
    {
      cerr << "Could not load " << WiredSettings->PlugConfFile.mb_str() << endl;
      return;
    }
  PluginMenuIndexCount = PLUG_MENU_INDEX_START;
  for (str = PluginsConfFile.GetFirstLine(); !PluginsConfFile.Eof();
       str = PluginsConfFile.GetNextLine())
    {
      if ((str.length() > 0) && (str.at(0) != '#'))
	{
	  p = new PluginLoader(str);
	  if (p->IsLoaded())
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
	      cout << "[MAINWIN] Plugin " << p->InitInfo.Name.mb_str() << " is working" << endl;
	    }
	  else
	    delete p;
	}
    }
}

void					MainWindow::LoadExternalPlugins()
{ 
//  map<int, wstring>				PluginsList;
  list<wxString>					PluginsList;
  //  map<int, wstring>::iterator	IterPluginsList;
  list<wxString>::iterator		IterPluginsList;
  int							PluginInfo;
  int							PluginId;
  wxString						PluginName, Sep(wxT("#"));
  
  CreateDSSIInstrMenu = NULL;
  CreateLADSPAInstrMenu = NULL;
  CreateDSSIEffectMenu = NULL;
  CreateLADSPAEffectMenu = NULL;
  LoadedExternalPlugins->LoadPLugins(TYPE_PLUGINS_DSSI | TYPE_PLUGINS_LADSPA);
  LoadedExternalPlugins->SetStartInfo(StartInfo);
  PluginsList = LoadedExternalPlugins->GetSortedPluginsList(Sep);
  
  for (IterPluginsList = PluginsList.begin(); IterPluginsList != PluginsList.end(); IterPluginsList++)
    {
      if ((*IterPluginsList).find_last_of(Sep.c_str()) > 0)
  	{
	  PluginName = (*IterPluginsList).substr(0, (*IterPluginsList).find_last_of(Sep));
	  PluginId = atoi(wxString((*IterPluginsList).substr((*IterPluginsList).find_last_of(Sep) + 1).c_str(), *wxConvCurrent).mb_str(*wxConvCurrent));
  	}
      //  	PluginInfo = LoadedExternalPlugins->GetPluginType(IterPluginsList->first);
      PluginInfo = LoadedExternalPlugins->GetPluginType(PluginId);
      
      //  	LoadedExternalPlugins->SetMenuItemId(IterPluginsList->first, 
      //  		AddPluginMenuItem(PluginInfo, PluginInfo & TYPE_PLUGINS_EFFECT, IterPluginsList->second));
      LoadedExternalPlugins->SetMenuItemId(PluginId, 
					   AddPluginMenuItem(PluginInfo, PluginInfo & TYPE_PLUGINS_EFFECT, PluginName));
    }
}

int						MainWindow::AddPluginMenuItem(int Type, bool IsEffect, const wxString& MenuName)
{
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
	  NewItem = CreateDSSIEffectMenu->Append(Id, MenuName.c_str());
	}
      else if (Type & TYPE_PLUGINS_LADSPA)
	{
	  if (!CreateLADSPAEffectMenu)
	    {
	      CreateLADSPAEffectMenu = new wxMenu();
	      CreateEffectMenu->Append(Id, wxT("LADSPA"), CreateLADSPAEffectMenu);
	      Id = PluginMenuIndexCount++;
	    }
	  NewItem = CreateLADSPAEffectMenu->Append(Id, MenuName.c_str());
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
	  NewItem = CreateDSSIInstrMenu->Append(Id, MenuName.c_str());
	}
      else if (Type & TYPE_PLUGINS_LADSPA)
	{
	  if (!CreateLADSPAInstrMenu)
	    {
	      CreateLADSPAInstrMenu = new wxMenu();
	      CreateInstrMenu->Append(Id, wxT("LADSPA"), CreateLADSPAInstrMenu);
	      Id = PluginMenuIndexCount++;
	    }
	  NewItem = CreateLADSPAInstrMenu->Append(Id, MenuName.c_str());
	}
    }
  if (NewItem)
    Connect(Id, wxEVT_COMMAND_MENU_SELECTED, 
	    (wxObjectEventFunction)(wxEventFunction) 
	    (wxCommandEventFunction)&MainWindow::OnCreateExternalPlugin);
  return Id;
}

void					MainWindow::OnCreateExternalPlugin(wxCommandEvent &event)
{
  if (LoadedExternalPlugins)
    {
      PluginLoader 	*NewPlugin = new PluginLoader(LoadedExternalPlugins, event.GetId(), StartInfo);
      
      LoadedPluginsList.push_back(NewPlugin);
      cout << "[MAINWIN] Creating rack for plugin: " << NewPlugin->InitInfo.Name.mb_str() << endl;
      cActionManager::Global().AddEffectAction(&StartInfo, NewPlugin, true);
    }
}

void					MainWindow::OnCreateRackClick(wxCommandEvent &event)
{
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
      CreateUndoRedoMenus(EditMenu);
    }
}

void					MainWindow::OnDeleteRack(wxCommandEvent &event)
{
  vector<PluginLoader *>::iterator	k;
  
  if (RackPanel->selectedPlugin)
    {
      for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
	if (COMPARE_IDS((*k)->InitInfo.UniqueId, RackPanel->selectedPlugin->InitInfo->UniqueId))
	  {
	    cActionManager::Global().AddEffectAction(&StartInfo, *k, false);
	    CreateUndoRedoMenus(EditMenu);
	    return;
	  }
    }
}

void					MainWindow::OnAddTrackAudio(wxCommandEvent &event)
{
  //cAddTrackAction			*action = new cAddTrackAction(true);

  //action->Do();
  //CreateUndoRedoMenus(EditMenu);
  SeqPanel->AddTrack(true);
}

void					MainWindow::OnAddTrackMidi(wxCommandEvent &event)
{
  //cAddTrackAction			*action = new cAddTrackAction(false);

  //action->Do();
  //CreateUndoRedoMenus(EditMenu);
  SeqPanel->AddTrack(false);
}

void					MainWindow::OnFloatTransport(wxCommandEvent &event)
{
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
      BottomSizer->Insert(0, TransportPanel, 0, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2); 
      BottomSizer->Layout();
      
      delete TransportFrame;
      TransportFrame = 0x0;
    }
}

void					MainWindow::OnFloatSequencer(wxCommandEvent &event)
{
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
}

void					MainWindow::OnFloatRack(wxCommandEvent &event)
{
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
}

void					MainWindow::OnFloatMediaLibrary(wxCommandEvent &event)
{
  if (MediaLibraryPanel->IsVisible() == false)
    {
      splitVert->SetSashPosition(200);
      MediaLibraryPanel->Show();
      MediaLibraryPanel->SetVisible();
    }
  if (MediaLibraryMenu->IsChecked(MainWin_FloatMediaLibrary))
    {
      splitVert->Unsplit(MediaLibraryPanel);
      
      cout << "[MEDIALIBRARY] Float" << endl;
      MediaLibraryFrame = new FloatingFrame(0x0, -1, _("MediaLibrary"), MediaLibraryPanel->GetPosition(), 
					    MediaLibraryPanel->GetSize(), MediaLibraryPanel->GetParent(),
					    ItemFloatingMediaLibrary, MainWin_FloatMediaLibrary);
      MediaLibraryPanel->Reparent(MediaLibraryFrame);
      MediaLibraryPanel->SetVisible();
      MediaLibraryPanel->SetFloating();
      MediaLibraryPanel->Show();
      MediaLibraryFrame->Show();
    }
  else
    {
      cout << "[MEDIALIBRARY] UnFloat" << endl;
      MediaLibraryPanel->Reparent(splitVert);
      splitVert->SplitVertically(MediaLibraryPanel, split);
      delete MediaLibraryFrame;
      MediaLibraryFrame = 0x0;
      MediaLibraryPanel->SetVisible();
      MediaLibraryPanel->SetDocked();
      splitVert->SetSashPosition(200);
    }
}

void					MainWindow::OnSwitchRackOptViewEvent(wxCommandEvent &event)
{
  SwitchRackOptView();
}

void					MainWindow::OnSwitchSeqOptViewEvent(wxCommandEvent &event)
{
  SwitchSeqOptView();
}

void					MainWindow::SwitchRackOptView()
{
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

void					MainWindow::MediaLibraryShow(wxCommandEvent &event)
{
  
  cout << "[MAINWIN] Launching MediaLibrary" << endl;
  
  if (MediaLibraryPanel->IsVisible() == false)
    {
      splitVert->SetSashPosition(200);
      MediaLibraryPanel->Show();
      MediaLibraryPanel->SetVisible();
    }
}

void					MainWindow::MediaLibraryHide(wxCommandEvent &event)
{
  cout << "[MAINWIN] Hiding MediaLibrary" << endl;
  
  if (!(MediaLibraryPanel->IsVisible() == false || MediaLibraryPanel->IsFloating() == true))
    {
      splitVert->SetSashPosition(1);
      //MediaLibraryPanel->Hide();
      MediaLibraryPanel->SetInvisible();
    }
}

void					MainWindow::OnSettings(wxCommandEvent &event)
{
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
	AlertDialog(_("audio engine"), 
		    _("You may check for your audio settings if you want to use Wired.."));
    }
}

void					MainWindow::AlertDialog(const wxString& from, const wxString& msg)
{
  wxMessageDialog			mdialog(MainWin, msg, from, wxICON_INFORMATION, wxDefaultPosition);

  mdialog.ShowModal();
}

void					MainWindow::OnOpenVideo(wxCommandEvent &event)
{
  WiredVideoObject->OpenFile();
  if (WiredVideoObject->asFile)
    {
      VideoMenu->Enable(MainWin_OpenVideo, false);
      VideoMenu->Enable(MainWin_CloseVideo, true);
    }
}

void					MainWindow::OnCloseVideo(wxCommandEvent &event)
{
  WiredVideoObject->CloseFile();
  VideoMenu->Enable(MainWin_OpenVideo, true);
  VideoMenu->Enable(MainWin_CloseVideo, false);
}

void					MainWindow::OnSeekVideo(wxCommandEvent &event)
{
  //  WiredVideoObject->SetSeek(VideoMenu->IsChecked(MainWin_SeekVideo));
}

void					MainWindow::OnDeleteTrack(wxCommandEvent &event)
{
  SeqPanel->DeleteSelectedTrack();
  /* Needs path in AudioPattern */
  //cActionManager::Global().AddImportWaveAction(selfile, true, false);
}

void					MainWindow::OnChangeAudioDir(wxCommandEvent &event)
{
  assert(CurrentXmlSession);
  
  wxDirDialog dir(this, _("Choose the Audio file directory"), 
		  CurrentXmlSession->GetAudioDir().empty() == true ? 
		  wxFileName::GetCwd() : CurrentXmlSession->GetAudioDir());
  if (dir.ShowModal() == wxID_OK)
    CurrentXmlSession->GetAudioDir() = dir.GetPath().c_str();    
}

void					MainWindow::OnUndo(wxCommandEvent &event)
{
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
  wxMenuItemList						menuItemList;
  wxMenuItemList::const_iterator		itermenuItems;
	
  menuItemList = menu->GetMenuItems();
  for (itermenuItems = menuItemList.begin(); itermenuItems != menuItemList.end(); itermenuItems++)
    menu->Delete(*itermenuItems);
}

void					MainWindow::CreateUndoRedoMenus(wxMenu *callingMenu)
{
  std::list<t_menuInfo*>					historyList;
  std::list<t_menuInfo*>::const_iterator	iter;
  wxMenu									*undoMenu;
  wxMenu									*redoMenu;
  int										separatorIndex;
  int										count;
  
  undoMenu = callingMenu->FindItemByPosition(INDEX_MENUITEM_UNDO)->GetSubMenu();
  redoMenu = callingMenu->FindItemByPosition(INDEX_MENUITEM_REDO)->GetSubMenu();
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
  SeqPanel->OnCut(event);
}

void					MainWindow::OnCopy(wxCommandEvent &event)
{
  SeqPanel->OnCopy(event);
}

void					MainWindow::OnPaste(wxCommandEvent &event)
{
  SeqPanel->OnPaste(event);
}

void					MainWindow::OnDelete(wxCommandEvent &event)
{
  SeqPanel->OnDeleteClick(event);
}

void					MainWindow::OnSelectAll(wxCommandEvent &event)
{
  SeqPanel->OnSelectAll(event);
}

void					MainWindow::OnFullScreen(wxCommandEvent &event)
{
  ShowFullScreen(!IsFullScreen(), wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION );
}

void					MainWindow::OnAbout(wxCommandEvent &event)
{
  wxBitmap splashbtm;
  if (splashbtm.LoadFile(wxString(WiredSettings->DataDir +
				  wxString(wxT("ihm/splash/splash.png"))), 
			 wxBITMAP_TYPE_PNG))
    {
      wxSplashScreen* splash = new wxSplashScreen(splashbtm,
						  wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
						  6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
						  wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    }
  //wxYield();
}

void					MainWindow::OnSpaceKey()
{
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
  wxCommandEvent			commandEvt;
  CursorEvent				cursorEvt;
  list<Pattern *>::iterator		patternIt;
  list<MidiPattern *>::iterator		midiPatternIt;
  list<Plugin *>::iterator		pluginIt;
  list<Track *>::iterator		trackIt;

  SeqMutex.Lock();

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

  for (pluginIt = UpdatePlugins.begin(); pluginIt != UpdatePlugins.end(); pluginIt++)
    (*pluginIt)->Update();
  UpdatePlugins.clear();

  for (trackIt = Seq->TracksToRefresh.begin(); trackIt != Seq->TracksToRefresh.end(); trackIt++)
    (*trackIt)->TrackOpt->SetVuValue();
  Seq->TracksToRefresh.clear();
  
  SeqMutex.Unlock();
}

void					MainWindow::AddUpdatePlugin(Plugin *p)
{
  list<Plugin *>::iterator		i;
  
  for (i = UpdatePlugins.begin(); i != UpdatePlugins.end(); i++)
    if (*i == p)
      return;
  UpdatePlugins.push_back(p);
}

void					MainWindow::OnFileLoaderStart(wxCommandEvent &event)
{
  FileLoader				*f = (FileLoader *)event.GetEventObject();
  
  Seq->PlayFile(f->GetSelectedFile(), f->IsAkai());
}

void					MainWindow::OnFileLoaderStop(wxCommandEvent &event)
{
  Seq->StopFile();
}

void					MainWindow::OnIntegratedHelp(wxCommandEvent &event)
{
  OptPanel->ShowHelp();
}

void                  MainWindow::OnShowDebug(wxCommandEvent &event)
{
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
  cout << "[MAINWIN] Killing Threads" << endl;
  exit (0);
}

void					MainWindow::OnIdle(wxIdleEvent &WXUNUSED(event))
{
#if wxUSE_STATUSBAR
  wxLogNull NoLog;
  if (SeqTimer)
    {
      wxString			LeftSpace(_("Left space on drive : ")), LeftMemory(_("Free memory : "));
      wxLongLong		Total, Free;
      
      wxLongLong Size = wxGetFreeMemory();
      if (Size > 0)        
	LeftMemory += Size.ToString();
      else
	LeftMemory += _("Unknown");
      if (CurrentXmlSession)
	wxGetDiskSpace(CurrentXmlSession->GetAudioDir(), &Total, &Free);
      else
	wxGetDiskSpace(wxFileName::GetCwd(), &Total, &Free);
      LeftSpace += FileLoader::FormatSize((off_t) Free.GetValue()) + wxT("/") + FileLoader::FormatSize((off_t)Total.GetValue());
      SetStatusText(LeftSpace, 0);
      SetStatusText(LeftMemory, 1);
    }
#endif
}

void		MainWindow::OnKey(wxKeyEvent& event)
{
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
  EVT_MENU(MainWin_DeleteTrack, MainWindow::OnDeleteTrack)
  EVT_MENU(MainWin_ChangeAudioDir, MainWindow::OnChangeAudioDir)
  EVT_MENU(MainWin_FloatTransport, MainWindow::OnFloatTransport) 
  EVT_MENU(MainWin_FloatSequencer, MainWindow::OnFloatSequencer) 
  EVT_MENU(MainWin_FloatRacks, MainWindow::OnFloatRack) 
  EVT_MENU(MainWin_FloatMediaLibrary, MainWindow::OnFloatMediaLibrary) 
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

// button
  EVT_BUTTON(FileLoader_Start, MainWindow::OnFileLoaderStart)
  EVT_BUTTON(FileLoader_Stop, MainWindow::OnFileLoaderStop)
//  EVT_MENU(MainWin_OpenVideo, MainWindow::OnOpenVideo)
//  EVT_MENU(MainWin_CloseVideo, MainWindow::OnCloseVideo)
//  EVT_MENU(MainWin_SeekVideo, MainWindow::OnSeekVideo)
  //EVT_TEXT_MAXLEN(101010, MainWindow::OnSetPosition)
  //EVT_PLAYPOSITION(313131, MainWindow::OnSetPosition)
END_EVENT_TABLE()

