// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __WAVEPANEL__
#define __WAVEPANEL__

#include 			<wx/toolbar.h>
#include			<wx/combobox.h>
#include 			<list>
#include 			"WaveEditor.h"
#include 			"WaveEditorDrawer.h"


#define 			SCROLLH_THUMB_WIDTH	(80)
#define 			SCROLLV_THUMB_WIDTH	(80)
#define 			SCROLLH				(20)
#define 			SCROLLV				(20)
#define 			WAVE_TOOLBAR_HEIGHT	(32)
#define				NB_CHOICE			(5)

enum
{
    ID_HSCROLL,
    ID_TOOL_SAVE_WAVE,
    ID_TOOL_COPY_WAVE, 
    ID_TOOL_PASTE_WAVE,
    ID_TOOL_CUT_WAVE,
    ID_TOOL_DEL_WAVE,
    ID_TOOLBAR_WAVE,
    ID_TOOL_UNDO_WAVE,
    ID_TOOL_REDO_WAVE,
    ID_TOOL_SELECT_WAVE,
    ID_TOOL_COMBO_WAVE,
    ID_SEBMENU_EFFECTS_WAVEEDITOR,
    ID_SUBMENU_GAIN_WAVEEDITOR,
    ID_SUBMENU_NORMA_WAVEEDITOR
};




typedef struct					s_choice
{
  wxString						s;
  double						value;
} t_choice;

extern const struct s_choice	Choice[NB_CHOICE + 1];


class WavePanel : public wxPanel
{
  
 public:
  WavePanel(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
	  long style = wxSIMPLE_BORDER | wxWS_EX_PROCESS_IDLE);
  ~WavePanel();

  void				OnCopy(wxCommandEvent &event);
  void				OnCut(wxCommandEvent &event);
  void				OnPaste(wxCommandEvent &event);
  void				OnDelete(wxCommandEvent &event);
  void 				OnScroll(wxScrollEvent &event);
  void				AdjustScrollbar();
  void				OnZoom(wxCommandEvent &event);
  void				OnUndo(wxCommandEvent &event);
  void				OnRedo(wxCommandEvent &event);
  void				OnDetach(wxFrame *f);
  void				OnSelect(wxCommandEvent &event);

  wxComboBox 		*combobox;
  wxScrollBar		*sbh;
  wxToolBar			*Toolbar;
  WaveEditor        *w;


 private:  
  //long 				mPosition;

  DECLARE_EVENT_TABLE()
};



#endif
