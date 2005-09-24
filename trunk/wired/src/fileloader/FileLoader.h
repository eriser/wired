/*
** Copyright (C) 2004 by Wired Team
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

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

#define EXT_FILE	"wired_exts.conf"
#define FAVORITE_FILE	"wired_favs.conf"
#define MRU_FILE	"wired_mru.conf"

DEFINE_EVENT_TYPE(wxEVT_FILELOADER_START)
DEFINE_EVENT_TYPE(wxEVT_FILELOADER_STOP)

#define FileLoader_Start 31313132
#define FileLoader_Stop  31313133

#define EVT_FILELOADER_START(winid, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_FILELOADER_START, winid, wxID_ANY, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &fn, (wxObject *)NULL),
#define EVT_FILELOADER_STOP(winid, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_FILELOADER_STOP, winid, wxID_ANY, (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &fn, (wxObject *)NULL),  

class FileLoader: public wxDialog
{
 public:
  FileLoader(wxWindow *parent, wxWindowID id, string, bool, bool, vector<string> *, bool = false);
  ~FileLoader();
  string GetSelectedFile();
  bool	 IsAkai() { return akai; }

 private:
  void OnExpandFolder(wxTreeEvent &e);
  void OnCollapseFolder(wxTreeEvent &e);
  void OnSelectFolder(wxTreeEvent &e);
  void OnActivateFolder(wxTreeEvent &e);
  void PopupMenuFolder(wxTreeEvent &e);

  void OnSelectFile(wxListEvent &e);
  void OnActivateFile(wxListEvent &e);

  void OnChangeType(wxCommandEvent &e);

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
  void ListFiles(string);
  void ListAkaiCD(wxTreeItemId);
  void ListAkaiVolume(string);
  
  void AddIcon(wxImageList *, wxIcon);
  
  void LoadSoundExt(vector<string> * = NULL);
  void LoadFolders();
  void SaveFolders();

  void StartPlaying();
  void StopPlaying();
  void GotoDir(char *);
  bool ExtMatch(char *, char *);
  char *FormatSize(off_t);
  
  wxTreeCtrl	*folder;
  wxListView	*files;
  wxTextCtrl	*filename;
  wxStaticText  *fntext;
  wxStaticText  *typtext;
  wxStaticText  *favtext;
  wxStaticText  *mrutext;
  wxComboBox	*type;
  wxButton	*preview;
  wxButton	*btopen;
  wxButton	*cancel;
  bool		playing;
  wxTreeItemId  pm_selitem;
  wxComboBox	*mru;
  wxComboBox	*favorites;
  wxBitmapButton *mrudel;
  wxBitmapButton *favdel;
  bool		akai;
  bool		save;
  int		akaifd;

  string	favdir;
  string	mrudir;


  DECLARE_EVENT_TABLE()
};


#endif
