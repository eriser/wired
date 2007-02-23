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
    ID_TOOL_MOVE_EDITMIDI,
    ID_TOOL_EDIT_EDITMIDI,
    ID_TOOL_DEL_EDITMIDI
  };

#define	MIDIPART_WIDTH			(500)
#define	SBS				(20)
#define	SBPASH				(10)
#define	SBPASV				(10)

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

  wxScrolledWindow			*swg;
  wxScrolledWindow			*swd;
  wxScrolledWindow			*swr;
  wxScrolledWindow			*swbg;
  wxScrolledWindow			*swbd;
  wxScrollBar				*sbv;
  wxScrollBar				*sbh;
  class Clavier				*clavier;
  class MidiPart			*mp;
  class RulerMidi			*rm;
  class MidiAttr			*ma;
  wxSlider				*ZoomX;
  wxSlider				*ZoomY;
  wxWindow				*top;
  wxWindow				*bottom;
  wxSplitterWindow			*splitter;
  wxToolBar				*toolbar;
  MidiPattern				*midi_pattern;
  int					detached;
  DECLARE_EVENT_TABLE()
};

#endif/*__EDITMIDI_H__*/
