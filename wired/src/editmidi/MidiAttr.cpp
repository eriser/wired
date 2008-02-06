// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include "MidiFile.h"
#include "MidiAttr.h"
#include "MidiPart.h"
#include "Clavier.h"
#include  "SequencerGui.h"

#ifdef DEBUG_MIDIATTR
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : " << __LINE__ << " : " << __FUNCTION__ << endl; }
#else
#define LOG
#endif

MidiAttr::MidiAttr(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                   const wxSize& size, long style):
        wxPanel(parent, id, pos, size, style), m_controller(-1)
{
  _magnetH = SeqPanel->GetMagnetismeValue();
  _magnetV = true;
}

void				MidiAttr::SetNotes(vector <Note *> recnote)
{
    this->Notes = recnote;
    this->ZoomX = 1;
}

void MidiAttr::SetController(int controller)
{
    LOG;
    m_controller = controller;
    this->Refresh(true);
}

void				MidiAttr::OnPaint(wxPaintEvent &e)
{
  ReDraw();
}

inline bool sort_event_by_pos(ControlChange *a, ControlChange *b)
{
  	return (a->GetPos() <= b->GetPos());
}

void				MidiAttr::ReDraw()
{
    LOG;
    wxPaintDC			dc(this);
    int oldx = 0;
    int oldy = -1;

    if (m_controller >= 0) // Control Change edit mode
    {
        dc.Clear();
	stable_sort(ControlChanges.begin(), ControlChanges.end(), sort_event_by_pos);
        for (vector<ControlChange *>::iterator cc_it = ControlChanges.begin(); cc_it != ControlChanges.end(); cc_it++)
        {
            ControlChange* cc = (*cc_it);
            if (cc && (cc->GetController() == m_controller))
            {
                dc.SetPen(wxPen(wxColor(0x00, cc->GetValue() + 75, 0x00)));
                dc.SetBrush(wxBrush(wxColor(0x00, cc->GetValue() + 128, 0x00)));
                int x = (int)floor(cc->GetPos() * 4 * ROW_WIDTH * ZoomX);
                int perc_height = (cc->GetValue() * GetSize().GetHeight()) / 128;
                int y = GetSize().GetHeight() - perc_height;
		if (oldy == -1)
		  oldy = y;
                //dc.DrawRectangle(x, y, 1, perc_height);
		dc.DrawLine(oldx, oldy, x, oldy);
		dc.DrawLine(x, oldy, x, y);
		oldx = x;
		oldy = y;
            }
        }
	//dc.SetPen(wxPen(wxColor(0x00, cc->GetValue() + 75, 0x00)));
	//dc.SetBrush(wxBrush(wxColor(0x00, cc->GetValue() + 128, 0x00)));
	dc.DrawLine(oldx, oldy, GetSize().GetWidth(), oldy);
    }
    else // Velocity edit mode
    {
        dc.Clear();
        for (vector<Note *>::iterator note_it = Notes.begin(); note_it != Notes.end(); note_it++)
        {
            Note* note = (*note_it);
            if (note)
            {
                int x = (int)floor(note->GetPos() * 4 * ROW_WIDTH * ZoomX);
                int perc_height = (note->GetVelocity() * GetSize().GetHeight()) / 128;
                int y = GetSize().GetHeight() - perc_height;

                dc.SetPen(wxPen(wxColor(note->GetVelocity() + 75, 0x00, 0x00)));
                dc.SetBrush(wxBrush(wxColor(note->GetVelocity() + 128, 0x00, 0x00)));
                dc.DrawRectangle(x, y, 5, perc_height);
            }
        }
    }
}

void MidiAttr::OnMouseMove(wxMouseEvent &e)
{
    if (e.LeftIsDown())
        UpdateController(e);
}

void MidiAttr::OnLeftUp(wxMouseEvent &e)
{
    UpdateController(e);
}

void MidiAttr::OnLeftDown(wxMouseEvent &e)
{
    UpdateController(e);
}

void MidiAttr::UpdateController(wxMouseEvent &e)
{
    LOG;
    if (m_controller >= 0) // Control Change edit mode
    {
      int msg[3];
      msg[0] = ME_CTRLCHANGE;
      msg[1] = m_controller;
      msg[2] = ((GetSize().GetHeight() - e.GetY()) * 128) / GetSize().GetHeight() - 1;
      double start_position = e.GetX() / (ROW_WIDTH * 4 * ZoomX);
      //double start_position_before = (e.GetX() - 1) / (ROW_WIDTH * 4 * ZoomX);
      double start_position_after = (e.GetX() + 1) / (ROW_WIDTH * 4 * ZoomX);
      start_position = floor(start_position * (double)pattern->GetPPQN()) / ((double)pattern->GetPPQN());
      if (_magnetV)
      {
	double padding = start_position_after - start_position;
	start_position = floor(start_position * _magnetH) / _magnetH;
	start_position_after = start_position + padding;
      }
      MidiEvent *evt = NULL;

      for (vector<MidiEvent*>::iterator event_ite = pattern->Events.begin(); event_ite != pattern->Events.end() ; event_ite++)
	if ((*event_ite)->Position >= start_position && (*event_ite)->Position < start_position_after)// && (*event_ite)->Position > start_position_before)
	{
	  evt = (*event_ite);
	  for (int i = 0; i < 3; i++)
	    evt->Msg[i] = msg[i];
	  evt->EndPosition = evt->Position;
	}
      if (!evt)
      {
	evt = new MidiEvent(0, start_position, msg);
	evt->EndPosition = evt->Position;
	pattern->Events.push_back(evt);
      }
      ControlChange *controlChange = new ControlChange(pattern, pattern->Events.size() - 1);
      ControlChanges.push_back(controlChange);
      Refresh(true);
      this->Refresh(true);
      pattern->SetToWrite();
    }
    else // Velocity edit mode
    {
        for (vector<Note *>::iterator note_it = Notes.begin(); note_it != Notes.end(); note_it++)
        {
            Note* note = (*note_it);
            if (note)
            {
                int x = (int)floor(note->GetPos() * 4 * ROW_WIDTH * ZoomX);
                if (e.GetX() >= x && e.GetX() <= (x + 5))
                {
                    int velocity = ((GetSize().GetHeight() - e.GetY()) * 128) / GetSize().GetHeight() - 1;
                    note->SetVelocity(velocity);
                    this->Refresh(true);
                }
            }
        }
    }
}

void				MidiAttr::OnResize(wxSizeEvent &e)
{
  ReDraw();
  this->Refresh(true);
}

void					MidiAttr::SetMidiPattern(MidiPattern *p)
{
  ControlChanges.clear();
  for (unsigned int i = 0; i < p->Events.size(); i++)
	  if (IS_ME_CTRLCHANGE(p->Events[i]->Msg[0]))
	    ControlChanges.push_back(new ControlChange(p, i));
//  ChangeMesureCount((int)ceil(p->GetEndPosition()));
  pattern = p;
}

BEGIN_EVENT_TABLE(MidiAttr, wxPanel)
    EVT_PAINT(MidiAttr::OnPaint)
    EVT_MOTION(MidiAttr::OnMouseMove)
    EVT_LEFT_UP(MidiAttr::OnLeftUp)
    EVT_LEFT_DOWN(MidiAttr::OnLeftDown)
    EVT_SIZE(MidiAttr::OnResize)
END_EVENT_TABLE()

