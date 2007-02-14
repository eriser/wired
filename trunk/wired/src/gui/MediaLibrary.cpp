// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/filename.h>
#include <wx/treectrl.h>
#include <wx/toolbar.h>
#include <wx/tbarbase.h>

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
#include "../midi/MidiThread.h"
#include "../plugins/PluginLoader.h"
#include "../dssi/WiredExternalPluginMgr.h"
#include "FileConversion.h"
#include <config.h>
#include "Threads.h"
#include "MediaLibrary.h"
#include "MLTree.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "Colour.h"
#include "HelpPanel.h"
#include "SaveCenter.h"
#include "debug.h"

extern SaveCenter *saveCenter;

#ifndef DEBUG_1647295
const struct s_combo_choice		SortSelectChoices[NB_SORTSELECT_CHOICES + 1] =
{
  { wxT("filename")	,	1	},
  { wxT("filetype")	,	2	},
  { wxT("filesize")	,	4	},
  { wxT("Modified")	,	8	},
  { wxT("")	      	,	4242	}
};
#endif

MediaLibrary::MediaLibrary(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style)
  : wxPanel(parent, wxID_ANY, pos, size, style)
{
  wxString	sortselect_choices[NB_SORTSELECT_CHOICES];
  long		c;

  Parent = parent;
  preview = false;
  this->SetInvisible();
  this->SetDocked();
  SetBackgroundColour(ML_BACKGROUND);
  SetForegroundColour(ML_FOREGROUND);

  MLTreeView = new MLTree(this, wxDefaultPosition,
			  wxDefaultSize,
			  wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | wxTR_MULTIPLE | wxTR_HIDE_ROOT);
  MLTreeView->SetTreeExpanded();

  TopToolbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_3DBUTTONS);
  TopToolbar->AddTool(MediaLibrary_Add, _("Add"), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_ADDUP_IMG)), wxBITMAP_TYPE_PNG), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_ADDDO_IMG)), wxBITMAP_TYPE_PNG), 
		      wxITEM_NORMAL, _("Add a file"), _("Add a file"), NULL);

  TopToolbar->AddTool(MediaLibrary_Remove, _("Remove"), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_REMOVEUP_IMG)), wxBITMAP_TYPE_PNG), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_REMOVEDO_IMG)), wxBITMAP_TYPE_PNG), 
		      wxITEM_NORMAL, _("Remove file"), _("Remove file"), NULL);
  
  TopToolbar->AddTool(MediaLibrary_TreeCollapse, _("Expand/Collapse"), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_COLLAPSEUP_IMG)), wxBITMAP_TYPE_PNG), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_COLLAPSEDO_IMG)), wxBITMAP_TYPE_PNG), 
		      wxITEM_NORMAL, _("Expand/Collapse all"), _("Expand or Collapse all tree branches"), NULL);
  
  TopToolbar->AddTool(MediaLibrary_Insert, _("Insert"), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_INSERTUP_IMG)), wxBITMAP_TYPE_PNG), 
		      wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_INSERTDO_IMG)), wxBITMAP_TYPE_PNG), 
		      wxITEM_NORMAL, _("Insert file"), _("Insert file in a new track"), NULL);
  TopToolbar->Realize();

#ifndef DEBUG_DISABLE_FILTERS
  BottomToolbar = new wxToolBar(this, -1, wxPoint(-1, this->GetSize().y - 50), wxSize(1000, 46), wxTB_3DBUTTONS);
#else
  BottomToolbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_3DBUTTONS);
#endif
  BottomToolbar->AddTool(MediaLibrary_Start_Preview, _("Start Preview"), 
			 wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_PREVIEWUP_IMG)), wxBITMAP_TYPE_PNG), 
			 wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_PREVIEWDO_IMG)), wxBITMAP_TYPE_PNG), 
			 wxITEM_NORMAL, _("Preview file"), _("Preview a file"), NULL);
  
  BottomToolbar->AddTool(MediaLibrary_Stop_Preview, _("Stop Preview"), 
			 wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_PREVIEWDO_IMG)), wxBITMAP_TYPE_PNG), 
			 wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_PREVIEWDO_IMG)), wxBITMAP_TYPE_PNG), 
			 wxITEM_NORMAL, _("Stop Preview"), _("Stop Preview"), NULL);

#ifndef DEBUG_1647295
  for (c = 0; c < NB_SORTSELECT_CHOICES; c++)
    sortselect_choices[c] = SortSelectChoices[c].s;

  SortSelect = new wxComboBox(BottomToolbar, MediaLibrary_SortSelect, 
			      DEFAULT_SORTSELECT_VALUE, 
			      wxPoint(-1, -1), wxSize(100, -1), 
			      4, sortselect_choices, wxCB_READONLY);

  BottomToolbar->AddControl(SortSelect);
#endif
  BottomToolbar->Realize();
  BottomToolbar->EnableTool(5, false);
  BottomToolbar->EnableTool(6, false);

#ifndef DEBUG_DISABLE_FILTERS
  FiltersToolbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_3DBUTTONS);
  FiltersToolbar->AddCheckTool(MediaLibrary_FilterAudio, _("FAudio"), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEAUDIOUP_IMG)), wxBITMAP_TYPE_PNG), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEAUDIODO_IMG)), wxBITMAP_TYPE_PNG), 
			       _("Show/Hide Audio files"), _("Show or hide audio files from the Media Library"), NULL);

  FiltersToolbar->AddCheckTool(MediaLibrary_FilterMIDI, _("FMIDI"), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEMIDIUP_IMG)), wxBITMAP_TYPE_PNG), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEMIDIDO_IMG)), wxBITMAP_TYPE_PNG), 
			       _("Show/Hide MIDI files"), _("Show or hide MIDI files from the Media Library"), NULL);
  
  FiltersToolbar->AddCheckTool(MediaLibrary_FilterVideo, _("FVideo"), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEVIDEOUP_IMG)), wxBITMAP_TYPE_PNG), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEVIDEODO_IMG)), wxBITMAP_TYPE_PNG), 
			       _("Show/Hide Video files"), _("Show or hide video files from the Media Library"), NULL);
  
  FiltersToolbar->AddCheckTool(MediaLibrary_FilterEffects, _("FEffects"), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEFXUP_IMG)), wxBITMAP_TYPE_PNG), 
			       wxBitmap(wxString(WiredSettings->DataDir + wxString(MEDIALIBRARY_FILEFXDO_IMG)), wxBITMAP_TYPE_PNG), 
			       _("Show/Hide Effects Files"), _("Show or hide effects from the Media Library"), NULL);

  FiltersToolbar->ToggleTool(MediaLibrary_FilterAudio, 3);
  FiltersToolbar->ToggleTool(MediaLibrary_FilterMIDI, 3);
  FiltersToolbar->ToggleTool(MediaLibrary_FilterVideo, 3);
  FiltersToolbar->ToggleTool(MediaLibrary_FilterEffects, 3);
  FiltersToolbar->Realize();
#endif

  MLTreeView->Fit();

  TopSizer = new wxBoxSizer(wxVERTICAL);
  TopSizer->Add(TopToolbar, 0, wxFIXED_MINSIZE | wxALL | wxEXPAND, 0);
  TopSizer->Add(MLTreeView, 0, wxALIGN_TOP | wxALIGN_BOTTOM | wxALL | wxEXPAND, 0);
  TopSizer->Add(BottomToolbar, 0, wxFIXED_MINSIZE | wxALL | wxEXPAND, 0);
#ifndef DEBUG_DISABLE_FILTERS
  TopSizer->Add(FiltersToolbar, 0, wxALL | wxEXPAND, 0);
#endif

  SetSizer(TopSizer);


}

MediaLibrary::~MediaLibrary()
{
  if(TopToolbar)
    delete TopToolbar;

  if(MLTreeView)
    delete MLTreeView;

#ifndef DEBUG_1647295
  if(SortSelect)
    delete SortSelect;
#endif

  if(BottomToolbar)
    delete BottomToolbar;

#ifndef DEBUG_DISABLE_FILTERS
  if(FiltersToolbar)
    delete FiltersToolbar;
#endif
}

void				MediaLibrary::SetFileConverter(FileConversion *Fileconv)
{
  FileConverter = Fileconv;
}

bool				MediaLibrary::IsVisible()
{
  return (visible);
}

void				MediaLibrary::SetInvisible()
{
  visible = false;
}

void				MediaLibrary::SetVisible()
{
  visible = true;
}

bool				MediaLibrary::IsFloating()
{
  return (floating);
}

void				MediaLibrary::SetFloating()
{
  floating = true;
}

void				MediaLibrary::SetDocked()
{
  floating = false;
}


void				MediaLibrary::OnAdd(wxCommandEvent &WXUNUSED(event))
{
  FileLoader		dlg(this, MainWin_FileLoader, _("Loading sound file"), false, false, FileConverter->GetCodecsExtensions(), true);
  int			res;

  if (dlg.ShowModal() == wxID_OK)
    {
      wxString 	selfile = dlg.GetSelectedFile();

      MLTreeView->OnAdd(selfile);
    }
}

void				MediaLibrary::OnRemove(wxCommandEvent &WXUNUSED(event))
{
  cout << "Removing" << endl;
  MLTreeView->OnRemove();
}

void				MediaLibrary::OnCollapse(wxCommandEvent &WXUNUSED(event))
{
  MLTreeView->OnCollapse();
}

void				MediaLibrary::OnSize(wxSizeEvent &event)
{
  //we assume that each toolbar has a size of 50px (toolbar + margins + ...)
  Layout();
  MLTreeView->Fit();
  MLTreeView->SetSize(this->GetSize().x, this->GetSize().y - 100);

#ifndef DEBUG_DISABLE_FILTERS
  BottomToolbar->Move(0, this->GetSize().y - 100);
  FiltersToolbar->Move(0, this->GetSize().y - 50);
#else
  BottomToolbar->Move(0, this->GetSize().y - 50);
#endif
}

void				MediaLibrary::OnEdit(wxCommandEvent &WXUNUSED(event))
{
  cout << "[MEDIALIBRARY] Edit fille (OnEdit)" << endl;
  wxString			selfile;

  wxTreeItemId			item;
  s_nodeInfo			infos;

  selfile = MLTreeView->getSelection(1);
  item = MLTreeView->GetSelection();
  infos = MLTreeView->GetTreeItemStructFromId(item);
  // Test the selfile content HERE
  if (item == MLTreeView->GetRootItem() || selfile == wxT("")  || infos.extension == wxT(""))
    return ;
  MidiMutex.Lock();
  MidiDeviceMutex.Lock();
  AudioMutex.Lock();
  SeqMutex.Unlock();
  FileConverter->ConvertFromCodec(selfile);
  FileConverter->ConvertSamplerate(selfile);
  cActionManager::Global().AddEditWaveAction(selfile, eAudioTrack, true);
  MidiMutex.Unlock();
  MidiDeviceMutex.Unlock();
  AudioMutex.Unlock();
}

void				MediaLibrary::OnInsert(wxCommandEvent &WXUNUSED(event))
{
  wxString			selfile;
  wxTreeItemId			item;
  s_nodeInfo			infos;

  selfile = MLTreeView->getSelection(1);
  item = MLTreeView->GetSelection();
  infos = MLTreeView->GetTreeItemStructFromId(item);
  if (item == MLTreeView->GetRootItem() || selfile == wxT("")  || infos.extension == wxT(""))
    return ;
  // Test the selfile content HERE
  //  cout << "[MEDIALIBRARY] Insert File (OnInsert)" << selfile << endl;
  MidiMutex.Lock();
  MidiDeviceMutex.Lock();
  AudioMutex.Lock();
  SeqMutex.Unlock();
  FileConverter->ImportFile(selfile);
  MidiMutex.Unlock();
  MidiDeviceMutex.Unlock();
  AudioMutex.Unlock();
}

#ifndef DEBUG_DISABLE_FILTERS
void				MediaLibrary::OnFilterAudio(wxCommandEvent &WXUNUSED(event))
{
  //  cout << "[MEDIALIBRARY] Filter AudioFile (OnFilterAudio)" << endl;
}

void				MediaLibrary::OnFilterMIDI(wxCommandEvent &WXUNUSED(event))
{
  //  cout << "[MEDIALIBRARY] Filter MIDIFile (OnFilterMidi)" << endl;
}

void				MediaLibrary::OnFilterVideo(wxCommandEvent &WXUNUSED(event))
{
  //  cout << "[MEDIALIBRARY] Filter VideoFile (OnFilterVideo)" << endl;
}

void				MediaLibrary::OnFilterEffects(wxCommandEvent &WXUNUSED(event))
{
  //  cout << "[MEDIALIBRARY] Filter EffectsFile (OnFilterEffects)" << endl;
}
#endif

void                            MediaLibrary::Converts(wxString Path)
{
  FileConverter->ConvertSamplerateNoGraph(Path);

  /*SNDFILE				*Result;
  SF_INFO				Info;
  int					Res = wxID_NO;
  bool				SameSampleRate, SameFormat;
  
  Info.format = 0;
  if ((Result = sf_open(Path.mb_str(*wxConvCurrent), SFM_READ, &Info)) != NULL)
    {      cerr << "test1" << endl;
      SameFormat = Seq->GetSampleRateConverter()->IsSameFormat(Info.format, Seq->GetSampleRateConverter()->Get_Apllication_Setting().Format);
      cerr << "test2" << endl;
      SameSampleRate = (int)Info.samplerate == (int)Seq->GetSampleRateConverter()->Get_Apllication_Setting().SampleRate ? true : false;
      cerr << "test3" << endl;
      if (!SameFormat || !SameSampleRate)
	{
	  cerr << "test4" << endl;
	  Seq->GetSampleRateConverter()->Convert(&Info, Path, Result);
	}
      sf_close(Result);
      }*/
}

void				MediaLibrary::OnPreview(wxCommandEvent &WXUNUSED(event))
{
  wxString			selfile;
  wxTreeItemId		item;
  s_nodeInfo		infos;

  selfile = MLTreeView->getSelection(1);
  item = MLTreeView->GetSelection();
  infos = MLTreeView->GetTreeItemStructFromId(item);

  if (preview == false)
    {

      preview = true;
      BottomToolbar->EnableTool(5, false);
      BottomToolbar->EnableTool(6, true);
      FileConverter->ConvertSamplerateNoGraph(selfile);
      if (wxFileExists(saveCenter->getAudioDir() + selfile.AfterLast('/')))
	Seq->PlayFile(saveCenter->getAudioDir() + selfile.AfterLast('/'), false);
      else
	Seq->PlayFile(selfile, false);
    }
  else
    {
      preview = false;
      BottomToolbar->EnableTool(5, true);
      BottomToolbar->EnableTool(6, false);
      Seq->StopFile();
    }
}

#ifndef DEBUG_1647295
void				MediaLibrary::OnSortToggle(wxCommandEvent &WXUNUSED(event))
{
  wxString selected = SortSelect->GetValue();
  //  cout << "[MEDIALIBRARY] Sort Files (OnSortToggle) [" << selected << "]" << endl;
  MLTreeView->SortNodes(selected);
}
#endif

void				MediaLibrary::OnLeftClick(wxMouseEvent &event)
{
  SetFocus();
}

BEGIN_EVENT_TABLE(MediaLibrary, wxPanel)
  EVT_SIZE(MediaLibrary::OnSize)
  EVT_TOOL(MediaLibrary_Add, MediaLibrary::OnAdd)
  EVT_TOOL(MediaLibrary_Remove, MediaLibrary::OnRemove)
  // Commented for FORUM PURPOSE
  //  EVT_TOOL(MediaLibrary_Edit, MediaLibrary::OnEdit)
  EVT_TOOL(MediaLibrary_Insert, MediaLibrary::OnInsert)
  EVT_TOOL(MediaLibrary_Start_Preview, MediaLibrary::OnPreview)
  EVT_TOOL(MediaLibrary_Stop_Preview, MediaLibrary::OnPreview)
  EVT_TOOL(MediaLibrary_TreeCollapse, MediaLibrary::OnCollapse)
#ifndef DEBUG_1647295
  EVT_COMBOBOX(MediaLibrary_SortSelect, MediaLibrary::OnSortToggle)
  EVT_TEXT_ENTER(MediaLibrary_SortSelect, MediaLibrary::OnSortToggle)
#endif
#ifndef DEBUG_DISABLE_FILTERS
  EVT_TOOL(MediaLibrary_FilterAudio, MediaLibrary::OnFilterAudio)
  EVT_TOOL(MediaLibrary_FilterMIDI, MediaLibrary::OnFilterMIDI)
  EVT_TOOL(MediaLibrary_FilterVideo, MediaLibrary::OnFilterVideo)
  EVT_TOOL(MediaLibrary_FilterEffects, MediaLibrary::OnFilterEffects)
#endif
  EVT_LEFT_DOWN(MediaLibrary::OnLeftClick)
END_EVENT_TABLE()
