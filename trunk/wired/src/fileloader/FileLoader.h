// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#ifndef __FILELOADER_H__
#define __FILELOADER_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <list>
#include "TreeItemData.h"
#include "akai.h"
#include "Settings.h"

using std::string;
using std::vector;

#define EXT_FILE	wxT("wired_exts.conf")
#define FAVORITE_FILE	wxT("wired_favs.conf")
#define MRU_FILE	wxT("wired_mru.conf")

DEFINE_EVENT_TYPE(wxEVT_FILELOADER_START)
DEFINE_EVENT_TYPE(wxEVT_FILELOADER_STOP)

#define FileLoader_Start 31313132
#define FileLoader_Stop  31313133

#define EVT_FILELOADER_START(winid, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_FILELOADER_START, winid, wxID_ANY, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &fn, (wxObject *)NULL),
#define EVT_FILELOADER_STOP(winid, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_FILELOADER_STOP, winid, wxID_ANY, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &fn, (wxObject *)NULL),  

class FileLoader: public wxDialog
{
 public:
  FileLoader(wxWindow *parent, wxWindowID id, wxString, bool, bool,
	     vector<wxString> *exts, bool LoadExtraExts = false);
  ~FileLoader();
  wxString GetSelectedFile();
  bool	 IsAkai() { return akai; }
  static wxString      FormatSize(off_t);

 private:
  void OnExpandFolder(wxTreeEvent &e);
  void OnCollapseFolder(wxTreeEvent &e);
  void OnSelectFolder(wxTreeEvent &e);
  void OnActivateFolder(wxTreeEvent &e);
  void PopupMenuFolder(wxTreeEvent &e);

  void OnSelectFile(wxListEvent &e);
  void OnActivateFile(wxListEvent &e);

  void OnChangeFilter(wxCommandEvent &e);

  void OnPreview(wxCommandEvent &e);
  void OnOpen(wxCommandEvent &e);
  void OnCancel(wxCommandEvent &e);

  void OnEnterFilename(wxCommandEvent &e);

  void OnAddToFavorites(wxCommandEvent &e);

  void OnSelectFavoriteDir(wxCommandEvent &e);

  void OnSelectRecentDir(wxCommandEvent &e);

  void OnDeleteFavorite(wxCommandEvent &e);

  void OnDeleteRecent(wxCommandEvent &e);

  void ListDirectories(wxTreeItemId);
  void ListFiles(wxString);
  void ListAkaiCD(wxTreeItemId);
  void ListAkaiVolume(wxString);
  
  void AddIcon(wxImageList *, wxIcon);
  
  void LoadSoundExt(vector<wxString> *exts, bool LoadExtraExts);
  void LoadFolders();
  void SaveFolders();

  void StartPlaying();
  void StopPlaying();
  //void GotoDir(const char *);
  void GotoDir(wxString path, wxTreeItemId parent);

  bool			ExtMatch(wxString);
  
  wxTreeCtrl		*folder;
  wxListView		*files;
  wxTextCtrl		*filename;
  wxStaticText		*fntext;
  wxStaticText		*typtext;
  wxStaticText		*favtext;
  wxStaticText		*mrutext;
  wxStaticText      *FileInfo;
  wxComboBox		*type;
  wxButton		*preview;
  wxButton		*btopen;
  wxButton		*cancel;
  bool			playing;
  wxTreeItemId		pm_selitem;
  wxComboBox		*mru;
  wxComboBox		*favorites;
  wxBitmapButton	*mrudel;
  wxBitmapButton	*favdel;
  bool			akai;
  bool			save;
  int			akaifd;
  wxString		filters;

  wxString		favdir;
  wxString		mrudir;


  DECLARE_EVENT_TABLE()
};


#endif
