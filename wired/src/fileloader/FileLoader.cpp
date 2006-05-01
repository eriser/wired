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

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/dir.h>

using namespace std;

#include "FileLoader.h"
#include "icon5.xpm"
#include "icon3.xpm"
#include "file.xpm"
#include "audio.xpm"
#include "delete.xpm"
#include "FileStat.h"

#define F_WIDTH		640
#define F_HEIGHT	520

#define FOLDER_ID		2001
#define FILE_ID			2002
#define FILENAME_ID		2003
#define TYPE_ID			2004
#define PREVIEW_ID		2005
#define OPEN_ID			2006
#define CANCEL_ID		2007
#define ADDTOFAVORITES_ID	2008
#define FAVORITES_ID		2009
#define MRU_ID			2010
#define DELFAVORITES_ID		2011
#define DELMRU_ID		2012

wxString  OldPath; // Used to store last visited path

BEGIN_EVENT_TABLE(FileLoader, wxDialog)
  EVT_TREE_ITEM_EXPANDING(FOLDER_ID, FileLoader::OnExpandFolder)
  EVT_TREE_ITEM_COLLAPSING(FOLDER_ID, FileLoader::OnCollapseFolder)
  EVT_TREE_SEL_CHANGED(FOLDER_ID, FileLoader::OnSelectFolder)
  EVT_TREE_ITEM_ACTIVATED(FOLDER_ID, FileLoader::OnActivateFolder)
  EVT_TREE_ITEM_RIGHT_CLICK(FOLDER_ID, FileLoader::PopupMenuFolder)

  EVT_LIST_ITEM_SELECTED(FILE_ID, FileLoader::OnSelectFile)
  EVT_LIST_ITEM_ACTIVATED(FILE_ID, FileLoader::OnActivateFile)

  EVT_TEXT_ENTER(FILENAME_ID, FileLoader::OnEnterFilename)

  EVT_COMBOBOX(TYPE_ID, FileLoader::OnChangeFilter)

  EVT_BUTTON(PREVIEW_ID, FileLoader::OnPreview)
  EVT_BUTTON(OPEN_ID, FileLoader::OnOpen)
  EVT_BUTTON(CANCEL_ID, FileLoader::OnCancel)

  EVT_MENU(ADDTOFAVORITES_ID, FileLoader::OnAddToFavorites)

  EVT_COMBOBOX(FAVORITES_ID, FileLoader::OnSelectFavoriteDir)

  EVT_COMBOBOX(MRU_ID, FileLoader::OnSelectRecentDir)

  EVT_BUTTON(DELFAVORITES_ID, FileLoader::OnDeleteFavorite)

  EVT_BUTTON(DELMRU_ID, FileLoader::OnDeleteRecent)
END_EVENT_TABLE()

FileLoader::FileLoader(wxWindow *parent, wxWindowID id, wxString title, bool pakai, bool psave, vector<wxString> *exts, bool LoadExtraExts) :
wxDialog(parent, id, title.c_str(), wxDefaultPosition, wxSize(F_WIDTH, F_HEIGHT))
{
  playing = false;
  Center();
  
  wxFileName f;

  f.AssignHomeDir();
  f.AppendDir(wxT(".wired"));
  favdir = f.GetFullPath() + FAVORITE_FILE;
  mrudir = f.GetFullPath() + MRU_FILE;
  akai = pakai;
  save = psave;
  folder = new wxTreeCtrl(this, FOLDER_ID, wxPoint(4, 34), 
			  wxSize(F_WIDTH / 3 - 8, F_HEIGHT - 150),
			  wxTR_HAS_BUTTONS | wxTR_SINGLE | 
			  wxSUNKEN_BORDER | wxTR_NO_LINES);
  files = new wxListView(this, FILE_ID, wxPoint(F_WIDTH / 3, 34), 
			  wxSize(2 * F_WIDTH / 3 - 8, F_HEIGHT - 150), 
			  wxSUNKEN_BORDER | wxLC_REPORT | 
			  wxLC_SINGLE_SEL);
  files->InsertColumn(0, _("Name"));
  files->SetColumnWidth(0, 180);
  files->InsertColumn(1, _("Size"), wxLIST_FORMAT_RIGHT);
  files->SetColumnWidth(1, 58);
  if (!akai)
    {
      files->InsertColumn(2, _("Modified"));
      files->SetColumnWidth(2, 180);
    }
  else
  {
    files->InsertColumn(2, _("Type"));
    files->SetColumnWidth(2, 180);
  }
  FileInfo = new wxStaticText(this, -1, wxT(""), 
          wxPoint(10, F_HEIGHT - 105), wxSize(-1, -1), wxALIGN_LEFT);
  wxImageList *images = new wxImageList(16, 16, TRUE);
  AddIcon(images, wxIcon(icon3_xpm));
  AddIcon(images, wxIcon(icon5_xpm));
  folder->AssignImageList(images);

  wxImageList *imgs = new wxImageList(16, 16, TRUE);
  AddIcon(imgs, wxIcon(audio_xpm));
  AddIcon(imgs, wxIcon(file_xpm));
  files->AssignImageList(imgs, wxIMAGE_LIST_SMALL);
      
  if (!akai)
  {
  	fntext = new wxStaticText(this, -1, _("Filename"), 
		  wxPoint(10, F_HEIGHT - 58), wxSize(-1, -1), wxALIGN_LEFT);
  	filename = new wxTextCtrl(this, FILENAME_ID, _T(""), 
				  wxPoint(100, F_HEIGHT - 60), 
				  wxSize(320, -1), wxTE_PROCESS_ENTER);
	typtext = new wxStaticText(this, -1, _("Type"),
				   wxPoint(10, F_HEIGHT - 30), 
				   wxSize(-1, -1), wxALIGN_LEFT);
	type = new wxComboBox(this, TYPE_ID, _T(" "), 
			      wxPoint(100, F_HEIGHT - 30), wxSize(320, -1), 0, 
			      NULL, wxCB_DROPDOWN | wxCB_READONLY);  
  }
  else
  {
  	fntext = new wxStaticText(this, -1, _("Akai device"), 
		  wxPoint(10, F_HEIGHT - 58), wxSize(-1, -1), wxALIGN_LEFT);
  	filename = new wxTextCtrl(this, FILENAME_ID, _T("/dev/cdrom"), 
				  wxPoint(100, F_HEIGHT - 60), 
				  wxSize(320, -1), wxTE_PROCESS_ENTER);
  }
//   if (!akai)
//   {
//     typtext = new wxStaticText(this, -1, _T(_("Type")),
//   		    wxPoint(10, F_HEIGHT - 30), wxSize(-1, -1), wxALIGN_LEFT);
//     type = new wxComboBox(this, TYPE_ID, _T(" "), wxPoint(100, F_HEIGHT - 30), 
//   		  wxSize(320, -1), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);  
//   }
  if (!save)
    {
      preview = new wxButton(this, PREVIEW_ID, _("Preview"), 
			     wxPoint(F_WIDTH - 168, F_HEIGHT - 60),
			     wxSize(-1, -1));
      btopen = new wxButton(this, OPEN_ID, _("Open"), 
			    wxPoint(F_WIDTH - 84, F_HEIGHT - 60),
			    wxSize(-1, -1));
      preview->Disable();
      btopen->Disable();
    }
  else
    btopen = new wxButton(this, OPEN_ID, _("Save"), 
			  wxPoint(F_WIDTH - 84, F_HEIGHT - 60), 
			  wxSize(-1, -1));
  cancel = new wxButton(this, CANCEL_ID, _("Cancel"), 
			wxPoint(F_WIDTH - 84, F_HEIGHT - 30), wxSize(-1, -1));
  if (!akai)
    {
      favtext = new wxStaticText(this, -1, _("Favorites"), wxPoint(10, 10),
				 wxSize(-1, -1), wxALIGN_LEFT);
      favorites = new wxComboBox(this, FAVORITES_ID, _T(" "), wxPoint(70, 4),
				 wxSize(160, -1), 0, NULL, 
				 wxCB_DROPDOWN | wxCB_READONLY);
      favdel = new wxBitmapButton(this, DELFAVORITES_ID, 
				  wxBitmap(delete_xpm).ConvertToImage(),
				  wxPoint(235, 0));
      mrutext = new wxStaticText(this, -1, _("Recents"),
				 wxPoint(330, 10), wxSize(-1, -1), 
				 wxALIGN_LEFT);
      mru = new wxComboBox(this, MRU_ID, _T(" "), wxPoint(390, 4), 
			   wxSize(160, -1), 0, NULL, 
			   wxCB_DROPDOWN | wxCB_READONLY);
      mrudel = new wxBitmapButton(this, DELMRU_ID, 
				  wxBitmap(delete_xpm).ConvertToImage(),
				  wxPoint(555, 0));
    }
  // if (!save)
//     {
//       preview->Disable();
//       btopen->Disable();
//     }
  if (!akai)
    {
      wxLogNull logNo;

      LoadSoundExt(exts, LoadExtraExts);
      LoadFolders();
      if (folder != NULL)
	ListDirectories(folder->AddRoot(wxT("/"), 0, -1, new TreeItemData(wxT("/"))));
      if (OldPath.empty())
	{
	  GotoDir(wxGetCwd(), folder->GetRootItem());
	}
      else
	//GotoDir((const char *)OldPath.c_str());
	GotoDir(OldPath, folder->GetRootItem());
    }
  else
    {
      akaifd = -1;     
      ListAkaiCD(folder->AddRoot(wxT("/"), 0, -1, new TreeItemData(wxT("/"))));
    }
}

//
// Reads the ${PREFIX}/etc/wired_exts.conf file for loading supported
// files extensions.
// If a parameter Exts is given, add the extensions specified into
// the supported extensions
// Objects allocated with the operator "new" and given to the
// Append method should be deleted automatically. See wxwidgets doc
// for further details.
//

void FileLoader::LoadSoundExt(vector<wxString> *Exts, bool LoadExtraExts)
{
  wxString	itemdesc;
  wxString	*itemdata = NULL;

  filters = wxT("");
  if (LoadExtraExts || !Exts)
    {
      wxTextFile	file(WiredSettings->ConfDir + EXT_FILE);
      wxString		l;

      if (file.Open())
	{
	  for (l = file.GetFirstLine(); ; l = file.GetNextLine())
	    {
	      l.Trim(false);
	      l = l.BeforeFirst('#');
	      if (!l.IsEmpty())
		{
		  itemdesc = l.AfterLast('\t');
		  itemdata = new wxString(l.BeforeFirst('\t'));
		  type->Append(itemdesc, itemdata);
		  filters += *itemdata + wxT(";");
		}
	      if (file.Eof())
		break;
	    }
	  file.Close();
	}
    }
  if (Exts)
    {
      for (unsigned int i = 0; i < Exts->size(); i++)
	{
	  unsigned int j;
	  wxString ext = (*Exts)[i];

	  itemdesc = ext.AfterLast('\t');
	  itemdata = new wxString(ext.BeforeFirst('\t'));

	  if ((j = ext.find(wxT("\t"), 0)) != (unsigned int) wxString::npos)
	    type->Append((ext.substr(j + 1, ext.size() - j - 1)),
			 strdup(wxString(ext.substr(0, j).c_str(), *wxConvCurrent).mb_str(*wxConvCurrent)));
	  else
	    type->Append(ext, strdup(ext.mb_str(*wxConvCurrent)));
	  filters += *itemdata + wxT(";");
	}
    }

//	  list<string>	CodecsList = _CodecsMgr.GetExtension();
//	  list<string>::iterator	iter;
//	  for (iter = CodecsList.begin(); iter != CodecsList.end(); iter++)
//	  {
//	  	line = *iter;
//		  if ((j = line.find("\t", 0)) != wstring::npos)
//	      {
//	        type->Append(_T(line.substr(j + 1, line.size() - j - 1).c_str()),
//	                     strdup(line.substr(i, j).c_str()));
//	        if (allext != NULL)
//	        {
//			  *allext += ";";
//			  *allext += line.substr(i, j);
//	        }
//	        else
//		  allext = new wstring(line.substr(i, j));
//	      }
//	  }

  if (!filters.IsEmpty() && !filters.AfterFirst(';').IsEmpty())
    {
      wxString	allext;

      allext = filters.Mid(0, filters.Len() - 1);
      allext.Replace(wxT(";"), wxT(";*."));
      itemdesc = _("All supported soundfiles (*.") + allext + wxT(")");
      itemdata = new wxString(filters);
      type->Insert(itemdesc, 0, itemdata);
      type->SetSelection(0);
    }
  filters = wxT(";") + filters;
  type->Append(_("All files (*.*)"), strdup("*"));
}

void FileLoader::AddIcon(wxImageList *images, wxIcon icon)
{
  int sizeInit = icon.GetWidth();

  if (16 == sizeInit)
    images->Add(icon);
  else
    images->Add(wxBitmap(wxBitmap(icon).ConvertToImage().Rescale(16, 16)));
}

void			FileLoader::LoadFolders()
{
  wxString		l;
  wxTextFile		file;
  
  if (favorites != NULL)
    {
      file.Create(favdir);
      if (file.Open(favdir) && file.GetLineCount() > 0)
	{
	  for (l = file.GetFirstLine(); !file.Eof(); l = file.GetNextLine())
	    if (favorites->FindString(l.c_str()) == -1)
	      favorites->Append(l);
	  if (favorites->FindString(l.c_str()) == -1)
	    favorites->Append(l);
	  file.Close();
	}
    }
  if (mru != NULL)
    {
      file.Create(mrudir);
      if (file.Open(mrudir) && file.GetLineCount() > 0)
	{
	  for (l = file.GetFirstLine(); !file.Eof(); l = file.GetNextLine())
	    if (mru->FindString(l.c_str()) == -1)
	      mru->Append(l);
	  if (mru->FindString(l.c_str()) == -1)
	    mru->Append(l);
	  file.Close();
	}
    }
  if (files != NULL)
    files->SetFocus();
  if (mru != NULL)
    while (mru->GetCount() > 10)
      mru->Delete(0);
}

void FileLoader::SaveFolders()
{
	ofstream f(favdir.mb_str(*wxConvCurrent));
	for (int i = 0; i < favorites->GetCount(); i++)
		f << favorites->GetString(i) << "\n";
	f.close();
	ofstream f2(mrudir.mb_str(*wxConvCurrent));
	for (int i = 0; i < mru->GetCount(); i++)
		f2 << mru->GetString(i) << "\n";
	f2.close();
	
	wxTreeItemId item = folder->GetSelection();
	TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
	if (path)
	  OldPath = path->GetPath();	    
}

FileLoader::~FileLoader()
{
  StopPlaying();
  if (!akai)
    SaveFolders();
  if (akaifd != -1)
    close(akaifd);	  
//  if (folder)	
//    delete folder;
//  if	(files)
//    delete files;
//  if (filename)
//    delete filename;	
//  if (fntext)
//    delete fntext;
//  if (typtext)
//    delete typtext;
//  if (favtext)
//    delete favtext;
//  if (mrutext)
//    delete mrutext;
//  if (type)
//    delete type;
//  if (preview)
//    delete preview;
//  if (btopen)
//    delete btopen;		
//  if (cancel)
//    delete cancel;
//  if (mru)
//    delete mru;
//  if (favorites)
//    delete favorites;
//  if (mrudel)
//    delete mrudel;
//  if  (favdel)
//    delete favdel;
  Destroy();
}

//
// Reads the file system to displays the directory tree 
// displayed on the left of the fileloader
//

void			FileLoader::ListDirectories(wxTreeItemId root)
{
 
  bool			cont;
  wxString		name;
  vector<wxString>	v;
  TreeItemData		*path = (TreeItemData *)folder->GetItemData(root);
  wxDir			dir(path->GetPath().c_str());

  if (dir.IsOpened())
    {
      cont = dir.GetFirst(&name, wxT(""), wxDIR_DIRS);
      while (cont)
	{
	  v.insert(v.begin(), (wxString) name);
	  cont = dir.GetNext(&name);
	}
      stable_sort(v.begin(), v.end());
      for (unsigned int i = 0; i < v.size(); i++)
	{
	  wxString subdir = path->GetPath() + v[i] + wxT("/");
	  folder->AppendItem(root, v[i].c_str(), 0, -1, new TreeItemData(subdir));
	}
    }
}

void	FileLoader::ListAkaiCD(wxTreeItemId root)
{
  if (akaifd == -1)
  {
	wxString dev = filename->GetValue().c_str();
    	if ((akaifd = open(dev.mb_str(*wxConvCurrent), O_RDONLY)) < 0)
	  return;
  }

  TreeItemData *tid = (TreeItemData *)folder->GetItemData(root);
  if (tid->GetPath() == wxT("/"))
  {
  	for (int part = 1; (akaiSelectPart(akaifd, part) > 0) && (part < 27); part++)
	  {
	    char a = 64 + part;
	    wxString partition = _("Partition ");
	    partition += (char)a;
	    if (akaiReadDir(akaifd, "/") != NULL)
	      folder->AppendItem(root, partition.c_str(), 0, -1, new TreeItemData(partition.c_str()));
	  }
  }
  else
  {
    if (tid->GetPath().substr(0, 10) == _("Partition "))
    {
	wxString path = tid->GetPath().substr(10, tid->GetPath().size() - 10);
	unsigned int pos = path.find(wxT("/"), 0);
	int part = 0;
	if (pos != (unsigned int) wxString::npos)
	{
		part = path.substr(0, pos).c_str()[0] - 64;
		path = path.substr(pos, path.size() - pos);
	}
	else
	{
		part = path.c_str()[0] - 64;
		path = wxT("/");
	}
	if (akaiSelectPart(akaifd, part) > 0)
	{
		t_list *dir = akaiReadDir(akaifd, (char *)path.c_str());
		if (dir)
		{
			for (t_list *t = dir; t; t = t->next)
				if (akaiReadDir(akaifd, (char *)elt(t, t_akaiDirent *)->name))
				{
					wxString str = tid->GetPath();
					str += wxT("/");
					str += wxString(elt(t, t_akaiDirent *)->name, *wxConvCurrent);
					str += wxT("/");
					folder->AppendItem(root, wxString(elt(t, t_akaiDirent *)->name, *wxConvCurrent), 0, -1, new TreeItemData(str));
				}	
		}
	}
    }
  }
}

void FileLoader::ListAkaiVolume(wxString p)
{
    files->DeleteAllItems();
    if (p.substr(0, 10) == _("Partition "))
    {
	wxString path = p.substr(10, p.size() - 10);
	unsigned int pos = path.find(wxT("/"), 0);
	int part = 0;
	if (pos != (unsigned int) wxString::npos)
	{
		part = path.substr(0, pos).c_str()[0] - 64;
		path = path.substr(pos + 1, path.size() - pos - 2);
	}
	else
	{
		part = path.c_str()[0] - 64;
		path = wxT("/");
	}
	if (akaiSelectPart(akaifd, part) > 0)
	{
	  t_list *dir = akaiReadDir(akaifd, (char *)path.c_str());
	  if (dir)
	  {
	    int i = 0;
	    for (t_list *t = dir; t; t = t->next)
	    {
		wxString fp = path;
		if (fp != wxT("/"))
		fp += wxT("/");
		else
		fp = wxT("");
		fp += wxString(elt(t, t_akaiDirent *)->name, *wxConvCurrent);
        const char    *temp = (const char *)fp.mb_str(*wxConvCurrent);
		if (!akaiReadDir(akaifd, (char *)temp))
		{
		  long item = files->InsertItem(i++, wxString(elt(t, t_akaiDirent *)->name, *wxConvCurrent));
		  files->SetItem(item, 1, FormatSize(elt(t, t_akaiDirent *)->size));
		  files->SetItem(item, 2, wxString(akaiGetType(elt(t, t_akaiDirent *)->type), *wxConvCurrent));
		  if ((elt(t, t_akaiDirent *)->type == 115) || (elt(t, t_akaiDirent *)->type == 243))
			files->SetItemImage(item, 0, -1);
	  	  else
			files->SetItemImage(item, 1, -1);
		}
	    }
	  }
	}
    }
}

// bool FileLoader::ExtMatch(char *fname, char *extlist)
// {
//   wstring exts = extlist;
//   if (!strcmp(exts.c_str(), "*"))
//     return (true);
//   wstring fn = fname;
//   unsigned int opos = wstring::npos;
//   unsigned int pos = 0;
//   while ((pos = fn.find(".", pos)) != (unsigned int) wstring::npos)
//     opos = ++pos;
//   if (opos == (unsigned int) wstring::npos)
//     return (false);
//   fn = fn.substr(opos, fn.size() - opos);
//   pos = opos = 0;
//   wstring pat = "";
//   while ((pos = exts.find(";", pos)) != (unsigned int) wstring::npos)
//     {
//       pat = exts.substr(opos, pos - opos);
//       if ((fn.size() >= pat.size()) &&
// 	  (!strcmp(fn.substr(fn.size() - pat.size(), 
// 			     pat.size()).c_str(), 
// 		   pat.c_str())))
// 	return (true);
//       opos = ++pos;
//     }
//   pat = exts.substr(opos, exts.size() - opos).c_str();
//   if ((fn.size() >= pat.size()) &&
//       (!strcmp(fn.substr(fn.size() - pat.size(), 
// 			 pat.size()).c_str(), 
// 	       pat.c_str())))
//     return (true);
//   return (false);
// }

//
// Checks if the file extension matches wired supported ones
// ";*;" means no filter
// ";;" in filters' pattern means file without extension
// match is case-insensitive
//

bool			FileLoader::ExtMatch(wxString path)
{
  wxFileName		filename(path);
  wxString		ext = wxT(";") + filename.GetExt() + wxT(";");
 
  if (filters.Find(wxT(";*;")) != -1)
    return (true);
  return (filters.Find(ext.MakeLower()) != -1);
}

//
// Returns a wstring containing a human readable file size
//

wxString		FileLoader::FormatSize(off_t size)
{
  int			            unit = 0;
  wxChar			        tmp[1024];
  
  for (unit = 0; size > 1024; unit++)
    size /= 1024;
  wxSnprintf(tmp, 1024, wxT("%i"), size);
  wxString s = tmp;
  switch (unit)
    {
    case 0:
      s += wxT(" bytes");
      break;
    case 1:
      s += wxT(" Kb");
      break;
    case 2:
      s += wxT(" Mb");
      break;
    case 3:
      s += wxT(" Gb");
      break;
    case 4:
      s += wxT(" Tb");
      break;
    }
  return (s);
}

//
// list the files in the selected directory
//

// void			FileLoader::ListFiles(wstring path)
// {
	
// //   DIR		*dir;
//   wxDir			dir(path);
//   wxString		filename;
//   bool			cont;
//   //struct		dirent  *de;
//   char			*exts;
//   struct stat		st;
//   vector<string>	v;

//   int sel = type->GetSelection();
//   if (sel != -1)
//     exts = (char *)type->GetClientData(sel);
//   else
//     exts = strdup("*");
//   files->DeleteAllItems();
//   //dir = opendir(path.c_str());
//   if (dir.IsOpened())
//     {
//       cont = dir.GetFirst(&filename, "", wxDIR_FILES);
//       while (cont)
// 	//while ((de = readdir(dir)))
// 	{
// 	  //wstring fp = path;
// 	  //fp += de->d_name;
// 	  //stat(fp.c_str(), &st);
// 	  if ((ExtMatch((char*)filename.c_str(), exts)))
// 	    v.insert(v.begin(), filename.c_str());
// 	  cont = dir.GetNext(&filename);
// 	}
//       //      closedir(dir);
//     }
//   stable_sort(v.begin(), v.end());
//   for (unsigned int i = 0; i < v.size(); i++)
//     {
//       wxStructStat	st;

//       wstring fp = path + v[i];
      
//       wxStat(fp.c_str(), &st);
//       long item = files->InsertItem(i, v[i].c_str());
//       files->SetItem(item, 1, FormatSize(st.st_size));
//       files->SetItem(item, 2, ctime(&st.st_mtime));
//       if (ExtMatch((char *)v[i].c_str(), (char *)type->GetClientData(0)))
// 	files->SetItemImage(item, 0, -1);
//       else
// 	files->SetItemImage(item, 1, -1);
//     }
// }

void			FileLoader::ListFiles(wxString path)
{
  wxDir			dir(path);
  wxStructStat		st;
  wxString		name;
  
  bool			cont;
  //char			*exts;
  vector<wxString>	v;

  files->DeleteAllItems();
  if (dir.IsOpened())
    {
      cont = dir.GetFirst(&name, wxT(""), wxDIR_FILES);
      while (cont)
	{
	  if (ExtMatch(name))
	    v.insert(v.begin(), name);
	  cont = dir.GetNext(&name);
	} 
    }
  stable_sort(v.begin(), v.end());
  for (unsigned int i = 0; i < v.size(); i++)
    {
      wxStat(path + v[i], &st);
      long item = files->InsertItem(i, v[i]);
      files->SetItem(item, 1, FormatSize(st.st_size));
      files->SetItem(item, 2, wxString(ctime(&st.st_mtime), *wxConvCurrent));
      files->SetItemImage(item, 0, -1);
    }
}

void			FileLoader::GotoDir(wxString path, wxTreeItemId parent)
{
  wxTreeItemId		child; 
  wxTreeItemIdValue	cookie;
  wxString		dir;
  
  path.StartsWith(wxT("/"), &path);
  dir = path.BeforeFirst('/');
  child = folder->GetFirstChild(parent, cookie);
  while (child.IsOk())
    if (dir == folder->GetItemText(child))
      {
	folder->SelectItem(child);
	folder->ScrollTo(child);
	GotoDir(path.AfterFirst('/'), child);
	break ;
      }
    else
      child = folder->GetNextChild(parent, cookie);
}

// void FileLoader::GotoDir(const char *path)
// {
//   unsigned int pos = 0;
//   int opos = 0;
//   wstring pt = path;
//   wstring dir = "";
//   wxTreeItemId dir_tree = folder->GetRootItem();
  
//   if (pt[0] == '/')
//     pt.erase(0, 1);
//   while ((pos = pt.find("/", pos)) != (unsigned int) wstring::npos)
//     {
//       if (!folder->ItemHasChildren(dir_tree))
// 	{
// 	  folder->SelectItem(dir_tree);
// 	  folder->ScrollTo(dir_tree);
// 	  return ;
// 	}
//       folder->Expand(dir_tree);
//       dir = pt.substr(opos, pos - opos);
//       wxTreeItemIdValue cookie;
//       wxTreeItemId son = folder->GetFirstChild(dir_tree, cookie);
//       bool found = false;
//       while ((!found) && (son.IsOk()))
// 	{
// 		  if (!strcmp(folder->GetItemText(son).c_str(), dir.c_str()))
// 		    {
// 		      dir_tree = son;
// 		      found = true;
// 		    }
// 		  else
// 		    son = folder->GetNextChild(dir_tree, cookie);
// 	}
//       if (!found)
// 	{
// 	  folder->SelectItem(dir_tree);
// 	  folder->ScrollTo(dir_tree);
// 	  return;
// 	}
//       opos = ++pos;
//     }
//   if (!folder->ItemHasChildren(dir_tree))
//     {
//       folder->SelectItem(dir_tree);
//       folder->ScrollTo(dir_tree);
//       return;
//     }
//   folder->Expand(dir_tree);
//   dir = pt.substr(opos, pt.size() - opos);
//   wxTreeItemIdValue cookie;
//   wxTreeItemId son = folder->GetFirstChild(dir_tree, cookie);
//   bool found = false;
//   while ((!found) && (son.IsOk()))
//     {
//       if (!strcmp(folder->GetItemText(son).c_str(), dir.c_str()))
// 	  {
// 	    dir_tree = son;
// 	    found = true;
// 	  }
//       else
// 	son = folder->GetNextChild(dir_tree, cookie);
//     }
//   folder->Expand(dir_tree);
//   folder->SelectItem(dir_tree);
//   folder->ScrollTo(dir_tree);
//   if (!found)
//     {
//       long file = files->FindItem(-1, dir.c_str());
//       if (file != -1)
// 	files->Select(file);
//     }
// }

void FileLoader::OnExpandFolder(wxTreeEvent &e)
{
  wxTreeItemIdValue cookie;
  wxTreeItemId item = e.GetItem();
  folder->SetItemImage(item, 1);
  wxLogNull logNo;
  if (folder->ItemHasChildren(item))
    {
      wxTreeItemId son = folder->GetFirstChild(item, cookie);
      while (son.IsOk())
	{
	  folder->DeleteChildren(son);
	  if (!akai)
	    ListDirectories(son);
	  else
	    ListAkaiCD(son);
	  son = folder->GetNextChild(item, cookie);
	}
    }
}

void FileLoader::OnCollapseFolder(wxTreeEvent &e)
{
  wxTreeItemId item = e.GetItem();
  folder->SetItemImage(item, 0);
}

void FileLoader::OnSelectFolder(wxTreeEvent &e)
{
  FileInfo->SetLabel(wxString(wxT("")));
  StopPlaying();
  wxTreeItemId item = e.GetItem();
  TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
  if (!akai)
    {
      ListFiles(path->GetPath().c_str());
      filename->SetValue(wxT(""));
    }
  else
    ListAkaiVolume(path->GetPath().c_str());
	if (!save)
	  {
	    preview->Disable();
	    btopen->Disable();
	  }
}

void FileLoader::OnActivateFolder(wxTreeEvent &e)
{
  FileInfo->SetLabel(wxString(wxT("")));
  wxTreeItemId item = e.GetItem();
  if (folder->ItemHasChildren(item))
    {
      if (folder->IsExpanded(item))
			folder->Collapse(item);	
      else
	folder->Expand(item);
    }
}

void FileLoader::PopupMenuFolder(wxTreeEvent &e)
{
  if (!akai)
    {
      pm_selitem = e.GetItem();
      TreeItemData *path = (TreeItemData *)folder->GetItemData(pm_selitem);
      wxString str = path->GetPath().c_str();
      wxMenu *menu = new wxMenu(str);
      menu->Append(ADDTOFAVORITES_ID, _("Add to favorites"));
      folder->PopupMenu(menu, e.GetPoint());
      delete menu;
    }
}

void FileLoader::OnSelectFile(wxListEvent &e)
{
  FileInfo->SetLabel(wxString(wxT("")));
  if (!akai)
    {
      wxTreeItemId item = folder->GetSelection();
      TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
      wxString fn = path->GetPath();
      fn += e.GetText();
      filename->SetValue(fn.c_str());	
      btopen->Enable();
      StopPlaying();
      if (!save)
	{
	  //	  if (ExtMatch((char *)e.GetText().c_str(), (char *)type->GetClientData(0)))
	  if (e.GetText())
	    preview->Enable();
	  else
	    preview->Disable();
      FileStat      Stats;
      if (Stats.StatFile(fn.mb_str(*wxConvCurrent)))
        FileInfo->SetLabel(wxString(Stats.GetFormat() + wxString(wxT(", ")) + Stats.GetBitNess() + 
                            Stats.GetLenght().Format(wxT("\t\t\t %H hours - %M minutes - %S seconds")) +
                            wxString(wxT("\n")) + Stats.GetNbChannels().ToString() + wxString(wxT(" channel(s), ")) + 
                            Stats.GetSampleRate().ToString() + wxT(" Hz")));
    }
    }
  else
    {
      StopPlaying();
      btopen->Enable();
      wxListItem it;
      it.SetId(e.GetIndex());
      it.SetColumn(2);
      files->GetItem(it);
      if(!save)
	{
	  if (it.GetText().Contains(wxT("SAMPLE")))
	    preview->Enable();
	  else
	    preview->Disable();
	}
      
    }
}

void FileLoader::OnActivateFile(wxListEvent &e)
{
  StopPlaying();
  if (!akai)
    {
      wxTreeItemId item = folder->GetSelection();
      TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
      wxString fn = path->GetPath();
      fn += e.GetText();
      filename->SetValue(fn.c_str());	
      if (!save)
	  {
	    if (mru->FindString(path->GetPath().c_str()) == -1)
	      {
		mru->Append(path->GetPath().c_str());
		while (mru->GetCount() > 10)
		  mru->Delete(0);
	      }
	  }
      else
	{
	  fn = filename->GetValue().c_str();
	  if (fn.size() == 0)
	    {	
	      wxMessageDialog *err = 
		new wxMessageDialog(this, _("Please enter a name"), wxT("wired"),
				    wxOK | wxICON_ERROR);
	      err->ShowModal();
	      return;
	    }
	  if (fn.c_str()[0] != '/')
	    {
	      wxTreeItemId item = folder->GetSelection();
	      TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
	      fn = path->GetPath() + fn;
	    }
	  if (wxFile::Exists(fn.c_str()))
	    {
	      wxMessageDialog *err = 
		new wxMessageDialog(this, _("File already exists, replace ?"), 
				    wxT("wired"), wxYES_NO | wxICON_QUESTION);
	      if (err->ShowModal() == wxID_NO)
		return;
	    }	
	}
    }
  if (akaifd != -1)
    close(akaifd);
  EndModal(wxID_OK);
}

//
// Called when the user changes the extension filter
// Update the displayed files in the list
//
// (Contains some ugly code because of wxwidget's
// wxComboBox::GetSelection() bug returning the previous
// value.)
//

void			FileLoader::OnChangeFilter(wxCommandEvent &e)
{
  wxString		entry = type->GetValue();
  int			start =  entry.find_last_of(wxT("(")) + 1;
  int			len = entry.find_last_of(wxT(")")) - start;
  wxTreeItemId		item = folder->GetSelection();
  TreeItemData		*path = (TreeItemData *)folder->GetItemData(item);

  StopPlaying();
  filename->SetValue(wxT(""));
 
  filters = wxT(";") + entry.substr(start, len);
  filters.Replace(wxT("*."), wxT(""));
  filters.Replace(wxT(" "), wxT(""));
  filters += wxT(";");

  if (!save)
    {
      preview->Disable();
      btopen->Disable();
    }
  ListFiles(path->GetPath().c_str());
}

void FileLoader::OnPreview(wxCommandEvent &e)
{
  if (!playing)
    StartPlaying();
  else
    StopPlaying();
}

void FileLoader::OnOpen(wxCommandEvent &e)
{
  StopPlaying();
  if (!akai)
    {
      if (!save)
	{
	  wxTreeItemId item = folder->GetSelection();
	  TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
	  if (mru->FindString(path->GetPath().c_str()) == -1)
	    {
	      mru->Append(path->GetPath().c_str());
	      while (mru->GetCount() > 10)
		mru->Delete(0);
	    }
	}
      else
	{
	  wxString fn = filename->GetValue().c_str();
	  if (fn.size() == 0)
	    {	
	      wxMessageDialog *err = 
		new wxMessageDialog(this, _("Please enter a name"), 
				    wxT("wired"), wxOK | wxICON_ERROR);
	      err->ShowModal();
	      return;
	    }
	  if (fn.c_str()[0] != '/')
	    {
	      wxTreeItemId item = folder->GetSelection();
	      TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
	      fn = path->GetPath() + fn;
	    }
	  if (wxFile::Exists(fn.c_str()))
	    {
	      wxMessageDialog *err = 
		new wxMessageDialog(this, _("File already exists, replace ?"), 
				    wxT("wired"), wxYES_NO | wxICON_QUESTION);
	      if (err->ShowModal() == wxID_NO)
		return;
	    }	
	}
    }
  if (akaifd != -1)
    close(akaifd);
  EndModal(wxID_OK);
}

void FileLoader::OnCancel(wxCommandEvent &e)
{
  StopPlaying();
  if (akaifd != -1)
    close(akaifd);
  EndModal(wxID_CANCEL);
}

void FileLoader::OnEnterFilename(wxCommandEvent &e)
{
  StopPlaying();
  if (!save)
    {
      preview->Disable();
      btopen->Disable();
    }
  if (!akai)
    {
      if (!save)
	{
	  //GotoDir((char *)filename->GetValue().c_str());
	  GotoDir(filename->GetValue(), folder->GetRootItem());
	  files->SetFocus();
	}
      else
	{
	  wxString fn = filename->GetValue().c_str();
	  if (fn.size() == 0)
	    {	
	      wxMessageDialog *err = 
		new wxMessageDialog(this, _("Please enter a name"), 
				    wxT("wired"), wxOK | wxICON_ERROR);
	      err->ShowModal();
	      return;
	    }
	  if (fn.c_str()[0] != '/')
	    {
	      wxTreeItemId item = folder->GetSelection();
	      TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
	      fn = path->GetPath() + fn;
	    }
	  if (wxFile::Exists(fn.c_str()))
	    {
	      wxMessageDialog *err = 
		new wxMessageDialog(this, _("File already exists, replace ?"), 
				    wxT("wired"), wxYES_NO | wxICON_QUESTION);
	      if (err->ShowModal() == wxID_NO)
		return;
	    }	
	  EndModal(wxID_OK);
	}
    }
  else
    {
      if (akaifd != -1)
	close(akaifd);
      akaifd = -1; 
      files->DeleteAllItems();
      folder->DeleteAllItems();
      ListAkaiCD(folder->AddRoot(wxT("/"), 0, -1, new TreeItemData(wxT("/"))));
    }
}

wxString FileLoader::GetSelectedFile()
{
  if (!save)
    {
      long sel = files->GetFirstSelected();
      if (sel != -1)
	{
	  wxString fn = files->GetItemText(sel).c_str();
	  wxTreeItemId item = folder->GetSelection();
	  TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
	  if (!akai)
	    fn = path->GetPath() + fn;
	  else
	    {
	      fn = path->GetPath() + fn;
	      fn = wxT(":") + fn;
	      fn = filename->GetValue().c_str() + fn;
	    }
	  return (fn);
	}
      else
	return (wxT(""));
    }
  else
    {
      wxString fn = filename->GetValue().c_str();
      if (fn.c_str()[0] != '/')
	{
	  wxTreeItemId item = folder->GetSelection();
	  TreeItemData *path = (TreeItemData *)folder->GetItemData(item);
	  fn = path->GetPath() + fn;
	}
      return (fn);
    }
  return(wxT(""));
}

void FileLoader::StopPlaying()
{
  FileInfo->SetLabel(wxString(wxT("")));
  playing = false;
  if (!save)
    preview->SetLabel(_("Preview"));
  files->SetFocus();
  //  wxCommandEvent event(wxEVT_FILELOADER_STOP, GetId());
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, FileLoader_Stop);  
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
}

void FileLoader::StartPlaying()
{
  playing = true;
  if (!save)
    preview->SetLabel(_("Stop"));
  files->SetFocus();

  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, FileLoader_Start);  
  event.SetEventObject(this);
  wxPostEvent(GetParent(), event);
}

void FileLoader::OnAddToFavorites(wxCommandEvent &e)
{
  TreeItemData *path = (TreeItemData *)folder->GetItemData(pm_selitem);
  if (favorites->FindString(path->GetPath().c_str()) == -1)
    favorites->Append(path->GetPath().c_str());
  files->SetFocus();
}

void FileLoader::OnSelectFavoriteDir(wxCommandEvent &e)
{
  files->SetFocus();
  long item = favorites->GetSelection();
  if (item != -1)
    //GotoDir((char *)favorites->GetString(item).c_str());
    GotoDir(favorites->GetString(item), folder->GetRootItem());
}

void FileLoader::OnSelectRecentDir(wxCommandEvent &e)
{
  files->SetFocus();
  long item = mru->GetSelection();
  if (item != -1)
    //    GotoDir((char *)mru->GetString(item).c_str());
    GotoDir(favorites->GetString(item), folder->GetRootItem());
}

void FileLoader::OnDeleteFavorite(wxCommandEvent &e)
{
  files->SetFocus();
  long item = favorites->GetSelection();
  if (item != -1)
    favorites->Delete(item);
  favorites->SetSelection(0);
  if (favorites->GetCount() == 0)
    {
      favorites->SetWindowStyle(favorites->GetWindowStyle() & (~wxCB_READONLY));
      favorites->SetValue(wxT(""));
      favorites->SetWindowStyle(favorites->GetWindowStyle() | wxCB_READONLY);
    }
}

void FileLoader::OnDeleteRecent(wxCommandEvent &e)
{
  files->SetFocus();
  long item = mru->GetSelection();
  if (item != -1)
    mru->Delete(item);
  mru->SetSelection(0);
  if (mru->GetCount() == 0)
    {
      mru->SetWindowStyle(mru->GetWindowStyle() & (~wxCB_READONLY));
      mru->SetValue(wxT(""));
      mru->SetWindowStyle(mru->GetWindowStyle() | wxCB_READONLY);
    }
}
