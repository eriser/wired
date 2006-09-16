// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MEDIALIBRARY_H__
#define __MEDIALIBRARY_H__

#include <wx/wx.h>
#include "MainWindow.h"

class				MainWindow;
class				DownButton;
class				HoldButton;
class				StaticLabel;
class				VUMCtrl;

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


class				MLTree;

class				MediaLibrary : public wxPanel
{
 public:
  MediaLibrary(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style);
  ~MediaLibrary();


 protected:
  friend class			MainWindow;
  friend class			MLTree;

  bool				visible;
  bool				floating;
  wxToolBar			*TopToolbar;
  wxToolBar			*BottomToolbar;
  wxToolBar			*FiltersToolbar;
  wxBoxSizer			*TopSizer;
  // A effacer ?
  //  wxBoxSizer			*TreeSizer;
  //  wxBoxSizer			*BottomSizer;
  ///////////
  wxComboBox			*SortSelect;
  wxTextCtrl			*mlTextCtrl;
  wxMenu			*PopMenu;
  MLTree			*MLTreeView;
  FileConversion		*FileConverter;
 
  void				SetFileConverter(FileConversion *FileConv);
  void				OnSize(wxSizeEvent &event);
  void				OnEdit(wxCommandEvent &WXUNUSED(event));
  void				OnInsert(wxCommandEvent &WXUNUSED(event));
  void				OnPreview(wxCommandEvent &WXUNUSED(event));
  void				OnAdd(wxCommandEvent &WXUNUSED(event));
  void				OnRemove(wxCommandEvent &WXUNUSED(event));
  void				OnCollapse(wxCommandEvent &WXUNUSED(event));
  void				OnSortToggle(wxCommandEvent &WXUNUSED(event));
  void				OnFilterAudio(wxCommandEvent &WXUNUSED(event));
  void				OnFilterMIDI(wxCommandEvent &WXUNUSED(event));
  void				OnFilterVideo(wxCommandEvent &WXUNUSED(event));
  void				OnFilterEffects(wxCommandEvent &WXUNUSED(event));
  void				OnRightClick(wxMouseEvent &event);
  void				ShowPopup(wxPoint pos);
 
  bool				IsVisible();
  void				SetInvisible();
  void				SetVisible();
  bool			        IsFloating();
  void				SetFloating();
  void				SetDocked();

  DECLARE_EVENT_TABLE()
};


extern const struct s_combo_choice	SortSelectChoices[NB_SORTSELECT_CHOICES + 1];

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
