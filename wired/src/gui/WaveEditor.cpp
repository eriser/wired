// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include 	"WaveEditor.h"

#include 	"Colour.h"
#include 	"Settings.h"
#include 	"ClipBoard.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include 	<wx/menu.h>
#include 	<math.h>
#include 	<iostream>
#include 	<WavePanel.h>
#include        <wx/choicdlg.h>
#include        "SequencerGui.h"

#define UNFINISHED(); wxMessageDialog msg(this, _("Unfinished capability"), wxT("Wired"), wxOK | wxCENTRE); msg.ShowModal(); return;

using namespace std;



WaveEditor::WaveEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
					  long style, bool fulldraw, bool use_settings)
  : wxPanel(parent, id, pos, size, style), WaveEditorDrawer(size, fulldraw, use_settings)
{
  cout << "[WaveEditor] - Constructor" << endl;

  xsrc = 0;
  sizePaste = 0;
  FullDraw = fulldraw;
  if (GetSize().x == 0)
    WaveEditorDrawer::SetSize(1, GetSize().y);

  PopMenu = new wxMenu();
  SubMenuEffect = new wxMenu();

  SubMenuEffect->Append(ID_SUBMENU_GAIN_WAVEEDITOR, _("Gain..."));
  SubMenuEffect->Append(ID_SUBMENU_NORMA_WAVEEDITOR, _("Normalize.."));

  PopMenu->Append(ID_TOOL_CUT_WAVE, _("Cut"));
  PopMenu->Append(ID_TOOL_COPY_WAVE, _("Copy"));
  PopMenu->Append(ID_TOOL_PASTE_WAVE, _("Paste"));
  PopMenu->Append(ID_TOOL_DEL_WAVE, _("Delete"));
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_TOOL_SELECT_WAVE, _("Select All"));
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_SEBMENU_EFFECTS_WAVEEDITOR, _("Effects"), SubMenuEffect);


  Connect(ID_SUBMENU_GAIN_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WaveEditor::OnGain);
  Connect(ID_SUBMENU_NORMA_WAVEEDITOR, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WaveEditor::OnNormalize);

  Refresh();

}


WaveEditor::~WaveEditor()
{
  cout << "WaveEditor::~WaveEditor" << endl;
}


void WaveEditor::OnPaint(wxPaintEvent &event)
{
  wxPaintDC		dc(this);
  wxRegionIterator	region(GetUpdateRegion());
  wxSize		s = GetSize();

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


void 					WaveEditor::OnSize(wxSizeEvent &event)
{
  wxSize				s = GetSize();

  if (Data || (Wave && !Wave->LoadedInMem))
    {
      SetDrawing();
      Refresh();
    }
}


void					WaveEditor::SetWave(float **data, unsigned long frame_length, long channel_count)
{
  wxSize				s = GetSize();
  WaveEditorDrawer::SetWave(data, frame_length, channel_count, GetSize());

  SetDrawing();
  Refresh();
}

void					WaveEditor::SetWave(WaveFile *w)
{
  wxSize				s = GetSize();

  WaveEditorDrawer::SetWave(w, GetSize());
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
}

void					WaveEditor::AdjustScrollbar(wxScrollBar *sbh, wxSize s)
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
  sbh->SetScrollbar(xsrc, thumbwidth, swidth, 1, true);
}


void 					WaveEditor::OnScroll(wxScrollBar *sbh)
{
  wxSize 	s = GetSize();
  long		inc;

  // Coefficient d'incr?mentation
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

void					WaveEditor::OnSelect(wxCommandEvent &event)
{
  mSelectedRegion.x = 0;
  mSelectedRegion.width = Wave->GetNumberOfFrames();
}

void					WaveEditor::OnZoom(wxComboBox *combobox)
{
  wxString				item;
  wxSize 					s = GetSize();

  item = combobox->GetStringSelection();
  if ( item == wxT("1/25"))
    PAINT_WIDTH = zoomx / 25;
  //zoomy = 2;
  if ( item == wxT("1/50"))
    PAINT_WIDTH = zoomx / 50;
  //zoomy = 3;
  if ( item == wxT("1/75"))
    PAINT_WIDTH = zoomx / 75;
  //zoomy = 4;
  if ( item == wxT("1/100"))
    PAINT_WIDTH = zoomx / 100;
  //zoomy = 5;
  if ( item == _("ZOOM"))
    PAINT_WIDTH = zoomx;
  //zoomy = 1;

  //AdjustScrollbar(s);
  SetDrawing();
  Refresh();
}

void					WaveEditor::OnCopy(wxCommandEvent &event)
{
  UNFINISHED();
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
  UNFINISHED();
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

  if ( Wave->GetOpenMode() != WaveFile::rwrite )
  {
     wxMessageDialog msg(this, _("File opened in read only mode"), wxT("Wired"),
 		  wxOK | wxCENTRE);
    int res = msg.ShowModal();
    if (res == wxOK)
	  return;
  }
  else
  {
	cCutAction 	*action = new cCutAction(Wave, from, width);
	action->Do();
  //cClipBoard::Global().Cut(*Wave, (mSelectedRegion.x+xsrc)*inc, (mSelectedRegion.width*inc));
	SeqPanel->UpdateAudioPatterns(Wave);
	mSelectedRegion.width = 0;
	SetWave(Wave);
  }
  Refresh();
}


void					WaveEditor::OnPaste(wxCommandEvent &event)
{
  UNFINISHED();
  wxSize 	s = GetSize();
  long	        to;
  long	        width;

  if (PAINT_WIDTH < EndWavePos)
    inc = (PAINT_WIDTH / s.x);
  else
    inc = (EndWavePos / s.x);

  to = (mPosition+xsrc)*inc;
  width = mSelectedRegion.width*inc;

 if ( Wave->GetOpenMode() != WaveFile::rwrite )
  {
     wxMessageDialog msg(this, _("File opened in read only mode"), wxT("Wired"),
 		  wxOK | wxCENTRE);
    int res = msg.ShowModal();
    if (res == wxOK)
	  return;
  }
  else
  {
	cPasteAction 	*action = new cPasteAction(Wave, to, sizePaste);
	action->Do();
  //cClipBoard::Global().Paste(*Wave, ((mPosition+xsrc)*inc));
	SeqPanel->UpdateAudioPatterns(Wave);
	SetWave(Wave);
  }

  Refresh();
}

void					WaveEditor::OnDelete(wxCommandEvent &event)
{
  UNFINISHED();
  wxSize 	s = GetSize();
  long	from;
  long	width;

  if (PAINT_WIDTH < EndWavePos)
    inc = (PAINT_WIDTH / s.x);
  else
    inc = (EndWavePos / s.x);

  from = (mSelectedRegion.x+xsrc)*inc;
  width = mSelectedRegion.width*inc;

  if ( Wave->GetOpenMode() != WaveFile::rwrite )
  {
     wxMessageDialog msg(this, _("File opened in read only mode"), wxT("Wired"),
 		  wxOK | wxCENTRE);
    int res = msg.ShowModal();
    if (res == wxOK)
	  return;
  }
  else
  {
	cClipBoard::Global().Delete(*Wave, (mSelectedRegion.x+xsrc)*inc, (mSelectedRegion.width*inc));
  //flag = 1;
	mSelectedRegion.width = 0;
	SetWave(Wave);
	SeqPanel->UpdateAudioPatterns(Wave);
  }

  Refresh();
}


void					WaveEditor::OnUndo(wxCommandEvent &event)
{
  UNFINISHED();
  wxSize 	s = GetSize();

  cActionManager::Global().Undo();
 // cout << "undo -- avant setwave -- EndWavePos =  "<< EndWavePos << endl;
  SetWave(Wave);
  Refresh();
}

void					WaveEditor::OnRedo(wxCommandEvent &event)
{
  UNFINISHED();
  wxSize 				s = GetSize();

  cActionManager::Global().Redo();
  SetWave(Wave);
  Refresh();
}

void					WaveEditor::OnGain(wxCommandEvent &event)
{
  wxSize 				s = GetSize();
  long					from;
  long					width, savew;
  long                  wtest;
  wxString 				text;
  int                   nb_read;
  PluginEffect			p;


  if (PAINT_WIDTH < EndWavePos)
    inc = (PAINT_WIDTH / s.x);
  else
    inc = (EndWavePos / s.x);

  from = (mSelectedRegion.x+xsrc)*inc;
  width = savew = mSelectedRegion.width*inc;
  if (width < 0)
    {
      from = from + width;
      width = -width;
    }

  if ( Wave->GetOpenMode() != WaveFile::rwrite )
  {
     wxMessageDialog msg(this, _("File opened in read only mode"), wxT("Wired"),
 		  wxOK | wxCENTRE);
    int res = msg.ShowModal();
    if (res == wxOK)
	  return;
  }
  else
  {
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, _("Enter gain "), _("Please enter text"), wxT("1"),
			wxOK | wxCANCEL, wxPoint(-1, -1));
	text = dlg->GetValue();
	int res = dlg->ShowModal();
	if (res == wxID_OK)
	  text = dlg->GetValue();
	else
	  return;
	float gain = atof(text.mb_str(*wxConvCurrent));

	WaveFile *input;
	WaveFile *output;
	if (input)
	  wxRemoveFile(wxT("/tmp/tmp2.wav"));
	if (output)
	  wxRemoveFile(wxT("/tmp/tmp3.wav"));
	if (Wave->GetNumberOfChannels() == 1)
	{
	input = new WaveFile(wxT("/tmp/tmp2.wav"), false, WaveFile::rwrite, 1);
	output= new WaveFile(wxT("/tmp/tmp3.wav"), false, WaveFile::rwrite, 1);
	}
	else
	{
	  input = new WaveFile(wxT("/tmp/tmp2.wav"), false, WaveFile::rwrite);
	  output = new WaveFile(wxT("/tmp/tmp3.wav"), false, WaveFile::rwrite);
	}
	float * rw_buffer = new float [Wave->GetNumberOfChannels() * WAVE_TEMP_SIZE];

	Wave->SetCurrentPosition(from);
	nb_read = Wave->ReadFloatF(rw_buffer);
	if (nb_read > width)
	  nb_read = width;

	while (nb_read && width)
	{
	  input->WriteFloatF(rw_buffer, nb_read);
	  width -= nb_read;
	  nb_read = Wave->ReadFloatF(rw_buffer);
	  if (width < WAVE_TEMP_SIZE)
	    nb_read = width;
	}
    p.Process(*input, *output, gain, Wave->GetNumberOfChannels(), 1);

    Wave->SetCurrentPosition(from);
    output->SetCurrentPosition(0);
    nb_read = output->ReadFloatF(rw_buffer);
    while (nb_read)
	{
	  Wave->WriteFloatF(rw_buffer, nb_read);
	  nb_read = output->ReadFloatF(rw_buffer);
	}
    wxRemoveFile(wxT("/tmp/tmp2.wav"));
    wxRemoveFile(wxT("/tmp/tmp3.wav"));
  }
  mSelectedRegion.width = 0;
  SetDrawing();
  SeqPanel->UpdateAudioPatterns(Wave);
  Refresh();
}


void					WaveEditor::OnNormalize(wxCommandEvent &event)
{
  long					from, width, savew;
  wxSize 				s = GetSize();
  int                   nb_read;
  PluginEffect			p;
  wxString 				text;

  if (PAINT_WIDTH < EndWavePos)
	inc = (PAINT_WIDTH / s.x);
  else
	inc = (EndWavePos / s.x);

  from = (mSelectedRegion.x+xsrc)*inc;
  width = savew = mSelectedRegion.width*inc;
  if ( Wave->GetOpenMode() != WaveFile::rwrite )
  {
     wxMessageDialog msg(this, _("File opened in read only mode"), wxT("Wired"),
 		  wxOK | wxCENTRE);
    int res = msg.ShowModal();
    if (res == wxOK)
	  return;
  }
  else
  {
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, _("Enter normalize level "), _("Please enter text"), wxT("1"),
			wxOK | wxCANCEL, wxPoint(-1, -1));
	text = dlg->GetValue();
	int res = dlg->ShowModal();
	if (res == wxID_OK)
	  text = dlg->GetValue();
	else
	  return;

	float norma = atof(text.mb_str(*wxConvCurrent));
	WaveFile *input;
	WaveFile *output;

	if (input)
	  wxRemoveFile(wxT("/tmp/tmp2.wav"));
	if (output)
	  wxRemoveFile(wxT("/tmp/tmp3.wav"));
	if (Wave->GetNumberOfChannels() == 1)
	{
	  input = new WaveFile(wxT("/tmp/tmp2.wav"), false, WaveFile::rwrite, 1);
	  output = new WaveFile(wxT("/tmp/tmp3.wav"), false, WaveFile::rwrite, 1);
	}
	else
	{
	  input = new WaveFile(wxT("/tmp/tmp2.wav"), false, WaveFile::rwrite);
	  output = new WaveFile(wxT("/tmp/tmp3.wav"), false, WaveFile::rwrite);
	}

	float * rw_buffer = new float [Wave->GetNumberOfChannels() * WAVE_TEMP_SIZE];

	Wave->SetCurrentPosition(from);
	nb_read = Wave->ReadFloatF(rw_buffer);
	if (nb_read > width)
	  nb_read = width;

	while (nb_read && width)
    {
      input->WriteFloatF(rw_buffer, nb_read);
      width -= nb_read;
      nb_read = Wave->ReadFloatF(rw_buffer);
      if (width < nb_read)
		nb_read = width;

    }

    p.Process(*input, *output, norma, Wave->GetNumberOfChannels(), 2);

	output->SetCurrentPosition(0);
	Wave->SetCurrentPosition(from);
	nb_read = output->ReadFloatF(rw_buffer);
	while (nb_read)
	{
	  Wave->WriteFloatF(rw_buffer, nb_read);
	  nb_read = output->ReadFloatF(rw_buffer);
	}
	wxRemoveFile(wxT("/tmp/tmp2.wav"));
	wxRemoveFile(wxT("/tmp/tmp3.wav"));
  }
  SetDrawing();
  SeqPanel->UpdateAudioPatterns(Wave);
  Refresh();
}



// ------------------- cCutAction

void				cCutAction::Do()
{
  cClipBoard::Global().Cut(*wave, from, width);
  NotifyActionManager();
}

void				cCutAction::Redo()
{
  Do();
}

void				cCutAction::Undo()
{
  if (wave)
	cClipBoard::Global().Paste(*wave, (int)from);
}


// ------------------- cPasteAction

void				cPasteAction::Do()
{
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
}

BEGIN_EVENT_TABLE(WaveEditor, wxPanel)
  EVT_MOUSE_EVENTS(WaveEditor::OnMouseEvent)
  EVT_PAINT(WaveEditor::OnPaint)
  EVT_SIZE(WaveEditor::OnSize)
END_EVENT_TABLE()

