#include  <wx/wx.h>
#include  <wx/dialog.h>
#include  <wx/filename.h>
#include  <wx/stdpaths.h>
#include  <wx/string.h>
#include  <vector>
#include  "Wizard.h"
#include  "Settings.h"
#include  "wired_session_folder.xpm"
#include  "AudioEngine.h"
#include  "MediaLibrary.h"
#include   "MLTree.h"

extern	AudioEngine			*Audio;
extern	MediaLibrary		*MediaLibraryPanel;

#define  WIZ_WIN_WIDTH		600
#define  WIZ_WIN_HEIGHT		400
#define  WIZ_WIN_SIZE		wxSize(WIZ_WIN_WIDTH, WIZ_WIN_HEIGHT)
#define  WIZ_ICON_SIZE		32

#ifndef RESIZE_ICON
# define RESIZE_ICON(ico, w, h)	(wxBitmap(ico).ConvertToImage().Rescale(w, h))
#endif

#ifdef DEBUG_WIZARD
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : "  << __LINE__ << " : " << __FUNCTION__  << endl; }
#else
#define LOG
#endif

Wizard::Wizard()
  : wxDialog(NULL, -1, _("Wired Wizard"), wxDefaultPosition, wxDefaultSize,
	  wxDEFAULT_DIALOG_STYLE, _("Wired Wizard")),
	  b_NewProject(NULL),
	  tc_ProjectName(NULL),
	  tc_NewPath(NULL)
{
  LOG;
  chosenDir = wxT("");

  st_ProjectName	= new wxStaticText(this, -1, _("Project name :"));
  st_NewPath		= new wxStaticText(this, -1, _("Path :"));
  st_RecentTitle	= new wxStaticText(this, -1, _("Recent sessions :"));

  tc_ProjectName	= new wxTextCtrl(this, Wizard_ProjectName, _("WiredProject"),
	  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  tc_NewPath		= new wxTextCtrl(this, Wizard_Path, wxGetCwd() + wxFileName::GetPathSeparator() + wxT("my_wired_projects"),
	  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

  b_NewProject		= new wxButton(this, wxID_NEW, _("New &Project"));
  b_BrowseNew		= new wxButton(this, wxID_FIND, _("&Browse"));

  // Recent projects
  lc_Recent	= new wxListCtrl(this, Wizard_RecentList, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxLC_SINGLE_SEL);
  st_Recent = new wxStaticText(this, -1, _("No recent sessions"));

  // PROCESSING BUTTONS
  b_OK = new wxButton(this, wxID_OK, _("&OK"));
  b_Remove = new wxButton(this, wxID_DELETE, _("&Remove"));
  b_Remove->Enable(false);
  b_Exit = new wxButton(this, wxID_EXIT, _("E&xit"));

  // Sizers
  wxBoxSizer*	nameSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer*	pathSizer = new wxBoxSizer(wxHORIZONTAL);
  wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();

  wxBoxSizer*	mainSizer = new wxBoxSizer(wxVERTICAL);

  nameSizer->Add(st_ProjectName, wxSizerFlags().Border().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Proportion(15));
  nameSizer->Add(tc_ProjectName, wxSizerFlags().Border().Center().Expand().Proportion(70));
  nameSizer->Add(b_NewProject, wxSizerFlags().Border().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Proportion(15));

  pathSizer->Add(st_NewPath, wxSizerFlags().Border().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Proportion(15));
  pathSizer->Add(tc_NewPath, wxSizerFlags().Border().Center().Expand().Proportion(70));
  pathSizer->Add(b_BrowseNew, wxSizerFlags().Border().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Proportion(15));

  buttonSizer->Add(b_OK, wxSizerFlags().Border());
  buttonSizer->Add(b_Remove, wxSizerFlags().Border());
  buttonSizer->Add(b_Exit, wxSizerFlags().Border());

  // main layout
  mainSizer->Add(nameSizer);
  mainSizer->Add(pathSizer);
  mainSizer->Add(st_RecentTitle, wxSizerFlags().Border());
  mainSizer->Add(lc_Recent, wxSizerFlags().Border().Center().Expand().Proportion(100));
  mainSizer->Add(st_Recent, wxSizerFlags().Border().Center().Expand().Proportion(100));
  mainSizer->Add(buttonSizer, wxSizerFlags().Border().Bottom());

  vector<wxFileName>	pathList = WiredSettings->GetRecentDirs();
  if (pathList.size())
  {
#if wxCHECK_VERSION (2,8,0)
	tc_NewPath->ChangeValue(pathList[0].GetPath());
	tc_ProjectName->ChangeValue(pathList[0].GetName());
#else
	tc_NewPath->SetValue(pathList[0].GetPath());
	tc_ProjectName->SetValue(pathList[0].GetName());
#endif
  }
  UpdateList();


  // set a minimal size to the window
  mainSizer->SetMinSize(WIZ_WIN_SIZE);
  buttonSizer->Realize();

  // center the window on the screen
  Center();
  // resize items and window
  SetSizer(mainSizer);
  mainSizer->SetSizeHints(this);

  tc_ProjectName->SetFocus();
}

void	Wizard::UpdateList()
{
  vector<wxFileName>	pathList = WiredSettings->GetRecentDirs();

  if (pathList.size())
  {
    lc_Recent->Show();
	st_Recent->Hide();
	wxString				thispath;

	wxImage				folder_icon(wired_session_folder_xpm);
	wxImageList			*imagelist = new wxImageList(WIZ_ICON_SIZE, WIZ_ICON_SIZE, true);
	imagelist->Add(RESIZE_ICON(wxIcon(wired_session_folder_xpm),WIZ_ICON_SIZE, WIZ_ICON_SIZE));
	lc_Recent->AssignImageList(imagelist, wxIMAGE_LIST_NORMAL);
	lc_Recent->SetColumnWidth(-1, 92);
	for (unsigned int i = 0; i < pathList.size(); i++)
	{
	  thispath = pathList[i].GetName();
	  lc_Recent->InsertItem(i, thispath, 0);
	}
  }
  else
  {
	lc_Recent->Hide();
	st_Recent->Show();
  }
}

Wizard::~Wizard()
{
}

wxString	Wizard::GetDir()
{
  return (chosenDir);
}

/*
bool	Wizard::MakeMyDirs(wxString fullDir)
{
  LOG;
  wxFileName	dir(fullDir);

  if (dir.DirExists())
	return true;
  if (!dir.IsOk())
	return false;
  if (MakeMyDirs(dir.GetPath()))
  {
	dir.Mkdir();
  return true;
  }
  return false;
}
*/

void	Wizard::OnNewClick(wxCommandEvent &event)
{
  NewProject();
}

void	Wizard::NewProject()
{
  wxMessageDialog	*dlg;

  chosenDir.Clear();
  chosenDir << tc_NewPath->GetValue() << wxFileName::GetPathSeparator() \
	<< tc_ProjectName->GetValue();

  wxFileName	dir(chosenDir);
  dir.MakeAbsolute();
  if (!dir.SetCwd())
  {
	if (!dir.DirExists())
	{
	  dlg = new wxMessageDialog(this, _("Directory does not exist.\nCreate new one ?"),
		  _("No such directory"), wxYES_NO | wxYES_DEFAULT);
	  if (dlg->ShowModal() == wxID_YES)
	  {
		if (wxFileName::Mkdir(dir.GetFullPath(), 0777, wxPATH_MKDIR_FULL))
		{
//		  dir.SetCwd();
		  EndModal(0);
		}
		else
		{
		  delete dlg;
		  dlg = new wxMessageDialog(this, _("Unable to create directory"), _("Sorry"),
			  wxICON_INFORMATION);
		  dlg->ShowModal();
		  delete dlg;
		}
	  }
	}
	else
	{
	  dlg = new wxMessageDialog(this, _("Access denied"), _("Sorry"), wxICON_INFORMATION);
	  dlg->ShowModal();
	  delete dlg;
	}
  }
  else
  {
	chosenDir = dir.GetFullPath();
	EndModal(0);
  }
}

void	Wizard::OnTextChange(wxCommandEvent &event)
{
  UpdateText();
}

void	Wizard::UpdateText()
{
  LOG;
  wxString	wired_xml;

  if ((tc_NewPath == NULL) || (tc_ProjectName == NULL) || (b_NewProject == NULL))
    return;
  wired_xml.Clear();
  wired_xml << tc_NewPath->GetValue() << wxFileName::GetPathSeparator() \
	<< tc_ProjectName->GetValue() << wxFileName::GetPathSeparator() \
	<< wxT("wired.xml");
  if (wxFileName::FileExists(wired_xml))
	b_NewProject->SetLabel(_("Load &Project"));
  else
	b_NewProject->SetLabel(_("New &Project"));
}

void	Wizard::OnBrowseClick(wxCommandEvent &event)
{
  wxString	wired_xml;
  wxString	selected_dir = ChooseSessionDir();

  wired_xml.Clear();
  wired_xml << selected_dir << wxFileName::GetPathSeparator() \
	<< wxT("wired.xml");
  if (wxFileName::FileExists(wired_xml))
  {
	wxFileName	dir(selected_dir);

#if wxCHECK_VERSION (2,8,0)
	tc_ProjectName->ChangeValue(dir.GetName());
	tc_NewPath->ChangeValue(dir.GetPath());
#else
	tc_ProjectName->SetValue(dir.GetName());
	tc_NewPath->SetValue(dir.GetPath());
#endif
  }
  else
#if wxCHECK_VERSION (2,8,0)
	tc_NewPath->ChangeValue(selected_dir);
#else
	tc_NewPath->SetValue(selected_dir);
#endif
}

wxString		Wizard::ChooseSessionDir()
{
  wxDirDialog	dirDialog(NULL, _("Select a project folder"), wxGetCwd());
  dirDialog.ShowModal();
  return (dirDialog.GetPath());
}

void	Wizard::OnRemove(wxCommandEvent &event)
{
  vector<wxFileName>	pathList = WiredSettings->GetRecentDirs();
  int					item = -1;

  item = lc_Recent->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item >= 0 && (unsigned int)item < pathList.size())
	WiredSettings->RemoveRecentDir(item);
  UpdateList();
  b_Remove->Enable(false);
}

void	Wizard::OnListDClick(wxListEvent &event)
{
  LoadProject();
}

void	Wizard::OnListClick(wxListEvent &event)
{
  long					item = -1;
  vector<wxFileName>	pathList = WiredSettings->GetRecentDirs();

  item = lc_Recent->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item >= 0 && (unsigned int)item < pathList.size())
  {
#if wxCHECK_VERSION (2,8,0)
	tc_ProjectName->ChangeValue(pathList[item].GetName());
	tc_NewPath->ChangeValue(pathList[item].GetPath());
#else
	tc_ProjectName->SetValue(pathList[item].GetName());
	tc_NewPath->SetValue(pathList[item].GetPath());
#endif
	b_Remove->Enable(true);
  }
}

void	Wizard::LoadProject()
{
  long					item = -1;
  vector<wxFileName>	pathList = WiredSettings->GetRecentDirs();

  // getting first item selected (only one)
  item = lc_Recent->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item != -1)
  {
	chosenDir = pathList[(int)item].GetFullPath();
	EndModal(0);
  }
  else
  {
	wxMessageDialog	dlg(this, _("No session selected"), _("Note"), wxICON_INFORMATION, wxDefaultPosition);
	dlg.ShowModal();
  }
}

void	Wizard::OnOkClick(wxCommandEvent &event)
{
  //lc_Recent is initialized at null (not to display)
  //if there is no recent project, let's do as if we are creating a new one
  if(!lc_Recent)
  {
    NewProject();
  }
  else
  {
    LoadProject();
  }

  std::cout << "[WIZARD] ChosenDir : " << chosenDir.mb_str() << std::endl;
}

void	Wizard::OnExitClick(wxCommandEvent &event)
{
    /*
	 *delete Audio;
	 *outputs "pure method called" randomly on delete Audio, don't know why
     */
  cout << "[WIZARD] Closing sound stream..." << endl;
  if (Audio)
	Audio->CloseStream();
  cout << "[WIZARD] Closing Media Library..." << endl;
  if(MediaLibraryPanel)
    delete MediaLibraryPanel;

  cout << "[WIZARD] Unloading session manager..." << endl;
  delete saveCenter;

  cout << "[WIZARD] Unloading user settings manager..." << endl;
  if (WiredSettings)
    delete WiredSettings;
  exit(0);
}


BEGIN_EVENT_TABLE(Wizard, wxDialog)
  EVT_BUTTON(wxID_NEW, Wizard::OnNewClick)
  EVT_BUTTON(wxID_FIND, Wizard::OnBrowseClick)
  EVT_BUTTON(wxID_OK, Wizard::OnOkClick)
  EVT_BUTTON(wxID_DELETE, Wizard::OnRemove)
  EVT_BUTTON(wxID_EXIT, Wizard::OnExitClick)
  EVT_TEXT_ENTER(Wizard_Path, Wizard::OnNewClick)
  EVT_TEXT_ENTER(Wizard_ProjectName, Wizard::OnNewClick)
  EVT_LIST_ITEM_ACTIVATED(Wizard_RecentList, Wizard::OnListDClick)
  EVT_LIST_ITEM_SELECTED(Wizard_RecentList, Wizard::OnListClick)
  EVT_TEXT(Wizard_Path, Wizard::OnTextChange)
  EVT_TEXT(Wizard_ProjectName, Wizard::OnTextChange)
END_EVENT_TABLE()
