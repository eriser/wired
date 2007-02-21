// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#ifndef __FILELOADER_H__
#define __FILELOADER_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vector>
#include <list>
#include "TreeItemData.h"
#include "akai.h"
#include "Settings.h"

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


/** Custom "Open File" dialog box.
 * This class create and handle the dialog box used in many features of Wired
 * such as opening a session, import a file, load a patch or a file, ...
 */
class FileLoader: public wxDialog
{
 public:
  /** Default constructor.
   * Build the dialog box into the parent window.
   * \param parent the window that call the fileloader
   * \param id identifier of the window
   * \param title title of the window
   * \param pakai true if it's an Akai device
   * \param psave true if it's for saving a file else false for opening it
   * \param exts vector of extensions supported for opening or saving
   * \param LoadExtraExts allow to load any type of file. Useless if no exts have been set
   */
  FileLoader(wxWindow *parent, wxWindowID id, wxString, bool, bool,
	     vector<wxString> *exts, bool LoadExtraExts = false);

  /** Default destructor */
  ~FileLoader();

  /** Get the name of the file
   * Give the name of the file specified for opening or saving in the dialog box.
   * \return the name of the file
   */
  wxString GetSelectedFile();

  /** Say if the file is an Akai device
   * \return true if the file is an Akai device
   */
  bool	 IsAkai() { return akai; }

  /** Give the unit of the file's size
   * Say if a file is of the order of byte, or kilobyte, megabyte, gigabye
   * \param size size of the file
   * \return the unit of the size
   */
  static wxString      FormatSize(off_t);

 private:

  void OnExpandFolder(wxTreeEvent &e);
  void OnCollapseFolder(wxTreeEvent &e);
  void OnSelectFolder(wxTreeEvent &e);
  void OnActivateFolder(wxTreeEvent &e);
  void PopupMenuFolder(wxTreeEvent &e);

  /** Called when a file is selected in the file List
   * Display informations about the file selected, enable preview, stop playing 
   * of another preview if there is one, and other things
   * \param e Event of the File List
   */ 
  void OnSelectFile(wxListEvent &e);

  /** Called when a file is activated (double-click or ENTER)
   * Perform actions of loading or saving the selected file, stopping the
   * preview, ...
   * \param e Event of the File List
   */ 
  void OnActivateFile(wxListEvent &e);

  /** Called when the user changes the extension filter
   * Update the displayed files in the list
   * \param e Event of the Filter Box
   */ 
  void OnChangeFilter(wxCommandEvent &e);

  /** Called when clicking on the preview button
   * Launch the playing of the file
   * \param e Event of the preview button
   */ 
  void OnPreview(wxCommandEvent &e);

  /** Called when clicking on the open button
   * Stop playing the preview and open the file
   * \param e Event of the open button
   */ 
  void OnOpen(wxCommandEvent &e);

  /** Called when clicking on the cancel button
   * Stop playing the preview and close the dialog box
   * \param e Event of the cancel button
   */ 
  void OnCancel(wxCommandEvent &e);

  /** Called when pressing Enter key while in filename text control
   * Stop playing the preview and open the file
   * \param e Event of the filename box
   */ 
  void OnEnterFilename(wxCommandEvent &e);

  /** Called when selecting the add to favorite in context menu
   * Add the folder to the list of favorites
   * \param e Event of the contextual menu
   */ 
  void OnAddToFavorites(wxCommandEvent &e);

  /** Called when selecting a favorite folder
   * Focus the directory list on the folder selected in the list of favorites
   * \param e Event of the favorites list
   */ 
  void OnSelectFavoriteDir(wxCommandEvent &e);

  /** Called when selecting a recent folder
   * Focus the folder list on the folder selected in the list of recent folders
   * \param e Event of the recent folder list
   */ 
  void OnSelectRecentDir(wxCommandEvent &e);

  /** Called when deleting a favorite folder
   * Remove the favorite folder from the favorite list when clicking the
   * delete favorite button
   * \param e Event of the delete favorite button
   */ 
  void OnDeleteFavorite(wxCommandEvent &e);

  /** Called when deleting a recent folder
   * Remove the recent folder selected from the recent folder list when clicking
   * the delete recent folder button
   * \param e Event of the delete recent button
   */ 
  void OnDeleteRecent(wxCommandEvent &e);

  /** Display directories list
   * Read the file system to display the directory tree displayed on the left
   * of the fileloader
   * \param root root folder of the tree
   */ 
  void ListDirectories(wxTreeItemId);

  /** Display file list
   * Read the file system to display the file list of the folder selected
   * \param path path of the folder to explore
   */ 
  void ListFiles(wxString);

  /** Display directories list of an Akai device
   * Read the device to display the directory tree displayed on the left
   * of the fileloader
   * \param root root folder of the akai device
   */ 
  void ListAkaiCD(wxTreeItemId);

  /** Display file list of an Akai device
   * Read the file system to display the file list of the folder selected
   * \param p path of the folder to explore
   */ 
  void ListAkaiVolume(wxString);

  /** Add an icon to the wxImageList specified
   * \param images wxImageList where add the icon
   * \param icon icon to add to the wxImageList
   */ 
  void AddIcon(wxImageList *, wxIcon);

  /** Add extension filter to the type box 
   * Add extensions and their description to the type combo box under the filename
   * \param exts vector of extension to load
   * \param LoadExtraExts allow to load any type of file. Useless if no exts have been set
   */ 
  void LoadSoundExt(vector<wxString> *exts, bool LoadExtraExts);

  /** Load lists of favorite folders and recent folders
   * Open the files favdir and mrudir and read them to find the list of favorites and
   * recent folders, and fill the combobox associated
   */ 
  void LoadFolders();
  /** Save lists of favorite folders and recent folders
   * Create the files favdir and mrudir and write into them the list of favorit and
   * recent folders
   */ 
  void SaveFolders();

  /** Start playing the preview  */ 
  void StartPlaying();

  /** Stop playing the preview */
  void StopPlaying();

  /** Focus the tree on the path designed
   * Search on the tree the item matching the path and put the focus on it
   * \param path the path to focus on
   * \param parent the root folder of the tree to start searching on
   */ 
  void GotoDir(wxString path, wxTreeItemId parent);

  /** Say if the filename match the extension filter selected 
   * Look at the selected extension filter and check if the filename correspond to it
   * \param path the path of the file
   * \return true if the file is matching the extension, else false
   */ 
  bool ExtMatch(wxString);

  /** The treeview for the folder list */
  wxTreeCtrl		*folder;
  
  /** The listview for the file list */
  wxListView		*files;
  
  /** The text box for the filename */
  wxTextCtrl		*filename;
  
  /** The "Filename" text */
  wxStaticText		*fntext;
  
  /** The "Type" text */
  wxStaticText		*typtext;
  
  /** The "Favorites" text */
  wxStaticText		*favtext;
  
  /** The "Recents" text */
  wxStaticText		*mrutext;

  /** The fileinfo text where all info on the selected file will be displayed */
  wxStaticText		*FileInfo;
  
  /** The select box for type filters */
  wxComboBox		*type;
  
  /** The preview button */
  wxButton		*preview;
  
  /** The open button */
  wxButton		*btopen;
  
  /** The cancel button */
  wxButton		*cancel;
  
  /** Boolean for saving the state of the preview, if it's playing or not */
  bool			playing;

  /** Save the folder selected on right click
   * Save the folder on the tree view when right clicking on it in order to add
   * to favorites when clicking on the option on the popup menu
   */
  wxTreeItemId		pm_selitem;
  
  /** The select box for more recents folders */
  wxComboBox		*mru;
  
  /** The select box for favorites folders */
  wxComboBox		*favorites;
  
  /** The button for deleting the more recent folder selected */
  wxBitmapButton	*mrudel;
  
  /** The button for deleting the favorite folder selected */
  wxBitmapButton	*favdel;
  
  /** True if it's an akai device, false if it's a normal device */
  bool			akai;
  
  /** True if it's a saving operation, false if it's a loading operation */
  bool			save;
  
  /** File descriptor of the akai device */
  int			akaifd;
  
  /** String which define file's filters*/
  wxString		filters;
  
  /** Path of favorite folder selected */
  wxString		favdir;
  
  /** Path of most recent folder selected */
  wxString		mrudir;


  DECLARE_EVENT_TABLE()
};


#endif
