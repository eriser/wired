// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "WaveEditor.h"
#include "Colour.h"
#include "Settings.h"
#include "ClipBoard.h"
#include	"OptionPanel.h"

#include <math.h>
#include <iostream>

using namespace std;


const struct s_choice		Choice[NB_CHOICE + 1] =
{
  { "ZOOM"	,	1	},
  { "1/25"	,	2	},
  { "1/50"	,	3	},
  { "1/75"	,	4	},
  { "1/100"	,	5	}
};

WaveEditor::WaveEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
					  long style, bool fulldraw, bool use_settings) 
  : wxPanel(parent, id, pos, size, style), WaveEditorDrawer(size, fulldraw, use_settings)
{
  cout << "[WaveEditor] - Constructor" << endl;
  
	wxBoxSizer				*row_1;
	wxBoxSizer				*row_2;
	wxBoxSizer				*row_3;
	wxBoxSizer				*row_4;
	wxBoxSizer				*col;
	wxString				combochoice[NB_CHOICE];
	long					c;
  
	xsrc = 0;
	sizePaste = 0;
	FullDraw = fulldraw;
	if (GetSize().x == 0)
	  WaveEditorDrawer::SetSize(1, GetSize().y);

    sbh = new wxScrollBar(this, ID_SCROLLH, wxPoint(0, size.GetHeight() - SCROLLH), 
			wxSize(size.GetWidth(), SCROLLH), wxSB_HORIZONTAL);
	  
//	sbv = new wxScrollBar(this, ID_SCROLLV, wxPoint(size.GetWidth() - SCROLLV, 30), 
//			      wxSize(SCROLLV, size.GetHeight() - SCROLLV), wxSB_VERTICAL);
//	
//	sbv->SetScrollbar(0, 1, 50, 1, true);
  
	frame = new wxFrame(this, -1, "", wxPoint(0,0),wxSize(size.GetWidth(), 30),wxDEFAULT_FRAME_STYLE);
	//Toolbar = new wxToolBar(this, ID_TOOLBAR_WAVEEDITOR, wxPoint(0, 0), wxSize(size.GetWidth(), 30), wxTB_FLAT | wxTB_DOCKABLE);
	Toolbar = frame->CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL, ID_TOOLBAR_WAVEEDITOR);  
	Toolbar->AddTool(ID_TOOL_SAVE_WAVEEDITOR, "Save", wxBitmap("data/toolbar/save_up.bmp", wxBITMAP_TYPE_BMP), 
	wxBitmap("data/toolbar/save_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Save", "Save File", NULL);
	Toolbar->AddTool(ID_TOOL_COPY_WAVEEDITOR, "Copy", wxBitmap("data/toolbar/copy_up.bmp", wxBITMAP_TYPE_BMP), 
	wxBitmap("data/toolbar/copy_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Copy", "Copy wave", NULL);
	Toolbar->AddTool(ID_TOOL_PASTE_WAVEEDITOR, "Paste", wxBitmap("data/toolbar/paste_up.bmp", wxBITMAP_TYPE_BMP), 
	wxBitmap("data/toolbar/paste_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Paste", "Paste wave", NULL);
  	Toolbar->AddTool(ID_TOOL_CUT_WAVEEDITOR, "Cut", wxBitmap("data/toolbar/cut_up.bmp", wxBITMAP_TYPE_BMP), 
  	wxBitmap("data/toolbar/cut_down.bmp", wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Cut", "Cut wave", NULL);
	Toolbar->AddSeparator();
	Toolbar->AddTool(ID_TOOL_UNDO_WAVEEDITOR, "Undo", wxBitmap("data/toolbar/undo.bmp", wxBITMAP_TYPE_BMP), 
	wxBitmap("data/toolbar/undo.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Undo", "Undo last action", NULL);
	Toolbar->AddTool(ID_TOOL_REDO_WAVEEDITOR, "Redo", wxBitmap("data/toolbar/redo.bmp", wxBITMAP_TYPE_BMP), 
	wxBitmap("data/toolbar/redo.bmp", wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Redo", "Redo", NULL);
	Toolbar->AddSeparator();
	
	for (c = 0; c < NB_CHOICE; c++)
    combochoice[c] = Choice[c].s;
	combobox = new wxComboBox(Toolbar, ID_TOOL_COMBO, "ZOOM", 
			       wxPoint(-1, -1), wxSize(68, -1), 5, combochoice, wxCB_DROPDOWN);
  
	Toolbar->AddControl(combobox);
  	Toolbar->Realize();
	frame->Show(TRUE);
	
	row_1 = new wxBoxSizer(wxHORIZONTAL);
	row_1->Add(this, 1, wxALL, 0);
	row_2 = new wxBoxSizer(wxHORIZONTAL);
	row_2->Add(sbh, 1, wxALL, 0);
	row_3 = new wxBoxSizer(wxHORIZONTAL);
	row_3->Add(frame, 1, wxALL | wxBOTTOM, 0);
	row_4 = new wxBoxSizer(wxVERTICAL);
	row_4->Add(row_3, 1, wxEXPAND | wxALL, 0);
  	row_4->Add(row_1, 1, wxEXPAND | wxALL, 0);
	row_4->Add(row_2, 1, wxEXPAND | wxALL, 0);
//	col = new wxBoxSizer(wxHORIZONTAL);
//	col->Add(row_4, 1, wxALL | wxEXPAND, 20);
//	col->Add(sbv, 1, wxALL | wxEXPAND, 5);
	
//	SetSizer(col);
	SetSizer(row_4);
	
	PopMenu = new wxMenu();  
 
	PopMenu->Append(ID_POPUP_CUT_WAVEEDITOR, "Cut");
	PopMenu->Append(ID_POPUP_COPY_WAVEEDITOR, "Copy");
	PopMenu->Append(ID_POPUP_PASTE_WAVEEDITOR, "Paste");
  
	Connect(ID_POPUP_COPY_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	&WaveEditor::OnCopy);
	Connect(ID_POPUP_CUT_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	&WaveEditor::OnCut);
	Connect(ID_POPUP_PASTE_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	&WaveEditor::OnPaste);
	flag = 0;
	Refresh();
	  
}


WaveEditor::~WaveEditor()
{
  cout << "WaveEditor::~WaveEditor" << endl;

  if (flag == 1)
  {
	int					res;
  
	wxMessageDialog msg(this, "Save File ?", "Wired", 
		      wxYES_NO | wxICON_QUESTION | wxCENTRE);
	res = msg.ShowModal();
	wxCommandEvent evt;
	if (res == wxID_YES)
		OnSave(evt);
  }
  delete PopMenu;
  delete sbh;
  delete frame;
}


void WaveEditor::OnPaint(wxPaintEvent &event)
{
  wxPaintDC					dc(this);
  wxRegionIterator			region(GetUpdateRegion());
 // wxSize					s	=	GetSize();
  
  cout << "WaveEditor::OnPaint " << endl;
  PrepareDC( dc );
  /* Wave = 0x0 */
  //if ((Data || !Wave->LoadedInMem) && Bmp)
  if ((Data || (Wave && !Wave->LoadedInMem)) && Bmp)
    {
      // Get the panel width and heigth 
      int width, height;
      GetSize(&width, &height);
      mSelectedRegion.height = height;
      // Create the selection background
      wxBrush selectedBrush;
      selectedBrush.SetColour(148, 148, 170);
      // Create the selecting pen
      wxPen selectedPen;
      selectedPen.SetColour(148, 148, 170);
      // Set back the initial background and border colour
      // and paint the wave background
      memDC.SetPen(GetPenColor());
      memDC.SetBrush(CL_WAVEEDITOR_BACKGROUND);
      memDC.DrawRectangle(0, 0, width, height);
      // Draw the selection region
      memDC.SetBrush(selectedBrush);
      memDC.SetPen(selectedPen);
      if (mSelectedRegion.width != 0)
	{
	  // cout << "\nDS - "<< mSelectedRegion.x << "; " << mSelectedRegion.y
	  // 	  << "; " << mSelectedRegion.width << "; " << mSelectedRegion.height << endl;
	  memDC.DrawRectangle(mSelectedRegion);  
	}
      else
	{
	  memDC.DrawLine(mSelectedRegion.x, 0, mSelectedRegion.x, height);
	}
      // Repaint the wave form
      memDC.SetPen(CL_WAVE_DRAW);
      long coeff = height / 2;
      for (int i = 0; i < width; i++)
	memDC.DrawLine(i, coeff - DrawData[i], i, coeff + DrawData[i]);
    }
  // Appel de la methode paint de la classe mere
	WaveEditorDrawer::OnPaint(dc, GetSize(), region); 
}


void WaveEditor::OnSize(wxSizeEvent &event)
{
  wxSize				s = GetSize();
  			
  sbh->SetSize(0, s.y - 20, s.x, 20);
  //sbv->SetSize(s.x - 20, 30, 20, s.y - 20);
  Toolbar->SetSize(0, 0, size.GetWidth(), 30);
  SetDrawing();
  Refresh();
}


void					WaveEditor::SetWave(float **data, unsigned long frame_length, long channel_count)
{
  wxSize				s = GetSize();
  
  WaveEditorDrawer::SetWave(data, frame_length, channel_count, GetSize());

  AdjustScrollbar(s);
  SetDrawing();
  Refresh();
}

void					WaveEditor::SetWave(WaveFile *w)
{
  wxSize				s = GetSize();
  
  WaveEditorDrawer::SetWave(w, GetSize());

  AdjustScrollbar(s);
  SetDrawing();
  Refresh();
}

void					WaveEditor::SetDrawing()
{
  wxSize				s = GetSize();

  WaveEditorDrawer::SetDrawing(GetSize(), xsrc);
}


void					WaveEditor::SetSize(wxSize s)
{
  if (s == GetSize())
    return;
  wxWindow::SetSize(s);
  if (Data || (Wave && !Wave->LoadedInMem))
    {
      SetDrawing();
      Refresh();
    }  
    
  AdjustScrollbar(s);
}

void					WaveEditor::AdjustScrollbar(wxSize s)
{
  if ( PAINT_WIDTH < EndWavePos)
    { 
	  thumbwidth = (long) ceill(s.x / (EndWavePos / PAINT_WIDTH));
	  swidth = ((EndWavePos * s.x) / PAINT_WIDTH);
	}
  else
	{
	  swidth = s.x;
	  thumbwidth = s.x;
	}
  cout << "EndWavePos  " << EndWavePos << endl;
  cout << "thumbwidth = " << thumbwidth << endl;
  cout << "swidth = " << swidth << endl;
  sbh->SetScrollbar(xsrc, thumbwidth, swidth, 1, true);
}


void 					WaveEditor::OnScroll(wxScrollWinEvent &event)
{
   //cout << "[WaveEditor] - OnScroll \n" ;
  
  wxSize 	s = GetSize();
  long		inc;
  
  // Coefficient d'incrémentation
  if (PAINT_WIDTH < EndWavePos)
	  inc = (long) ceill(PAINT_WIDTH / s.x);
  else
	  inc = (long) ceill(EndWavePos / s.x);
  
  if ( PAINT_WIDTH >= EndWavePos)
	 sbh->SetScrollbar(0, s.x, s.x, 1, true);
  else
	if ( ((sbh->GetThumbPosition()*inc) + PAINT_WIDTH) < EndWavePos)
	  {
		sbh->SetScrollbar(sbh->GetThumbPosition(), thumbwidth, swidth, 1, true);
		xsrc = sbh->GetThumbPosition();
	  }
	else
	  if ( ((sbh->GetThumbPosition()*inc) + PAINT_WIDTH) == EndWavePos)
		sbh->SetScrollbar(swidth, thumbwidth, swidth, 1, true);

  SetDrawing();
  Refresh();
}



void 					WaveEditor::OnMouseEvent(wxMouseEvent& event)
{
  // Get the panel width and heigth
  int width, height;
  
  GetSize(&width, &height);
  
  if (PAINT_WIDTH <= EndWavePos)
	inc = (PAINT_WIDTH / width);
  else
	inc = (EndWavePos / width);
	
  mSelectedRegion.height = height;
  
  if (event.ButtonDown(1))
	cout << "pos = " << event.m_x*inc << endl;
  if (event.ButtonDown(1) && (mIsSelecting == false))
    {
      mIsSelecting = true;
      mSelectedRegion.x = event.m_x;
      mSelectedRegion.width = 0;
    }
  else
    if (mIsSelecting == true)
	  mSelectedRegion.width =  event.m_x - mSelectedRegion.x;
	  
  if (event.ButtonUp(1))
      mIsSelecting = false;
  if (event.ButtonDown(3))
	  PopupMenu(PopMenu, event.GetPosition() + GetPosition());
  if (event.ButtonDown(1))
	  mPosition = event.m_x;
  Refresh ();
}


void					WaveEditor::OnZoom(wxCommandEvent &event)
{
	wxString				item;
	wxSize 					s = GetSize();
  
	item = combobox->GetStringSelection();
	if ( item == "1/25")
	  PAINT_WIDTH = zoomx / 25;
	  //zoomy = 2;	  
	if ( item == "1/50")
	  PAINT_WIDTH = zoomx / 50;
	  //zoomy = 3; 
    if ( item == "1/75")
	  PAINT_WIDTH = zoomx / 75;
	  //zoomy = 4;
	if ( item == "1/100")
	  PAINT_WIDTH = zoomx / 100;
	  //zoomy = 5;
	if ( item == "ZOOM")
	  PAINT_WIDTH = zoomx;
	  //zoomy = 1;
		
	AdjustScrollbar(s);
	SetDrawing();
	Refresh();
}

void					WaveEditor::OnCopy(wxCommandEvent &event)
{
	wxSize 	s = GetSize();
	
	if (PAINT_WIDTH < EndWavePos)
	  inc = (PAINT_WIDTH / s.x);
	else
	  inc = (EndWavePos / s.x);
	  
	sizePaste = mSelectedRegion.width*inc;
	cClipBoard::Global().Copy(*Wave, (mSelectedRegion.x+xsrc)*inc, (mSelectedRegion.width*inc));
	mSelectedRegion.width = 0;
	Refresh();
}

void					WaveEditor::OnCut(wxCommandEvent &event)
{
	wxSize 	s = GetSize();
	long	from;
	long	width;
	
	if (PAINT_WIDTH < EndWavePos)
	  inc = (PAINT_WIDTH / s.x);
    else
	  inc = (EndWavePos / s.x);
	  
	from = (mSelectedRegion.x+xsrc)*inc;
	width = mSelectedRegion.width*inc;
	sizePaste = mSelectedRegion.width*inc;
	cCutAction 	*action = new cCutAction(Wave, from, width);
	action->Do();
	//cClipBoard::Global().Cut(*Wave, (mSelectedRegion.x+xsrc)*inc, (mSelectedRegion.width*inc));
	flag = 1;
	mSelectedRegion.width = 0;
	
	SetWave(Wave);
	AdjustScrollbar(s);
	SetDrawing();
	Refresh();
	//delete action;
  }


void					WaveEditor::OnPaste(wxCommandEvent &event)
{
	wxSize 	s = GetSize();
	long	to;
	long	width;
	 
	if (PAINT_WIDTH < EndWavePos)
	  inc = (PAINT_WIDTH / s.x);
	else
	  inc = (EndWavePos / s.x);
	
	to = (mPosition+xsrc)*inc;
	width = mSelectedRegion.width*inc;
	cout << "avant paste ----- EndWavePos = " << EndWavePos << endl;
	cPasteAction 	*action = new cPasteAction(Wave, to, sizePaste);
	action->Do();
	//cClipBoard::Global().Paste(*Wave, ((mPosition+xsrc)*inc));
	flag = 1;
	cout << "Avant setwave ----- EndWavePos = " << EndWavePos << endl;
	SetWave(Wave);
	cout << "Apres setwave----- EndWavePos = " << EndWavePos << endl;
	AdjustScrollbar(s);
	SetDrawing();
	Refresh();
	//delete action;
}

void					WaveEditor::OnSave(wxCommandEvent &event)
{
	cout << "WaveEditor :: OnSave" << endl;
  
	flag = 0;
}

void					WaveEditor::OnUndo(wxCommandEvent &event)
{
  wxSize 	s = GetSize();
  
  cout << "rentre ds undo "<< endl;
  cActionManager::Global().Undo();
  cout << "undo -- avant setwave -- EndWavePos =  "<< EndWavePos << endl;
  SetWave(Wave);
  cout << "undo -- apres setwave -- EndWavePos =  "<< EndWavePos << endl;
  SetDrawing();
  AdjustScrollbar(s);
  Refresh();
}

void					WaveEditor::OnRedo(wxCommandEvent &event)
{
  wxSize 	s = GetSize();
  cout << "rentre ds redo "<< endl;
  cActionManager::Global().Redo();
  cout << "redo -- avant setwave -- EndWavePos =  "<< EndWavePos << endl;
  SetWave(Wave);
  cout << "redo -- apres setwave -- EndWavePos =  "<< EndWavePos << endl;
  AdjustScrollbar(s);
  SetDrawing();
  Refresh();
}

// ------------------- cCutAction

void				cCutAction::Do()
{  
 // cout << "cCutAction::Do - begin --- from = " << from << endl;
  cClipBoard::Global().Cut(*wave, from, width);	
  NotifyActionManager();
}

void				cCutAction::Redo()
{  
  Do();
}

void				cCutAction::Undo()
{  
 cout << "cCutAction::UnoDo - begin --- from = " << from << endl;
  if (wave)
	cClipBoard::Global().Paste(*wave, (int)from);
}


// ------------------- cPasteAction

void				cPasteAction::Do()
{  
//cout << "cPasteAction::Do - begin --- to= " << to << endl;
  cClipBoard::Global().Paste(*wave, to);
  	
  NotifyActionManager();
}

void				cPasteAction::Redo()
{  
  Do();
}

void				cPasteAction::Undo()
{  
	cClipBoard::Global().Cut(*wave, to, width);
	
  //cout << "cPasteAction::UnDo - end";
}

BEGIN_EVENT_TABLE(WaveEditor, wxPanel)
  EVT_MOUSE_EVENTS(WaveEditor::OnMouseEvent)
  EVT_PAINT(WaveEditor::OnPaint)
  EVT_SIZE(WaveEditor::OnSize)
  EVT_COMMAND_SCROLL(ID_SCROLLH, WaveEditor::OnScroll)
  EVT_TOOL(ID_TOOL_SAVE_WAVEEDITOR, WaveEditor::OnSave)
  EVT_TOOL(ID_TOOL_COPY_WAVEEDITOR, WaveEditor::OnCopy)
  EVT_TOOL(ID_TOOL_PASTE_WAVEEDITOR, WaveEditor::OnPaste)
  EVT_TOOL(ID_TOOL_CUT_WAVEEDITOR, WaveEditor::OnCut)
  EVT_TOOL(ID_TOOL_UNDO_WAVEEDITOR, WaveEditor::OnUndo)
  EVT_TOOL(ID_TOOL_REDO_WAVEEDITOR, WaveEditor::OnRedo)
  EVT_COMBOBOX(ID_TOOL_COMBO, WaveEditor::OnZoom)
END_EVENT_TABLE()

