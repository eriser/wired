#include  <wx/wx.h>
#include  <wx/dialog.h>
#include  <wx/filename.h>
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
#define  WIZ_BTN_HEIGHT		22
#define  WIZ_BTN_WIDTH		80
#define  WIZ_BTN_SIZE		wxSize(WIZ_BTN_WIDTH, WIZ_BTN_HEIGHT)
#define  WIZ_NEW_BTN_WIDTH	80
#define  WIZ_NEW_BTN_SIZE	wxSize(WIZ_NEW_BTN_WIDTH, WIZ_BTN_HEIGHT)
#define  WIZ_MARGIN			10
#define  WIZ_TOP_MARGIN		1
#define  WIZ_LABEL_WIDTH	120
#define  WIZ_TC_HEIGHT		32
#define  WIZ_TC_PATH_WIDTH	(WIZ_WIN_WIDTH - WIZ_NEW_BTN_WIDTH - WIZ_MARGIN * 4 - WIZ_LABEL_WIDTH)
#define  WIZ_TC_PATH_HEIGHT	-1
#define  WIZ_TC_PATH_SIZE	wxSize(WIZ_TC_PATH_WIDTH, WIZ_TC_PATH_HEIGHT)
#define  WIZ_BROWSE_POS		wxPoint(WIZ_WIN_WIDTH - WIZ_MARGIN - WIZ_BTN_WIDTH, WIZ_MARGIN + WIZ_TOP_MARGIN + WIZ_TC_HEIGHT)
#define  WIZ_NEW_BTN_POS	wxPoint(WIZ_WIN_WIDTH - WIZ_MARGIN - WIZ_BTN_WIDTH, WIZ_MARGIN * 2 + WIZ_TOP_MARGIN)
#define  WIZ_RECENT_TITLE_POS_Y	(WIZ_BTN_HEIGHT + WIZ_MARGIN * 3)
#define  WIZ_BOTTOM_Y		(WIZ_WIN_HEIGHT - WIZ_BTN_HEIGHT - WIZ_MARGIN)
#define  WIZ_LC_RECENT_HEIGHT	(WIZ_WIN_HEIGHT - WIZ_BTN_HEIGHT - WIZ_BTN_HEIGHT * 2 - WIZ_MARGIN * 6)
#define  WIZ_LC_RECENT_Y	(WIZ_RECENT_TITLE_POS_Y + WIZ_MARGIN * 3)
#define  WIZ_ICON_SIZE		32
#define  WIZ_TC_PATH_POS	wxPoint(WIZ_MARGIN * 2 + WIZ_LABEL_WIDTH, WIZ_MARGIN + WIZ_TOP_MARGIN + WIZ_TC_HEIGHT)
#define  WIZ_ST_NAME_SIZE	wxSize(WIZ_LABEL_WIDTH, WIZ_TC_HEIGHT)
#define  WIZ_ST_NAME_POS	wxPoint(WIZ_MARGIN, WIZ_MARGIN * 2 + WIZ_TOP_MARGIN)
#define  WIZ_TC_NAME_POS	wxPoint(WIZ_MARGIN * 2 + WIZ_LABEL_WIDTH, WIZ_MARGIN * 2 + WIZ_TOP_MARGIN)
#define  WIZ_ST_PATH_POS	wxPoint(WIZ_MARGIN, WIZ_MARGIN + WIZ_TOP_MARGIN + WIZ_TC_HEIGHT)
#define  WIZ_ST_PATH_SIZE	WIZ_ST_NAME_SIZE


#ifndef RESIZE_ICON
# define RESIZE_ICON(ico, w, h)	(wxBitmap(ico).ConvertToImage().Rescale(w, h))
#endif

Wizard::Wizard()
  : wxDialog(0x0, -1, _("Wired Wizard"), wxDefaultPosition, WIZ_WIN_SIZE,
	  wxDEFAULT_DIALOG_STYLE, _("Wired Wizard"))
{
  chosenDir = wxT("");

  st_ProjectName	= new wxStaticText(this, -1, _("Project name :"),
	  WIZ_ST_NAME_POS, WIZ_ST_NAME_SIZE,
	  wxALIGN_RIGHT);

  tc_ProjectName	= new wxTextCtrl(this, Wizard_ProjectName, _("WiredProject"),
	  WIZ_TC_NAME_POS, WIZ_TC_PATH_SIZE, wxTE_PROCESS_ENTER);

  st_NewPath		= new wxStaticText(this, -1, _("Path :"),
	  WIZ_ST_PATH_POS, WIZ_ST_PATH_SIZE,
	  wxALIGN_RIGHT);

  tc_NewPath		= new wxTextCtrl(this, Wizard_Path, wxGetCwd(),
	  WIZ_TC_PATH_POS, WIZ_TC_PATH_SIZE, wxTE_PROCESS_ENTER);

  b_NewProject		= new wxButton(this, Wizard_NewBtn, _("New &Project"),
	  WIZ_NEW_BTN_POS, WIZ_NEW_BTN_SIZE);

  b_BrowseNew		= new wxButton(this, Wizard_Browse, _("&Browse"),
	  WIZ_BROWSE_POS, WIZ_BTN_SIZE);

  st_RecentTitle	= new wxStaticText(this, -1, _("Recent sessions :"),
	  wxPoint(WIZ_MARGIN, WIZ_RECENT_TITLE_POS_Y + WIZ_MARGIN),
	  wxSize(WIZ_WIN_WIDTH / 2, WIZ_BTN_HEIGHT));

  b_Remove			= new wxButton(this, Wizard_Remove, _("&Remove"),
	  wxPoint(WIZ_WIN_WIDTH - WIZ_BTN_WIDTH * 2 - WIZ_MARGIN * 2, WIZ_BOTTOM_Y),
	  WIZ_BTN_SIZE);

  b_Remove->Enable(false);

  b_Exit			= new wxButton(this, Wizard_Exit, _("E&xit"),
	  wxPoint(WIZ_WIN_WIDTH - WIZ_BTN_WIDTH - WIZ_MARGIN, WIZ_BOTTOM_Y), WIZ_BTN_SIZE);

  lc_Recent = 0x0;
  st_Recent = 0x0;

  vector<wxFileName>	pathList = WiredSettings->GetRecentDirs();
  if (pathList.size())
  {
	tc_NewPath->SetValue(pathList[0].GetPath());
	tc_ProjectName->SetValue(pathList[0].GetName());
  }
  else
	UpdateText();
  UpdateList();
  tc_ProjectName->SetFocus();
}

void	Wizard::UpdateList()
{
  vector<wxFileName>	pathList = WiredSettings->GetRecentDirs();

  delete lc_Recent;
  delete st_Recent;
  if (pathList.size())
  {
	st_Recent = 0x0;
	lc_Recent			= new wxListCtrl(this, Wizard_RecentList,
		wxPoint(WIZ_MARGIN, WIZ_LC_RECENT_Y),
		wxSize(WIZ_WIN_WIDTH - (WIZ_MARGIN*2), WIZ_LC_RECENT_HEIGHT),
		wxLC_ICON | wxLC_SINGLE_SEL);
	wxString				thispath;

	wxImage				folder_icon(wired_session_folder_xpm);
	wxImageList			*imagelist = new wxImageList(WIZ_ICON_SIZE, WIZ_ICON_SIZE, true);
	imagelist->Add(RESIZE_ICON(wxIcon(wired_session_folder_xpm),WIZ_ICON_SIZE, WIZ_ICON_SIZE));
	lc_Recent->AssignImageList(imagelist, wxIMAGE_LIST_NORMAL);
	lc_Recent->SetColumnWidth(-1, 92);
	for (int i = 0; i < pathList.size(); i++)
	{
	  thispath = pathList[i].GetName();
	  lc_Recent->InsertItem(i, thispath, 0);
	}
  }
  else
  {
	lc_Recent = 0x0;
	st_Recent = new wxStaticText(this, -1, _("No recent sessions"),
		wxPoint(WIZ_MARGIN, WIZ_LC_RECENT_Y + (WIZ_LC_RECENT_HEIGHT) / 2),
		wxSize(WIZ_WIN_WIDTH - (WIZ_MARGIN*2), WIZ_LC_RECENT_HEIGHT),
		wxALIGN_CENTER);
  }
}

Wizard::~Wizard()
{
}

wxString	Wizard::GetDir()
{
  return (chosenDir);
}

bool	Wizard::MakeMyDirs(wxString fullDir)
{
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

void	Wizard::OnNewClick(wxCommandEvent &event)
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
		if (MakeMyDirs(dir.GetFullPath()))
		{
		  dir.SetCwd();
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
	EndModal(0);
}

void	Wizard::OnTextChange(wxCommandEvent &event)
{
  UpdateText();
}

void	Wizard::UpdateText()
{
  wxString	wired_xml;
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

	tc_ProjectName->SetValue(dir.GetName());
	tc_NewPath->SetValue(dir.GetPath());
  }
  else
	tc_NewPath->SetValue(selected_dir);
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
  if (item >= 0 && item < pathList.size())
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
  if (item >= 0 && item < pathList.size())
  {
	tc_ProjectName->SetValue(pathList[item].GetName());
	tc_NewPath->SetValue(pathList[item].GetPath());
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
  EVT_BUTTON(Wizard_NewBtn, Wizard::OnNewClick)
  EVT_TEXT_ENTER(Wizard_Path, Wizard::OnNewClick)
  EVT_TEXT_ENTER(Wizard_ProjectName, Wizard::OnNewClick)
  EVT_BUTTON(Wizard_Browse, Wizard::OnBrowseClick)
  EVT_LIST_ITEM_ACTIVATED(Wizard_RecentList, Wizard::OnListDClick)
  EVT_LIST_ITEM_SELECTED(Wizard_RecentList, Wizard::OnListClick)
  EVT_BUTTON(Wizard_Remove, Wizard::OnRemove)
  EVT_BUTTON(Wizard_Exit, Wizard::OnExitClick)
  EVT_TEXT(Wizard_Path, Wizard::OnTextChange)
  EVT_TEXT(Wizard_ProjectName, Wizard::OnTextChange)
END_EVENT_TABLE()
