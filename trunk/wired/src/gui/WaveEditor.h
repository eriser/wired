// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __WAVEEDITOR__
#define __WAVEEDITOR__

#include 			"WaveDrawer.h"
#include 			"WaveFile.h"
#include 			<wx/toolbar.h>
#include			<wx/combobox.h>
#include 			<list>

#define SCROLLH_THUMB_WIDTH		(80)
#define SCROLLV_THUMB_WIDTH		(80)
#define SCROLLH					(20)
#define SCROLLV					(20)
enum
{
    ID_SCROLL,
    ID_SCROLLH,
    ID_SCROLLV,
    ID_POPUP_CUT_WAVEEDITOR,
    ID_POPUP_COPY_WAVEEDITOR, 
    ID_POPUP_PASTE_WAVEEDITOR,
    ID_TOOL_SAVE_WAVEEDITOR,
    ID_TOOL_COPY_WAVEEDITOR, 
    ID_TOOL_PASTE_WAVEEDITOR,
    ID_TOOL_CUT_WAVEEDITOR,
    ID_TOOLBAR_WAVEEDITOR,
    ID_TOOL_UNDO_WAVEEDITOR,
    ID_TOOL_REDO_WAVEEDITOR,
	ID_TOOL_COMBO
};


#define NB_CHOICE		(5)

typedef struct				s_choice
{
  wxString				s;
  double				value;
} t_choice;

extern const struct s_choice	Choice[NB_CHOICE + 1];


class WaveEditor : public wxPanel, public WaveEditorDrawer 
{
 public:
  WaveEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
	  long style = wxSIMPLE_BORDER | wxWS_EX_PROCESS_IDLE, bool fulldraw = false, bool use_settings = true);
  ~WaveEditor();

  void			OnPaint(wxPaintEvent &event);
  void			OnSize(wxSizeEvent &event);
  void			OnMouseEvent(wxMouseEvent& event);
  virtual void			SetWave(float **data, unsigned long frame_length, long channel_count);
  virtual void			SetWave(WaveFile *w);
  void					SetDrawing();
  void					SetSize(wxSize s);
  void					OnCopy(wxCommandEvent &event);
  void					OnCut(wxCommandEvent &event);
  void					OnPaste(wxCommandEvent &event);
  void 					OnScroll(wxScrollWinEvent &event);
  void					RedrawBitmap(wxSize s);
  void					OnSave(wxCommandEvent &event);
  void					AdjustScrollbar(wxSize s);
  void					OnZoom(wxCommandEvent &event);
  void					OnUndo(wxCommandEvent &event);
  void					OnRedo(wxCommandEvent &event);

  wxMenu				*PopMenu;
  wxFrame				*frame;
  wxComboBox 			*combobox;
 private:  
  wxRect		mSelectedRegion;
  bool			mIsSelecting;
  wxScrollBar			*sbh;
  wxScrollBar			*sbv;
  long 					inc;
  long 					mPosition;
  long					oldposx;
  long					xsrc;
  int					len;
  wxToolBar				*Toolbar;
  int					flag;
  long					sizePaste;
 
  DECLARE_EVENT_TABLE()
};

// ---------- class CutAction

#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"


class cCutAction : public cAction 
{
private:
   WaveFile* wave;
   long		from;
   long		width;
 
public:
  cCutAction (WaveFile *Wave, long From, long Width)
  { wave = Wave; 
	from = From;
	width = Width; };

  ~cCutAction ()
  {};

  virtual void Do ();

  virtual void Redo ();

  virtual void Undo ();

  virtual void Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};


// ---------- class cPasteAction

class cPasteAction : public cAction 
{
private:
   WaveFile* wave;
   long		to;
   long		width;
 
public:
  cPasteAction (WaveFile *Wave, long TO, long Width)
  { wave = Wave; 
	to = TO;
	width = Width; 
  };

  ~cPasteAction ()
  {
	std::cout << "[cPasteAction] - Destructeur Test" << std::endl;
  };

  virtual void Do ();

  virtual void Redo ();

  virtual void Undo ();

  virtual void Accept (cActionVisitor& visitor)
  { cout << "Test accept" << "\n"; visitor.Visit (*this); };
};


#endif
