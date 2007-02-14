// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include <vector>
#include "SequencerGui.h"
#include "Sequencer.h"
#include "Colour.h"
#include "OptionPanel.h"
#include "ColoredBox.h"
#include "MidiPattern.h"
#include "HelpPanel.h"
#include "midi.h"
#include "MidiFile.h"
#include "Track.h"

BEGIN_EVENT_TABLE(MidiPattern, wxWindow)
  EVT_PAINT(MidiPattern::OnPaint)
END_EVENT_TABLE()

static int midi_pattern_count = 1;

MidiPattern::MidiPattern(WiredDocument *parent, double pos, double endpos, long trackindex)
  : Pattern(parent, wxT("MidiPattern"), pos, endpos, trackindex)
{
  Init(parent);
}

MidiPattern::MidiPattern(WiredDocument *parent, double pos,
			 MidiTrack *midiTrack, long trackindex)
  : Pattern(parent, wxT("MidiPattern"), pos,
	    ((double) midiTrack->GetMaxPos()) / 
	    (Seq->SigNumerator * midiTrack->GetPPQN()),
	    trackindex)
{
  Init(parent);
  SetMidiTrack(midiTrack);
}

MidiPattern::~MidiPattern()
{
  OptPanel->DeleteTools(this);
}

void					MidiPattern::Init(WiredDocument* parent)
{
  _filename.clear();
  _noTrack = 0;
  Name.Printf(wxT("T%d M%d"), TrackIndex + 1, midi_pattern_count++);
  PenColor = CL_MIDI_DRAW;
  BrushColor = CL_MIDIDRAWER_BRUSH;
  ppqn = 1;
  Bmp = 0x0;
  Connect(GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnMotion);
  Connect(GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnClick);
  Connect(GetId(), wxEVT_LEFT_UP, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnLeftUp);
  Connect(GetId(), wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnRightClick);
  Connect(GetId(), wxEVT_LEFT_DCLICK, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnDoubleClick);
  Connect(GetId(), wxEVT_ENTER_WINDOW, (wxObjectEventFunction)(wxEventFunction)
	  (wxMouseEventFunction) &MidiPattern::OnHelp);
  SeqPanel->PutCursorsOnTop();
}

// we can't keep MidiTrack instance, because in MidiFile dtor, it delete them
void					MidiPattern::SetMidiTrack(MidiTrack* midiTrack)
{
  vector<MidiFileEvent *>		me;
  unsigned long				i;

  _filename = midiTrack->GetFileName();
  _noTrack = midiTrack->GetNoTrack();
  ppqn = midiTrack->GetPPQN();
  temp.clear();
  me = midiTrack->GetMidiEvents();
  for (i = 0; i < me.size(); i++)
    AddEvent(me[i]);
}

void					MidiPattern::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s(_("This is a MIDI pattern. Double-click on it to open the MIDI editor."));
      HelpWin->SetText(s);
    }
}

void					MidiPattern::Update()
{
  Pattern::Update();
  wxWindow::SetSize(Pattern::GetSize());
  wxWindow::SetPosition(Pattern::GetMPosition() -
			wxPoint((int) floor(SeqPanel->CurrentXScrollPos), (int) SeqPanel->CurrentYScrollPos));
  DrawMidi();
}

void					MidiPattern::SetSelected(bool sel)
{
  Pattern::SetSelected(sel);
  if (sel)
    BrushColor = CL_MIDIDRAWER_BRUSH_SEL;
  else
    BrushColor = CL_MIDIDRAWER_BRUSH;
  DrawMidi();
  wxWindow::Refresh(true);
}

Pattern					*MidiPattern::CreateCopy(double pos)
{
  MidiPattern				*p;
  vector<MidiEvent *>::iterator		o;

  p = new MidiPattern(_documentParent, pos, pos + Length, TrackIndex);
  for (o = Events.begin(); o != Events.end(); o++)
    p->Events.push_back(*o);
  p->ppqn = ppqn;
  Seq->Tracks[TrackIndex]->AddPattern(p);
  return (p);
}

void					MidiPattern::SetDrawColour(wxColour c)
{
  PenColor = c;
  DrawMidi();
  Refresh();
}

void					MidiPattern::OnClick(wxMouseEvent &e)
{
  Pattern::OnClick(e);
  if (SeqPanel->Tool == ID_TOOL_SPLIT_SEQUENCER)
    Split((double) ((Pattern::GetMPosition().x + e.m_x)
		    / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor)));
  else if (SeqPanel->Tool == ID_TOOL_MERGE_SEQUENCER)
    {
    }
  else
    if (SeqPanel->Tool == ID_TOOL_PAINT_SEQUENCER)
      SetDrawColour(SeqPanel->ColorBox->GetColor());
}

void					MidiPattern::OnLeftUp(wxMouseEvent &e)
{
  Pattern::OnLeftUp(e);
}

void					MidiPattern::Split(double pos)
{
  MidiPattern				*p;
  vector<MidiEvent *>::iterator		o;

  if ((Position < pos) && (pos < EndPosition))
    {
#ifdef __DEBUG__
      cout << " >>> HERE OLD:\n\t Position = " << Position << "\n\t Length = " << Length << "\n\t EndPosition = " << EndPosition << endl;
      cout << "new pos: " << pos << endl;
#endif
      p = new MidiPattern(_documentParent, pos, EndPosition, TrackIndex);
#ifdef __DEBUG__
      cout << " >>> HERE NEW :\n\t p->Position = " << p->Position << "\n\t p->Length = " << p->Length << "\n\t p->EndPosition = " << p->EndPosition << endl;
#endif

      SeqMutex.Lock();
      Length = (EndPosition = pos) - Position;
      for (o = Events.begin(), pos -= Position; o != Events.end(); )
	if ((*o)->Position >= pos)
	  {
#ifdef __DEBUG__
	    cout << "Moving event (position " << (*o)->Position << ") to (" << (*o)->Position - pos << ")" << endl;
#endif
	    if ((*o)->Position == (*o)->EndPosition)
	      {
#ifdef __DEBUG__
		cout << "NOTE OFF DETECTED AT POS " << (*o)->Position << endl;
#endif
		(*o)->Position = Length;
		(*o)->EndPosition = Length;
	      }
	    else
	      {
		(*o)->Position -= pos;
		(*o)->EndPosition -= pos;
		p->Events.push_back(*o);
		Events.erase(o);
	      }
	  }
	else
	  {
	    if (((*o)->Position >= 0) && (*o)->EndPosition >= pos)
	      (*o)->EndPosition = pos;
	    o++;
	  }
      p->SetDrawColour(PenColor);
      p->DrawMidi();
      if (IsSelected())
	p->SetSelected(false);
      p->SetCursor(GetCursor());
      p->Update();
      Update();
      SeqMutex.Unlock();
    }
  else
    cout << "C QUOI CE DELIRE DE POS ?? " << pos << " MAIS mpos.x ? " << Pattern::GetMPosition().x << endl;
}

void					MidiPattern::OnDoubleClick(wxMouseEvent &e)
{
  OnClick(e);
  if (SeqPanel->Tool == ID_TOOL_MOVE_SEQUENCER)
    OptPanel->ShowMidi(this);
}

void					MidiPattern::OnRightClick(wxMouseEvent &e)
{
  Pattern::OnRightClick(e);
}

void					MidiPattern::OnMotion(wxMouseEvent &e)
{
  Pattern::OnMotion(e);
}

void					MidiPattern::OnDeleteClick(wxCommandEvent &e)
{
  SeqPanel->DeleteSelectedPatterns();
}

void					MidiPattern::OnMoveToCursorClick(wxCommandEvent &e)
{
  SeqPanel->MoveToCursor();
}

 void					MidiPattern::AddEvent(MidiFileEvent *event)
 {
   MidiEvent				*e;
   int					msg[3];

   msg[0] = event->GetID();
   msg[0] += event->GetChannel();
   msg[1] = event->GetParam(0);
   msg[2] = event->GetParam(1);
   e = new MidiEvent(0, ((double)event->GetPos()) / (Seq->SigNumerator * GetPPQN()), msg);
   e->EndPosition = -1;
   if (IS_ME_NOTEON(e->Msg[0]) && (e->Msg[2] != 0))
     temp.push_back(e);
   else
     if (IS_ME_NOTEOFF(e->Msg[0]) ||
         (IS_ME_NOTEON(e->Msg[0]) && (e->Msg[2] == 0)))
       {
 	for (list<MidiEvent *>::iterator j = temp.begin(); j != temp.end(); j++)
 	  {
 	    if (((*j)->Msg[1] == e->Msg[1]) &&
 		((*j)->EndPosition == -1) &&
 		(ME_CHANNEL(e->Msg[0]) == ME_CHANNEL((*j)->Msg[0])))
 	      {
 		(*j)->EndPosition = e->Position;
 		Events.push_back(*j);
 		Events.push_back(e);
 		temp.erase(j);
 		break;
 	      }
 	  }
       }
     else
       Events.push_back(e);
 }

void                    MidiPattern::AddEvent(MidiEvent *event)
{
  list<MidiEvent *>::iterator        j;
  MidiEvent                *e;
 
  cout << "msg : " << hex << event->Msg[0];
  if ((event->EndPosition == 0.0) &&
      ((event->Msg[0] == M_NOTEON1) || (event->Msg[0] == M_NOTEON2)))
    {
      // Gestion du NOTE OFF
      cout << " " << hex << event->Msg[1] << " " << hex << event->Msg[2] << endl;;
      if (event->Msg[2] == 0)
        for (j = RecordingEvents.begin(); j != RecordingEvents.end(); j++)
          {
            if ((*j)->Msg[1] == event->Msg[1])
              {
                (*j)->EndPosition = event->Position;
                delete event;
                Events.push_back(*j);
        // adds the note off           
		e = new MidiEvent(0, (*j)->EndPosition, (*j)->Msg);
        e->EndPosition = e->Position;
        e->Msg[2] = 0;
        Events.push_back(e);
                cout << "[MIDIPATTERN] Note added with position: " << (*j)->Position
             << "; end position: " << (*j)->EndPosition << endl;
                RecordingEvents.erase(j);
                break;
              }
          }
      else // On ajoute la note dans RecordingEvents en attendant de recevoir un NOTE OFF
        RecordingEvents.push_back(event);
    }
  else
    Events.push_back(event);
  cout << endl;
}

/* TODO: gerer le draw d'un seul event */
void					MidiPattern::DrawMidi()
{
  long					z;
  long					size_x, size_y;
  double				inc;
  vector<MidiEvent *>::iterator		j;
  
  if (Bmp)
    delete Bmp;
  Bmp = new wxBitmap(size_x = GetSize().x, size_y = GetSize().y);
  memDC.SelectObject(*Bmp);
  memDC.SetPen(PenColor);
  memDC.SetBrush(BrushColor);
  memDC.DrawRectangle(0, 0, size_x, size_y);
  inc = size_y / 127.0;
  for (j = Events.begin(); j != Events.end(); j++)
    if (((*j)->Position >= 0.0) && ((*j)->EndPosition > (*j)->Position))
      {
	z = size_y - (long) floor((*j)->Msg[1] * inc);
	memDC.DrawLine(GetXPos((*j)->Position), z, GetXPos((*j)->EndPosition), z);
      }
}


void					MidiPattern::OnPaint(wxPaintEvent &e)
{
  wxPaintDC				dc(this);
  wxSize				s = wxWindow::GetSize();
  wxRegionIterator			region;
 
  dc.SetPen(PenColor);
  dc.SetBrush(BrushColor);
  if (Bmp)
    for(region = GetUpdateRegion(); region; region++)
      dc.Blit(region.GetX(), region.GetY(),
	      region.GetW(), region.GetH(),
	      &memDC, region.GetX(), region.GetY(), 
	      wxCOPY, FALSE);
  else
    dc.DrawRectangle(0, 0, s.x, s.y);
  Pattern::DrawName(dc, s);
}

void				MidiPattern::Save()
{
  SaveElement*			saved;

  saved = new SaveElement(wxT("FileName"), _filename);
  saved->addAttribute(wxT("NoTrack"), (int)_noTrack);
  saveDocData(saved);
  Pattern::Save();
}

void				MidiPattern::Load(SaveElementArray data)
{
  int				i;

  for (i = 0; i < data.GetCount(); i++)
    {
      if (data[i]->getKey() == wxT("FileName"))
	{
	  MidiFile*	midiFile;
	  int		noTrack;

	  midiFile = new MidiFile(data[i]->getValue());
	  noTrack = data[i]->getAttributeInt(wxT("NoTrack"));
	  if (noTrack < midiFile->GetNumberOfTracks())
	    SetMidiTrack(midiFile->GetTrack(noTrack));
	  delete midiFile;
	  Pattern::Load(data);
	}
    }
}
