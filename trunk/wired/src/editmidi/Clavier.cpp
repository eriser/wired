#include <cmath>
#include "Clavier.h"
#include "Key.h"
#include "Sequencer.h"
#include "MidiPattern.h"
#include "../redist/Plugin.h"
#include "../sequencer/Track.h"
#include "../gui/SeqTrack.h"

Clavier::Clavier(wxWindow *parent, wxWindowID id, const wxPoint& pos,
		 const wxSize& size, long style, EditMidi *editmidi)
  : wxControl(parent, id, pos, size, style)
{
  em = editmidi;
  ZoomY = 1;
  bool				inter_2 = true;
  int				j = 0;
  int				posY = static_cast<int>(ceil(BLACKKEY_HEIGHT * 2 * ZoomY));
  int				posYW = static_cast<int>(ceil(BLACKKEY_HEIGHT * 3 * ZoomY / 2));
  int				wkeyh = static_cast<int>(ceil(BLACKKEY_HEIGHT * 3 * ZoomY / 2));
  wxString			gamme = wxT("CDEFGAB");
  int				oct = -2;
  int				note = 0;
  int				code = 0;
  int				whprog[7] = {2, 2, 1, 2, 2, 2, 1};
  int				blprog[5] = {2, 3, 2, 2, 3};

  // creation des touches blanches
  for (int i = NB_WHITEKEY; i > 0; i--)
    {
      wxString notestr = wxT("");
      notestr += gamme.GetChar(note);
      notestr = notestr << oct;
      keys.push_back(new Key(this, -1, wxPoint(size.GetWidth() - WHITEKEY_WIDTH, 
					       size.GetHeight() - posYW), wxSize(WHITEKEY_WIDTH, wkeyh),
			     FALSE, notestr, code));
      code += whprog[note++];
      if (note == 7)
	{
	  note = 0;
	  oct++;
	}
      if (inter_2)
	{
	  if (j != 0)
	    wkeyh = static_cast<int>(ceil(ZoomY * BLACKKEY_HEIGHT * 3 / 2));
	  else
	    wkeyh = static_cast<int>(ceil(ZoomY * BLACKKEY_HEIGHT * 2));
	  if (j == 2)
	    {
	      j = -1;
	      inter_2 = false;
	    }
	}
      else
	{
	  if ((j != 0) && (j != 1))
	    wkeyh = static_cast<int>(ceil(BLACKKEY_HEIGHT * 3 * ZoomY / 2));
	  else
	    wkeyh = static_cast<int>(ceil(ZoomY * BLACKKEY_HEIGHT * 2));
	  if (j == 3)
	    {
	      j = -1;
	      inter_2 = true;
	    }
	}
      posYW += wkeyh;
      j++;
    }
 
  // creation des touches noires
  j = 0;
  inter_2 = true;
  oct = -2;
  note = 0;
  code = 1;
int arf = 0;
  for (int i = NB_WHITEKEY; i > 1; i--)
    {
      wxString notestr = wxT("");
      notestr += gamme.GetChar(note);
      notestr += wxT("#");
      notestr = notestr << oct;
      note++;
      if (note == 7)
	{
	  note = 0;
	  oct++;
	}
      if (inter_2 && j == 2)
	{
	  inter_2 = false;
	  j = 0;
	}
      else
	{
	  if (!inter_2 && j == 3)
	    {
	      inter_2 = true;
	      j = 0;
	    }
	  else
	    {
	      keys.push_back(new Key (this, -1, wxPoint(size.GetWidth() - WHITEKEY_WIDTH, 
							size.GetHeight() - posY), wxSize(BLACKKEY_WIDTH, static_cast<int>(ceil(BLACKKEY_HEIGHT * ZoomY))), TRUE, notestr, code));

code += blprog[(arf++) % 5];
	      int fact = 2;
	      if (inter_2 && (j == 1))
		fact = 3;
	      if (!inter_2 && (j == 2))
		fact = 3;
	      posY += static_cast<int>(ceil(ZoomY * BLACKKEY_HEIGHT * fact));
	      j++;
	    }
	} 
    }
}

void				Clavier::RecalcKeyPos()
{
  bool				inter_2 = true;
  int				j = 0;
  int				posY = (int) ceil(BLACKKEY_HEIGHT * 2 * ZoomY);
  int				posYW = (int) ceil(BLACKKEY_HEIGHT * 3 * ZoomY / 2);
  int				wkeyh = (int) ceil(BLACKKEY_HEIGHT * 3 * ZoomY / 2);
  wxSize			size = GetSize();
  int				curkey = 0;

  // deplacement des touches blanches
  for (int i = NB_WHITEKEY; i > 0; i--)
    {
      keys[curkey++]->SetSize(size.GetWidth() - WHITEKEY_WIDTH, 
			      size.GetHeight() - posYW,
			      WHITEKEY_WIDTH, wkeyh);
      if (inter_2)
	{
	  if (j != 0)
	    wkeyh = (int) ceil(ZoomY * BLACKKEY_HEIGHT * 3 / 2);
	  else
	    wkeyh = (int) ceil(ZoomY * BLACKKEY_HEIGHT * 2);
	  if (j == 2)
	    {
	      j = -1;
	      inter_2 = false;
	    }
	}
      else
	{
	  if ((j != 0) && (j != 1))
	    wkeyh = (int) ceil(BLACKKEY_HEIGHT * 3 * ZoomY / 2);
	  else
	    wkeyh = (int) ceil(ZoomY * BLACKKEY_HEIGHT * 2);
	  if (j == 3)
	    {
	      j = -1;
	      inter_2 = true;
	    }
	}
      posYW += wkeyh;
      j++;
    }
 
  // creation des touches noires
  j = 0;
  inter_2 = true;
  for (int i = NB_WHITEKEY; i > 1; i--)
    {
      if (inter_2 && j == 2)
	{
	  inter_2 = false;
	  j = 0;
	}
      else
	{
	  if (!inter_2 && j == 3)
	    {
	      inter_2 = true;
	      j = 0;
	    }
	  else
	    {
	      keys[curkey++]->SetSize(size.GetWidth() - WHITEKEY_WIDTH,
				      size.GetHeight() - posY, BLACKKEY_WIDTH,
				      (int) ceil(BLACKKEY_HEIGHT * ZoomY));
	      int fact = 2;
	      if (inter_2 && (j == 1))
		fact = 3;
	      if (!inter_2 && (j == 2))
		fact = 3;
	      posY += (int) ceil(ZoomY * BLACKKEY_HEIGHT * fact);
	      j++;
	    }
	} 
    }
}

void				Clavier::SetZoomY(double pZoomY)
{
  /*
    ZoomY = pZoomY;
    em->mp->SetZoomY(pZoomY);
    em->ResizeClavier(GetSize().GetWidth(), (int) ceil(ZoomY * CLAVIER_HEIGHT));
    RecalcKeyPos();
  */
}

void				Clavier::OnPaint(wxPaintEvent &event)
{
  wxPaintDC			dc(this);
  wxSize			s = GetSize();
  int				j = -2;
  int				posY = (int) ceil(ZoomY * BLACKKEY_HEIGHT * 3 / 2);
  wxFont			f = dc.GetFont();

  // dessine le texte
  f.SetPointSize(6);
  dc.SetFont(f);
  for (int i = NB_WHITEKEY; i > 0; i = i - 7)
    {
      wxString str = wxT("C");
      str = str << j;
      dc.DrawText(str, s.GetWidth() - WHITEKEY_WIDTH - 20, 
		  s.GetHeight() - posY + 4);
      j++;
      posY += (int) ceil(12 * BLACKKEY_HEIGHT * ZoomY);
    } 
}

void				Clavier::OnKeyDown(wxMouseEvent &event)
{
  Key				*k = (Key *)event.GetEventObject();

  WiredEvent	midievent;
  midievent.Type = WIRED_MIDI_EVENT;
  midievent.NoteLength = 0;
  midievent.DeltaFrames = 0;
  midievent.MidiData[0] = 0x90;
  midievent.MidiData[1] = k->code;
  midievent.MidiData[2] = 100;
  if (Seq->Tracks[em->midi_pattern->GetTrackIndex()]->TrackOpt->Connected)
    Seq->Tracks[em->midi_pattern->GetTrackIndex()]->TrackOpt->Connected->ProcessEvent(midievent);
}

void				Clavier::OnKeyUp(wxMouseEvent &event)
{
  Key				*k = (Key *)event.GetEventObject();
  WiredEvent			midievent;

  midievent.Type = WIRED_MIDI_EVENT;
  midievent.NoteLength = 0;
  midievent.DeltaFrames = 0;
  midievent.MidiData[0] = 0x90;
  midievent.MidiData[1] = k->code;
  midievent.MidiData[2] = 0;
  if (Seq->Tracks[em->midi_pattern->GetTrackIndex()]->TrackOpt->Connected)
    Seq->Tracks[em->midi_pattern->GetTrackIndex()]->TrackOpt->Connected->ProcessEvent(midievent);
}

BEGIN_EVENT_TABLE(Clavier, wxControl)
  EVT_PAINT(Clavier::OnPaint)
  EVT_LEFT_DOWN(Clavier::OnKeyDown)
  EVT_LEFT_UP(Clavier::OnKeyUp)
END_EVENT_TABLE()
