// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
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
        wxPanel(parent, id, pos, size, style), m_controler(-1)
{}

void				MidiAttr::SetNotes(vector <Note *> recnote)
{
    this->Notes = recnote;
    this->ZoomX = 1;
}

void MidiAttr::SetControler(int controler)
{
    LOG;
    m_controler = controler;
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

    if (m_controler >= 0)
        dc.Clear();
    else
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
        UpdateVelocity(e);
}

void MidiAttr::OnLeftUp(wxMouseEvent &e)
{
    UpdateVelocity(e);
}

void MidiAttr::OnLeftDown(wxMouseEvent &e)
{
    UpdateVelocity(e);
}

void MidiAttr::UpdateVelocity(wxMouseEvent &e)
{
    LOG;
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

void				MidiAttr::OnResize(wxSizeEvent &e)
{
  //wxPaintDC			dc(this);
  //dc.Clear();
  ReDraw();
}

BEGIN_EVENT_TABLE(MidiAttr, wxPanel)
    EVT_PAINT(MidiAttr::OnPaint)
    EVT_MOTION(MidiAttr::OnMouseMove)
    EVT_LEFT_UP(MidiAttr::OnLeftUp)
    EVT_LEFT_DOWN(MidiAttr::OnLeftDown)
    EVT_SIZE(MidiAttr::OnResize)
END_EVENT_TABLE()

