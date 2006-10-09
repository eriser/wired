// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MEDIALIBRARY_H__
#define __MEDIALIBRARY_H__

#include <wx/wx.h>
#include "MainWindow.h"
#include "FileConversion.h"

class					MainWindow;
class					DownButton;
class					HoldButton;
class					StaticLabel;
class					VUMCtrl;
class					MLTree;

#define MEDIALIBRARY_ADDUP_IMG		wxT("ihm/toolbar/add_up.png")
#define MEDIALIBRARY_ADDDO_IMG		wxT("ihm/toolbar/add_up.png")
#define MEDIALIBRARY_REMOVEUP_IMG	wxT("ihm/toolbar/del_up.png")
#define MEDIALIBRARY_REMOVEDO_IMG	wxT("ihm/toolbar/del_up.png")
#define MEDIALIBRARY_EDITUP_IMG		wxT("ihm/toolbar/edit_up.png")
#define MEDIALIBRARY_EDITDO_IMG		wxT("ihm/toolbar/edit_up.png")
#define MEDIALIBRARY_INSERTUP_IMG	wxT("ihm/toolbar/insert_up.png")
#define MEDIALIBRARY_INSERTDO_IMG	wxT("ihm/toolbar/insert_up.png")

#define MEDIALIBRARY_PREVIEWUP_IMG	wxT("ihm/toolbar/play.png")
#define MEDIALIBRARY_PREVIEWDO_IMG	wxT("ihm/toolbar/play.png")
#define MEDIALIBRARY_COLLAPSEUP_IMG	wxT("ihm/toolbar/collapse_up.png")
#define MEDIALIBRARY_COLLAPSEDO_IMG	wxT("ihm/toolbar/collapse_up.png")

#define MEDIALIBRARY_FILEAUDIOUP_IMG	wxT("ihm/toolbar/file_audio_up.png")
#define MEDIALIBRARY_FILEAUDIODO_IMG	wxT("ihm/toolbar/file_audio_up.png")
#define MEDIALIBRARY_FILEVIDEOUP_IMG	wxT("ihm/toolbar/file_mov_up.png")
#define MEDIALIBRARY_FILEVIDEODO_IMG	wxT("ihm/toolbar/file_mov_up.png")
#define MEDIALIBRARY_FILEMIDIUP_IMG	wxT("ihm/toolbar/file_midi_up.png")
#define MEDIALIBRARY_FILEMIDIDO_IMG	wxT("ihm/toolbar/file_midi_up.png")
#define MEDIALIBRARY_FILEFXUP_IMG	wxT("ihm/toolbar/file_fx_up.png")
#define MEDIALIBRARY_FILEFXDO_IMG	wxT("ihm/toolbar/file_fx_up.png")

#define ML_BACKGROUND			wxColour(142, 142, 169)
#define ML_FOREGROUND			wxColour(142, 142, 155)
#define NB_SORTSELECT_CHOICES		(4)
#define DEFAULT_SORTSELECT_VALUE	wxT("Sort by...")


/**
 * The MediaLibrary class is the main class for the media library feature
 * It is designed to build the interface dedicated to the medialibrary
 * It is derived from wxPanel. It include the buttons and the tree
*/
class				MediaLibrary : public wxPanel
{
 public:
/**
 * Default constructor for the class MediaLibrary
*/
  MediaLibrary(wxWindow *parent,
	       const wxPoint &pos = wxDefaultPosition,
	       const wxSize &size = wxDefaultSize,
	       long style = wxTAB_TRAVERSAL);
/**
 * Default destructor for the class MediaLibrary
*/
  ~MediaLibrary();

 protected:

  friend class			MLTree;
/**
 * Boolean used to know if the media library panel is visible or unvisible
*/
  bool				visible;
/**
 * Boolean used to know if the media library panel is floating or docked
*/
  bool				floating;
/**
 * wxToolBar containing the butons located on top of the medialibrary panel
*/
  wxToolBar			*TopToolbar;
/**
 * wxToolBar containing the butons located on bottom of the medialibrary
 * panel
*/
  wxToolBar			*BottomToolbar;
/**
 * wxToolBar containing the butons dedicated to the filters of the filetypes
 * This wxToolBar is located under the BottomToolbar
*/
  wxToolBar			*FiltersToolbar;
/**
 * wxBoxSizer containing all the previously defined wxToolBar
*/
  wxBoxSizer			*TopSizer;
/**
 * wxComboBox designed to hold the rolling menu offering sorting
 * possibilities
*/
  wxComboBox			*SortSelect;
/**
 * MLTree derived from wxTreeCtrl. Contains the tree of the Media Library
*/
  MLTree			*MLTreeView;
/**
 * FileConversion for converting files to the used samplerate before using
 * them in the Sequencer
*/
  FileConversion		*FileConverter;

 public:
/**
 * The SetFileConverter, used to define a fileconverter in the medialibrary
 * \param FileConv an *FileConversion object
 * \return void
*/
  void				SetFileConverter(FileConversion *FileConv);
/**
 * The OnSize function is called by an event every time the media library
 * panel is resized by the user. It change the size of the Tree and the
 * position of the buttons.
 * \param event a &wxSizeEvent because it is the normal wxSize event handling
 * \return void
*/
  void				OnSize(wxSizeEvent &event);
/**
 * The OnEdit function is called by launching the Edit action, using the
 * dedicated button or the contextual menu. It insert the file into a track
 * and then into the Wav editor
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnEdit(wxCommandEvent &WXUNUSED(event));
/**
 * The OnInsert function is called by launching the Insert action, using the
 * dedicated button or the contextual menu. It insert the file into a track
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnInsert(wxCommandEvent &WXUNUSED(event));
/**
 * The OnPreview function is called by launching the Preview action, using
 * the dedicated button or the contextual menu. It launch a preview of the
 * file selected into the Media Library Tree
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnPreview(wxCommandEvent &WXUNUSED(event));
/**
 * The OnAdd function is called by launching the Add File action, using the
 * dedicated button. It launch the wired file browser allowing the user to
 * select a file. When validating, the file is added to the selected node
 * or automaticaly sorted if no node is selected
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnAdd(wxCommandEvent &WXUNUSED(event));
/**
 * The OnRemove function is called by launching the Delete action, using the
 * dedicated button or the contextual menu. It delete the selected file
 * from the Media Library Tree
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnRemove(wxCommandEvent &WXUNUSED(event));

/**
 * The OnCollapse function is called by launching the Remove action using
 * the dedicated button. It collapse or expand all existing nodes from the
 * MediaLibrary Tree, depending of the state of the tree
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnCollapse(wxCommandEvent &WXUNUSED(event));
/**
 * The OnSortToggle function is called when changing the selection of the
 * SortToggle variable. It retreive the new selection before calling the
 * SortNodes(wxString MLselected) function from the MLTree class.
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnSortToggle(wxCommandEvent &WXUNUSED(event));
/**
 * The OnFilterAudio function is called by launching the Filter Audio action
 * using the dedicated button. It will browse all existing nodes in the Tree
 * and show or hide the audio files.
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnFilterAudio(wxCommandEvent &WXUNUSED(event));
/**
 * The OnFilterMIDI function is called by launching the Filter MIDI action
 * using the dedicated button. It will browse all existing nodes in the Tree
 * and show or hide the MIDI files.
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnFilterMIDI(wxCommandEvent &WXUNUSED(event));
/**
 * The OnFilterVideo function is called by launching the Filter Video action
 * using the dedicated button. It will browse all existing nodes in the Tree
 * and show or hide the video files.
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnFilterVideo(wxCommandEvent &WXUNUSED(event));
/**
 * The OnFilterEffects function is called by launching the Filter Effects 
 * action using the dedicated button. It will browse all existing nodes in 
 * the Tree and show or hide the Effects files.
 * \param event an unused wxCommandEvent because it is the normal wx event
 * handling
 * \return void
*/
  void				OnFilterEffects(wxCommandEvent &WXUNUSED(event));
/**
 * The OnRightClick function is called by using the mouse right click on a
 * MLTree node. It displays a menu with possibles actions depending on the
 * kind of item selected.
 * \param event a wxMouseEvent because it is the normal wx mouse event
 * handling
 * \return void
*/
  void				OnRightClick(wxMouseEvent &event);
/**
 * The OnLeftClick function is called by using the mouse left click on a
 * MLTree node. 
 * \param event a wxMouseEvent because it is the normal wx mouse event
 * handling
 * \return void
*/
  void				OnLeftClick(wxMouseEvent &event);
/**
 * The ShowPopup function is called 
 * \param event a wxMouseEvent because it is the normal wx mouse event
 * handling
 * \return void
*/
//  void				ShowPopup(wxPoint pos);

/**
 * The IsVisible function is used to test if the MediaLibrary is visible
 * or hidden.
 \return a boolean. true if the MediaLibrary is visible, else return false
*/
  bool				IsVisible();
/**
 * The SetInVisible function is a seter called when seting the MediaLibrary
 * panel to invisible. It set the boolean visible to false.
 \return void
*/
  void				SetInvisible();
/**
 * The SetVisible function is a seter called when seting the MediaLibrary
 * panel to visible. It set the boolean visible to true.
 \return void
*/
  void				SetVisible();
/**
 * The IsFloating function is used to test if the MediaLibrary is floating
 * or docked.
 \return a boolean. true if the MediaLibrary is floating, else return false
*/
  bool			        IsFloating();
/**
 * The SetFloating function is a seter called when seting the MediaLibrary
 * panel to floating. It set the boolean floating to true.
 \return void
*/
  void				SetFloating();
/**
 * The SetDocked function is a seter called when seting the MediaLibrary
 * panel to docked. It set the booelan floating to false.
 \return void
*/
  void				SetDocked();

/**
 * Event table where are enumerated all the events associated to the 
 * MediaLibrary.
*/
  DECLARE_EVENT_TABLE()
};

/**
 * Struct s_combo_choice containing the list of possible choices in the 
 * select button dedicated to the filetypes sorting.
*/
extern const struct s_combo_choice	SortSelectChoices[NB_SORTSELECT_CHOICES + 1];

/**
 * The enum table
*/
enum
{
   MediaLibrary_Add = 1,
   MediaLibrary_Remove,
   MediaLibrary_Edit,
   MediaLibrary_Insert,
   MediaLibrary_Preview,
   MediaLibrary_TreeCollapse,
   MediaLibrary_SortSelect,
   MediaLibrary_FilterAudio,
   MediaLibrary_FilterMIDI,
   MediaLibrary_FilterVideo,
   MediaLibrary_FilterEffects
};


#endif
