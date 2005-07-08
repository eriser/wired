// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include 	"WavePanel.h"
#include 	"Colour.h"
#include 	"Settings.h"
//#include	"OptionPanel.h"
#include 	<math.h>
#include 	<iostream>
#include	<wx/textdlg.h>

using namespace std;


const struct s_choice		Choice[NB_CHOICE + 1] =
{
  { "ZOOM"	,	1	},
  { "1/25"	,	2	},
  { "1/50"	,	3	},
  { "1/75"	,	4	},
  { "1/100"	,	5	}
};



WavePanel::WavePanel(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
					  long style) 
					: wxPanel(parent, id, pos, size, style)
{
  wxBoxSizer			*row;
  wxString				combochoice[NB_CHOICE];
  long					c;
  wxSize 				s = GetSize();

  
  
  sbh = new wxScrollBar(this, ID_HSCROLL, wxPoint(0, size.GetHeight() - SCROLLH), 
			wxSize(size.GetWidth(), SCROLLH), wxSB_HORIZONTAL);
  
  w = new WaveEditor(this, -1, wxPoint(0, WAVE_TOOLBAR_HEIGHT), 
		     wxSize(GetSize().GetWidth(), GetSize().GetHeight() - WAVE_TOOLBAR_HEIGHT), true);
  
  Toolbar = new wxToolBar(this, ID_TOOLBAR_WAVE, wxPoint(0, 0), wxSize(size.GetWidth(),WAVE_TOOLBAR_HEIGHT ), wxTB_FLAT);
 
  Toolbar->AddTool(ID_TOOL_COPY_WAVE, "Copy", wxBitmap("data/toolbar/copy_up.bmp", wxBITMAP_TYPE_BMP), 
		   wxBitmap("data/toolbar/copy_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Copy", "Copy wave", NULL);
  Toolbar->AddTool(ID_TOOL_PASTE_WAVE, "Paste", wxBitmap("data/toolbar/paste_up.bmp", wxBITMAP_TYPE_BMP), 
		   wxBitmap("data/toolbar/paste_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Paste", "Paste wave", NULL);
  Toolbar->AddTool(ID_TOOL_CUT_WAVE, "Cut", wxBitmap("data/toolbar/cut_up.bmp", wxBITMAP_TYPE_BMP), 
		   wxBitmap("data/toolbar/cut_down.bmp", wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Cut", "Cut wave", NULL);
  Toolbar->AddTool(ID_TOOL_DEL_WAVE, "Delete", wxBitmap("data/toolbar/delete.png", wxBITMAP_TYPE_PNG), 
		   wxBitmap("data/toolbar/delete.png", wxBITMAP_TYPE_PNG), wxITEM_NORMAL, "Delete", "Delete", NULL);
  Toolbar->AddSeparator();
  Toolbar->AddTool(ID_TOOL_UNDO_WAVE, "Undo", wxBitmap("data/toolbar/undo.bmp", wxBITMAP_TYPE_BMP), 
		   wxBitmap("data/toolbar/undo.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Undo", "Undo last action", NULL);
  Toolbar->AddTool(ID_TOOL_REDO_WAVE, "Redo", wxBitmap("data/toolbar/redo.bmp", wxBITMAP_TYPE_BMP), 
		   wxBitmap("data/toolbar/redo.bmp", wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Redo", "Redo", NULL);
  Toolbar->AddSeparator();
  
  for (c = 0; c < NB_CHOICE; c++)
    combochoice[c] = Choice[c].s;
  combobox = new wxComboBox(Toolbar, ID_TOOL_COMBO_WAVE, "ZOOM", 
			    wxPoint(-1, -1), wxSize(68, -1), 5, combochoice, wxCB_DROPDOWN);
  
  Toolbar->AddControl(combobox);
  Toolbar->AddSeparator();
  
  Toolbar->Realize();
  
  row = new wxBoxSizer(wxVERTICAL);
  row->Add(Toolbar, 0, wxALL | wxEXPAND, 0);
  row->Add(w, 1, wxALL | wxEXPAND , 0);
  row->Add(sbh, 0, wxALL | wxEXPAND | wxFIXED_MINSIZE, 0);
  
  SetSizer(row);
  
  Connect(ID_TOOL_COPY_WAVE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WavePanel::OnCopy);
  Connect(ID_TOOL_CUT_WAVE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WavePanel::OnCut);
  Connect(ID_TOOL_PASTE_WAVE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WavePanel::OnPaste);
  Connect(ID_TOOL_DEL_WAVE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WavePanel::OnDelete);
  Connect(ID_TOOL_SELECT_WAVE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WavePanel::OnSelect);
  
  //	Toolbar->ToggleTool(ID_TOOL_UNDO_WAVEEDITOR, NULL);
  //	Toolbar->ToggleTool(ID_TOOL_REDO_WAVEEDITOR, NULL);

  Refresh();
  
}


WavePanel::~WavePanel()
{

}


void 					WavePanel::AdjustScrollbar()
{
  wxSize 	s = GetSize();
  w->AdjustScrollbar(sbh, s);
}


void 					WavePanel::OnScroll(wxScrollEvent &event)
{
   w->OnScroll(sbh);
}

void					WavePanel::OnSelect(wxCommandEvent &event)
{
  w->OnSelect(event);
}

void					WavePanel::OnZoom(wxCommandEvent &event)
{
  w->OnZoom(combobox);
  AdjustScrollbar();
}

void					WavePanel::OnCopy(wxCommandEvent &event)
{
  w->OnCopy(event);
}

void					WavePanel::OnCut(wxCommandEvent &event)
{
  w->OnCut(event);
  AdjustScrollbar();
}


void					WavePanel::OnPaste(wxCommandEvent &event)
{
  w->OnPaste(event);
  AdjustScrollbar();
}

void					WavePanel::OnDelete(wxCommandEvent &event)
{
  w->OnDelete(event);
  AdjustScrollbar();
}


void					WavePanel::OnUndo(wxCommandEvent &event)
{
  w->OnUndo(event);
  AdjustScrollbar();
}

void					WavePanel::OnRedo(wxCommandEvent &event)
{
  w->OnRedo(event);
  AdjustScrollbar();
}

void					WavePanel::OnDetach(wxFrame *f)
{

  sbh->SetSize(0,(GetSize().GetHeight() - SCROLLH), GetSize().GetWidth(), SCROLLH);
  AdjustScrollbar();
}



BEGIN_EVENT_TABLE(WavePanel, wxPanel)
  EVT_COMMAND_SCROLL(ID_HSCROLL, WavePanel::OnScroll)
  EVT_TOOL(ID_TOOL_COPY_WAVE, WavePanel::OnCopy)
  EVT_TOOL(ID_TOOL_PASTE_WAVE, WavePanel::OnPaste)
  EVT_TOOL(ID_TOOL_CUT_WAVE, WavePanel::OnCut)
  EVT_TOOL(ID_TOOL_DEL_WAVE, WavePanel::OnDelete)
  EVT_TOOL(ID_TOOL_UNDO_WAVE, WavePanel::OnUndo)
  EVT_TOOL(ID_TOOL_REDO_WAVE, WavePanel::OnRedo)
  EVT_COMBOBOX(ID_TOOL_COMBO_WAVE, WavePanel::OnZoom)
END_EVENT_TABLE()
