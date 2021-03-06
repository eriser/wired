// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __WAVEEDITOR__
#define __WAVEEDITOR__

#include 			<wx/toolbar.h>
#include			<wx/combobox.h>
#include 			<list>
#include 			"WaveEditorDrawer.h"
#include 			"WaveFile.h"
#include 			"PluginEffect.h"

#define 			SCROLLH_THUMB_WIDTH	(80)
#define 			SCROLLV_THUMB_WIDTH	(80)
#define 			SCROLLH			(20)
#define 			SCROLLV			(20)
#define 			NB_CHOICE_NORMA		(2)



typedef struct		      s_nchoice
{
  wxString		      s;
  double		      value;
} t_nchoice;


extern const struct s_nchoice  NChoice[NB_CHOICE_NORMA+1];


class WaveEditor : public wxPanel, public WaveEditorDrawer 
{
 public:
  WaveEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
	  long style = wxSIMPLE_BORDER | wxWS_EX_PROCESS_IDLE, bool fulldraw = false, bool use_settings = true);
  ~WaveEditor();

  void				OnPaint(wxPaintEvent &event);
  void				OnSize(wxSizeEvent &event);
  void				OnMouseEvent(wxMouseEvent& event);
  virtual void		        SetWave(float **data, unsigned long frame_length, long channel_count);
  virtual void		        SetWave(WaveFile *w);
  void				SetDrawing();
  void				SetSize(wxSize s);
  void				OnCopy(wxCommandEvent &event);
  void				OnCut(wxCommandEvent &event);
  void				OnPaste(wxCommandEvent &event);
  void				OnDelete(wxCommandEvent &event);
  void 				OnScroll(wxScrollBar *sbh);
  void				RedrawBitmap(wxSize s);
  void				AdjustScrollbar(wxScrollBar *sbh, wxSize s);
  void				OnZoom(wxComboBox *combobox);
  void				OnUndo(wxCommandEvent &event);
  void				OnRedo(wxCommandEvent &event);
  void				OnGain(wxCommandEvent &event);
  void				OnNormalize(wxCommandEvent &event);
  void				OnSelect(wxCommandEvent &event);
  void				TestWave();
  
  wxMenu			*PopMenu;
  wxMenu			*SubMenuEffect;
  
 private:  
  wxRect			mSelectedRegion;
  bool				mIsSelecting;
  long 				inc;
  long 				mPosition;
  long				oldposx;
  long				xsrc;
  int				len;
  int				flag;
  long				sizePaste;
 
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

  ~cPasteAction (){};

  virtual void Do ();

  virtual void Redo ();

  virtual void Undo ();

  virtual void Accept (cActionVisitor& visitor){};
};


#endif
