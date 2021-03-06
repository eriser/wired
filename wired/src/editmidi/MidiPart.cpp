// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include "MidiPart.h"
#include "Clavier.h"
#include "RulerMidi.h"
#include "MidiAttr.h"
#include "Note.h"
#include "EditNote.h"
#include "MidiPattern.h"
#include "MidiFile.h"
#include "SequencerGui.h"

MidiPart::MidiPart(wxWindow *parent, wxWindowID id, const wxPoint& pos,
		   const wxSize& size, long style, EditMidi *editmidi) :
  wxControl(parent, id, pos, size, style)
{
  _vertMagnet = true;
  _magnetH = SeqPanel->GetMagnetismeValue();
  _magnetV = SeqPanel->GetMagnetisme();
  em = editmidi;
  ZoomX = 1;
  ZoomY = 1;
  NPM = 4;
  NbRows = NB_ROWS;
  NbMesures = 24;
  BlackKeys.push_back(false);
  BlackKeys.push_back(true);
  BlackKeys.push_back(false);
  BlackKeys.push_back(true);
  BlackKeys.push_back(false);
  BlackKeys.push_back(false);
  BlackKeys.push_back(true);
  BlackKeys.push_back(false);
  BlackKeys.push_back(true);
  BlackKeys.push_back(false);
  BlackKeys.push_back(true);
  BlackKeys.push_back(false);
  selected = NULL;
  tool = ID_TOOL_MOVE_MIDIPART;
  selected2 = NULL;
  pattern = NULL;
}

void					MidiPart::SetZoomX(double pZoomX)
{
  ZoomX = pZoomX;
  em->rm->SetZoomX(pZoomX);
  em->ma->SetZoomX(pZoomX);
  em->ResizeMidiPart((int)ceil(ZoomX * NbMesures * 4 * ROW_WIDTH),
                     GetSize().GetHeight());

  //em->ma->SetNotes(Notes);
  em->ma->Refresh(true);
  em->Refresh();
  SeqPanel->UpdateMidiPattern(em->midi_pattern);

  if (selected != NULL)
    {
      selected->SetZoomX(pZoomX);
    }
}

void					MidiPart::SetZoomY(double pZoomY)
{
  ZoomY = pZoomY;
}

void					MidiPart::SetNPM(int pNPM)
{
  NPM = pNPM;
  em->ResizeMidiPart((int)ceil(ZoomX * NbMesures * 4 * ROW_WIDTH),
                     GetSize().GetHeight());
}

int					MidiPart::GetNPM()
{
  return (NPM);
}

void					MidiPart::ChangeMesureCount(int NbMes)
{
  cout << "[MIDIPART] Changed Mesure Count to " << NbMes << endl;
  NbMesures = NbMes;
  em->ResizeMidiPart((int)ceil(ZoomX * NbMesures * 4 * ROW_WIDTH),
                     GetSize().GetHeight());
}

void					MidiPart::SetMidiPattern(MidiPattern *p)
{
  Notes.clear();
  for (unsigned int i = 0; i < p->Events.size(); i++)
	  if (IS_ME_NOTEON(p->Events[i]->Msg[0]) && (p->Events[i]->Msg[2] != 0))
	    Notes.push_back(new Note(p, i));
  ChangeMesureCount((int)ceil(p->GetEndPosition()));
  pattern = p;
  em->ma->SetNotes(Notes);
  if (selected != NULL)
    delete selected;
  selected = NULL;
}

void					MidiPart::OnPaint(wxPaintEvent &e)
{
  static wxPen				greyPen(wxColor(0x80, 0x80, 0x80), 1);
  static wxPen				whitePen(wxColor(0x00, 0x00, 0x00), 3);
  static wxPen				purplePen(wxColor(0xD6, 0xD6, 0xD6));
  static wxPen				LightPurplePen(wxColor(0xE6, 0xE6, 0xE6));
  static wxBrush			purpleBrush(wxColor(0xD6, 0xD6, 0xD6));
  static wxBrush			LightPurpleBrush(wxColor(0xE6, 0xE6, 0xE6));
  static wxColor			noteColors[12] =
    {
      wxColor(0x00, 0x00, 0xEF),
      wxColor(0x00, 0x00, 0xF7),
      wxColor(0x10, 0x10, 0xF7),
      wxColor(0x20, 0x20, 0xFF),
      wxColor(0x30, 0x30, 0xFF),
      wxColor(0x40, 0x40, 0xFF),
      wxColor(0x50, 0x50, 0xFF),
      wxColor(0x60, 0x60, 0xFF),
      wxColor(0x70, 0x70, 0xFF),
      wxColor(0x80, 0x80, 0xFF),
      wxColor(0x90, 0x90, 0xFF),
      wxColor(0xA0, 0xA0, 0xFF)
    };
  static wxBrush			noteBrushes[12] =
    {
      wxBrush(noteColors[ 0]),
      wxBrush(noteColors[ 1]),
      wxBrush(noteColors[ 2]),
      wxBrush(noteColors[ 3]),
      wxBrush(noteColors[ 4]),
      wxBrush(noteColors[ 5]),
      wxBrush(noteColors[ 6]),
      wxBrush(noteColors[ 7]),
      wxBrush(noteColors[ 8]),
      wxBrush(noteColors[ 9]),
      wxBrush(noteColors[10]),
      wxBrush(noteColors[11])
    };
  static wxPen				notePens[12] =
    {
      wxPen(noteColors[ 0]),
      wxPen(noteColors[ 1]),
      wxPen(noteColors[ 2]),
      wxPen(noteColors[ 3]),
      wxPen(noteColors[ 4]),
      wxPen(noteColors[ 5]),
      wxPen(noteColors[ 6]),
      wxPen(noteColors[ 7]),
      wxPen(noteColors[ 8]),
      wxPen(noteColors[ 9]),
      wxPen(noteColors[10]),
      wxPen(noteColors[11])
    };
  wxPaintDC				dc(this);
  wxRegionIterator			upd(GetUpdateRegion());
  while (upd)
    {
      int fr = upd.GetY() / (int)ceil(ZoomY * ROW_HEIGHT);
      int nr = upd.GetH() / (int)ceil(ZoomY * ROW_HEIGHT) + 2;
      int fc = upd.GetX() / (int)ceil(ZoomX * ROW_WIDTH);
      int nc = upd.GetW() / (int)ceil(ZoomX * ROW_WIDTH) + 2;
      dc.SetPen(greyPen);
      for (int i = 0; i < nr; i++)
	{
	  if (BlackKeys[(NbRows - (fr + i) - 1) % 12])
	    dc.SetBrush(purpleBrush);
	  else
	    dc.SetBrush(LightPurpleBrush);
	  dc.DrawRectangle((int)floor(ZoomX * fc * ROW_WIDTH),
			   (int)floor(ZoomY * (fr + i) * ROW_HEIGHT),
			   (int)ceil(ZoomX * nc * ROW_WIDTH),
			   (int)ceil(ZoomY * ROW_HEIGHT));
	}
      for (unsigned int i = 0; i < Notes.size(); i++)
	{
	  wxRect a(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH());
	  wxRect b((int)floor(Notes[i]->GetPos() * 4 * ROW_WIDTH * ZoomX),
		   (int)floor((127 - Notes[i]->GetNote()) * ROW_HEIGHT * ZoomY) + 1,
		   (int)ceil(Notes[i]->GetDuration() * 4 * ROW_WIDTH * ZoomX),
		   (int)ceil(ROW_HEIGHT * ZoomY) - 2);
	  wxRect *rect = CalcIntersection(a, b);
	  if (rect != NULL)
	    {
	      dc.SetPen(notePens[(127 - Notes[i]->GetNote()) % 12]);
	      dc.SetBrush(noteBrushes[(127 - Notes[i]->GetNote()) % 12]);
	      dc.DrawRectangle(rect->x, rect->y, rect->width, rect->height);
	      delete rect;
	    }

	}
      //int incx = (int)ceil(ZoomX * ROW_WIDTH);
      double incx = (ZoomX * ROW_WIDTH);
      if (fc == 0)
	fc++;
      int mesx = (int)ceil(fc * ZoomX * ROW_WIDTH);
      int j;
      double i;
      for (i = mesx, j = (fc % 4); (int)i < upd.GetX() + upd.GetW() + 10; i += incx, j++)
	{
	  if (j % 4)
	    dc.SetPen(greyPen);
	  else
	    dc.SetPen(whitePen);
	  dc.DrawLine((int)i, (int)floor(ZoomY * fr * ROW_HEIGHT),
		      (int)i, (int)ceil(ZoomY * (fr + nr) * ROW_HEIGHT));
	}
      upd++;
    }
}

wxRect					*CalcIntersection(wxRect &a, wxRect &b)
{
  int					x = (a.x < b.x) ? b.x : a.x;
  int					y = (a.y < b.y) ? b.y : a.y;
  int					width = ((a.x + a.width) < (b.x + b.width)) ?
    (a.x + a.width - x) : (b.x + b.width - x);
  int					height = ((a.y + a.height) < (b.y + b.height)) ?
    (a.y + a.height - y) : (b.y + b.height - y);
  return (((width < 0) || (height < 0)) ?
	  NULL : new wxRect(x, y, width, height));
}

void					MidiPart::OnMouseMove(wxMouseEvent &e)
{
  if (e.LeftIsDown())
  {
    if (selected2)
    {
      double nx = (selected2->GetPos() * 4 * ROW_WIDTH * ZoomX);
      if (e.GetX() - nx > 0)
      {
	double duration = (e.GetX() - nx) / (ROW_WIDTH * 4 * ZoomX);
	if (_magnetV)
	  duration = ceil(duration * _magnetH) / _magnetH;
	selected2->SetDuration(duration);
	Refresh(true);
	em->ma->SetNotes(Notes);
	em->ma->Refresh(true);

	SeqPanel->UpdateMidiPattern(em->midi_pattern);
      }
    }

    // mouse still on the same note ?
    if (_vertMagnet)
      if (lastOne != 127 - e.GetY() / ROW_HEIGHT)
      {
	OnReleaseClick(e);
	OnClick(e);
      }
  }
}

void					MidiPart::OnReleaseClick(wxMouseEvent &e)
{
    switch (tool)
    {
      case ID_TOOL_MOVE_MIDIPART:
	break;
      case ID_TOOL_EDIT_MIDIPART:
	if (selected2)
	{
	  double nx = (selected2->GetPos() * 4 * ROW_WIDTH * ZoomX);
	  if (e.GetX() - nx >= 0)
	  {
	    double mouse_duration = (e.GetX() - nx) / (ROW_WIDTH * 4 * ZoomX);
	    double duration = mouse_duration;
	    if (_magnetV)
	    {
	      duration = ceil(mouse_duration * _magnetH) / _magnetH;
	      if (duration <= 0)
		duration = floor(mouse_duration * _magnetH + 1) / _magnetH;
	    }

	    selected2->SetDuration(duration);
	    // assuming Events.end() is the last ME_NOTEOFF set by OnClick()
	    pattern->Events.pop_back();
	    // getting the corresponding ME_NOTEON
	    MidiEvent *evt_save = pattern->Events[pattern->Events.size() - 1];
	    pattern->Events.pop_back();

	    int msg[3];
	    msg[0] = ME_NOTEOFF;
	    // getting the original note from e.GetY() at OnClick()
	    msg[1] = lastOne; // = 127 - e.GetY() / ROW_HEIGHT;
	    msg[2] = 0;
	    MidiEvent *evt;
	    double mouse_end_pos = e.GetX() / (ROW_WIDTH * 4 * ZoomX);
	    double end_pos = mouse_end_pos;
	    if (_magnetV)
	    {
	      end_pos = ceil(mouse_end_pos * _magnetH) / _magnetH;
	      if (end_pos == 0)
		end_pos = floor(mouse_end_pos * _magnetH + 1) / _magnetH;
	    }
	    else
	      if (end_pos == 0)
	      {
		// assuming Events.end() is now the last ME_NOTEON set by OnClick()
		cout << "[MidiPart] OnReleaseClick() : note with null duration not added" << endl;
		pattern->Events.pop_back();
		selected2 = NULL;
		return;
	      }
	    evt = new MidiEvent(0, end_pos, msg);
	    evt_save->EndPosition = end_pos;
	    pattern->Events.push_back(evt_save);
	    pattern->Events.push_back(evt);
	    Refresh(true);
	    em->ma->SetNotes(Notes);
	    em->ma->Refresh(true);

	    SeqPanel->UpdateMidiPattern(em->midi_pattern);
	  }
	  selected2 = NULL;
	}
	break;
      case ID_TOOL_DEL_MIDIPART:
	break;
    }
  
}

void					MidiPart::OnClick(wxMouseEvent &e)
{
  pattern->SetToWrite();
  for (vector<Note *>::iterator i = Notes.begin(); i < Notes.end(); i++)
  {
    wxRect b((int)floor((*i)->GetPos() * 4 * ROW_WIDTH * ZoomX),
	(int)floor((127 - (*i)->GetNote()) * ROW_HEIGHT * ZoomY),
	(int)ceil((*i)->GetDuration() * 4 * ROW_WIDTH * ZoomX),
	(int)ceil(ROW_HEIGHT * ZoomY));
    if ((e.GetX() >= b.x) && (e.GetX() <= (b.x + b.width)) &&
	(e.GetY() >= b.y) && (e.GetY() <= (b.y + b.height)))
    {
      switch (tool)
      {
	case ID_TOOL_MOVE_MIDIPART:
	  if (selected != NULL)
	  {
	    delete selected;
	    Refresh(true);
	  }
	  selected = new EditNote(this, -1, wxPoint(b.x, b.y),
	      wxSize(b.width, b.height), *i);
	  selected->SetZoomX(ZoomX);

	  return ;
	  break;
	case ID_TOOL_EDIT_MIDIPART:
	  if (!selected2)
	  {
	    selected2 = (*i);
	  }
	  if (selected2 == *i)
	  {
	    if (e.GetX() - b.x > 0)
	    {
	      long duration = (long)((e.GetX() - b.x) / (ROW_WIDTH * 4 * ZoomX));
	      selected2->SetDuration(duration);
	      Refresh(true);
	      em->ma->SetNotes(Notes);
	      em->ma->Refresh(true);

	      SeqPanel->UpdateMidiPattern(em->midi_pattern);
	    }
	  }
	  return ;
	  break;
	case ID_TOOL_DEL_MIDIPART:
	  (*i)->Erase();
	  Notes.erase(i);
	  Refresh(true);
	  em->ma->SetNotes(Notes);
	  em->ma->Refresh(true);

	  SeqPanel->UpdateMidiPattern(em->midi_pattern);
	  return ;
	  break;
      }
    }
  }
  if (tool == ID_TOOL_MOVE_MIDIPART)
  {
    if (selected != NULL)
    {
      delete selected;
      Refresh(true);
    }
    selected = NULL;
    SeqPanel->UpdateMidiPattern(em->midi_pattern);
  }
  else
    if (tool == ID_TOOL_EDIT_MIDIPART)
    {
      int msg[3];
      msg[0] = ME_NOTEON;
      msg[1] = 127 - e.GetY() / ROW_HEIGHT;
      lastOne = msg[1];
      msg[2] = 64;
      double start_position = e.GetX() / (ROW_WIDTH * 4 * ZoomX);
      if (_magnetV)
	start_position = floor(start_position * _magnetH) / _magnetH;
      double duration = .25 / 4;
      if (_magnetV)
	duration = 1 / _magnetH;
      MidiEvent *evt = new MidiEvent(0, start_position, msg);
      evt->EndPosition = evt->Position + duration;
      pattern->Events.push_back(evt);
      Note *note = new Note(pattern, pattern->Events.size() - 1);
      Notes.push_back(note);
      selected2 = note;
      msg[0] = ME_NOTEOFF;
      msg[2] = 0;
      selected2->SetDuration(duration);
      evt = new MidiEvent(0, evt->EndPosition, msg);
      pattern->Events.push_back(evt);
/*
 *      Refresh(true);
 *      em->ma->SetNotes(Notes);
 *      em->ma->Refresh(true);
 *
 *      SeqPanel->UpdateMidiPattern(em->midi_pattern);
 */
    }
}

void					MidiPart::SetTool(int numtool)
{
  tool = numtool;
  if (selected != NULL)
    delete selected;
  selected = NULL;
  switch (tool)
    {
    case ID_TOOL_EDIT_MIDIPART:
      SetCursor(wxCURSOR_PENCIL);
      break;
    case ID_TOOL_MOVE_MIDIPART:
      SetCursor(wxNullCursor);
      break;
    case ID_TOOL_DEL_MIDIPART:
      SetCursor(wxCURSOR_BULLSEYE);
      break;
    }
}

BEGIN_EVENT_TABLE(MidiPart, wxControl)
  EVT_PAINT(MidiPart::OnPaint)
  EVT_LEFT_DOWN(MidiPart::OnClick)
  EVT_LEFT_UP(MidiPart::OnReleaseClick)
  EVT_MOTION(MidiPart::OnMouseMove)
END_EVENT_TABLE()

