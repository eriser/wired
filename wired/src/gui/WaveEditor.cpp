// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include 	"WaveEditor.h"
#include 	"Colour.h"
#include 	"Settings.h"
#include 	"ClipBoard.h"
//#include	"OptionPanel.h"

#include 	<math.h>
#include 	<iostream>

using namespace std;


// const struct s_choice		Choice[NB_CHOICE + 1] =
// {
//   { "ZOOM"	,	1	},
//   { "1/25"	,	2	},
//   { "1/50"	,	3	},
//   { "1/75"	,	4	},
//   { "1/100"	,	5	}
// };

WaveEditor::WaveEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
					  long style, bool fulldraw, bool use_settings) 
  : wxPanel(parent, id, pos, size, style), WaveEditorDrawer(size, fulldraw, use_settings)
{
  cout << "[WaveEditor] - Constructor" << endl;
  
  //wxBoxSizer				*row;
	//wxString				combochoice[NB_CHOICE];
	long					c;
  
	xsrc = 0;
	sizePaste = 0;
	FullDraw = fulldraw;
	if (GetSize().x == 0)
	  WaveEditorDrawer::SetSize(1, GetSize().y);
	
	// sbh = new wxScrollBar(this, ID_SCROLLH, wxPoint(0, size.GetHeight() - SCROLLH), 
// 			      wxSize(size.GetWidth(), SCROLLH), wxSB_HORIZONTAL);
	

// 	Toolbar = new wxToolBar(this, ID_TOOLBAR_WAVEEDITOR, wxPoint(0, 0), wxSize(size.GetWidth(), 30), wxTB_FLAT | wxTB_DOCKABLE);
// 	Toolbar->AddTool(ID_TOOL_SAVE_WAVEEDITOR, "Save", wxBitmap("data/toolbar/save_up.bmp", wxBITMAP_TYPE_BMP), 
// 	wxBitmap("data/toolbar/save_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Save", "Save File", NULL);
// 	Toolbar->AddTool(ID_TOOL_COPY_WAVEEDITOR, "Copy", wxBitmap("data/toolbar/copy_up.bmp", wxBITMAP_TYPE_BMP), 
// 	wxBitmap("data/toolbar/copy_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Copy", "Copy wave", NULL);
// 	Toolbar->AddTool(ID_TOOL_PASTE_WAVEEDITOR, "Paste", wxBitmap("data/toolbar/paste_up.bmp", wxBITMAP_TYPE_BMP), 
// 	wxBitmap("data/toolbar/paste_down.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Paste", "Paste wave", NULL);
//   	Toolbar->AddTool(ID_TOOL_CUT_WAVEEDITOR, "Cut", wxBitmap("data/toolbar/cut_up.bmp", wxBITMAP_TYPE_BMP), 
//   	wxBitmap("data/toolbar/cut_down.bmp", wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Cut", "Cut wave", NULL);
//   	Toolbar->AddTool(ID_TOOL_DEL_WAVEEDITOR, "Delete", wxBitmap("data/toolbar/delete.png", wxBITMAP_TYPE_PNG), 
//   	wxBitmap("data/toolbar/delete.png", wxBITMAP_TYPE_PNG), wxITEM_NORMAL, "Delete", "Delete", NULL);
// 	Toolbar->AddSeparator();
// 	Toolbar->AddTool(ID_TOOL_UNDO_WAVEEDITOR, "Undo", wxBitmap("data/toolbar/undo.bmp", wxBITMAP_TYPE_BMP), 
// 	wxBitmap("data/toolbar/undo.bmp",wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Undo", "Undo last action", NULL);
// 	Toolbar->AddTool(ID_TOOL_REDO_WAVEEDITOR, "Redo", wxBitmap("data/toolbar/redo.bmp", wxBITMAP_TYPE_BMP), 
// 	wxBitmap("data/toolbar/redo.bmp", wxBITMAP_TYPE_BMP), wxITEM_NORMAL, "Redo", "Redo", NULL);
// 	Toolbar->AddSeparator();
	
// 	for (c = 0; c < NB_CHOICE; c++)
// 	  combochoice[c] = Choice[c].s;
// 	combobox = new wxComboBox(Toolbar, ID_TOOL_COMBO, "ZOOM", 
// 			       wxPoint(-1, -1), wxSize(68, -1), 5, combochoice, wxCB_DROPDOWN);
  
// 	Toolbar->AddControl(combobox);
//   	Toolbar->Realize();
	
// 	row = new wxBoxSizer(wxVERTICAL);
// 	row->Add(Toolbar, 0, wxALL | wxEXPAND, 0);
// 	row->Add(this, 1, wxALL, 0);
// 	row->Add(sbh, 0, wxALL | wxEXPAND | wxFIXED_MINSIZE, 0);
	
//	SetSizer(row);
	
	PopMenu = new wxMenu();  
 
	PopMenu->Append(ID_POPUP_CUT_WAVEEDITOR, "Cut");
	PopMenu->Append(ID_POPUP_COPY_WAVEEDITOR, "Copy");
	PopMenu->Append(ID_POPUP_PASTE_WAVEEDITOR, "Paste");
	PopMenu->Append(ID_POPUP_DEL_WAVEEDITOR, "Delete");
  
	Connect(ID_POPUP_COPY_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	&WaveEditor::OnCopy);
	Connect(ID_POPUP_CUT_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	&WaveEditor::OnCut);
	Connect(ID_POPUP_PASTE_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	&WaveEditor::OnPaste);
	Connect(ID_POPUP_DEL_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	&WaveEditor::OnDelete);
	flag = 0;
  
//	Toolbar->ToggleTool(ID_TOOL_UNDO_WAVEEDITOR, NULL);
//	Toolbar->ToggleTool(ID_TOOL_REDO_WAVEEDITOR, NULL);
	Refresh();
	  
}


WaveEditor::~WaveEditor()
{
  cout << "WaveEditor::~WaveEditor" << endl;

//  if (flag == 1)
//  {
//	int					res;
//  
//	wxMessageDialog msg(this, "Save File ?", "Wired", 
//		      wxYES_NO | wxICON_QUESTION | wxCENTRE);
//	res = msg.ShowModal();
//	wxCommandEvent evt;
//	if (res == wxID_YES)
//		OnSave(evt);
//  }
}


void WaveEditor::OnPaint(wxPaintEvent &event)
{
  wxPaintDC					dc(this);
  wxRegionIterator			region(GetUpdateRegion());
  wxSize					s	=	GetSize();
  
  PrepareDC( dc );
  if ((Data || (Wave && !Wave->LoadedInMem)) && Bmp)
    {
      // Get the panel width and heigth 
      int width, height;
      width = s.GetWidth();
      height = s.GetHeight();
      //GetSize(&width, &height);
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
		memDC.DrawRectangle(mSelectedRegion);  
      else
		memDC.DrawLine(mSelectedRegion.x, 0, mSelectedRegion.x, height);
      // Repaint the wave form
      memDC.SetPen(CL_WAVE_DRAW);
      long coeff = height / 2;
      for (int i = 0; i < width; i++)
		memDC.DrawLine(i, coeff - DrawData[i], i, coeff + DrawData[i]);
    }
  // Appel de la methode paint de la classe mere
	WaveEditorDrawer::OnPaint(dc, s, region); 
}


void WaveEditor::OnSize(wxSizeEvent &event)
{
  wxSize				s = GetSize();
  
  cout << " WaveEditor -- Onsize" << endl;
  if (Data || (Wave && !Wave->LoadedInMem))
    {
      SetDrawing();
      Refresh();
    }  		
  cout << " WaveEditor -- End Onsize" << endl;  
}


void					WaveEditor::SetWave(float **data, unsigned long frame_length, long channel_count)
{
  wxSize				s = GetSize();
  WaveEditorDrawer::SetWave(data, frame_length, channel_count, GetSize());

  //AdjustScrollbar(s);
  SetDrawing();
  Refresh();
}

void					WaveEditor::SetWave(WaveFile *w)
{
  wxSize				s = GetSize();
  
  WaveEditorDrawer::SetWave(w, GetSize());
  //AdjustScrollbar(s);
  SetDrawing();
  Refresh();
}

void					WaveEditor::SetDrawing()
{
  wxSize				s = GetSize();
  //cout << " WaveEditor -- SetDrawing" << endl;
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
      //AdjustScrollbar(s);
      Refresh();
    }  
}

void					WaveEditor::AdjustScrollbar(wxScrollBar *sbh, wxSize s)
{
  //cout << " WaveEditor -- AdjustScrollbar--- xsrc  " << xsrc << endl;
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
 
  sbh->SetScrollbar(xsrc, thumbwidth, swidth, 1, true);
}


void 					WaveEditor::OnScroll(wxScrollBar *sbh)
{
  // cout << "[WaveEditor] - OnScroll \n" ;
  
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


void					WaveEditor::OnZoom(wxComboBox *combobox)
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
		
	//AdjustScrollbar(s);
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
	  
	//cout << "OnCopy ----- inc = " << inc << endl;
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
	
	//cout << "OnCut ----- inc = " << inc << endl;
	
	from = (mSelectedRegion.x+xsrc)*inc;
	width = mSelectedRegion.width*inc;
	sizePaste = mSelectedRegion.width*inc;
	
//	if ( Wave->GetOpenMode() != WaveFile::rwrite )
//	{
//	  wxMessageDialog msg(this, "File opened in read only mode", "Wired", 
//		      wxOK | wxCENTRE);
//	  int res = msg.ShowModal();
//	  if (res == wxOK)
//		return;
//	}
	cCutAction 	*action = new cCutAction(Wave, from, width);
	action->Do();
	//cClipBoard::Global().Cut(*Wave, (mSelectedRegion.x+xsrc)*inc, (mSelectedRegion.width*inc));
	flag = 1;
	mSelectedRegion.width = 0;
	
	SetWave(Wave);
	Refresh();
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
//	if ( Wave->GetOpenMode() != WaveFile::rwrite )
//	{
//	  wxMessageDialog msg(this, "File opened in read only mode", "Wired", 
//		      wxOK | wxCENTRE);
//	  int res = msg.ShowModal();
//	  if (res == wxOK)
//		return;
//	}
  	cPasteAction 	*action = new cPasteAction(Wave, to, sizePaste);
	action->Do();
	//cClipBoard::Global().Paste(*Wave, ((mPosition+xsrc)*inc));
	SetWave(Wave);
	Refresh();
}

void					WaveEditor::OnDelete(wxCommandEvent &event)
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
	
//	if ( Wave->GetOpenMode() != WaveFile::rwrite )
//	{
//	  wxMessageDialog msg(this, "File opened in read only mode", "Wired", 
//		      wxOK | wxCENTRE);
//	  int res = msg.ShowModal();
//	  if (res == wxOK)
//		return;
//	}

	cClipBoard::Global().Delete(*Wave, (mSelectedRegion.x+xsrc)*inc, (mSelectedRegion.width*inc));
	flag = 1;
	mSelectedRegion.width = 0;
	
	SetWave(Wave);
	Refresh();
}


void					WaveEditor::OnUndo(wxCommandEvent &event)
{
  wxSize 	s = GetSize();
  
  cActionManager::Global().Undo();
 // cout << "undo -- avant setwave -- EndWavePos =  "<< EndWavePos << endl;
  SetWave(Wave);
  Refresh();
}

void					WaveEditor::OnRedo(wxCommandEvent &event)
{
  wxSize 	s = GetSize();
  
  cActionManager::Global().Redo();
  SetWave(Wave);
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
 //cout << "cCutAction::UnoDo - begin --- from = " << from << endl;
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
END_EVENT_TABLE()

