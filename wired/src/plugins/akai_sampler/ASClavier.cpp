// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <cmath>
#include "ASClavier.h"
#include "ASKey.h"
#include "AkaiSampler.h"

  BEGIN_EVENT_TABLE(ASClavier, wxControl)
  EVT_PAINT(ASClavier::OnPaint)
  EVT_LEFT_DOWN(ASClavier::OnKeyDown)
  EVT_LEFT_UP(ASClavier::OnKeyUp)
END_EVENT_TABLE()

ASClavier::ASClavier(wxWindow *parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style, class AkaiSampler *as)
: wxControl(parent, id, pos, size, style)
{
  bool	inter_2 = true;
  int		j = 0;
  int		posY = BLACKASKEY_WIDTH * 3 / 2 - BLACKASKEY_WIDTH / 2;
  int		posYW = 0; 
  int		wkeyh = BLACKASKEY_WIDTH * 3 / 2;
  wxString	gamme = _("CDEFGAB");
  int		oct = 0; //-2;
  int		note = 0;
  int		code = 24; //0;
  int		whprog[7] = {2, 2, 1, 2, 2, 2, 1};
  int		blprog[5] = {2, 3, 2, 2, 3};

  this->as = as;
  // creation des touches blanches
  for (int i = NB_WHITEASKEY; i > 0; i--)
  {
    wxString notestr = wxT("");
    notestr += gamme.GetChar(note);
    notestr = notestr << oct;
    keys.push_back(new ASKey(this, -1, wxPoint(posYW, 
            size.GetHeight() - WHITEASKEY_HEIGHT ), wxSize(wkeyh, WHITEASKEY_HEIGHT),
          FALSE, notestr, code));
    code += whprog[note++];
    if (note == 7)
    {
      note = 0;
      oct++;
    }
    posYW += wkeyh;
    if (inter_2)
    {
      if (j != 0)
        wkeyh = BLACKASKEY_WIDTH * 3 / 2;
      else
        wkeyh = BLACKASKEY_WIDTH * 2;
      if (j == 2)
      {
        j = -1;
        inter_2 = false;
      }
    }
    else
    {
      if ((j != 0) && (j != 1))
        wkeyh = BLACKASKEY_WIDTH * 3  / 2;
      else
        wkeyh = BLACKASKEY_WIDTH * 2;
      if (j == 3)
      {
        j = -1;
        inter_2 = true;
      }
    }
    j++;
  }

  // creation des touches noires
  j = 0;
  inter_2 = true;
  oct = 0; //-2;
  note = 0;
  code = 25; //1;
  int arf = 0;
  for (int i = NB_WHITEASKEY; i > 1; i--)
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
        keys.push_back(new ASKey (this, -1, wxPoint(posY, 
                size.GetHeight() - WHITEASKEY_HEIGHT), wxSize(BLACKASKEY_WIDTH, BLACKASKEY_HEIGHT), TRUE, notestr, code));

        code += blprog[(arf++) % 5];
        int fact = 2;
        if (inter_2 && (j == 1))
          fact = 3;
        if (!inter_2 && (j == 2))
          fact = 3;
        posY += BLACKASKEY_WIDTH * fact;
        j++;
      }
    } 
  }
}

void	ASClavier::OnPaint(wxPaintEvent &event)
{
  wxPaintDC	dc(this);
  wxSize s = GetSize();
  int j = 0; //-2;
  int posY = 0;
  wxFont f = dc.GetFont();

  // dessine le texte
  f.SetPointSize(6);
  dc.SetFont(f);
  for (int i = NB_WHITEASKEY; i > 0; i = i - 7)
  {
    wxString str = _("C");
    str = str << j;
    dc.DrawText(str, posY + (BLACKASKEY_WIDTH * 3 / 2 - 6) / 2, s.GetHeight() - WHITEASKEY_HEIGHT - 10); 
    j++;
    posY += 12 * BLACKASKEY_WIDTH;
  } 
}

void	ASClavier::OnKeyDown(wxMouseEvent &event)
{
  if (!as)
  {
    wxMouseEvent ev(wxEVT_LEFT_DOWN);
    ev.SetEventObject(event.GetEventObject());
    GetParent()->GetParent()->ProcessEvent(ev);
  }
  else
  {
    ASKey *k = (ASKey *)event.GetEventObject();
    WiredEvent	midievent;
    midievent.Type = WIRED_MIDI_EVENT;
    midievent.NoteLength = 0;
    midievent.DeltaFrames = 0;
    midievent.MidiData[0] = 0x90;
    midievent.MidiData[1] = k->code;
    midievent.MidiData[2] = 100;
    //printf("[ASClavier] Sending 0x%02X 0X%02X 0X%02X\n", midievent.MidiData[0], midievent.MidiData[1], midievent.MidiData[2]);
    as->ProcessEvent(midievent);
  }
}

void	ASClavier::OnKeyUp(wxMouseEvent &event)
{
  if (!as)
  {
    wxMouseEvent ev(wxEVT_LEFT_UP);
    ev.SetEventObject(event.GetEventObject());
    GetParent()->GetParent()->ProcessEvent(ev);
  }
  else
  {
    ASKey *k = (ASKey *)event.GetEventObject();
    WiredEvent	midievent;
    midievent.Type = WIRED_MIDI_EVENT;
    midievent.NoteLength = 0;
    midievent.DeltaFrames = 0;
    midievent.MidiData[0] = 0x90;
    midievent.MidiData[1] = k->code;
    midievent.MidiData[2] = 0;
    //printf("[ASClavier] Sending 0x%02X 0X%02X 0X%02X\n", midievent.MidiData[0], midievent.MidiData[1], midievent.MidiData[2]);
    as->ProcessEvent(midievent);
  }
}
