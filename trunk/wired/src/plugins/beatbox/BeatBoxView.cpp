#include <math.h>
#include "BeatBox.h"

BEGIN_EVENT_TABLE(Ruler, wxWindow)
  EVT_PAINT(Ruler::OnPaint)
END_EVENT_TABLE()

#define GET_CHAN_ID()		\
 ViewPtr->TrackView->BeatTracks[ViewPtr->TrackView->SelectedTrack]->Channel->Id

#define GET_TRACK_NOTES(track)						\
  ViewPtr->TrackView->BeatTracks[track]->Channel->			\
    Rythms[ViewPtr->DRM31->EditedBank][ViewPtr->DRM31->EditedPattern]
				  
#define LIGHT_BLUE		wxColour(204,255,255)
#define DEEP_BLUE		wxColour(0,51,153)
#define DEEP_GREEN		wxColour(28,162,56)
#define SELECTED_NOTE_COLOUR	wxColour(1,74,17)
#define DEFAULT_NOTE_COLOUR	wxColour(110,110,110)
#define SELECTION_COLOUR	wxColour(111,226,122)


#define TEXT_COLOUR		*wxBLACK
#define TRACK_COLOUR		wxColour(165,174,180)
#define SELECTEDTRACK_COLOUR	wxColour(101,113,122)
#define BAR_COLOUR		wxColour(142,155,166)
#define SUBBAR_COLOUR		wxColour(184,204,242)
#define SEPARATOR_COLOUR	wxColour(124,124,223)
#define BORDER_COLOUR		wxColour(61,67,71)
#define BGTRACK_COLOUR		BORDER_COLOUR
#define BGVIEW_COLOUR		wxColour(242,242,255)
#define BGRULER_COLOUR		wxColour(237,237,242)
#define SCROLLBAR_COLOUR	wxColour(142,142,169)
#define RULERTEXT_HEIGHT	6

Ruler::Ruler(wxWindow *parent, wxWindowID id, 
	     const wxPoint &pos, const wxSize &size,
	     BeatBoxView* view_ptr)
  : wxWindow(parent, id, pos, size, wxSUNKEN_BORDER)
{
  SetBackgroundColour(BGRULER_COLOUR);
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
  dc.SetPen(wxPen(BORDER_COLOUR, 1, wxSOLID));
  dc.SetBrush(wxBrush(BGRULER_COLOUR));
  dc.SetFont(wxFont(RULERTEXT_HEIGHT, wxDEFAULT, wxNORMAL, wxNORMAL));
  dc.SetTextForeground(TEXT_COLOUR);
  dc.DrawRoundedRectangle(0, 0, size.x, size.y, 3);
  dc.SetPen(wxPen(BAR_COLOUR, 1, wxSOLID));
  
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
  : wxWindow(parent, id, pos, size, wxSUNKEN_BORDER)
{
  SetBackgroundColour(BGTRACK_COLOUR);
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
  
  
  dc.SetPen(BORDER_COLOUR); 
  dc.SetTextForeground(TEXT_COLOUR);
  dc.SetBrush(wxBrush(TRACK_COLOUR, wxSOLID));
  
  wxString s;
  
  long tr = 0;
  
  for (vector<BeatTrack*>::iterator bt = BeatTracks.begin(); 
       bt != BeatTracks.end(); bt++)
    {
      if (tr == SelectedTrack)
	{
	  dc.SetBrush(wxBrush(SELECTEDTRACK_COLOUR, wxSOLID));
	  dc.DrawRoundedRectangle(0, y, TRACK_WIDTH, ViewPtr->TrackHeight, 3);
	  dc.SetBrush(wxBrush(TRACK_COLOUR, wxSOLID));
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

BeatTrack::BeatTrack(BeatBoxChannel* channel)
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
  : wxScrolledWindow(parent, id, pos, size, wxSUNKEN_BORDER)
{
  //SetBackgroundColour(*wxWHITE);
  SetBackgroundColour(BGVIEW_COLOUR);
  
  SetForegroundColour(BAR_COLOUR);
  CtrlDown = false;
  ViewPtr = view_ptr;
  OnSelecting = false;
  ClickPosX = ClickPosY = MotionPosX = MotionPosY = 0;
  SelectedNote = 0x0;
  SubDiv = 0;
  Param = 0;

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
}

BeatBoxScrollView::~BeatBoxScrollView()
{}

void BeatBoxScrollView::OnKeyDown(wxKeyEvent& event)
{
  switch (event.GetKeyCode())
    {
    case WXK_CONTROL:
      CtrlDown = true;
      break;
    case 'C':
      if (event.ControlDown())
	{
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
    }
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
      ViewPtr->DRM31->AddBeatNote(n,
				  ViewPtr->DRM31->Channels[chan],
				  ViewPtr->DRM31->EditedBank,
				  ViewPtr->DRM31->EditedPattern);
      new_selection.push_back(n);
    }
  ClearSelected();
  SelectedNotes = new_selection;
  Refresh();
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
  
  
  
  dc.SetBrush(wxBrush(BGVIEW_COLOUR));
  dc.SetTextForeground(*wxBLACK);
    
  if (OnSelecting)
    {
      dc.SetPen(BORDER_COLOUR);
      dc.SetBrush(wxBrush(SELECTION_COLOUR, wxSOLID));
      dc.DrawRectangle(ClickPosX, ClickPosY, 
		       MotionPosX - ClickPosX, MotionPosY - ClickPosY);
    }
  
  int steps = ViewPtr->DRM31->GetSteps();
  double res = 
    static_cast<double>
    ( ViewPtr->XSize / static_cast<double>(steps) );
  long sub;
  
  for (int cpt = 0; x < size.x; 
       x = static_cast<long>
	 (ceil(static_cast<double>(cpt * res))) - ViewPtr->XScroll)
    {
      dc.SetPen(wxPen(BAR_COLOUR, 1, wxSOLID));
      dc.DrawLine(x, 0, x, size.y);
      dc.SetPen(wxPen(SUBBAR_COLOUR, 1, wxSOLID));
      
      for (int i = 1; i <= SubDiv; i++)
      {
	sub = x + static_cast<long>(floor((res/(SubDiv+1))*i));
	dc.DrawLine(sub, 0, sub, size.y);
      }
      cpt++;
    }
  
  dc.SetPen(wxPen(BORDER_COLOUR, 1, wxSOLID));
  
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
	  yn = (ViewPtr->TrackHeight * cpt) 
	    + DEC + static_cast<long> 
	    ( floor
	      ((1.27 - (*note)->Params[Param]) / 1.27 * (ViewPtr->TrackHeight - DEC)))
	    - ViewPtr->YScroll;
	  
	  if ((*note)->Selected)
	    dc.SetBrush(SELECTED_NOTE_COLOUR);
	  else
	    dc.SetBrush(DEFAULT_NOTE_COLOUR);
	  
	  dc.DrawCircle(wxPoint(xn, yn), 3);
	}
      cpt++;
      y += ViewPtr->TrackHeight;
      
      dc.SetPen(wxPen(SEPARATOR_COLOUR, 1, wxSOLID));
      dc.DrawLine(0, y, GetClientSize().x, y);
      dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
    }
  
}

/*
inline void CalcXPos(double* xpos, int x)
{
  *xpos = floor( (static_cast<double>
		  (x + static_cast<double>
		   (ViewPtr->XScroll)) 
		  / static_cast<double>(ViewPtr->XSize) 
		  *  ViewPtr->DRM31->GetSteps()) 
		 * 100) / 100;
}
*/

void BeatBoxScrollView::OnMotion(wxMouseEvent& event)
{
  if (!event.LeftIsDown())
    {
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
  
  long track = (event.m_y + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  double ypos = (event.m_y + ViewPtr->YScroll) % ViewPtr->TrackHeight;
  double vel = floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) 
					  / ViewPtr->TrackHeight) * 1.30) 
		     * 100) / 100;
  /*
    if (vel < 0.0)
    vel = 0;
  else if (vel > 1.27)
    vel = 1.27;
  */
  
  if (SelectedNote)
    {
      float yparam = SelectedNote->Params[Param];
      for (list<BeatNote*>::iterator note = SelectedNotes.begin(); 
	   note != SelectedNotes.end(); note++)
	{
	  ViewPtr->DRM31->RemBeatNote(*note, 
				      ViewPtr->DRM31->Channels[(*note)->NumChan], 
				      ViewPtr->DRM31->EditedBank, 
				      ViewPtr->DRM31->EditedPattern);
	}
      cout << "cur x pos: " << xpos << " Selected note pos: " 
	   << SelectedNote->Position << endl;
      
      double delta_x = xpos - SelectedNote->Position;
      //double delta_x = xpos - SelectedNote->Position;
      cout << "delta X: " << delta_x << endl;
      //printf("delta x: %f\n", delta_x);
      float delta_y = vel - yparam;//SelectedNote->Params[Param];
      
  int n = 0;
  for (list<BeatNote*>::iterator note = SelectedNotes.begin(); 
       note != SelectedNotes.end(); note++)
    {
      n++;
      cout << n << " old position: " << (*note)->Position;
      (*note)->Position += delta_x;
      cout << " new one: " << (*note)->Position;
      if ((*note)->Position >= static_cast<double>(ViewPtr->DRM31->GetSteps()))
	{
      cout << " modulus " << static_cast<double>(ViewPtr->DRM31->GetSteps());

	  (*note)->Position = 
	    fmod((*note)->Position,
		 static_cast<double>(ViewPtr->DRM31->GetSteps()));
	}
      else if ((*note)->Position < 0.00)
	{
	  (*note)->Position = static_cast<double>(ViewPtr->DRM31->GetSteps()) 
	    + fmod((*note)->Position, 
		   static_cast<double>(ViewPtr->DRM31->GetSteps()));
	}
      cout << " intermediate: " ;//<< (*note)->Position << endl;
      printf("%f\n",(*note)->Position);
      //(*note)->Position = floor((*note)->Position * 100) / 100;
      //cout << " floored: " << (*note)->Position << endl;
      (*note)->BarPos = floor(((*note)->Position / 
			       static_cast<double>(ViewPtr->DRM31->GetSteps())
			       ) * 100) / 100;
      (*note)->Params[Param] += delta_y;
      if ((*note)->Params[Param] > 1.27)
	(*note)->Params[Param] = fmodf((*note)->Params[Param], 1.27);
      else if ((*note)->Params[Param] < 0.00)
	(*note)->Params[Param] = 1.27 + fmodf((*note)->Params[Param], 1.27);
      
      (*note)->Params[Param] = floor((*note)->Params[Param] * 100) / 100;
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
  if (!OnSelecting)
    {
      if (!SelectedNote)
	{
	  ClearSelected();
	}
      Refresh();
      return;
    }
  else
    OnSelecting = false;
  
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
      for (list<BeatNote*>::iterator note = GET_TRACK_NOTES(track).begin();
	   note != GET_TRACK_NOTES(track).end(); note++)
	{
	  if ( (((*note)->Position > xpos && (*note)->Position < to_xpos)
	       || ((*note)->Position < xpos && (*note)->Position > to_xpos))
	       && (((*note)->Params[Param] >= vel && (*note)->Params[Param] <= to_vel) 
		   || ((*note)->Params[Param] <= vel && (*note)->Params[Param] >= to_vel)) )
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
  
  ClickPosY = event.m_y;
  ClickPosX = event.m_x;
  
  long track = (event.m_y + ViewPtr->YScroll) / ViewPtr->TrackHeight;
  ViewPtr->TrackView->SelectedTrack = track;
  ViewPtr->TrackView->Refresh();
  

  double ypos = (event.m_y + ViewPtr->YScroll) % ViewPtr->TrackHeight;
  
  double vel = floor(static_cast<double>(((ViewPtr->TrackHeight - ypos) / ViewPtr->TrackHeight) * 1.30) * 100) / 100;
  
  double min, max;
  min = vel - 0.05;
  max = vel + 0.05;
  double inf, sup;
  inf = 
    (event.m_x - 3 - 2 + static_cast<double>(ViewPtr->XScroll)) 
    / static_cast<double>(ViewPtr->XSize) * ViewPtr->DRM31->GetSteps();
  sup = (event.m_x + 3 + static_cast<double>(ViewPtr->XScroll)) 
    / static_cast<double>(ViewPtr->XSize) * ViewPtr->DRM31->GetSteps();
  
  bool none = true;
  bool already = false;
  BeatTrack* bt = ViewPtr->TrackView->BeatTracks[track];
  for (list<BeatNote*>::iterator note = 
	 bt->Channel->Rythms[ViewPtr->DRM31->EditedBank][ViewPtr->DRM31->EditedPattern].begin(); 
       note != bt->Channel->Rythms[ViewPtr->DRM31->EditedBank][ViewPtr->DRM31->EditedPattern].end(); 
       note++)
    {
      
      if ( (*note)->Position > inf && (*note)->Position < sup 
	   && (*note)->Params[Param] >= min && (*note)->Params[Param] <= max )
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
    }
  if (none)
    {
      
      SelectedNote = 0x0;
      ViewPtr->UpdateToolBar();
    }
  Refresh();
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
  EVT_COMBOBOX(ID_ParamsCombo, BeatBoxView::OnParamChange)
  EVT_TOOL(ID_Magnet, BeatBoxView::OnMagnetism)
END_EVENT_TABLE()

BeatBoxView::BeatBoxView(wxWindow* parent, wxWindowID id, WiredBeatBox* bb,
			 const wxPoint& pos, const wxSize& size, 
			 wxMutex* mutex)
  : wxPanel(parent, id, pos, size)
{
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
  HScrollBar->SetBackgroundColour(SCROLLBAR_COLOUR);
  VScrollBar->SetBackgroundColour(SCROLLBAR_COLOUR);
  //HScrollBar->SetForegroundColour(SCROLLBAR_COLOUR);
  //VScrollBar->SetForegroundColour(SCROLLBAR_COLOUR);
  //HZoomSlider->SetForegroundColour(SCROLLBAR_COLOUR);
  HZoomSlider->SetBackgroundColour(SCROLLBAR_COLOUR);
  //VZoomSlider->SetForegroundColour(SCROLLBAR_COLOUR);
  VZoomSlider->SetBackgroundColour(SCROLLBAR_COLOUR);
  /* ToolBar */
  
  ToolBar->AddCheckTool(ID_Magnet, "Magnet", 
			wxBitmap(_T(string(DRM31->GetDataDir()
					   + string(MAGN_UP)).c_str()), 
				 wxBITMAP_TYPE_PNG), 
			wxBitmap(_T(string(DRM31->GetDataDir() 
					   + string(MAGN_DOWN)).c_str()), 
				 wxBITMAP_TYPE_PNG), 
			"Activate magnetism", "Deactivate magnetism", NULL);
  
  
  wxString choices[NB_COMBO_CHOICES];
  for (int i = 0; i < NB_COMBO_CHOICES; i++)
      choices[i].Printf("1/%d", i+1);
  
  SubCombo = new wxComboBox(ToolBar, ID_SubCombo, _T("1/1"), 
			    wxPoint(-1, -1), wxSize(64, -1), 
			    NB_COMBO_CHOICES, choices);
  
    
  //PosTextCtrl = new wxTextCtrl(ToolBar, ID_PosTextCtrl, _T("pos"),
  PosTextCtrl = new wxTextCtrl(ToolBar, ID_PosTextCtrl, _T("POS"),
			       wxPoint(-1,-1), wxSize(48, -1));
  VelTextCtrl = new wxTextCtrl(ToolBar, ID_VelTextCtrl, _T("--------------"),
				  wxPoint(-1,-1), wxSize(48, -1));
  
  wxString params[NB_PARAMS_CHOICES];
  /*
    params[0].Printf("level");
  params[1].Printf("velocity");
  params[2].Printf("pitch");
  params[3].Printf("pan");
  params[4].Printf("start");
  params[5].Printf("end");
  */
  params[0].Printf("LEVEL");
  params[1].Printf("VELOCITY");
  params[2].Printf("PITCH");
  params[3].Printf("PAN");
  params[4].Printf("START");
  params[5].Printf("END");
  
  //ParamsCombo = new wxComboBox(ToolBar, ID_ParamsCombo, _T("level"), 
  ParamsCombo = new wxComboBox(ToolBar, ID_ParamsCombo, _T("LEVEL"), 
			       wxPoint(-1, -1), wxSize(96, -1), 
			       NB_PARAMS_CHOICES, params);
    
  
  ToolBar->AddControl(SubCombo);
  ToolBar->AddSeparator();
  ToolBar->AddControl(PosTextCtrl);
  ToolBar->AddSeparator();
  ToolBar->AddControl(ParamsCombo);
  ToolBar->AddControl(VelTextCtrl);
  
  
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
  
  XScroll = 
    static_cast<long>
    ( floor((1.0 - 1.0/HZoom) * (XScrollCoef * XSize)) );

  
  AdjustHScrolling();
  RulerView->Refresh();
  BeatView->Refresh();
}

void BeatBoxView::OnVZoom(wxCommandEvent& event)
{
  VZoom = static_cast<double>(VZoomSlider->GetValue() / 100.0);
  TrackHeight = static_cast<long>(floor(BEAT_HEIGHT * VZoom));
  
  YSize =  TrackHeight * TrackView->BeatTracks.size();
  

  YScroll = static_cast<long>
    ( floor((1.0 - 1.0/VZoom) * (YScrollCoef * YSize)) );
  
  AdjustVScrolling();
  BeatView->Refresh();
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
  
  AdjustHScrolling();
  AdjustVScrolling();
}

inline void BeatBoxView::AdjustHScrolling(void)
{
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

void BeatBoxView::OnParamChange(wxCommandEvent& WXUNUSED(event))
{
  int i = ParamsCombo->GetSelection();
  BeatView->Param = i;
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
      VelTextCtrl->SetValue(_T("----"));
      return;
    }
  wxString s;
  s.Printf("%f", BeatView->SelectedNote->Position + 1.0);
  s.Truncate(4);
  PosTextCtrl->SetValue(s);
  
  s.Printf("%f", BeatView->SelectedNote->Params[BeatView->Param]);
  /*
  if (BeatView->Param == 0)
    s.Printf("%f", BeatView->SelectedNote->Lev);
  else if (BeatView->Param == 1)
    s.Printf("%f", BeatView->SelectedNote->Vel);
  else if (BeatView->Param == 2)
    s.Printf("%f", BeatView->SelectedNote->Pitch);
  else if (BeatView->Param == 3)
    s.Printf("%f", BeatView->SelectedNote->Pan);
  else if (BeatView->Param == 4)
    s.Printf("%f", BeatView->SelectedNote->Start);
  else if (BeatView->Param == 5)
    s.Printf("%f", BeatView->SelectedNote->End);
  */  
  s.Truncate(4);
  VelTextCtrl->SetValue(s);
}
