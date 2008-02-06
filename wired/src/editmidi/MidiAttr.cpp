// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include "MidiFile.h"
#include "MidiAttr.h"
#include "MidiPart.h"
#include "Clavier.h"

#ifdef DEBUG_MIDIATTR
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : " << __LINE__ << " : " << __FUNCTION__ << endl; }
#else
#define LOG
#endif

MidiAttr::MidiAttr(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                   const wxSize& size, long style):
        wxPanel(parent, id, pos, size, style), m_controller(-1)
{}

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

void				MidiAttr::ReDraw()
{
    LOG;
    wxPaintDC			dc(this);

    if (m_controller >= 0) // Control Change edit mode
    {
        dc.Clear();
        for (vector<ControlChange *>::iterator cc_it = ControlChanges.begin(); cc_it != ControlChanges.end(); cc_it++)
        {
            ControlChange* cc = (*cc_it);
            if (cc && (cc->GetController() == m_controller))
            {
                int x = (int)floor(cc->GetPos() * 4 * ROW_WIDTH * ZoomX);
                int perc_height = (cc->GetValue() * GetSize().GetHeight()) / 128;
                int y = GetSize().GetHeight() - perc_height;

                dc.SetPen(wxPen(wxColor(0x00, cc->GetValue() + 75, 0x00)));
                dc.SetBrush(wxBrush(wxColor(0x00, cc->GetValue() + 128, 0x00)));
                dc.DrawRectangle(x, y, 1, perc_height);
            }
        }
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
      /*
      if (_magnetV)
        start_position = floor(start_position * _magnetH) / _magnetH;
      */
      MidiEvent *evt = new MidiEvent(0, start_position, msg);
      evt->EndPosition = evt->Position;
      pattern->Events.push_back(evt);
      ControlChange *controlChange = new ControlChange(pattern, pattern->Events.size() - 1);
      ControlChanges.push_back(controlChange);
      Refresh(true);
      this->Refresh(true);
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

