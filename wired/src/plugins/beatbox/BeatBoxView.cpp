#include <math.h>
#include "BeatBox.h"

BEGIN_EVENT_TABLE(Ruler, wxWindow)
  EVT_PAINT(Ruler::OnPaint)
END_EVENT_TABLE()

#define GET_CHAN_ID()		\
 ViewPtr->TrackView->BeatTracks[ViewPtr->TrackView->SelectedTrack]->Channel->Id
  
#define LIGHT_BLUE		wxColour(204,255,255)
#define DEEP_BLUE		wxColour(0,51,153)
#define DEEP_GREEN		wxColour(28,162,56)
#define SELECTED_NOTE_COLOUR	wxColour(1,74,17)
#define DEFAULT_NOTE_COLOUR	wxColour(110,110,110)
#define SELECTION_COLOUR	wxColour(111,226,122)

Ruler::Ruler(wxWindow *parent, wxWindowID id, 
	     const wxPoint &pos, const wxSize &size,
	     BeatBoxView* view_ptr)
  : wxWindow(parent, id, pos, size)
{
  SetBackgroundColour(DEEP_GREEN);
  ViewPtr = view_ptr;
}

Ruler::~Ruler()
{}

void Ruler::OnPaint(wxPaintEvent& event)
{ 
  wxPaintDC	dc(this);
  wxSize	size;
  wxString	s;

  PrepareDC(dc);
  size = GetSize();
  dc.SetPen(wxPen(LIGHT_BLUE, 1, wxSOLID));
  dc.SetBrush(wxBrush(DEEP_GREEN));
  dc.SetTextForeground(*wxWHITE);
  dc.DrawRoundedRectangle(0, 0, size.x, size.y, 3);
  dc.SetPen(wxPen(*wxBLACK, 1, wxSOLID));
  
  int steps = ViewPtr->DRM31->GetSteps();
  
  double res = 
    static_cast<double>
    ( ViewPtr->XSize / static_cast<double>(steps) );
  
  long x = 0 - ViewPtr->XScroll;
  

  for (int cpt = 0; x < size.x; 
x = static_cast<long>(ceil(static_cast<double>(cpt * res))) - ViewPtr->XScroll)
    {
      if (cpt < steps)
	{
	  s.Printf("%d", cpt+1);
	  dc.DrawText(s, x+1, 0);
	}
      dc.DrawLine(x, 0, x, RULER_HEIGHT);
      cpt++;
    }
}

BEGIN_EVENT_TABLE(BeatBoxTrackView, wxWindow)
  EVT_PAINT(BeatBoxTrackView::OnPaint)
  EVT_LEFT_DOWN(BeatBoxTrackView::OnLeftDown)
END_EVENT_TABLE()

BeatBoxTrackView::BeatBoxTrackView(wxWindow *parent,  wxWindowID id,
				   const wxPoint &pos, const wxSize &size,
				   BeatBoxView* view_ptr)
  : wxWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxBLACK);
  SelectedTrack = 0;
  ViewPtr = view_ptr;
  
  BeatTrack* beat_track;
  for (int i = 0; i < 11; i++)
    {
      try 
	{
	  beat_track = new BeatTrack(ViewPtr->DRM31->Channels[i]);
	}
      catch (std::bad_alloc)
	{
	  cout << "[BeatBoxView] bad alloc" << endl;
	}
      catch (...) 
	{ 
	  cout <<"[BeatBoxView] unexpected error during new" << endl; 
	}
      BeatTracks.push_back(beat_track);
    }
}

BeatBoxTrackView::~BeatBoxTrackView()
{}

void BeatBoxTrackView::OnLeftDown(wxMouseEvent& event)
{
  SelectedTrack = 
    (event.m_y + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  Refresh();
}

void BeatBoxTrackView::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  PrepareDC(dc);
  
  int y = 0 - ViewPtr->YScroll;
  
  
  dc.SetPen(*wxMEDIUM_GREY_PEN); 
  dc.SetTextForeground(*wxBLACK);
  dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
  
  wxString s;
  
  long tr = 0;
  
  for (vector<BeatTrack*>::iterator bt = BeatTracks.begin(); 
       bt != BeatTracks.end(); bt++)
    {
      if (tr == SelectedTrack)
	{
	  dc.SetBrush(wxBrush(DEEP_GREEN, wxSOLID));
	  dc.DrawRoundedRectangle(0, y, TRACK_WIDTH, ViewPtr->TrackHeight, 3);
	  dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
	}
      else 
	dc.DrawRoundedRectangle(0, y, TRACK_WIDTH, ViewPtr->TrackHeight, 3);

      //s = (*bt)->Channel->WaveLabel->GetLabel();
      //s.Printf("%d", (*bt)->Channel->Wave->);
      if ((*bt)->Channel->Wave)
	{
	  wxFileName fn((*bt)->Channel->Wave->Filename.c_str());
	  s = fn.GetName();
	}
      else
	s = _T("channel empty");
      //s.Truncate(5);

      dc.DrawText(s, 5, y + 5);
      y += ViewPtr->TrackHeight;
      tr++;
    }
}

//BEGIN_EVENT_TABLE(BeatTrack, wxWindow)
  //EVT_PAINT(BeatTrack::OnPaint)
//END_EVENT_TABLE()

//BeatTrack::BeatTrack(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, BeatBoxChannel* channel)
BeatTrack::BeatTrack(BeatBoxChannel* channel)
  //  : wxWindow(parent, id, pos, size)
{
  Channel = channel;
  /*
    wxStaticText* text = 
    new wxStaticText(this, -1, _T("channel"), wxPoint(0,0), 
		     wxDefaultSize);
  */
}

BeatTrack::~BeatTrack()
{}

/*
void BeatTrack::OnPaint(wxPaintEvent& event)
{
wxPaintDC dc(this);
  wxSize s;
  
  PrepareDC(dc);
  s = GetSize();
  dc.SetPen(*wxMEDIUM_GREY_PEN); 
  
  dc.SetBrush(wxBrush(*wxBLACK, wxTRANSPARENT));//*wxLIGHT_GREY_BRUSH); 
  dc.DrawRoundedRectangle(0, 0, s.x, s.y, 3);
  
}
*/

BEGIN_EVENT_TABLE(BeatBoxScrollView, wxScrolledWindow)
  EVT_PAINT(BeatBoxScrollView::OnPaint)
  EVT_LEFT_DOWN(BeatBoxScrollView::OnLeftDown)
  EVT_LEFT_UP(BeatBoxScrollView::OnLeftUp)
  EVT_MOTION(BeatBoxScrollView::OnMotion)
  EVT_RIGHT_DOWN(BeatBoxScrollView::OnRightDown)
  EVT_KEY_DOWN(BeatBoxScrollView::OnKeyDown)
  EVT_KEY_UP(BeatBoxScrollView::OnKeyUp)
END_EVENT_TABLE()

BeatBoxScrollView::BeatBoxScrollView(wxWindow *parent, wxWindowID id,
				     const wxPoint &pos, const wxSize &size,
				     BeatBoxView* view_ptr)
  : wxScrolledWindow(parent, id, pos, size)
{
  SetBackgroundColour(*wxWHITE);
  SetForegroundColour(*wxWHITE);
  CtrlDown = false;
  ViewPtr = view_ptr;
  OnSelecting = false;
  ClickPosX = ClickPosY = MotionPosX = MotionPosY = 0;
  SelectedNote = 0x0;
  SubDiv = 0;
  
  PopMenu = new wxMenu();
  //PopMenu->Append(ID_POPUP_MOVE_TO_CURSOR, "Move to cursor");
  PopMenu->Append(ID_PopNew, "Add");
  PopMenu->Append(ID_PopDelete, "Delete");
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_PopCut, "Cut");
  PopMenu->Append(ID_PopCopy, "Copy");
  PopMenu->Append(ID_PopPaste, "Paste");
  PopMenu->AppendSeparator();
  PopMenu->Append(ID_PopSelect, "Select all");
  //PopMenu->AppendSeparator();
  //PopMenu->Append(ID_PopNotes, "Notes");
  
  Connect(ID_PopNew, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &BeatBoxScrollView::OnNewNote);
  Connect(ID_PopDelete, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &BeatBoxScrollView::OnDeleteNotes);
  Connect(ID_PopCut, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &BeatBoxScrollView::OnCutNotes);
  Connect(ID_PopCopy, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &BeatBoxScrollView::OnCopyNotes);
  Connect(ID_PopPaste, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &BeatBoxScrollView::OnPasteNotes);
 Connect(ID_PopSelect, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &BeatBoxScrollView::OnSelectAllNotes);
  

/*
    wxStaticText* text = 
    new wxStaticText(this, -1, _T("This is the scroll view"), wxPoint(0,0), 
		     wxDefaultSize);
  */
  
}

BeatBoxScrollView::~BeatBoxScrollView()
{}

void BeatBoxScrollView::OnKeyDown(wxKeyEvent& event)
{
  switch (event.GetKeyCode())
    {
    case WXK_CONTROL:
      //cout << "ctrl pressed" << endl;
      CtrlDown = true;
      break;
    case 'C':
      //cout << "c clicked" << endl;
      if (event.ControlDown())
	{
	  //cout << "ctrl + c" << endl;
	  CopyNotes();
	}
      break;
    case 'V':
      if (event.m_controlDown)
	{
	  PasteNotes();
	}
      break;
    case 'X':
      if (event.m_controlDown)
	{
	  
	}
      break;
    default :
      break;
    }
}

void BeatBoxScrollView::OnKeyUp(wxKeyEvent& event)
{
  switch (event.GetKeyCode())
    {
    case WXK_CONTROL:
      CtrlDown = false;
      break;
    default :
      break;
    }
}

void BeatBoxScrollView::OnNewNote(wxCommandEvent& WXUNUSED(event))
{}

void BeatBoxScrollView::OnDeleteNotes(wxCommandEvent& WXUNUSED(event))
{
  for (list<BeatNote*>::iterator note = SelectedNotes.begin();
       note != SelectedNotes.end(); note++)
    {
      ViewPtr->DRM31->RemBeatNote(*note, 
				  ViewPtr->DRM31->Channels[(*note)->NumChan],
				  ViewPtr->DRM31->EditedBank, 
				  ViewPtr->DRM31->EditedPattern);
      delete *note;
    }
  SelectedNotes.clear();
}

void BeatBoxScrollView::OnCutNotes(wxCommandEvent& WXUNUSED(event))
{
  
}

void BeatBoxScrollView::OnCopyNotes(wxCommandEvent& WXUNUSED(event))
{
  CopyNotes();
}

inline void BeatBoxScrollView::CopyNotes()
{
  for (list<BeatNote*>::iterator note = TmpNotes.begin();
       note != TmpNotes.end(); note++)
    {
      delete *note;
    }
  TmpNotes.clear();
  for (list<BeatNote*>::iterator note = SelectedNotes.begin();
       note != SelectedNotes.end(); note++)
    {
      TmpNotes.push_back(new BeatNote(*note));
      /*
	ViewPtr->DRM31->RemBeatNote(*note, 
				  ViewPtr->DRM31->Channels[(*note)->NumChan],
				  ViewPtr->DRM31->EditedBank, 
				  ViewPtr->DRM31->EditedPattern);
      delete *note;
      */
    }
  //SelectedNotes.clear();
  /*for (list<BeatNote*>::iterator note = TmpNotes.begin();
       note != TmpNotes.end(); note++)
    {
      SelectedNotes.push_back(*note);
    }
  */
}

void BeatBoxScrollView::OnPasteNotes(wxCommandEvent& WXUNUSED(event))
{
  PasteNotes();
}

inline void BeatBoxScrollView::PasteNotes()
{
  BeatNote* n;
  int chan = GET_CHAN_ID();
  list<BeatNote*> new_selection;
  
  for (list<BeatNote*>::iterator note = TmpNotes.begin();
       note != TmpNotes.end(); note++)
    {
      (*note)->Position += 0.5; 
      n = new BeatNote(*note);
      n->NumChan = chan;
      //n->Selected = true;
      ViewPtr->DRM31->AddBeatNote(n,
				  ViewPtr->DRM31->Channels[chan],
				  ViewPtr->DRM31->EditedBank,
				  ViewPtr->DRM31->EditedPattern);
      new_selection.push_back(n);
    }
  ClearSelected();
  SelectedNotes = new_selection;
  Refresh();
  
  /*
  BeatNote* bn;
  for (list<BeatNote*>::iterator note = TmpNotes.begin();
       note != TmpNotes.end(); note++)
    {
      bn = new BeatNote(*note);
      SelectedNotes.push_back(bn);
      ViewPtr->DRM31->Channels[ViewPtr->SelectedTrack];
    }
  */
}

void BeatBoxScrollView::OnSelectAllNotes(wxCommandEvent& WXUNUSED(event))
{
  ClearSelected();
  for (list<BeatNote*>::iterator note = 
	 ViewPtr->TrackView->BeatTracks[ViewPtr->TrackView->SelectedTrack]->
	 Channel->Rythms[ViewPtr->DRM31->EditedBank]
	                [ViewPtr->DRM31->EditedPattern].begin();
       note != 
	 ViewPtr->TrackView->BeatTracks[ViewPtr->TrackView->SelectedTrack]->
	 Channel->Rythms[ViewPtr->DRM31->EditedBank]
	                [ViewPtr->DRM31->EditedPattern].end();
       note++)
    {
      SelectNote(*note);
    }
}

void BeatBoxScrollView::OnPaint(wxPaintEvent&event)
{
  wxPaintDC dc(this);
  wxSize size;
  long	 m;
  long y = 0 - ViewPtr->YScroll;
  long x = 0 - ViewPtr->XScroll;
  wxString s;

  PrepareDC(dc);
  size = GetClientSize();
  
  
  
  dc.SetBrush(wxBrush(VIEW_BGCOLOR));
  //dc.SetTextForeground(VIEW_FGCOLOR);
  dc.SetTextForeground(*wxBLACK);
  ////
  
  if (OnSelecting)
    {
      dc.SetPen(*wxBLACK_PEN);
      //dc.SetBrush(wxBrush(wxColour(28,162,56), wxTRANSPARENT));
      dc.SetBrush(wxBrush(SELECTION_COLOUR, wxSOLID));
      dc.DrawRectangle(ClickPosX, ClickPosY, 
		       MotionPosX - ClickPosX, MotionPosY - ClickPosY);
    }
    
  
  
  int steps = ViewPtr->DRM31->GetSteps();
  double res = 
    static_cast<double>
    ( ViewPtr->XSize / static_cast<double>(steps) );
  long sub;
  /*if (SubDiv)
    sub = static_cast<long>(floor(res/ static_cast<double>(SubDiv+1)));*/
  //cout << "one note==" << res << " sub==" << sub << " subdivisions==" << SubDiv << endl;
  for (int cpt = 0; x < size.x; 
       x = static_cast<long>(ceil(static_cast<double>(cpt * res))) - ViewPtr->XScroll)
    {
      dc.SetPen(wxPen(wxColour(84,91,98), 1, wxSOLID));
      dc.DrawLine(x, 0, x, size.y);
      dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxSOLID));
      
      for (int i = 1; i <= SubDiv; i++)
      {
	sub = x + static_cast<long>(floor((res/(SubDiv+1))*i));
	dc.DrawLine(sub, 0, sub, size.y);
      }
      cpt++;
    }
  
  dc.SetPen(wxPen(VIEW_BGCOLOR, 1, wxSOLID));
  
  long xn, yn, cpt = 0;
  double pos;
  for (vector<BeatTrack*>::iterator bt = 
	 ViewPtr->TrackView->BeatTracks.begin();
       bt != ViewPtr->TrackView->BeatTracks.end(); 
       bt++)
    {
      for (list<BeatNote*>::iterator note = 
	     (*bt)->Channel->Rythms[ViewPtr->DRM31->EditedBank]
	     [ViewPtr->DRM31->EditedPattern].begin();
	   note != (*bt)->Channel->Rythms[ViewPtr->DRM31->EditedBank]
	     [ViewPtr->DRM31->EditedPattern].end();
	   note++)
	{
	  pos = 
	    (*note)->Position 
	    / static_cast<double>(ViewPtr->DRM31->GetSteps())
	    * ViewPtr->XSize;
	  xn = 
	    static_cast<long>(ceil(pos))
	    - ViewPtr->XScroll;
	  /*
	    yn = (ViewPtr->TrackHeight * cpt) 
	    + 3 + static_cast<long> 
	    ( floor
	      (((1.27 - (*note)->Vel) / 1.30) * ViewPtr->TrackHeight))
	    - ViewPtr->YScroll;
	  */
	  yn = (ViewPtr->TrackHeight * cpt) 
	    + DEC + static_cast<long> 
	    ( floor
	      ((1.27 - (*note)->Vel) / 1.27 * (ViewPtr->TrackHeight - DEC)))
	    - ViewPtr->YScroll;
	  
	  //cout << "note  xn " << xn << " yn "<< yn << endl;
	  /*cout << "note  y " << yn -3-(ViewPtr->TrackHeight * cpt) 
	       << " yn "<< yn << endl;
	  */
	  if ((*note)->Selected)
	    dc.SetBrush(SELECTED_NOTE_COLOUR);
	  else
	    dc.SetBrush(DEFAULT_NOTE_COLOUR);
	  
	  dc.DrawCircle(wxPoint(xn, yn), 3);
	}
      
      //cpt++;
      
      /*
	yn =
	     y + 3 + 
	static_cast<long>(floor(ViewPtr->TrackHeight * (0.27 / 1.27 / 1.30)));
	   */
      /*
      yn = (ViewPtr->TrackHeight * cpt) 
	+ DEC + static_cast<long> 
	( floor
	  (((1.27 - 1.0) / 1.30) * (ViewPtr->TrackHeight - DEC)))
	- ViewPtr->YScroll;
      */
      
      /*
	yn = (ViewPtr->TrackHeight * cpt) 
	+ DEC + static_cast<long> 
	( floor
	  (((1.27 - 1.0)) / 1.27 * (ViewPtr->TrackHeight - DEC)))
	- ViewPtr->YScroll;
      */
      /*

      (yn - (ViewPtr->TrackHeight * cpt) - 3) 
	/ ViewPtr->TrackHeight
	=
	((1.27 - 1.0) / 1.30);
      */
      
      /*
	dc.SetPen(wxPen(*wxRED, 1, wxSOLID));
      dc.DrawLine(0, yn, GetClientSize().x, yn);
      
      yn = (ViewPtr->TrackHeight * cpt) 
	+ DEC + static_cast<long> 
	( floor
	  (((1.27 - 1.27)) / 1.27 * (ViewPtr->TrackHeight - DEC)))
	- ViewPtr->YScroll;
      dc.DrawLine(0, yn, GetClientSize().x, yn);
      */
      //yn = y+3+ static_cast<long>(floor(ViewPtr->TrackHeight * (1.27/1.30)));
      /*
 yn = (ViewPtr->TrackHeight * cpt) 
	+ DEC + static_cast<long> 
	( floor
	  ((1.27 - 0.0) / 1.27 * (ViewPtr->TrackHeight+DEC)))
	- ViewPtr->YScroll;
      
      dc.SetPen(*wxMEDIUM_GREY_PEN); 
      dc.DrawLine(0, yn, GetClientSize().x, yn);
      */      
      cpt++;
      y += ViewPtr->TrackHeight;
      
      //dc.SetPen(*wxMEDIUM_GREY_PEN); 
      dc.SetPen(wxPen(*wxBLACK, 1, wxSOLID));
      dc.DrawLine(0, y, GetClientSize().x, y);
      dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
      //dc.DrawRoundedRectangle(0, yn, GetClientSize().x, y - yn, 3);
    }
  
}

void BeatBoxScrollView::OnMotion(wxMouseEvent& event)
{
  if (!event.LeftIsDown())
    {
      //cout << "helas" << endl;
      return;
    }
  
  if (!SelectedNote)
    OnSelecting = true;
  
  double xpos = floor( (static_cast<double>
    (event.m_x 
     + static_cast<double>
     (ViewPtr->XScroll)) / static_cast<double>(ViewPtr->XSize) 
			*  ViewPtr->DRM31->GetSteps()) 
		       * 100) / 100;
  if (ViewPtr->Magnetism)
    {
      int i = SubDiv + 1;
      xpos = xpos - fmod(xpos, static_cast<double>(1/static_cast<double>(i)));
    }
  //cout << "xpos " << xpos << endl;
  
  long track = (event.m_y + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  double ypos = (event.m_y + ViewPtr->YScroll) % ViewPtr->TrackHeight;
  double vel = floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) 
					  / ViewPtr->TrackHeight) * 1.30) 
		     * 100) / 100;
  //cout << "motion vel " << vel << endl;
  /*
    if (vel < 0.0)
    vel = 0;
  else if (vel > 1.27)
    vel = 1.27;
  */
  if (SelectedNote)
    {
      int n = 0;
  for (list<BeatNote*>::iterator note = SelectedNotes.begin(); 
       note != SelectedNotes.end(); note++)
    {
      /*
	cout << "note removing" << endl;
	cout << "chan " << (*note)->NumChan 
	<< ", bank " << ViewPtr->DRM31->EditedBank 
	<< ", track " << ViewPtr->DRM31->EditedPattern 
	<< endl;
      */
      ViewPtr->DRM31->RemBeatNote(*note, 
       			  ViewPtr->DRM31->Channels[(*note)->NumChan], 
				  ViewPtr->DRM31->EditedBank, 
				  ViewPtr->DRM31->EditedPattern);
      //cout << "note removed" << endl;

    }
  //cout << "SelectedNote pos: " << SelectedNote->Position << endl;
  double delta_x = xpos - SelectedNote->Position;
  //cout << SelectedNote->Vel << endl;
  float delta_y = vel - SelectedNote->Vel;
  //cout << "delta vel: " << delta_y << " delta x: " << delta_x << endl;
  for (list<BeatNote*>::iterator note = SelectedNotes.begin(); 
       note != SelectedNotes.end(); note++)
    {
      n++;
      (*note)->Position += delta_x;
      if ((*note)->Position >= static_cast<double>(ViewPtr->DRM31->GetSteps()))
	{
	  (*note)->Position = 
	    fmod((*note)->Position, 
		 static_cast<double>(ViewPtr->DRM31->GetSteps()));
	    //- static_cast<double>(ViewPtr->DRM31->GetSteps());
	}
      else if ((*note)->Position < 0.00)
	{
	  (*note)->Position = static_cast<double>(ViewPtr->DRM31->GetSteps()) 
	    + fmod((*note)->Position, 
		   static_cast<double>(ViewPtr->DRM31->GetSteps()));
	}
      (*note)->Position = floor((*note)->Position * 100) / 100;
      (*note)->BarPos = floor(((*note)->Position / 
			       static_cast<double>(ViewPtr->DRM31->GetSteps())
			       ) * 100) / 100;
      (*note)->Vel += delta_y;
      if ((*note)->Vel > 1.27)
	(*note)->Vel = fmodf((*note)->Vel, 1.27);
      else if ((*note)->Vel < 0.00)
	(*note)->Vel = 1.27 + fmodf((*note)->Vel, 1.27);
      
      (*note)->Vel = floor((*note)->Vel * 100) / 100;
    }
  
  for (list<BeatNote*>::iterator note = SelectedNotes.begin(); 
       note != SelectedNotes.end(); note++)
    {
      ViewPtr->DRM31->AddBeatNote(*note,
				  ViewPtr->DRM31->Channels[(*note)->NumChan], 
				  ViewPtr->DRM31->EditedBank, 
				  ViewPtr->DRM31->EditedPattern);
    }
  
    }
  
  MotionPosX = event.m_x; 
  MotionPosY = event.m_y; 
  
  Refresh();
  ViewPtr->UpdateToolBar();
  
  //cout << vel << endl;
  /*
  vel = floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) / (ViewPtr->TrackHeight - DEC)) * 1.27) * 100) / 100;
  cout << vel << endl;
  */
  
    
}

inline void BeatBoxScrollView::ClearSelected(void)
{
  for (list<BeatNote*>::iterator note = SelectedNotes.begin(); 
       note != SelectedNotes.end(); note++)
    {
      (*note)->Selected = false;
    }
  
  SelectedNotes.clear();
}

void BeatBoxScrollView::OnLeftUp(wxMouseEvent& event)
{
  //ClearSelected();
  if (!OnSelecting)
    {
      if (!SelectedNote)
	{
	  cout << "SelectedNote = false" << endl;
	  ClearSelected();
	}
	  //ClearSelected();
      //SelectNote(SelectedNote);
      Refresh();
      return;
    }
  else
    OnSelecting = false;
  /*
    if (SelectedNote)
    {
      SelectedNote = 0x0;
      return;
    }
  */
  
  long track = (ClickPosY + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  
  //from
  double xpos = static_cast<double>
    (ClickPosX 
     + static_cast<double>
     (ViewPtr->XScroll)) 
    / static_cast<double>(ViewPtr->XSize) *  ViewPtr->DRM31->GetSteps();
  double ypos = (ClickPosY + ViewPtr->YScroll) % ViewPtr->TrackHeight;
  double vel = 
    floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) / 
			       ViewPtr->TrackHeight) * 1.30) * 100) / 100;
  
  //to
  double to_xpos = static_cast<double>
    (MotionPosX 
     + static_cast<double>
     (ViewPtr->XScroll)) 
    / static_cast<double>(ViewPtr->XSize) *  ViewPtr->DRM31->GetSteps();
  
  long to_track = (MotionPosY + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  double to_vel;
  if ( to_track > track )
    {
      ypos = ViewPtr->TrackHeight;
      to_vel = 0.0;
    }
  else if ( to_track < track )
    {
      ypos = 0;
      to_vel = 1.27;
    }
  else
    {
      ypos = (MotionPosY + ViewPtr->YScroll) % ViewPtr->TrackHeight;
      to_vel = 
	floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) / 
				   ViewPtr->TrackHeight) * 1.30) * 100) / 100;
    }
  //cout << "from xy: " << xpos << " "<< vel << " to " << to_xpos << " " << to_vel << endl;
  if (vel > 1.27)
    vel = 1.27;
  if (vel < 0)
    vel = 0.0;
  if (to_vel > 1.27)
    to_vel = 1.27;
  if (to_vel < 0)
    to_vel = 0.0;
  
  if (!SelectedNote)
    {
      if (!CtrlDown)
	ClearSelected();
      for (list<BeatNote*>::iterator note = 
	     ViewPtr->TrackView->BeatTracks[track]->Channel->Rythms[ViewPtr->DRM31->EditedBank][ViewPtr->DRM31->EditedPattern].begin(); 
	   note != ViewPtr->TrackView->BeatTracks[track]->Channel->Rythms[ViewPtr->DRM31->EditedBank][ViewPtr->DRM31->EditedPattern].end(); 
	   note++)
	{
	  if ( (((*note)->Position > xpos && (*note)->Position < to_xpos)
	       || ((*note)->Position < xpos && (*note)->Position > to_xpos))
	       && (((*note)->Vel >= vel && (*note)->Vel <= to_vel) 
		   || ((*note)->Vel <= vel && (*note)->Vel >= to_vel)) )
	    {
	      if ((*note)->Selected)
		{
		  DeSelectNote(*note);
		}
	      else
		{
		  SelectNote(*note);
		  SelectedNote = 0x0;
		}
	      ViewPtr->UpdateToolBar();
	      //cout << "new note selected: " << SelectedNotes.size() << endl;
	    }
	}
    }
  Refresh();
}

void BeatBoxScrollView::OnLeftDown(wxMouseEvent& event)
{
  double xpos = static_cast<double>
    (event.m_x 
     + static_cast<double>
     (ViewPtr->XScroll)) 
    / static_cast<double>(ViewPtr->XSize) *  ViewPtr->DRM31->GetSteps();
  
  //  OnSelecting = true;
  
  ClickPosY = event.m_y;
  ClickPosX = event.m_x;
  
  long track = (event.m_y + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  ViewPtr->TrackView->SelectedTrack = track;
  ViewPtr->TrackView->Refresh();
  

  double ypos = (event.m_y + ViewPtr->YScroll) % ViewPtr->TrackHeight;
  
  double vel = floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) / ViewPtr->TrackHeight) * 1.30) * 100) / 100;
  //cout << vel << endl;
  //vel = floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) / (ViewPtr->TrackHeight - DEC)) * 1.27) * 100) / 100;
  //cout << vel << endl;
  //  double vel = floor(static_cast<double>(((ViewPtr->TrackHeight - ypos + DEC) / ViewPtr->TrackHeight) * 1.30) * 100) / 100;
  //  cout << vel << endl;
  
  double min, max;
  min = vel - 0.05;
  max = vel + 0.05;
  double inf, sup;
  inf = 
    (event.m_x - 3 - 2 + static_cast<double>(ViewPtr->XScroll)) 
    / static_cast<double>(ViewPtr->XSize) * ViewPtr->DRM31->GetSteps();
  sup = (event.m_x + 3 + static_cast<double>(ViewPtr->XScroll)) 
    / static_cast<double>(ViewPtr->XSize) * ViewPtr->DRM31->GetSteps();
  
  //cout << "pos "<< xpos << " inf "  << inf << " sup " << sup << endl;
  
  bool none = true;
  bool already = false;
  BeatTrack* bt = ViewPtr->TrackView->BeatTracks[track];
  for (list<BeatNote*>::iterator note = 
	 bt->Channel->Rythms[ViewPtr->DRM31->EditedBank][ViewPtr->DRM31->EditedPattern].begin(); 
       note != bt->Channel->Rythms[ViewPtr->DRM31->EditedBank][ViewPtr->DRM31->EditedPattern].end(); 
       note++)
    {
      if ( (*note)->Position > inf && (*note)->Position < sup 
	   && (*note)->Vel >= min && (*note)->Vel <= max )
	{
	  none = false;
	  for (list<BeatNote*>::iterator sel = SelectedNotes.begin();
	       sel != SelectedNotes.end(); sel++)
	    if (*sel == *note)
	      { 
		already = true;
		if ((*note)->Selected && CtrlDown)
		  {
		    DeSelectNote(*note);
		  }
		else
		  {
		    (*note)->Selected = true;
		    SelectedNote = *note;
		    //ClearSelected(); 
		    //SelectNote(*note); 
		  }
		ViewPtr->UpdateToolBar();
		break; 
	      }
	  if (!already)
	    {
	      if (!CtrlDown)
		ClearSelected();
	      SelectNote(*note);
	    }
	}
      //cout << "note clicked" << endl;//cout << "note clicked position: " << (*note)->Position<< endl;
      //cout << "note vel: " << (*note)->Vel << endl;
    }
  if (none)
    {
      
      SelectedNote = 0x0;
      ViewPtr->UpdateToolBar();
      //ClearSelected();
      //OnSelecting = true;
    }
  Refresh();
  //cout << "pos " << pos << endl;
}

void BeatBoxScrollView::OnRightDown(wxMouseEvent& event)
{
  ViewPtr->TrackView->SelectedTrack = 
    (event.m_y + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  ViewPtr->TrackView->Refresh();
  
  PopupMenu(PopMenu, wxPoint(event.m_x, event.m_y));
}

inline void BeatBoxScrollView::SelectNote(BeatNote* note)
{
  note->Selected = true;
  SelectedNote = note;
  ViewPtr->UpdateToolBar();
  SelectedNotes.push_back(note);
  /*
  ViewPtr->DRM31->RemBeatNote(note, ViewPtr->DRM31->Channels[(*note)->NumChan],
			      ViewPtr->DRM31->EditedBank, 
			      ViewPtr->DRM31->EditedPattern);
  */
}
inline void BeatBoxScrollView::DeSelectNote(BeatNote* n)
{
  for (list<BeatNote*>::iterator note = SelectedNotes.begin();
       note != SelectedNotes.end(); note++)
    {
      if (*note == n)
	{
	  n->Selected = false;
	  
	  SelectedNotes.erase(note);
	  if (SelectedNote == n || !SelectedNote)
	    {
	      if (!SelectedNotes.empty())
		SelectedNote = SelectedNotes.front();
	      else
		SelectedNote = 0x0;
	      ViewPtr->UpdateToolBar();
	    }
	  return;
	}
    }
}

BEGIN_EVENT_TABLE(BeatBoxView, wxPanel)
  EVT_SLIDER(ID_HZoom, BeatBoxView::OnHZoom)
  EVT_SLIDER(ID_VZoom, BeatBoxView::OnVZoom)
  EVT_COMMAND_SCROLL(ID_HScroll, BeatBoxView::OnHScroll)
  EVT_COMMAND_SCROLL(ID_VScroll, BeatBoxView::OnVScroll)
  EVT_SIZE(BeatBoxView::OnSize)
  //EVT_PAINT(BeatBoxView::OnPaint)
  EVT_TEXT_ENTER(ID_PosTextCtrl, BeatBoxView::OnPosChange)
  EVT_TEXT_ENTER(ID_VelTextCtrl, BeatBoxView::OnVelChange)
  EVT_COMBOBOX(ID_SubCombo, BeatBoxView::OnSubdivChange)
  EVT_TOOL(ID_Magnet, BeatBoxView::OnMagnetism)
END_EVENT_TABLE()

BeatBoxView::BeatBoxView(wxWindow* parent, wxWindowID id, WiredBeatBox* bb,
			 const wxPoint& pos, const wxSize& size, 
			 wxMutex* mutex)
  : wxPanel(parent, id, pos, size)
{
  //wxString combo[(NB_COMBO_CHOICES + 1)];
  
  Mutex = mutex;
  DRM31 = bb;
  
  Magnetism = false;
  HZoom = VZoom = 1.0;
  XScroll = YScroll = 0;
  TrackHeight = BEAT_HEIGHT;
  XScrollCoef = YScrollCoef = 0.0;
  
    
  HZoomSlider = new wxSlider(this, ID_HZoom, 100, 100, 800, 
			     wxPoint(0,0), 
			     wxSize(TRACK_WIDTH,RULER_HEIGHT)); 
  RulerView = new Ruler(this, -1, wxPoint(TRACK_WIDTH,0), 
			wxSize(GetClientSize().x - TRACK_WIDTH, RULER_HEIGHT),
			this);
  
  TrackView = new BeatBoxTrackView(this, -1, wxPoint(0,RULER_HEIGHT), 
				   wxSize(TRACK_WIDTH, GetClientSize().y),
				   this);
  /*
  BeatTrack* beat_track;
  for (int i = 0; i < 11; i++)
    {
      try 
	{
	  beat_track = new BeatTrack(TrackView, -1,wxPoint(0, i * TrackHeight),
				     wxSize(TRACK_WIDTH, TrackHeight), 
				     DRM31->Channels[i]);
	}
      catch (std::bad_alloc)
	{
	  cout << "[BeatBoxView] bad alloc" << endl;
	}
      catch (...) 
	{ 
	  cout <<"[BeatBoxView] unexpected error during new" << endl; 
	}
      TrackView->BeatTracks.push_back(beat_track);
    }
  */
  ToolBar = new wxToolBar(this, -1, wxPoint(-1, -1), 
			  wxSize(-1, TOOLBAR_HEIGHT), wxTB_FLAT);
  
  BeatView = new BeatBoxScrollView(this, -1, wxPoint(TRACK_WIDTH,RULER_HEIGHT),
				   GetClientSize()-wxSize(150,20), this);
  
  VZoomSlider = new wxSlider(this, ID_VZoom, 100, 100, 400, 
			     wxPoint(0,GetClientSize().y-RULER_HEIGHT), 
			     wxSize(TRACK_WIDTH, RULER_HEIGHT));
  
  HScrollBar = new wxScrollBar(this, ID_HScroll, wxDefaultPosition, 
			       wxSize(-1, RULER_HEIGHT), wxSB_HORIZONTAL);
  
  VScrollBar = new wxScrollBar(this, ID_VScroll, wxDefaultPosition, 
			       wxSize(RULER_HEIGHT, -1), wxSB_VERTICAL);
  
  
  
  ToolBar->AddCheckTool(ID_Magnet, "Magnet", 
			wxBitmap(_T(string(DRM31->GetDataDir()
					   + string(MAGN_UP)).c_str()), 
				 wxBITMAP_TYPE_PNG), 
			wxBitmap(_T(string(DRM31->GetDataDir() 
					   + string(MAGN_DOWN)).c_str()), 
				 wxBITMAP_TYPE_PNG), 
			"Activate magnetism", "Deactivate magnetism", NULL);
  
  //wxString combo[(NB_COMBO_CHOICES + 1)];
  
  wxString choices[NB_COMBO_CHOICES];
  
  for (int i = 0; i < NB_COMBO_CHOICES; i++)
    {
      choices[i].Printf("1/%d", i+1);
    }
  SubCombo = new wxComboBox(ToolBar, ID_SubCombo, _T("1/1"), 
			    wxPoint(-1, -1), wxSize(64, -1), 
			    NB_COMBO_CHOICES, choices);
  ToolBar->AddControl(SubCombo);
  ToolBar->AddSeparator();
  
  PosTextCtrl = new wxTextCtrl(ToolBar, ID_PosTextCtrl, _T("pos"),
				  wxPoint(-1,-1), wxSize(32, -1));
  VelTextCtrl = new wxTextCtrl(ToolBar, ID_VelTextCtrl, _T("vel"),
				  wxPoint(-1,-1), wxSize(32, -1));
  ToolBar->AddControl(PosTextCtrl);
  ToolBar->AddControl(VelTextCtrl);
  //Connect()
  
  wxBoxSizer *col_1;
  wxBoxSizer *col_2;
  wxBoxSizer *glob_2;
  wxBoxSizer *glob_1;
  
  col_1 = new wxBoxSizer(wxVERTICAL);
  col_1->Add(HZoomSlider, 0, 0, 0);
  col_1->Add(TrackView, 1, 0, 0);
  col_1->Add(VZoomSlider, 0,0,0);
  
  col_2 = new wxBoxSizer(wxVERTICAL);
  col_2->Add(RulerView, 0, wxEXPAND, 0);
  
  wxBoxSizer* row;
  
  row = new wxBoxSizer(wxHORIZONTAL);
  row->Add(BeatView, 1, wxEXPAND, 0);
  row->Add(VScrollBar, 0, wxEXPAND, 0);
  col_2->Add(row, 1, wxEXPAND, 0);
  col_2->Add(HScrollBar, 0, wxEXPAND, 0);
  
  glob_1 = new wxBoxSizer(wxHORIZONTAL);
  glob_1->Add(col_1, 0, wxEXPAND, 0);
  glob_1->Add(col_2, 1, wxEXPAND, 0);
  
  glob_2 = new wxBoxSizer(wxVERTICAL);
  glob_2->Add(ToolBar, 0, wxEXPAND, 0);
  glob_2->Add(glob_1, 1, wxEXPAND, 0);
  
  SetSizer(glob_2);
  
  XSize = BeatView->GetClientSize().x;
  YSize = BEAT_HEIGHT * TrackView->BeatTracks.size();
  
  AdjustHScrolling();
  AdjustVScrolling();
}

BeatBoxView::~BeatBoxView()
{
  delete HZoomSlider;
  delete VZoomSlider;
}

void BeatBoxView::OnHZoom(wxCommandEvent& event)
{
  HZoom = static_cast<double>(HZoomSlider->GetValue() / 100.0);
  XSize = static_cast<long>(floor(BeatView->GetClientSize().x * HZoom));
  
  //XScrollCoef = ;
  //XScroll = static_cast<long>(floor(static_cast<double>(XScrollCoef * XSize)));
  XScroll = 
    static_cast<long>
    ( floor((1.0 - 1.0/HZoom) * (XScrollCoef * XSize)) );
  
  //cout << XScroll << endl;
  
  AdjustHScrolling();
  RulerView->Refresh();
  BeatView->Refresh();
}

void BeatBoxView::OnVZoom(wxCommandEvent& event)
{
  VZoom = static_cast<double>(VZoomSlider->GetValue() / 100.0);
  TrackHeight = static_cast<long>(floor(BEAT_HEIGHT * VZoom));
  
  //cout << "height: " << TrackHeight << ", zoom: " << VZoom << endl;
  
  YSize =  TrackHeight * TrackView->BeatTracks.size();
  

  YScroll = static_cast<long>
    ( floor((1.0 - 1.0/VZoom) * (YScrollCoef * YSize)) );
    //static_cast<long>(floor(static_cast<double>(YScrollCoef * YSize)));
  
  AdjustVScrolling();
  BeatView->Refresh();
  /*
    for (vector<BeatTrack*>::iterator bt = TrackView->BeatTracks.begin(); 
       bt != TrackView->BeatTracks.end(); bt++)
    {
      (*bt)->SetSize(TRACK_WIDTH, TrackHeight);
    }
  */
  //TrackView->SetSize(TRACK_WIDTH, YSize);
  TrackView->Refresh();
}

void BeatBoxView::OnHScroll(wxScrollEvent& event)
{
  long xscroll = HScrollBar->GetThumbPosition();
  
  XScrollCoef = 
    static_cast<double>(xscroll /
			static_cast<double>(XSize));
  
  
  BeatView->ScrollWindow( XScroll - xscroll, 0,(const wxRect *) NULL);
  
  XScroll = xscroll;
  RulerView->Refresh();
  BeatView->Refresh();
}

void BeatBoxView::OnVScroll(wxScrollEvent& event)
{
  long yscroll = VScrollBar->GetThumbPosition();
  
  YScrollCoef = 
    static_cast<double>(yscroll /
			static_cast<double>(YSize));
  
  BeatView->ScrollWindow( 0, YScroll - yscroll, (const wxRect *) NULL);
  TrackView->ScrollWindow( 0, YScroll - yscroll, (const wxRect *) NULL);
  
  YScroll = yscroll;
  BeatView->Refresh();
  TrackView->Refresh();
}

void BeatBoxView::OnSize(wxSizeEvent& event)
{
  Layout();
  XSize = static_cast<long>(floor(BeatView->GetClientSize().x * HZoom));
  
  /*
  XScroll = 
    static_cast<long>
    ( floor((1.0 - 1.0/HZoom) * (XScrollCoef * XSize)) );
  YScroll = static_cast<long>
    ( floor((1.0 - 1.0/VZoom) * (YScrollCoef * YSize)) );
  */
  
  AdjustHScrolling();
  AdjustVScrolling();
}

inline void BeatBoxView::AdjustHScrolling(void)
{
  //cout << "SetScrollbar: scroll:" << XScroll << "\tthumb size:" << BeatView->GetClientSize().x << "\trange:"<< XSize << endl;
  HScrollBar->SetScrollbar(XScroll, BeatView->GetClientSize().x, 
			   XSize, 1, true);
  
}

inline void BeatBoxView::AdjustVScrolling(void)
{
  VScrollBar->SetScrollbar(YScroll, BeatView->GetClientSize().y, 
			   YSize, TrackHeight, true);
}

void BeatBoxView::OnSubdivChange(wxCommandEvent& WXUNUSED(event))
{
  int i = SubCombo->GetSelection();
  BeatView->SubDiv = i;
  BeatView->Refresh();
}

void BeatBoxView::OnPosChange(wxCommandEvent& WXUNUSED(event))
{
  if (!BeatView->SelectedNote)
    return;
  wxString s = PosTextCtrl->GetValue();
  
}

void BeatBoxView::OnVelChange(wxCommandEvent& WXUNUSED(event))
{
  if (!BeatView->SelectedNote)
    return;
  wxString s = VelTextCtrl->GetValue();

}

void BeatBoxView::OnMagnetism(wxCommandEvent& WXUNUSED(event))
{
  Magnetism = 
    ToolBar->GetToolState(ID_Magnet) ? true : false;
}

void BeatBoxView::UpdateToolBar(void)
{
  if (!BeatView->SelectedNote)
    {
      PosTextCtrl->SetValue(_T("pos"));
      VelTextCtrl->SetValue(_T("vel"));
      return;
    }
  wxString s;
  s.Printf("%f", BeatView->SelectedNote->Position + 1.0);
  s.Truncate(4);
  PosTextCtrl->SetValue(s);
  s.Printf("%f", BeatView->SelectedNote->Vel);
  s.Truncate(4);
  VelTextCtrl->SetValue(s);
}

/*
void BeatBoxView::OnPaint(wxPaintEvent& event)
{
  
}
*/
