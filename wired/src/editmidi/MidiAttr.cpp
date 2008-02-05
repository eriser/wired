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
        wxPanel(parent, id, pos, size, style)
{}

void				MidiAttr::SetNotes(vector <Note *> recnote)
{
    this->Notes = recnote;
    this->ZoomX = 1;
}

void				MidiAttr::OnPaint(wxPaintEvent &e)
{
    LOG;
    wxPaintDC			dc(this);

    wxRegionIterator upd(GetUpdateRegion());
    while (upd)
    {
        for (unsigned int i = 0; i < Notes.size(); i++)
        {
            if (Notes[i])
            {
                int x = (int)floor(Notes[i]->GetPos() * 4 * ROW_WIDTH * ZoomX);
                int perc_height = (Notes[i]->GetVelocity() * GetSize().GetHeight()) / 128;
                int y = GetSize().GetHeight() - perc_height;

                dc.SetPen(wxPen(wxColor(Notes[i]->GetVelocity() + 75, 0x00, 0x00)));
                dc.SetBrush(wxBrush(wxColor(Notes[i]->GetVelocity() + 128, 0x00, 0x00)));
                wxRect a(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH());
                wxRect b(x, y, 5, perc_height);
                wxRect *rect = CalcIntersection(a, b);
                if (rect != NULL)
                {
                    dc.DrawRectangle(rect->x, rect->y, rect->width, rect->height);
                    delete rect;
                }
            }
        }
        upd++;
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

BEGIN_EVENT_TABLE(MidiAttr, wxPanel)
    EVT_PAINT(MidiAttr::OnPaint)
    EVT_MOTION(MidiAttr::OnMouseMove)
    EVT_LEFT_UP(MidiAttr::OnLeftUp)
    EVT_LEFT_DOWN(MidiAttr::OnLeftDown)
END_EVENT_TABLE()

