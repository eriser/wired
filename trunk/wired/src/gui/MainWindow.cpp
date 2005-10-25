// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "MainWindow.h"

#include <dlfcn.h>
#include <wx/splitter.h>
#include <wx/progdlg.h>
#include <wx/filename.h>
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
WiredSessionXml				*CurrentXmlSession;
WiredExternalPluginMgr		*LoadedExternalPlugins;
FileConversion				*FileConverter;

MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size)
  : wxFrame((wxFrame *) NULL, -1, title, pos, size, 
	    wxDEFAULT_FRAME_STYLE | wxWS_EX_PROCESS_IDLE)
{
  WiredSettings = new Settings(); // FIXME catch what we can here

  CurrentSession = new WiredSession("");
  CurrentXmlSession = new WiredSessionXml("");
  LoadedExternalPlugins = new WiredExternalPluginMgr();
  
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
  UndoMenu = new wxMenu;
  RedoMenu = new wxMenu;
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

  cout << "Known warning ...." << endl;  
    
  SeqPanel = new SequencerGui(split, wxPoint(0, 254), wxSize(800, 200));
  cout << "done :-)" << endl;  

  //  OptPanel = new OptionPanel(this, wxPoint(306, 452), wxSize(470, 150), wxSIMPLE_BORDER);
  TransportPanel = new Transport(this, wxPoint(0, 452), wxSize(300, 150), wxNO_BORDER);

  split->SplitHorizontally(RackPanel, SeqPanel);
  
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

  LoadExternalPlugins();

  RackModeView = true;
  SeqModeView = true;
  InitFileConverter();

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


  InitUndoRedoMenuItems();

  SeqTimer = new wxTimer(this, MainWin_SeqTimer);
  SeqTimer->Start(40);
}

void					MainWindow::InitFileConverter()
{
	FileConverter = new FileConversion();
	t_samplerate_info info;
	
	
	wxDirDialog dir(this, "Choose the audio working directory", wxFileName::GetCwd(), wxDD_NEW_DIR_BUTTON | wxCAPTION | wxSUNKEN_BORDER);
	if (dir.ShowModal() == wxID_OK)
		CurrentXmlSession->GetAudioDir() = dir.GetPath().c_str();
	else
		CurrentXmlSession->GetAudioDir() = wxFileName::GetCwd();
	info.WorkingDirectory = CurrentXmlSession->GetAudioDir();
	info.SampleRate = (unsigned long) Audio->SampleRate;
	info.SamplesPerBuffer = (unsigned long) Audio->SamplesPerBuffer;
	if (FileConverter->Init(&info, string(CurrentXmlSession->GetAudioDir()), (unsigned long) Audio->SamplesPerBuffer, this) == false)
		cout << "[MAINWIN] Create file converter thread failed !" << endl; 
}

void					MainWindow::InitUndoRedoMenuItems()
{
	EditMenu->Insert(INDEX_MENUITEM_UNDO, MainWin_Undo, "U&ndo", UndoMenu);
	EditMenu->Insert(INDEX_MENUITEM_REDO, MainWin_Redo, "&Redo", RedoMenu);
	EditMenu->Enable(MainWin_Undo, false);
	EditMenu->Enable(MainWin_Redo, false);
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

  WiredSettings->Save();
  cout << "[MAINWIN] Unloading shared libraries..."<< endl;
  for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
    (*k)->Unload();
    
  cout << "[MAINWIN] Unloading external plugins..." << endl;
  if (Seq)
  {
	  Seq->Delete();
	  delete Seq;
  }
  if (LoadedExternalPlugins)
	  delete LoadedExternalPlugins;
  
  if(FileConverter)
  {
	  //FileConverter->Stop();
    delete FileConverter;
  }
  if (Audio)
	  delete Audio;
  if (MidiEngine)
	{
  	MidiEngine->Delete();
	  delete MidiEngine;
	}
  if (WiredSettings)
	  delete WiredSettings;
  cout << "[MAINWIN] Closing..."<< endl; 
  exit(0);
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

  wxMessageDialog			msg(this, "Save current session ?", "Wired", 
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
  CurrentXmlSession = new WiredSessionXml("");

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
  vector<string>			exts;
  FileLoader				*dlg;
  
  exts.insert(exts.begin(), "wrd\tWired session file (*.wrd)");
  exts.insert(exts.begin(), "xml\tWired session file (*.xml)");
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
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  dlg->Destroy();  
}

void					MainWindow::OnSave(wxCommandEvent &event)
{
	if (CurrentXmlSession)
	{
		std::string			DocumentName(CurrentXmlSession->GetDocumentName());
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
  vector<string>			exts;
  FileLoader				*dlg;
  
  exts.insert(exts.begin(), "xml\tWired session file (*.xml)");
  dlg = new FileLoader(this, MainWin_FileLoader, "Save session", false, true, &exts);
  if (dlg->ShowModal() == wxID_OK)
    {
      string selfile = dlg->GetSelectedFile();    

      wxFileName f(selfile.c_str());
      if (!f.HasExt())
	selfile = selfile + XML_EXTENSION;
      cout << "[MAINWIN] User saves to " << selfile << endl;

      string audiodir;
      
      if (CurrentXmlSession)
	{
	audiodir = std::string(CurrentXmlSession->GetAudioDir());
	if (audiodir.size() == 0)
	{
		char	buffer[2048];
		
		getcwd(buffer, 2048);
		audiodir = buffer;
	}  	
	  delete CurrentXmlSession;
	}
      CurrentXmlSession = new WiredSessionXml(selfile, audiodir);
      CurrentXmlSession->Save();
    }
  else
    cout << "[MAINWIN] User cancels open dialog" << endl;
  dlg->Destroy();
}

void					MainWindow::OnImportWave(wxCommandEvent &event)
{
  FileLoader				*dlg = new FileLoader(this, MainWin_FileLoader, "Loading sound file", false, false, FileConverter->GetCodecsExtensions(), true);
  int						res;

  if (dlg->ShowModal() == wxID_OK)
    {
      string 	selfile = dlg->GetSelectedFile();
      
      //dlg->Destroy();
      
      if (CurrentXmlSession->GetAudioDir().empty() == false)
	      res = wxID_OK;
      else	     
      {
	    wxDirDialog dir(this, "Choose the Audio file directory", wxFileName::GetCwd());
		if (dir.ShowModal() == wxID_OK)
		{
			CurrentXmlSession->GetAudioDir() = dir.GetPath().c_str(); 
			res = wxID_OK;
		}
	    else
			res = wxID_CANCEL;
		//dir.Destroy();
      }
      if (res != wxID_CANCEL)
      {
      	//SeqMutex.Lock();
      	MidiMutex.Lock();
      	MidiDeviceMutex.Lock();
      	//Audio->StopStream();
      	AudioMutex.Lock();
      	SeqMutex.Unlock();
      	//int result = Seq->Pause();
      	//MidiEngine->Pause();
//      	cout << "Result == " << result << "wxTHREAD_NO_ERROR == " << wxTHREAD_NO_ERROR << ", wxTHREAD_RUNNING" << wxTHREAD_RUNNING << endl;
     	//cout << "[MAINWIN] Importing file" << endl;
      	FileConverter->ConvertFromCodec(&selfile);
      	//cout << "[MAINWIN] Importing file 01" << endl;
		FileConverter->ConvertSamplerate(&selfile);
      	//cout << "[MAINWIN] Importing file 02" << endl;
      	FileConverter->ImportWaveFile(&selfile);
      	//cout << "[MAINWIN] Importing file done" << endl;
      	//Seq->Resume();
      	//MidiEngine->Resume();
      	//SeqMutex.Unlock();
      	MidiMutex.Unlock();  
      	MidiDeviceMutex.Unlock();
      	AudioMutex.Unlock();
      	//Audio->StartStream();    	
      }
    }
  else
    {
      //dlg->Destroy();  
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
      
      //dlg->Destroy();
      wxFileName f(selfile.c_str());
      if (f.GetExt().IsEmpty())
	{
	  f.SetExt("wav");
	  selfile = f.GetFullPath();
	}      
      cout << "[MAINWIN] User exports " << selfile << endl;
      if (Seq->ExportToWave(selfile) == false)
      {
      	cout << "[MAINWIN] Export canceled by user " << endl;
      	return;
      }

      //wxProgressDialog Progress("Exporting mix", "Please wait...", 
	//					       (int)Seq->EndLoopPos * 1000, this, 
		//				       wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
      bool done = false;

      while (!done)
	{
	  //Progress.Update((int) Seq->CurrentPos * 1000);
	  //cout << "pos: " << Seq->CurrentPos << "; end: " << Seq->EndLoopPos << endl;
	  wxMilliSleep(50);
	  //SeqMutex.Lock();
	  if (Seq->CurrentPos >= Seq->EndLoopPos)
	    done = true;
	  //SeqMutex.Unlock();
	}
    }
  else
    {
      //dlg->Destroy();  
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

void					MainWindow::LoadExternalPlugins()
{ 
  map<int, string>				PluginsList;
  map<int, string>::iterator	IterPluginsList;
  int							PluginInfo;
  
  CreateDSSIInstrMenu = NULL;
  CreateLADSPAInstrMenu = NULL;
  CreateDSSIEffectMenu = NULL;
  CreateLADSPAEffectMenu = NULL;
  LoadedExternalPlugins->LoadPLugins(TYPE_PLUGINS_DSSI | TYPE_PLUGINS_LADSPA);
  LoadedExternalPlugins->SetStartInfo(StartInfo);
  PluginsList = LoadedExternalPlugins->GetPluginsList();
  for (IterPluginsList = PluginsList.begin(); IterPluginsList != PluginsList.end(); IterPluginsList++)
  {
  	PluginInfo = LoadedExternalPlugins->GetPluginType(IterPluginsList->first);

  	LoadedExternalPlugins->SetMenuItemId(IterPluginsList->first, 
  		AddPluginMenuItem(PluginInfo, PluginInfo & TYPE_PLUGINS_EFFECT, IterPluginsList->second));
  }
}

int						MainWindow::AddPluginMenuItem(int Type, bool IsEffect, const string& MenuName)
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
				CreateEffectMenu->Append(Id, "DSSI", CreateDSSIEffectMenu);
				Id = PluginMenuIndexCount++;
			}
			NewItem = CreateDSSIEffectMenu->Append(Id, MenuName.c_str());
		}
		else if (Type & TYPE_PLUGINS_LADSPA)
		{
			if (!CreateLADSPAEffectMenu)
			{
				CreateLADSPAEffectMenu = new wxMenu();
				CreateEffectMenu->Append(Id, "LADSPA", CreateLADSPAEffectMenu);
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
				CreateInstrMenu->Append(Id, "DSSI", CreateDSSIInstrMenu);
				Id = PluginMenuIndexCount++;
			}
			NewItem = CreateDSSIInstrMenu->Append(Id, MenuName.c_str());
		}
		else if (Type & TYPE_PLUGINS_LADSPA)
		{
			if (!CreateLADSPAInstrMenu)
			{
				CreateLADSPAInstrMenu = new wxMenu();
				CreateInstrMenu->Append(Id, "LADSPA", CreateLADSPAInstrMenu);
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
		cout << "[MAINWIN] Creating rack for plugin: " << NewPlugin->InitInfo.Name << endl;
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
      cout << "[MAINWIN] Creating rack for plugin: " << p->InitInfo.Name << endl; 
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
      FileConverter->SetFormat((PaSampleFormat)Audio->SampleFormat);
      FileConverter->SetBufferSize(Audio->SamplesPerBuffer);		
      FileConverter->SetSampleRate(Audio->SampleRate);		
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
  
  wxDirDialog dir(this, "Choose the Audio file directory", 
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
		string		tmpString;
		
		if (separatorIndex > 0)
		{
			tmpString = (*iter)->label.c_str();
			if (count == 0)
				tmpString += "\tCtrl+Z";
			insertedMenuItem = undoMenu->Append((*iter)->id, tmpString);
			Connect((*iter)->id, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)(wxEventFunction) 
		      (wxCommandEventFunction)&MainWindow::OnUndo);
		}
		else
		{
			tmpString = (*iter)->label.c_str();
			if (separatorIndex == 0)
				tmpString += "\tCtrl+Shift+Z";
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
  list<Track *>::iterator		t;

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

  for (t = Seq->TracksToRefresh.begin(); t != Seq->TracksToRefresh.end(); t++)
    (*t)->TrackOpt->SetVuValue();
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
  EVT_CLOSE(MainWindow::OnClose)
  EVT_TIMER(MainWin_SeqTimer, MainWindow::OnTimer)
  EVT_BUTTON(FileLoader_Start, MainWindow::OnFileLoaderStart)
  EVT_BUTTON(FileLoader_Stop, MainWindow::OnFileLoaderStop)
  //EVT_IDLE(MainWindow::OnIdle)
  //EVT_TEXT_MAXLEN(101010, MainWindow::OnSetPosition)
  //EVT_PLAYPOSITION(313131, MainWindow::OnSetPosition)
END_EVENT_TABLE()

