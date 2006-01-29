#include "EditMidi.h"
#include "Clavier.h"
#include "MidiAttr.h"
#include "RulerMidi.h"
#include "MidiPart.h"
#include "../gui/OptionPanel.h"
#include "../engine/Settings.h"

EditMidi::EditMidi(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style) 
  :  wxPanel(parent, id, pos, size, style)
{
  toolbar = NULL;
  // barre pour couper en deux la fenetre
  splitter = new wxSplitterWindow(this, ID_SPLITTER_EDITMIDI, wxPoint(0, 0), wxSize(size.GetWidth(), size.GetHeight() - SBS), wxSP_3DBORDER | wxSP_3DSASH | wxSP_3D);
  
  detached = 0;
  // fenetre du haut
  top = new wxWindow(splitter, -1, wxPoint(0, 0), wxSize(size.GetWidth(), size.GetHeight() - SBS - 128), wxTHICK_FRAME);
  // fenetre du bas
  bottom = new wxWindow(splitter, -1, wxPoint(0, 0), wxSize(size.GetWidth(), 128), wxTHICK_FRAME);
  
  // coupe la fenetre en 2 dans le sens horizontal
  splitter->SplitHorizontally(top, bottom, size.GetHeight() - SBS);
  //splitter->SetMinimumPaneSize(MIDI_RULER_HEIGHT * 3);
  
  // fenetre en bas à gauche (sous le clavier)
  swbg = new wxScrolledWindow(bottom, -1, wxPoint(0, 0), wxSize(CLAVIER_WIDTH, 128), wxSIMPLE_BORDER);
  swbg->SetScrollRate(SBPASH, SBPASV);
  
  // fenetre en bas à droite (midiattr)
  swbd = new wxScrolledWindow(bottom, -1, wxPoint(CLAVIER_WIDTH, 0), 
			      wxSize(size.GetWidth() - SBS - CLAVIER_WIDTH, 128), wxTHICK_FRAME); 
  swbd->SetScrollRate(SBPASH, SBPASV);
  
  // MidiAttr
  ma = new MidiAttr(swbd, ID_MIDIATTR_EDITMIDI, wxPoint(0, 0), wxSize(MIDIPART_WIDTH, 128), wxSIMPLE_BORDER);
  
  // scrollbar horizontale (en bas)
  sbh = new wxScrollBar(this, ID_SCROLLH_EDITMIDI, wxPoint(CLAVIER_WIDTH, 
							  size.GetHeight() - SBS), 
			wxSize(size.GetWidth() - SBS - CLAVIER_WIDTH, SBS), 
			wxSB_HORIZONTAL);
  sbh->SetScrollbar(0, 1, 100, 1);
  
  // scrollbar verticale (a droite)
  sbv = new wxScrollBar(top, ID_SCROLLV_EDITMIDI, wxPoint(size.GetWidth() - SBS, 
							 MIDI_RULER_HEIGHT), wxSize(SBS, size.GetHeight() 
										    - SBS - MIDI_RULER_HEIGHT - 128), wxSB_VERTICAL);
  sbv->SetScrollbar(0, 1, 100, 1);
  
  // fenetre en haut à gauche (clavier)
  swg = new wxScrolledWindow(top, -1, wxPoint(0, MIDI_RULER_HEIGHT),
			     wxSize(CLAVIER_WIDTH, 
				    size.GetHeight() - SBS - MIDI_RULER_HEIGHT), 
			     wxTHICK_FRAME); 
  swg->SetScrollRate(SBPASH, SBPASV);
  
  // clavier
  clavier = new Clavier(swg, ID_CLAVIER_EDITMIDI, wxPoint(0, 0), 
			wxSize(CLAVIER_WIDTH, CLAVIER_HEIGHT), 
			wxSIMPLE_BORDER, this);
  
  // fenetre en haut à droite (midipart)
  swd = new wxScrolledWindow(top, -1, wxPoint(CLAVIER_WIDTH, MIDI_RULER_HEIGHT), 
			     wxSize(size.GetWidth() - CLAVIER_WIDTH - SBS, 
				    size.GetHeight() - SBS - MIDI_RULER_HEIGHT), wxTHICK_FRAME); 
  swd->SetScrollRate(SBPASH, SBPASV);
  
  // fenetre tt en haut (ruler midi)
  swr = new wxScrolledWindow(top, -1, wxPoint(CLAVIER_WIDTH, 0),
			     wxSize(size.GetWidth() - CLAVIER_WIDTH - SBS, 
				    MIDI_RULER_HEIGHT), wxSIMPLE_BORDER);
  swr->SetScrollRate(SBPASH, SBPASV);
  
  // ruler midi
  rm = new RulerMidi(swr, -1, wxPoint(0, 0),
		     wxSize(MIDIPART_WIDTH, MIDI_RULER_HEIGHT),
		     this);
  
  // MidiPart
  mp = new MidiPart(swd, ID_MIDIPART_EDITMIDI, wxPoint(0, 0),
		    wxSize(MIDIPART_WIDTH, CLAVIER_HEIGHT),
		    wxSIMPLE_BORDER, this); 
  mp->SetNPM(8);
  
  /*
   *      // barre de zoom en Y
   ZoomY = new wxSlider(this, ID_ZOOMY_EDITMIDI, 100, 100, 200, wxPoint(0, 0), 
   wxSize(CLAVIER_WIDTH, MIDI_RULER_HEIGHT), 
   wxSL_HORIZONTAL);
  */
  
  // barre de zoom en X
  ZoomX = new wxSlider(this, ID_ZOOMX_EDITMIDI, 100, 100, 1600, 
		       wxPoint(0, size.GetHeight() - SBS), 
		       wxSize(CLAVIER_WIDTH, SBS), 
		       wxSL_HORIZONTAL);
}

void					EditMidi::SetMidiPattern(MidiPattern *mpattern)
{
  midi_pattern = mpattern;
  mp->SetMidiPattern(mpattern);
}

void					EditMidi::Resize(long w, long h)
{
  long					sashp;
  
  if (detached)
    sashp = h - SBS - bottom->GetSize().GetHeight();
  else
    sashp = h - SBS;
  splitter->SetSize(w, h - SBS);
  splitter->SetSashPosition(sashp);
  long					fenh = splitter->GetSashPosition() - SBS;
  long					fenw = w - CLAVIER_WIDTH - SBS;
  long					rh = sbh->GetRange();
  long					rv = sbv->GetRange();
  long					ph = sbh->GetThumbPosition();
  long					pv = sbv->GetThumbPosition();
  long					nw = mp->GetSize().GetWidth() / SBPASH;
  long					nh = clavier->GetSize().GetHeight() / SBPASV;
  long					nph = nw * ph / rh;
  if (nph > (nw - fenw / SBPASH))
    nph = nw - fenw / SBPASH;
  long					npv = nh * pv / rv;

  if (npv > (nh - fenh / SBPASV))
    npv = nh - fenh / SBPASV;
  sbh->SetSize(CLAVIER_WIDTH, h - SBS, w - SBS - CLAVIER_WIDTH, SBS);
  sbh->SetScrollbar(nph, fenw / SBPASH, nw, SBPASH);
  sbv->SetSize(w - SBS, MIDI_RULER_HEIGHT, SBS, fenh);
  sbv->SetScrollbar(npv, fenh / SBPASV, nh, SBPASV);
  swg->SetSize(CLAVIER_WIDTH, fenh);
  swd->SetSize(w - CLAVIER_WIDTH - SBS, fenh);
  swr->SetSize(w - CLAVIER_WIDTH - SBS, MIDI_RULER_HEIGHT);
  swbd->SetSize(w - CLAVIER_WIDTH, bottom->GetSize().GetHeight());
  swbg->SetSize(CLAVIER_WIDTH, bottom->GetSize().GetHeight());
  ma->SetSize(ma->GetSize().GetWidth(), bottom->GetSize().GetHeight());
  ZoomX->SetSize(0, h - SBS, CLAVIER_WIDTH, SBS);
}

void					EditMidi::ResizeClavier(long w, long h)
{
  long					fenh = GetSize().GetHeight() - SBS - MIDI_RULER_HEIGHT;
  long					nh = h / SBPASV;
  long					rv = sbv->GetRange();
  long					pv = sbv->GetThumbPosition();
  long					npv = nh * pv / rv;

  if (npv > (nh - fenh / SBPASV))
    npv = nh - fenh / SBPASV;
  clavier->SetSize(w, h);
  mp->SetSize(mp->GetSize().GetWidth(), h);
  sbv->SetScrollbar(npv, fenh / SBPASV, nh, SBPASV);
}

void					EditMidi::ResizeMidiPart(long w, long h)
{
  long					rh = sbh->GetRange();
  long					ph = sbh->GetThumbPosition();
  long					fenw = GetSize().GetWidth() - CLAVIER_WIDTH - SBS;
  long					nw = w / SBPASH;
  long					nph = nw * ph / rh;

  if (nph > (nw - fenw / SBPASH))
    nph = nw - fenw / SBPASH;
  mp->SetSize(w, h);
  sbh->SetScrollbar(nph, fenw / SBPASH, nw, SBPASH);
  rm->SetSize(w, MIDI_RULER_HEIGHT);
  ma->SetSize(w, ma->GetSize().GetHeight());
}

void					EditMidi::OnResize(wxSizeEvent &e)
{
  wxSize				s = e.GetSize();

  Resize(s.GetWidth(), s.GetHeight());
}

void					EditMidi::OnScroll(wxScrollEvent &e)
{
  static int				oldposx = 0;
  static int				oldposy = 0;

  if (e.GetOrientation() == wxHORIZONTAL)
    {
      int sbpos = (sbh->GetThumbPosition() - oldposx) * SBPASH;
      swd->ScrollWindow(-sbpos, 0, NULL);
      swr->ScrollWindow(-sbpos, 0, NULL);
      swbd->ScrollWindow(-sbpos, 0, NULL);
      oldposx = sbh->GetThumbPosition();;
    }
  else
    {
      int sbpos = (sbv->GetThumbPosition() - oldposy) * SBPASV;
      swd->ScrollWindow(0, -sbpos, NULL);
      swg->ScrollWindow(0, -sbpos, NULL);
      oldposy = sbv->GetThumbPosition();
    }
}

void					EditMidi::OnZoomX(wxCommandEvent &e)
{
  mp->SetZoomX(((double)ZoomX->GetValue()) / 100);
}

void					EditMidi::OnZoomY(wxCommandEvent &e)
{
  //clavier->SetZoomY(((double)ZoomY->GetValue()) / 100);
}

void					EditMidi::MoveSash(long newsashpos)
{
  long					w = GetSize().GetWidth();
  long					h = GetSize().GetHeight();
  long					fenh = newsashpos - SBS;
  long					fenw = w - CLAVIER_WIDTH - SBS;
  long					rh = sbh->GetRange();
  long					rv = sbv->GetRange();
  long					ph = sbh->GetThumbPosition();
  long					pv = sbv->GetThumbPosition();
  long					nw = mp->GetSize().GetWidth() / SBPASH;
  long					nh = clavier->GetSize().GetHeight() / SBPASV;
  long					nph = nw * ph / rh;
  if (nph > (nw - fenw / SBPASH))
    nph = nw - fenw / SBPASH;
  long					npv = nh * pv / rv;

  if (npv > (nh - fenh / SBPASV))
    npv = nh - fenh / SBPASV;
  splitter->SetSize(w, h - SBS);
  sbh->SetSize(CLAVIER_WIDTH, h - SBS, w - SBS - CLAVIER_WIDTH, SBS);
  sbh->SetScrollbar(nph, fenw / SBPASH, nw, SBPASH);
  sbv->SetSize(w - SBS, MIDI_RULER_HEIGHT, SBS, fenh);
  sbv->SetScrollbar(npv, fenh / SBPASV, nh, SBPASV);
  swg->SetSize(CLAVIER_WIDTH, fenh);
  swd->SetSize(w - CLAVIER_WIDTH - SBS, fenh);
  swr->SetSize(w - CLAVIER_WIDTH - SBS, MIDI_RULER_HEIGHT);
  swbd->SetSize(w - CLAVIER_WIDTH, bottom->GetSize().GetHeight());
  swbg->SetSize(CLAVIER_WIDTH, bottom->GetSize().GetHeight());
  ma->SetSize(ma->GetSize().GetWidth(), bottom->GetSize().GetHeight());
  ZoomX->SetSize(0, h - SBS, CLAVIER_WIDTH, SBS);
}

void					EditMidi::OnSplitterChanged(wxSplitterEvent &e)
{
	MoveSash(e.GetSashPosition());
}

void					EditMidi::OnAttach()
{
  toolbar = NULL;
  detached = 0;
  MoveSash(GetSize().GetHeight() - SBS);
}

void					EditMidi::OnDetach(wxFrame *f)
{
  toolbar = new wxToolBar(f, ID_TOOLBAR_EDITMIDI, wxPoint(-1, -1), wxSize(-1, -1), wxTB_FLAT | wxTB_DOCKABLE);
  toolbar->AddRadioTool(ID_TOOL_MOVE_MIDIPART, _("Move"), wxBitmap(wxString(string(WiredSettings->DataDir + "ihm/toolbar/hand_up.png").c_str()), wxBITMAP_TYPE_PNG), wxBitmap(wxString(string(WiredSettings->DataDir + "ihm/toolbar/hand_down.png").c_str()), wxBITMAP_TYPE_PNG), _("Move"), _("Moves notes"), NULL);
  toolbar->AddRadioTool(ID_TOOL_EDIT_MIDIPART, _("Edit"), wxBitmap(wxString(string(WiredSettings->DataDir + "ihm/toolbar/draw_up.png").c_str()), wxBITMAP_TYPE_PNG), wxBitmap(wxString(string(WiredSettings->DataDir + "ihm/toolbar/draw_down.png").c_str()), wxBITMAP_TYPE_PNG), _("Edit"), _("Resizes notes"), NULL);
  toolbar->AddRadioTool(ID_TOOL_DEL_MIDIPART, _("Del"), wxBitmap(wxString(string(WiredSettings->DataDir + "ihm/toolbar/erase_up.png").c_str()), wxBITMAP_TYPE_PNG), wxBitmap(wxString(string(WiredSettings->DataDir + "ihm/toolbar/erase_down.png").c_str()), wxBITMAP_TYPE_PNG), _("Del"), _("Deletes notes"), NULL);
  toolbar->Realize();
  ((WiredFrame *)f)->em = this;
  f->SetToolBar(toolbar);

  detached = 1;
  MoveSash((GetSize().GetHeight() - SBS) / 2);
}

void					EditMidi::OnToolMove(wxCommandEvent &e)
{
  mp->SetTool(ID_TOOL_MOVE_MIDIPART);
}

void					EditMidi::OnToolEdit(wxCommandEvent &e)
{
  mp->SetTool(ID_TOOL_EDIT_MIDIPART);
}

void					EditMidi::OnToolDel(wxCommandEvent &e)
{
  mp->SetTool(ID_TOOL_DEL_MIDIPART);
}

BEGIN_EVENT_TABLE(EditMidi, wxPanel)
  EVT_COMMAND_SCROLL(ID_SCROLLH_EDITMIDI, EditMidi::OnScroll)
  EVT_COMMAND_SCROLL(ID_SCROLLV_EDITMIDI, EditMidi::OnScroll)
  EVT_SIZE(EditMidi::OnResize)
  EVT_SLIDER(ID_ZOOMX_EDITMIDI, EditMidi::OnZoomX)
  EVT_SLIDER(ID_ZOOMY_EDITMIDI, EditMidi::OnZoomY)
  EVT_SPLITTER_SASH_POS_CHANGED(ID_SPLITTER_EDITMIDI, EditMidi::OnSplitterChanged)
END_EVENT_TABLE()
