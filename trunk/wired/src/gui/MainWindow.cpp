// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "MainWindow.h"

#include <dlfcn.h>
#include <wx/splitter.h>
#include <wx/progdlg.h>
#include <wx/filename.h>
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
#include "../engine/AudioEngine.h"
#include "../engine/Settings.h"
#include "../engine/EngineError.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/Track.h"
#include "../mixer/Mixer.h"
#include "../engine/WiredSession.h"
#include "../midi/MidiThread.h"
#include "../plugins/PluginLoader.h"

Rack					*RackPanel;
SequencerGui				*SeqPanel;
Sequencer				*Seq;
AudioEngine				*Audio;
Mixer					*Mix;
AudioCenter				WaveCenter;
Transport				*TransportPanel;
PlugStartInfo				StartInfo;
vector<PluginLoader *>			LoadedPluginsList;
WiredSession				*CurrentSession;

MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size)
  : wxFrame((wxFrame *) NULL, -1, title, pos, size, 
	    wxDEFAULT_FRAME_STYLE | wxWS_EX_PROCESS_IDLE)
{
  WiredSettings = new Settings(); // FIXME catch what we can here

  CurrentSession = new WiredSession("");
  
  try
    {
      Audio = new AudioEngine();
      Audio->GetDevices();
      Audio->GetDeviceSettings(); 
      // FIXME we just overwrote detected settings
      // with saved ones, even if they're wrong/fucked/don't exist
      
      Audio->OpenStream(); 
    }
  catch (Error::NoDevice)
    {
      cout << "[MAINWIN] No Device :\nplease check you have a soundcard and Alsa installed" << endl;
      
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
      
      //exit(1);
    }
  catch (Error::InvalidDeviceSettings)
    {
      cout << "[MAINWIN] Invalid Device Settings" << endl;
      Audio->IsOk = false;
      AlertDialog("audio engine", "you may check for your audio settings if you want to use wired..");
      
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
            
      //Audio->SetDefaultSettings();
      //Audio->GetDevices();
      //Audio->GetDeviceSettings();
      /*
	SettingWindow s;
      if (s.ShowModal() == wxID_OK)
	{
	  Audio->GetDeviceSettings();
	  //Mix->InitOutputBuffers();
	  Audio->OpenStream();
	  //Audio->OpenStream();
	}
      else
	;
      */
    }
  catch (Error::ChannelsNotSet)
    {
      cout << "[MAINWIN] Channels Not Set" << endl;
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
    }
  catch (Error::StreamNotOpen)
    {
      cout << "[MAINWIN] Stream Not Opened" << endl;
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
    }
  catch (Error::InitFailure &f)
    {
      cout << "[MAINWIN] Portaudio Failure :" << f.getMsg() << endl;
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
      //exit(1);
    }
  catch (Error::AudioEngineError)
    {
      cout << "[MAINWIN] General AudioEngine Error" << endl;
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
      // FIXME add exit()s on every catch
    }
  catch (std::bad_alloc)
    {
      cout << "[MAINWIN] oom" << endl;
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
      
    }
  catch (std::exception &e)
    {
      cout << "[MAINWIN] Stdlib failure during AudioEngine init, check your code" << endl;
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
      
    }
  catch (...)
    {
      // FIXME fenetre de dialogue blabla
      cout << "[MAINWIN] Unknown AudioEngine error" << endl;
      AudioMutex.Lock();/* This will lock the sequencer			\
			   until audio parameters are properly set */
      // FIXME handle some kind of error recovery
    }
  
  // Le Mixer doit etre declarer apres AudioEngine
  Mix = new Mixer();
  
  OptPanel = new OptionPanel(this, wxPoint(306, 452), wxSize(470, 150), wxSIMPLE_BORDER);
  Seq = new Sequencer();
  
  MidiEngine = new MidiThread();
  MidiEngine->OpenDefaultDevices();
  
  if (MidiEngine->Create() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Create MidiEngine thread failed !" << endl;
  if (MidiEngine->Run() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Run MidiEngine thread failed !" << endl;  
  /* Creation Menu */

  TransportFrame = 0x0;
  OptFrame = 0x0;
  SequencerFrame = 0x0;
  RackFrame = 0x0; 

  MenuBar = new wxMenuBar;
  FileMenu = new wxMenu;
  EditMenu = new wxMenu;
  SequencerMenu = new wxMenu;
  RacksMenu = new wxMenu;
  CreateInstrMenu = new wxMenu;
  CreateEffectMenu = new wxMenu;
  HelpMenu = new wxMenu;
  WindowMenu = new wxMenu;
  
  FileMenu->Append(MainWin_New, "&New\tCtrl-N");
  FileMenu->Append(MainWin_Open, "&Open...\tCtrl-O");
  FileMenu->Append(MainWin_Save, "&Save\tCtrl-S");
  FileMenu->Append(MainWin_SaveAs, "Save &as...\tF12");
  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_ImportWave, "&Import Wave file...");
  FileMenu->Append(MainWin_ImportMIDI, "&Import MIDI file...");
  FileMenu->Append(MainWin_ImportAKAI, "&Import AKAI sample...");
  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_ExportWave, "&Export Wave file...");
  FileMenu->Append(MainWin_ExportMIDI, "&Export MIDI file...");

  FileMenu->AppendSeparator();
  FileMenu->Append(MainWin_Quit, "&Quit");

  EditMenu->Append(MainWin_Undo, "U&ndo\tCtrl+Z");
  EditMenu->Append(MainWin_Redo, "&Redo\tShift+Ctrl+Z");

  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Cut, "C&ut\tCtrl+X");
  EditMenu->Append(MainWin_Copy, "&Copy\tCtrl+C");
  EditMenu->Append(MainWin_Paste, "&Paste\tCtrl+V");
  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Delete, "&Delete\tDel");
  EditMenu->Append(MainWin_SelectAll, "&Select all\tCtrl+A");
  EditMenu->AppendSeparator();
  EditMenu->Append(MainWin_Settings, "&Settings...");
  
  SequencerMenu->Append(MainWin_AddTrackAudio, "&Add Audio Track");
  SequencerMenu->Append(MainWin_AddTrackMidi, "&Add MIDI Track");
  SequencerMenu->Append(MainWin_DeleteTrack, "&Delete Track");
  SequencerMenu->AppendSeparator();
  SequencerMenu->Append(MainWin_ChangeAudioDir, "&Change Audio directory...");
  
  RacksMenu->Append(MainWin_DeleteRack, "D&elete Rack");

  HelpMenu->Append(MainWin_IntHelp, "&Show Integrated Help");
  HelpMenu->Append(MainWin_About, "&About...");
  
  WindowMenu->Append(MainWin_SwitchRack, "Switch &Rack/Optional view\tTAB");
  WindowMenu->Append(MainWin_SwitchSeq, "Switch &Sequencer/Optional view\tCtrl+TAB");
  WindowMenu->AppendSeparator();
  WindowMenu->AppendCheckItem(MainWin_FloatTransport, "Floating Transport");
  WindowMenu->AppendCheckItem(MainWin_FloatSequencer, "Floating Sequencer");
  WindowMenu->AppendCheckItem(MainWin_FloatRacks, "Floating Racks");
  WindowMenu->AppendCheckItem(MainWin_FloatView, "Floating Optional View");
  WindowMenu->AppendSeparator();
  WindowMenu->AppendCheckItem(MainWin_FullScreen, "&Fullscreen");
  
  MenuBar->Append(FileMenu, "&File");
  MenuBar->Append(EditMenu, "&Edit");
  MenuBar->Append(SequencerMenu, "&Sequencer");
  MenuBar->Append(RacksMenu, "&Racks");
  MenuBar->Append(CreateInstrMenu, "&Instruments");
  MenuBar->Append(CreateEffectMenu, "Effec&ts");
  MenuBar->Append(WindowMenu, "&Window");
  MenuBar->Append(HelpMenu, "&Help");
    
  SetMenuBar(MenuBar);

  split = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(800, 450)); 

  /* Creation Panel */
  RackPanel = new Rack(split, -1, wxPoint(0, 0), wxSize(800, 250));
  SeqPanel = new SequencerGui(split, wxPoint(0, 254), wxSize(800, 200));
  //  OptPanel = new OptionPanel(this, wxPoint(306, 452), wxSize(470, 150), wxSIMPLE_BORDER);
  TransportPanel = new Transport(this, wxPoint(0, 452), wxSize(300, 150), wxNO_BORDER);

  cout << "1111" << endl;  
  split->SplitHorizontally(RackPanel, SeqPanel);
  cout << "2222" << endl;  
  
  /* Placement Panel */
    
  BottomSizer = new wxBoxSizer(wxHORIZONTAL);
  BottomSizer->Add(TransportPanel, 0, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2); 
  BottomSizer->Add(OptPanel, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2); 
  
  TopSizer = new wxBoxSizer(wxVERTICAL);
    
  TopSizer->Add(split, 1, wxEXPAND | wxALL, 2);
  TopSizer->Add(BottomSizer, 0, wxEXPAND | wxALL, 0);
  SetSizer(TopSizer);
  
  RackPanel->SetBackgroundColour(*wxBLACK);
  SeqPanel->SetBackgroundColour(*wxWHITE);
  OptPanel->SetBackgroundColour(*wxLIGHT_GREY);
  
  RackPanel->Show();
  SeqPanel->Show();
  OptPanel->Show();
  TransportPanel->Show();


  StartInfo.HostCallback = HostCallback;
  StartInfo.Version = WIRED_VERSION;
  StartInfo.Rack = RackPanel;    
  
  LoadPlugins();

  RackPanel->AddPlugToMenu();

  RackModeView = true;
  SeqModeView = true;

  // Taille minimum de la fenetre
  SetSizeHints(400, 300);

  if (Seq->Create() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Create sequencer thread failed !" << endl;
  Seq->SetPriority(WXTHREAD_MAX_PRIORITY);
  if (Seq->Run() != wxTHREAD_NO_ERROR)
    cout << "[MAINWIN] Run sequencer thread failed !" << endl; 

  Connect(MainWin_ImportWave, wxEVT_COMMAND_MENU_SELECTED, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&MainWindow::OnImportWave);


  SeqTimer = new wxTimer(this, MainWin_SeqTimer);
  SeqTimer->Start(40);
}

void					MainWindow::OnClose(wxCloseEvent &event)
{
  vector<RackTrack *>::iterator		i;
  vector<Plugin *>::iterator		j;
  vector<PluginLoader *>::iterator	k;
  int					res;

  wxMessageDialog msg(this, "Save current session ?", "Wired", 
		      wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE);
  res = msg.ShowModal();
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

  cout << "[MAINWIN] Unloading shared libraries..."<< endl;
  for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
    (*k)->Unload();

  delete Audio;

  WiredSettings->Save();
  delete WiredSettings;
  cout << "[MAINWIN] Closing..."<< endl; 
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
  // une session existe déja, demande de confirmation d'enregistrement

  wxMessageDialog			msg(this, "Save current session ?", "Wired", 
					    wxYES_NO | wxCANCEL | wxICON_QUESTION);
  int					res;
  
  res = msg.ShowModal();
  if (res == wxID_YES)
    CurrentSession->Save();
  else if (res == wxID_CANCEL)
    return (false);
  delete CurrentSession;
  CurrentSession = new WiredSession("");

  Seq->Stop();

  SeqMutex.Lock();
  
  WaveCenter.Clear();

  SeqPanel->DeleteAllTracks();
  RackPanel->DeleteAllRacks();
  OptPanel->DeleteTools();

  SeqMutex.Unlock();
  return (true);
}

void					MainWindow::OnOpen(wxCommandEvent &event)
{
  vector<string>			exts;
  FileLoader				*dlg;
  
  exts.insert(exts.begin(), "wrd\tWired session file (*.wrd)");
  dlg = new FileLoader(this, MainWin_FileLoader, "Open session", false, false, &exts);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();    

      cout << "[MAINWIN] User opens " << selfile << endl;
      if (!NewSession())
	{
	  dlg->Destroy();
	  return;
	}
      CurrentSession = new WiredSession(selfile);
      CurrentSession->Load();
    }
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  dlg->Destroy();
}

void					MainWindow::OnSave(wxCommandEvent &event)
{
  if (!CurrentSession->FileName.empty())
    CurrentSession->Save();
  else
    OnSaveAs(event);
}

void					MainWindow::OnSaveAs(wxCommandEvent &event)
{
  vector<string>			exts;
  FileLoader				*dlg;
  
  exts.insert(exts.begin(), "wrd\tWired session file (*.wrd)");
  dlg = new FileLoader(this, MainWin_FileLoader, "Save session", false, true, &exts);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();    

      wxFileName f(selfile.c_str());
      if (!f.HasExt())
	selfile = selfile + WIRED_FILE_EXT;
      cout << "[MAINWIN] User saves to " << selfile << endl;

      string audiodir;
      
      if (CurrentSession)
	{
	  audiodir = CurrentSession->AudioDir;
	  delete CurrentSession;
	}
      CurrentSession = new WiredSession(selfile, audiodir);
      CurrentSession->Save();
    }
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  dlg->Destroy();
}

void					MainWindow::OnImportWave(wxCommandEvent &event)
{
  //  TransportPanel->OnStop(event);
  FileLoader				*dlg;

  dlg = new FileLoader(this, MainWin_FileLoader, "Loading sound file", false, false, NULL);
  /*  Connect(FileLoader_Start, wxEVT_COMMAND_BUTTON_CLICKED, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&MainWindow::OnFileLoaderStart);
  
  Connect(FileLoader_Stop, wxEVT_COMMAND_BUTTON_CLICKED, 
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&MainWindow::OnFileLoaderStop);*/
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();
      
      dlg->Destroy();
      wxMessageDialog msg(this, 
			  "Do you want to copy this file to your project directory ?", "Wired", 
			  wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE);
      int res = msg.ShowModal();
      if (res == wxID_YES)
	{
	  if (CurrentSession->AudioDir.empty())
	    {
	      wxDirDialog dir(this, "Choose the Audio file directory", 
			      wxFileName::GetCwd());
	      if (dir.ShowModal() == wxID_OK)
		CurrentSession->AudioDir = dir.GetPath().c_str(); 
	      else
		res = wxID_CANCEL;		
	    }
	  if (res != wxID_CANCEL)
	    {
	      wxFileName fn(selfile.c_str());
	      
	      fn.SetPath(CurrentSession->AudioDir.c_str());		
	      if (!wxCopyFile(selfile.c_str(), fn.GetFullPath().c_str()))
		{
		  wxMessageDialog copymsg(this, 
					  "Could not copy file", "Wired", 
					  wxOK | wxICON_EXCLAMATION |wxCENTRE);
		  copymsg.ShowModal();
		  res = wxID_CANCEL;
		}
	      else
		selfile = fn.GetFullPath().c_str();
	    }
	}
      if (res != wxID_CANCEL)
	{
	  wxProgressDialog *Progress = new wxProgressDialog("Loading wave file", "Please wait...", 100, 
							    this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT 
							    | wxPD_REMAINING_TIME);
	  Progress->Update(1);
	  cImportWaveAction* action = new cImportWaveAction(selfile, true);
	  action->Do();
	  Progress->Update(99);	
	  delete Progress;
	}
    }
  else
    {
      dlg->Destroy();  
      cout << "[MAINWIN] User cancels open dialog" << endl;
    }
}

void					MainWindow::OnImportMIDI(wxCommandEvent &event)
{
  vector<string>			exts;
  FileLoader				*dlg;
  
  exts.insert(exts.begin(), "mid\tMidi file (*.mid)");
  dlg = new FileLoader(this, MainWin_FileLoader, "Import MIDI file", false, false, &exts);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();

      cout << "[MAINWIN] Users imports MIDI file : " << selfile << endl;
      wxProgressDialog *Progress = new wxProgressDialog("Loading midi file", "Please wait...", 100, 
							this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT 
							| wxPD_REMAINING_TIME);
      Progress->Update(1);
      cImportMidiAction* action = new cImportMidiAction(selfile, false);
      action->Do();
      Progress->Update(99);	
      delete Progress;
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
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  dlg->Destroy();
}

void					MainWindow::OnImportAKAI(wxCommandEvent &event)
{
  //TransportPanel->OnStop(event);
  FileLoader				*dlg;
  
  dlg = new FileLoader(this, MainWin_FileLoader, "Import AKAI samples", true, false, NULL);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();

      wxProgressDialog *Progress = new wxProgressDialog("Loading midi file", "Please wait...", 100, 
							this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT 
							| wxPD_REMAINING_TIME);
      Progress->Update(1);
      cImportAkaiAction* action = new cImportAkaiAction(selfile, true);
      action->Do();
      Progress->Update(99);	
      delete Progress;
      /*
	cout << "[MAINWIN] Users imports AKAI sample : " << selfile << endl;
	string dev = selfile.substr(0, selfile.find(":", 0));
	selfile = selfile.substr(selfile.find(":", 0) + 1, selfile.size() - selfile.find(":", 0));
	string path = selfile.substr(10, selfile.size() - 10);
	unsigned int pos = path.find("/", 0);
	int part = path.substr(0, pos).c_str()[0] - 64;
	path = path.substr(pos, path.size() - pos);
	int opos = 0;
	while ((pos = path.find("/", opos)) != string::npos)
	opos = pos + 1;
	string name = path.substr(opos, path.size() - opos);
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
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  dlg->Destroy();
}

void					MainWindow::OnExportWave(wxCommandEvent &event)
{
  //  TransportPanel->OnStop(event);
  FileLoader				*dlg;
  double total = Seq->EndLoopPos - Seq->BeginLoopPos; 

  if (total <= 0)
    {
      wxMessageDialog msg(this, "Please correctly place the Left and Right markers", "Wired", 
			  wxOK | wxICON_EXCLAMATION | wxCENTRE);
      msg.ShowModal();
      return;
    }
  
  dlg = new FileLoader(this, MainWin_FileLoader, "Exporting sound file", false, true, NULL);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();    
      
      dlg->Destroy();
      wxFileName f(selfile.c_str());
      if (f.GetExt().IsEmpty())
	{
	  f.SetExt("wav");
	  selfile = f.GetFullPath();
	}      
      cout << "[MAINWIN] User exports " << selfile << endl;
      Seq->ExportToWave(selfile);

      wxProgressDialog *Progress = new wxProgressDialog("Exporting song", "Please wait...", 
						       100, this, 
						       wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
      int up = 0;

      bool done = false;

      while (!done)
	{
	  up += 10;
	  if (up > 99)
	    up = 99;
	  //Progress->Update(up); ** Generates segfault in pango
	  //cout << "pos: " << Seq->CurrentPos << "; end: " << Seq->EndLoopPos << endl;
	  wxMilliSleep(50);
	  SeqMutex.Lock();
	  if (Seq->CurrentPos >= Seq->EndLoopPos)
	    done = true;
	  SeqMutex.Unlock();
	}
      delete Progress;
    }
  else
    {
      dlg->Destroy();  
      cout << "[MAINWIN] User cancels open dialog" << endl;
    }
}

void					MainWindow::OnExportMIDI(wxCommandEvent &event)
{
  vector<string>			exts;
  FileLoader				*dlg;
  
  exts.insert(exts.begin(), "mid\tMidi file (*.mid)");
  dlg = new FileLoader(this, MainWin_FileLoader, 
		       "Export MIDI file", false, true, &exts);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();
      cout << "[MAINWIN] Users exports MIDI file : " << selfile << endl;
      
    }
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  dlg->Destroy();
}

void					MainWindow::LoadPlugins()
{
  string				str;
  PluginLoader				*p;
  
  if (!PluginsConfFile.Open(WiredSettings->PlugConfFile.c_str()))
    {
      cerr << "Could not load " << PLUG_CONF_FILE << endl;
      return;
    }
  PluginMenuIndexCount = PLUG_MENU_INDEX_START;
  for (str = PluginsConfFile.GetFirstLine(); 42;
       str = PluginsConfFile.GetNextLine())
    {
      if ((str.length() > 0) && (str.at(0) != '#'))
	{
	  p = new PluginLoader(str);
	  LoadedPluginsList.push_back(p);
	  
	  p->Id = PluginMenuIndexCount++;
	  if (p->InitInfo.Type == PLUG_IS_INSTR)
	    {
	      CreateInstrMenu->Append(p->Id, (p->InitInfo.Name).c_str());
	      Connect(p->Id, wxEVT_COMMAND_MENU_SELECTED, 
		      (wxObjectEventFunction)(wxEventFunction) 
		      (wxCommandEventFunction)&MainWindow::OnCreateRackClick);
	    }
	  else
	    {
	      CreateEffectMenu->Append(p->Id, (p->InitInfo.Name).c_str());
	      Connect(p->Id, wxEVT_COMMAND_MENU_SELECTED, 
		      (wxObjectEventFunction)(wxEventFunction) 
		      (wxCommandEventFunction)&MainWindow::OnCreateEffectClick);
	    }
	  cout << "[MAINWIN] Plugin " << p->InitInfo.Name << " is working" << endl;
	}
      if (PluginsConfFile.Eof())
	break;
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
      cout << "[MAINWIN] Creating rack for plugin: " << p->InitInfo.Name << endl;     
      //      if ((plug = p->CreateRack(StartInfo)))
      // RackPanel->AddTrack(StartInfo, p);
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
      cout << "[MAINWIN] Creating rack for plugin: " << p->InitInfo.Name << endl;     
      // RackPanel->AddToSelectedTrack(StartInfo,  p);
      cCreateEffectAction* action = new cCreateEffectAction(&StartInfo,  p);
      action->Do();
    }
}

void					MainWindow::OnDeleteRack(wxCommandEvent &event)
{
  vector<RackTrack *>::iterator		i;
  vector<Plugin *>::iterator		j;
  vector<PluginLoader *>::iterator	k;
  
  wxMutexLocker m(SeqMutex);
  if (RackPanel->selectedPlugin)
    {
      /*  for (i = RackPanel->RackTracks.begin(); i != RackPanel->RackTracks.end(); 
	  i++)
	  for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	  {*/
      for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
	if (COMPARE_IDS((*k)->InitInfo.UniqueId, RackPanel->selectedPlugin->InitInfo->UniqueId))
	  {
	    cout << "[MAINWIN] Destroying plugin: " 
		 << RackPanel->selectedPlugin->Name << endl;
	    RackPanel->RemoveChild(RackPanel->selectedPlugin);
	    (*k)->Destroy(RackPanel->selectedPlugin);
	    break;
	  }
      RackPanel->DeleteRack(RackPanel->selectedPlugin);     
    }
  /* return;
     } */
}

//#include <unistd.h>

void					MainWindow::StartStream(wxCommandEvent &event)
{
  Audio->StartStream();
}

void					MainWindow::StopStream(wxCommandEvent &event)
{
  Audio->StopStream();
}

void					MainWindow::OnAddTrackAudio(wxCommandEvent &event)
{
  cAddTrackAction			*action = new cAddTrackAction(true);

  action->Do();
  //SeqPanel->AddTrack(true);
}

void					MainWindow::OnAddTrackMidi(wxCommandEvent &event)
{
  cAddTrackAction			*action = new cAddTrackAction(false);

  action->Do();
  // SeqPanel->AddTrack(false);
}

void					MainWindow::OnFloatTransport(wxCommandEvent &event)
{
  if (WindowMenu->IsChecked(MainWin_FloatTransport))
    {
      TransportFrame = new wxFrame(0x0, -1, "Transport", TransportPanel->GetPosition(), TransportPanel->GetSize());
      TransportPanel->Reparent(TransportFrame);
      TransportFrame->Show();
    }
  else
    {
      TransportPanel->Reparent(this);
      delete TransportFrame;
      TransportFrame = 0x0;
    }
}

void					MainWindow::OnFloatSequencer(wxCommandEvent &event)
{
  if (WindowMenu->IsChecked(MainWin_FloatSequencer))
    {
      SequencerFrame = new wxFrame(0x0, -1, "Sequencer", SeqPanel->GetPosition(), SeqPanel->GetSize());
      SeqPanel->Reparent(SequencerFrame);
      SequencerFrame->Show();
    }
  else
    {
      SeqPanel->Reparent(this);
      delete SequencerFrame;
      SequencerFrame = 0x0;
    }
}

void					MainWindow::OnFloatRack(wxCommandEvent &event)
{
  if (WindowMenu->IsChecked(MainWin_FloatRacks))
    {
      RackFrame = new wxFrame(0x0, -1, "Racks", RackPanel->GetPosition(), RackPanel->GetSize());
      RackPanel->Reparent(RackFrame);
      RackFrame->Show();
    }
  else
    {
      RackPanel->Reparent(this);
      delete RackFrame;
      RackFrame = 0x0;
    }
}

void MainWindow::OnSwitchRackOptViewEvent(wxCommandEvent &event)
{
  SwitchRackOptView();
}

void MainWindow::OnSwitchSeqOptViewEvent(wxCommandEvent &event)
{
  SwitchSeqOptView();
}

void					MainWindow::SwitchRackOptView()
{
  if (!SeqModeView)
    SwitchSeqOptView();

  RackModeView = !RackModeView;
  if (RackModeView)
    {
      OptPanel->Show(false);
      OptPanel->SetSize(wxSize(470, 150));
      OptPanel->Reparent(this);
      
      RackPanel->Reparent(split);      
      OptPanel->Show(true);
      //split->SplitHorizontally(RackPanel, SeqPanel);
      split->ReplaceWindow(OptPanel, RackPanel);

      BottomSizer = new wxBoxSizer(wxHORIZONTAL);
      BottomSizer->Add(TransportPanel, 0, wxEXPAND | wxALL, 2); 
      BottomSizer->Add(OptPanel, 1, wxEXPAND | wxALL, 2); 
  
      TopSizer = new wxBoxSizer(wxVERTICAL);
      
      TopSizer->Add(split, 1, wxEXPAND | wxALL, 2);
      TopSizer->Add(BottomSizer, 0, wxEXPAND | wxALL, 0);
      SetSizer(TopSizer);
    }
  else
    {
      RackPanel->SetSize(wxSize(470, 150));
      RackPanel->Reparent(this);
      RackPanel->SetPosition(wxPoint(306, 452));
      
      OptPanel->Reparent(split);
      split->ReplaceWindow(RackPanel, OptPanel);
      //split->SplitHorizontally(OptPanel, SeqPanel);


      BottomSizer = new wxBoxSizer(wxHORIZONTAL);
      BottomSizer->Add(TransportPanel, 0, wxEXPAND | wxALL, 2); 
      BottomSizer->Add(RackPanel, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2); 
  
      TopSizer = new wxBoxSizer(wxVERTICAL);
      
      TopSizer->Add(split, 1, wxEXPAND | wxALL, 2);
      TopSizer->Add(BottomSizer, 0, wxEXPAND | wxALL, 0);
      SetSizer(TopSizer);

      RackPanel->SetSize(wxSize(470, 150));
    }
}

void					MainWindow::SwitchSeqOptView()
{
  if (!RackModeView)
    SwitchRackOptView();

  SeqModeView = !SeqModeView;
  if (SeqModeView)
    {
      OptPanel->Show(false);
      OptPanel->SetSize(wxSize(470, 150));
      OptPanel->Reparent(this);
      
      SeqPanel->Reparent(split);      
      OptPanel->Show(true);
      //split->SplitHorizontally(SeqPanel, SeqPanel);
      split->ReplaceWindow(OptPanel, SeqPanel);

      BottomSizer = new wxBoxSizer(wxHORIZONTAL);
      BottomSizer->Add(TransportPanel, 0, wxEXPAND | wxALL, 2); 
      BottomSizer->Add(OptPanel, 1, wxEXPAND | wxALL, 2); 
  
      TopSizer = new wxBoxSizer(wxVERTICAL);
      
      TopSizer->Add(split, 1, wxEXPAND | wxALL, 2);
      TopSizer->Add(BottomSizer, 0, wxEXPAND | wxALL, 0);
      SetSizer(TopSizer);
    }
  else
    {
      SeqPanel->SetSize(wxSize(470, 150));
      SeqPanel->Reparent(this);
      SeqPanel->SetPosition(wxPoint(306, 452));
      
      OptPanel->Reparent(split);
      split->ReplaceWindow(SeqPanel, OptPanel);
      //split->SplitHorizontally(OptPanel, SeqPanel);

      BottomSizer = new wxBoxSizer(wxHORIZONTAL);
      BottomSizer->Add(TransportPanel, 0, wxEXPAND | wxALL, 2); 
      BottomSizer->Add(SeqPanel, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE, 2); 
  
      TopSizer = new wxBoxSizer(wxVERTICAL);
      
      TopSizer->Add(split, 1, wxEXPAND | wxALL, 2);
      TopSizer->Add(BottomSizer, 0, wxEXPAND | wxALL, 0);
      SetSizer(TopSizer);
    }
}

void					MainWindow::OnSettings(wxCommandEvent &event)
{
  SettingWindow				s;
  vector<Track *>::iterator		i;

  if (s.ShowModal() == wxID_OK)
    {
      SeqMutex.Lock();
      
      if (!Audio->CloseStream())
	{
	  cout 
	    << "[MAINWIN] Could not close audio stream, you may restart Wired" 
	    << endl;
	  /*
	    if (AudioMutex.TryLock() == wxMUTEX_NO_ERROR)
	    {
	    AudioMutex.Lock();
	    SeqMutex.Unlock();
	    }
	  */
	  SeqMutex.Unlock();
	  return;
	}
      try 
	{ 
	  Audio->GetDeviceSettings();
	  Mix->InitOutputBuffers();
	  
	  // Refill tracks connections
	  if (s.AudioLoaded || s.MidiLoaded)
	    for (i = Seq->Tracks.begin(); i != Seq->Tracks.end(); i++)
	      (*i)->TrackOpt->FillChoices();      
	  Audio->IsOk = true;
	  // Sends sample rate and buffer size modifications to plugins
	  if (s.AudioLoaded)
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
	}
      catch (Error::NoDevice)
	{
	  Audio->IsOk = false;
	  cout << "[MAINWIN] No Device :\nplease check you have a soundcard and Alsa installed" 
	       << endl;
	  if (AudioMutex.TryLock() == wxMUTEX_NO_ERROR)
	    {
	      AudioMutex.Lock();/* This will lock the sequencer		\
				   until audio parameters are		\
				   properly set				*/
	      SeqMutex.Unlock();
	    }
	}
      catch (Error::InvalidDeviceSettings)
	{
	  cout << "[MAINWIN] Invalid Device Settings" << endl;
	  Audio->IsOk = false;
	  AlertDialog("audio engine", 
		      "you may check for your audio settings if you want to use wired..");
	  if (AudioMutex.TryLock() == wxMUTEX_NO_ERROR)
	    {
	      AudioMutex.Lock();/* This will lock the sequencer		\
				   until audio parameters are		\
				   properly set				*/
	      SeqMutex.Unlock();
	    }
	}
      if ( Audio->IsOk )
	{
	  Audio->OpenStream();
	  if (Audio->StartStream())
	    AudioMutex.Unlock();/* This will unlock the sequencer	\
				   as audio parameters are properly set	*/
	  else
	    cout << "[MAINWIN] AudioMutex still locked"<< endl;
	}
      SeqMutex.Unlock();
      
      if (s.MidiLoaded)
	{
	  MidiDeviceMutex.Lock();
	  // Reopen midi devices
	  MidiEngine->OpenDefaultDevices(); 
	  MidiDeviceMutex.Unlock();	  
	}
    }
  else
    {
    }
}

void					MainWindow::AlertDialog(const wxString& from, const wxString& msg)
{
  wxMessageDialog			mdialog(this, msg, from, wxOK, wxDefaultPosition);

  mdialog.ShowModal();  
}

void					MainWindow::OnDeleteTrack(wxCommandEvent &event)
{
  SeqPanel->DeleteSelectedTrack();
}

void					MainWindow::OnChangeAudioDir(wxCommandEvent &event)
{
  assert(CurrentSession);
  
  wxDirDialog dir(this, "Choose the Audio file directory", 
		  wxFileName::GetCwd());
  if (dir.ShowModal() == wxID_OK)
    CurrentSession->AudioDir = dir.GetPath().c_str();    
}

void					MainWindow::OnUndo(wxCommandEvent &event)
{
  cActionManager::Global().Undo();
}

void					MainWindow::OnRedo(wxCommandEvent &event)
{
  cActionManager::Global().Redo();
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
  if (splashbtm.LoadFile(string(WiredSettings->DataDir +
				string("ihm/splash/splash.png")).c_str(), 
			 wxBITMAP_TYPE_PNG))
    {
      wxSplashScreen* splash = new wxSplashScreen(splashbtm,
						  wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
						  6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
						  wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    }
  //wxYield();
}

/*void MainWindow::OnKey(wxKeyEvent &event)
  {
  if (event.GetKeyCode() == WXK_SPACE)
  cout << "oouaaaaaa" << endl;
  else
  event.Skip();
  }*/

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
  wxCommandEvent			f;
  CursorEvent				e;
  list<Pattern *>::iterator		i;
  list<MidiPattern *>::iterator		j;
  list<Plugin *>::iterator		k;

  SeqMutex.Lock();

  MixerPanel->OnMasterChange(f);
  if (Seq->Playing)
    {
      SeqPanel->OnSetPosition(e);
      if (Seq->Recording)
	{
	  for (i = Seq->PatternsToResize.begin(); i != Seq->PatternsToResize.end(); i++)
	    {
	      f.SetEventObject((wxObject *)*i);
	      SeqPanel->OnResizePattern(f);
	    }	  
	  Seq->PatternsToResize.clear();
	  for (j = Seq->PatternsToRefresh.begin(); j != Seq->PatternsToRefresh.end(); j++)
	    {
	      f.SetEventObject((wxObject *)*j);
	      SeqPanel->OnDrawMidi(f);
	    }	 
	  Seq->PatternsToRefresh.clear();
	}
    }

  for (k = UpdatePlugins.begin(); k != UpdatePlugins.end(); k++)
    (*k)->Update();
  UpdatePlugins.clear();

  SeqMutex.Unlock();
}

void					MainWindow::AddUpdatePlugin(Plugin *p)
{
  UpdatePlugins.push_back(p);
}

void					MainWindow::OnFileLoaderStart(wxCommandEvent &event)
{
  FileLoader				*f = (FileLoader *)event.GetEventObject();

  wxFileName fn(f->GetSelectedFile().c_str());      
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

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxSetCursorPos, 313131)
END_DECLARE_EVENT_TYPES()

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
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
  EVT_MENU(MainWin_Undo, MainWindow::OnUndo) 
  EVT_MENU(MainWin_Redo, MainWindow::OnRedo)
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
  EVT_CLOSE(MainWindow::OnClose)
  EVT_TIMER(MainWin_SeqTimer, MainWindow::OnTimer)
  EVT_BUTTON(FileLoader_Start, MainWindow::OnFileLoaderStart)
  EVT_BUTTON(FileLoader_Stop, MainWindow::OnFileLoaderStop)
  //EVT_IDLE(MainWindow::OnIdle)
  //EVT_TEXT_MAXLEN(101010, MainWindow::OnSetPosition)
  //EVT_PLAYPOSITION(313131, MainWindow::OnSetPosition)
END_EVENT_TABLE()

