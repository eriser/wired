// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include <vector>
#include "SequencerGui.h"
#include "Sequencer.h"
#include "Colour.h"
#include "OptionPanel.h"
#include "ColoredBox.h"
#include "MidiPattern.h"
#include "HelpPanel.h"
#include "../midi/midi.h"
#include "../midi/MidiFile.h"
#include "../sequencer/Track.h"

BEGIN_EVENT_TABLE(MidiPattern, wxWindow)
  EVT_PAINT(MidiPattern::OnPaint)
END_EVENT_TABLE()


static int midi_pattern_count = 1;

MidiPattern::MidiPattern(double pos, double endpos, long trackindex)
  : Pattern(pos, endpos, trackindex)
{
  Init();
}

MidiPattern::MidiPattern(double pos, MidiTrack *t, long trackindex)
  : Pattern(pos, ((double)t->GetMaxPos()) / (Seq->SigNumerator * t->GetPPQN()), trackindex)
{
  vector<MidiFileEvent *>		me;
  unsigned long				i;

  Init();
  ppqn = t->GetPPQN();
  temp.clear();
  me = t->GetMidiEvents();
  for (i = 0; i < me.size(); i++)
    AddEvent(me[i]);    
}

MidiPattern::~MidiPattern()
{

}

void					MidiPattern::Init()
{
  wxString	s;

  s.Printf("T%d M%d", TrackIndex + 1, midi_pattern_count++);
  PenColor = CL_MIDI_DRAW;
  BrushColor = CL_MIDIDRAWER_BRUSH;
  Name = s.c_str();
  ppqn = 1;
  Bmp = 0x0;
  Connect(GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnMotion);
  Connect(GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnClick);
  Connect(GetId(), wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnRightClick);
  Connect(GetId(), wxEVT_LEFT_DCLICK, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &MidiPattern::OnDoubleClick);
  Connect(GetId(), wxEVT_ENTER_WINDOW, (wxObjectEventFunction)(wxEventFunction)
	  (wxMouseEventFunction) &MidiPattern::OnHelp);
  SeqPanel->PutCursorsOnTop();
}

void					MidiPattern::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("This is a MIDI pattern. Double-click on it to open the MIDI editor.");
      HelpWin->SetText(s);
    }
}

void					MidiPattern::Update()
{
  Pattern::Update();
  wxWindow::SetSize(Pattern::GetSize());
  wxWindow::SetPosition(Pattern::GetMPosition());
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

  p = new MidiPattern(pos, EndPosition, TrackIndex);
  p->Events = Events;
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
  MidiPattern				*p;
  double				d;
  vector<MidiEvent *>::iterator		o;

  Pattern::OnClick(e);
  if (SeqPanel->Tool == ID_TOOL_SPLIT_SEQUENCER)
    {
      d = (double) ((SeqPanel->CurrentXScrollPos 
		     + Pattern::GetMPosition().x + e.m_x)
		    / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor));
#ifdef __DEBUG__
      cout << " >>> HERE OLD:\n\t Position = " << Position << "\n\t Length = " << Length << "\n\t EndPosition = " << EndPosition << endl;
      cout << "new pos: " << d << endl;
#endif
      p = new MidiPattern(d, EndPosition, TrackIndex);
#ifdef __DEBUG__
      cout << " >>> HERE NEW :\n\t p->Position = " << p->Position << "\n\t p->Length = " << p->Length << "\n\t p->EndPosition = " << p->EndPosition << endl;
#endif
      for (o = Events.begin(); o != Events.end(); )
	if ((*o)->Position >= d)
	  {
#ifdef __DEBUG__
	    printf("Moving event (position %d) \n", (*o)->Position);
#endif
	    (*o)->Position -= d;
	    p->Events.push_back(*o);
	    Events.erase(o);
	  }
	else
	  {
	    if (((*o)->Position >= 0) && (*o)->EndPosition >= d)
	      (*o)->EndPosition = d;
	    o++;
	  }
      p->DrawMidi();
      SeqMutex.Lock();
      Length = (EndPosition = d) - Position;
      Update();
      SeqMutex.Unlock();
      Seq->Tracks[TrackIndex]->AddPattern(p);
      Refresh();
    }
  else
    if (SeqPanel->Tool == ID_TOOL_PAINT_SEQUENCER)
      SetDrawColour(SeqPanel->ColorBox->GetColor());
}

void					MidiPattern::OnLeftUp(wxMouseEvent &e)
{
  Pattern::OnLeftUp(e);
}

void					MidiPattern::OnDoubleClick(wxMouseEvent &e)
{
  Pattern::OnDoubleClick(e);
  OnClick(e);
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

void					MidiPattern::AddEvent(MidiEvent *event)
{
  list<MidiEvent *>::iterator		j;
  MidiEvent				*e;
  
  if ((event->EndPosition == 0.0) && 
      ((event->Msg[0] == M_NOTEON1) || (event->Msg[0] == M_NOTEON2)))
    {
      // Gestion du NOTE OFF
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
    if ((*j)->EndPosition >= 0.0)
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

