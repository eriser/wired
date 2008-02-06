// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991



#ifndef __EDITMIDI_H__
#define __EDITMIDI_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>

enum
  {
    ID_CLAVIER_EDITMIDI = 1442,
    ID_MIDIPART_EDITMIDI,
    ID_SCROLLH_EDITMIDI,
    ID_SCROLLV_EDITMIDI,
    ID_ZOOMX_EDITMIDI,
    ID_ZOOMY_EDITMIDI,
    ID_SPLITTER_EDITMIDI,
    ID_MIDIATTR_EDITMIDI,
    ID_TOOLBAR_EDITMIDI,
    ID_CB_CONTROLCHANGE
  };

#define	MIDIPART_WIDTH			(500)
#define	SBS				(20)
#define	SBPASH				(10)
#define	SBPASV				(10)

#define MAGNETISM_EDITMIDI			(3)
#define NB_COMBO_CHOICES		(9)
#define DEFAULT_MAGNETISM_COMBO_VALUE	wxT("1/4")

#define  BOTTOM_HEIGHT			128

/**
 * The MidiPattern class is used for something.
 */

class					MidiPattern;

/**
 * The EditMidi class is used to handle the midi audio format.
 */

class					EditMidi: public wxPanel
{
 public:
  EditMidi(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,
	   long style = wxSIMPLE_BORDER);

  void					SetMidiPattern(MidiPattern *mpattern);
  void					OnScroll(wxScrollEvent &e);
  void					OnResize(wxSizeEvent &e);
  void					Resize(long w, long h);
  void					ResizeClavier(long w, long h);
  void					ResizeMidiPart(long w, long h);
  void					OnZoomX(wxCommandEvent &e);
  void					OnZoomY(wxCommandEvent &e);
  void					OnSplitterChanged(wxSplitterEvent &e);
  void					MoveSash(long);
  void					OnDetach(wxFrame *f);
  void					OnAttach();
  void					OnToolMove(wxCommandEvent &e);
  void					OnToolEdit(wxCommandEvent &e);
  void					OnToolDel(wxCommandEvent &e);
  void					OnToolMagnetH(wxCommandEvent &event);
  void					OnToolMagnetV(wxCommandEvent &event);
  void					OnMagnetismChange(wxCommandEvent &event);
  void          OnControlChange(wxCommandEvent &event);

  wxScrolledWindow			*swg;
  wxScrolledWindow			*swd;
  wxScrolledWindow			*swr;
  wxScrolledWindow			*swbg;
  wxComboBox            *cbControlChange;
  wxScrolledWindow			*swbd;
  wxScrollBar				*sbv;
  wxScrollBar				*sbh;
  class Clavier				*clavier;
  class MidiPart			*mp;
  class RulerMidi			*rm;
  class MidiAttr			*ma;
  wxSlider					*ZoomX;
  wxSlider					*ZoomY;
  wxWindow					*top;
  wxWindow					*bottom;
  wxSplitterWindow			*splitter;
  wxToolBar					*toolbar;
  MidiPattern				*midi_pattern;
  bool						Magnetism;
  wxComboBox				*MagnetQuant;
  unsigned short		CursorMagnetism;
  unsigned short		PatternMagnetism;

  private:
    // Strings used in the bottom left control box (control change)
    wxString cbControlChangeChoices[129];
    // Size of this window before resizing
    wxSize   previousSize;
    // Boolean to know if this is a free window or this is docked in option panel
    bool     detached;

  DECLARE_EVENT_TABLE()
};

#endif/*__EDITMIDI_H__*/
