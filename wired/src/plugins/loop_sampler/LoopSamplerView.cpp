// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "LoopSamplerView.h"
#include "Colour.h"

#define TOOLBAR_HEIGHT 120

BEGIN_EVENT_TABLE(LoopSamplerView, wxPanel)
  EVT_BUTTON(LoopSamplerView_Drag, LoopSamplerView::OnDragClick)
  EVT_BUTTON(LoopSamplerView_Pen, LoopSamplerView::OnPenClick)
  EVT_BUTTON(LoopSamplerView_Invert, LoopSamplerView::OnInvertClick)
  EVT_BUTTON(LoopSamplerView_Auto, LoopSamplerView::OnAutoClick)
  EVT_BUTTON(LoopSamplerView_Wave, LoopSamplerView::OnSliceSelected)
  /*  EVT_COMMAND_SCROLL(LoopSamplerView_Note, LoopSamplerView::OnNote)
  EVT_COMMAND_SCROLL(LoopSamplerView_Pitch, LoopSamplerView::OnPitch)
  EVT_COMMAND_SCROLL(LoopSamplerView_Vol, LoopSamplerView::OnVolume)
  EVT_COMMAND_SCROLL(LoopSamplerView_Affect, LoopSamplerView::OnAffectMidi)*/
END_EVENT_TABLE()

LoopSamplerView::LoopSamplerView(wxMutex *mutex, wxWindow *parent, const wxPoint &pos, 
				 const wxSize &size, string datadir, LoopPos *loopinfo)
  : wxPanel(parent, -1, pos, size), Mutex(mutex), DataDir(datadir), LoopInfo(loopinfo)
{
  SetBackgroundColour(wxColour(146, 155, 162));//CL_SEQVIEW_BACKGROUND);
  Toolbar = new wxPanel(this, -1, wxPoint(0, 0), wxSize(TOOLBAR_HEIGHT, size.y), wxSIMPLE_BORDER);
  Toolbar->SetBackgroundColour(wxColour(146, 155, 162));
  Rule = new RulerLoop(this, -1, wxPoint(TOOLBAR_HEIGHT, 0), wxSize(size.x - TOOLBAR_HEIGHT, 
								    RULER_HEIGHT));
  Wave = 0x0;
  Beats = 4;
  Bars = 4;

  hand_up = new wxImage(string(DataDir + string(IMG_LSV_HAND_UP)).c_str(), wxBITMAP_TYPE_PNG);
  hand_down = new wxImage(string(DataDir + string(IMG_LSV_HAND_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  pen_up = new wxImage(string(DataDir + string(IMG_LSV_PEN_UP)).c_str(), wxBITMAP_TYPE_PNG);
  pen_down = new wxImage(string(DataDir + string(IMG_LSV_PEN_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  inverse_up = new wxImage(string(DataDir + string(IMG_LSV_INV_UP)).c_str(), wxBITMAP_TYPE_PNG);
  inverse_down = new wxImage(string(DataDir + string(IMG_LSV_INV_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  create_up = new wxImage(string(DataDir + string(IMG_LSV_CREATE_UP)).c_str(), wxBITMAP_TYPE_PNG);
  create_down = new wxImage(string(DataDir + string(IMG_LSV_CREATE_DOWN)).c_str(), wxBITMAP_TYPE_PNG);

  fader_bg = new wxImage(string(DataDir + string(IMG_LSV_FADER_BG)).c_str(), wxBITMAP_TYPE_PNG);
  fader_fg = new wxImage(string(DataDir + string(IMG_LSV_FADER_FG)).c_str(), wxBITMAP_TYPE_PNG);
  knob_bg = new wxImage(string(DataDir + string(IMG_LSV_KNOB_BG)).c_str(), wxBITMAP_TYPE_PNG);
  knob_fg = new wxImage(string(DataDir + string(IMG_LSV_KNOB_FG)).c_str(), wxBITMAP_TYPE_PNG);

  DragBtn = new DownButton(this, LoopSamplerView_Drag, 
			   wxPoint(6, 8), wxSize(31, 29), hand_up, hand_down, false);
  PenBtn = new DownButton(this, LoopSamplerView_Pen, 
			   wxPoint(6, 38), wxSize(31, 29), pen_up, pen_down, false);
  InvertBtn = new DownButton(this, LoopSamplerView_Invert, 
			   wxPoint(6, 68), wxSize(31, 29), inverse_up, inverse_down, false);
  AutoBtn = new DownButton(this, LoopSamplerView_Auto, 
			   wxPoint(6, 98), wxSize(31, 29), create_up, create_down, true);
  
  wxStaticText *t;

  NoteLabel = new wxStaticText(Toolbar, -1, "0", wxPoint(52, 2), wxSize(10, -1));
  NoteLabel->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  NoteLabel->SetForegroundColour(*wxWHITE);

  PitchLabel = new wxStaticText(Toolbar, -1, "0", wxPoint(92, 2), wxSize(10, -1));
  PitchLabel->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  PitchLabel->SetForegroundColour(*wxWHITE);

  t = new wxStaticText(Toolbar, -1, "Note", wxPoint(48, 50), wxSize(10, -1));
  t->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  t->SetForegroundColour(*wxWHITE);
  t = new wxStaticText(Toolbar, -1, "Pitch", wxPoint(86, 50), wxSize(10, -1));
  t->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  t->SetForegroundColour(*wxWHITE);

  VolLabel = new wxStaticText(Toolbar, -1, "100", wxPoint(48, 64), wxSize(10, -1));
  VolLabel->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  VolLabel->SetForegroundColour(*wxWHITE);

  AffectLabel = new wxStaticText(Toolbar, -1, "C0", wxPoint(90, 64), wxSize(10, -1));
  AffectLabel->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  AffectLabel->SetForegroundColour(*wxWHITE);

  t = new wxStaticText(Toolbar, -1, "Volume", wxPoint(40, 112), wxSize(10, -1));
  t->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  t->SetForegroundColour(*wxWHITE);
  t = new wxStaticText(Toolbar, -1, "Affect", wxPoint(84, 112), wxSize(10, -1));
  t->SetFont(wxFont(7, wxDEFAULT, wxNORMAL, wxNORMAL));
  t->SetForegroundColour(*wxWHITE);

  NoteKnob = new KnobCtrl(this, LoopSamplerView_Note, knob_bg, knob_fg, 1, 24, 13, 1, 
			  wxPoint(46, 20), wxSize(23, 23));
  PitchKnob = new KnobCtrl(this, LoopSamplerView_Pitch, knob_bg, knob_fg, 1, 200, 100, 1,  
			   wxPoint(84, 20), wxSize(23, 23));
  VolKnob = new KnobCtrl(this, LoopSamplerView_Vol, knob_bg, knob_fg, 0, 127, 100, 1,  
			 wxPoint(46, 82), wxSize(23, 23));
  AffectKnob = new KnobCtrl(this, LoopSamplerView_Affect, knob_bg, knob_fg, 0, 127, 48, 1,   
			    wxPoint(84, 82), wxSize(23, 23));

  Connect(LoopSamplerView_Note, wxEVT_SCROLL_TOP, (wxObjectEventFunction)(wxEventFunction)
	  (wxScrollEventFunction)&LoopSamplerView::OnNote);
  Connect(LoopSamplerView_Pitch, wxEVT_SCROLL_TOP, (wxObjectEventFunction)(wxEventFunction)
	  (wxScrollEventFunction)&LoopSamplerView::OnPitch);
  Connect(LoopSamplerView_Vol, wxEVT_SCROLL_TOP, (wxObjectEventFunction)(wxEventFunction)
	  (wxScrollEventFunction)&LoopSamplerView::OnVolume);
  Connect(LoopSamplerView_Affect, wxEVT_SCROLL_TOP, (wxObjectEventFunction)(wxEventFunction)
	  (wxScrollEventFunction)&LoopSamplerView::OnAffectMidi);
}

LoopSamplerView::~LoopSamplerView()
{
  delete hand_up;
  delete hand_down;
  delete pen_up;
  delete pen_down;
  delete inverse_up;
  delete inverse_down;
  delete create_up;
  delete create_down;

  delete fader_bg;
  delete fader_fg;
  delete knob_bg;
  delete knob_fg;  
}

void LoopSamplerView::SetSlices(list<Slice *> *slices) 
{ 
  Slices = slices;
  if (Wave)
    Wave->SetSlices(slices); 
}

void LoopSamplerView::SetWaveFile(WaveFile *w)
{
  if (Wave)
    {
      Wave->Destroy();
    }
  Wave = new WaveLoop(Mutex, LoopInfo, this, LoopSamplerView_Wave, wxPoint(TOOLBAR_HEIGHT, RULER_HEIGHT), 
		      wxSize(GetSize().x - TOOLBAR_HEIGHT - 2, GetSize().y - RULER_HEIGHT));
  Wave->SetWave(w);

  wxBoxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
  sizer2->Add(Rule, 0, wxEXPAND | wxALL, 0); 
  sizer2->Add(Wave, 1, wxEXPAND | wxALL, 0); 

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(Toolbar, 0, wxEXPAND | wxALL, 0); 
  sizer->Add(sizer2, 1, wxEXPAND | wxALL, 0); 

  SetSizer(sizer);
}

void LoopSamplerView::SetBeats(int bars, int beats)
{
  Rule->SetBeats(bars, beats);
  Beats = beats;
  Bars = bars;
}

void LoopSamplerView::OnDragClick(wxCommandEvent &event)
{
  if (!Wave)
    return;
  if (DragBtn->GetOn())
    {
      PenBtn->SetOff();
      Wave->SetSelect(true);
      Wave->SetDrawing(false);
    }
  else    
    Wave->SetSelect(false);
}

void LoopSamplerView::OnPenClick(wxCommandEvent &event)
{
  if (!Wave)
    return;
  if (PenBtn->GetOn())
    {
      DragBtn->SetOff();
      Wave->SetDrawing(true);
    }
  else
    Wave->SetDrawing(false);
}

void LoopSamplerView::OnInvertClick(wxCommandEvent &event)
{
  if (!Wave)
    return;

  list<Slice *>::iterator i;

  Mutex->Lock();

  for (i = Slices->begin(); i != Slices->end(); i++)
    if ((*i)->Selected)
      {
	(*i)->Invert = InvertBtn->GetOn();
	break;
      }
  Mutex->Unlock();
}

void LoopSamplerView::OnAutoClick(wxCommandEvent &event)
{
  if (!Wave)
    return;
  Wave->CreateSlices(Beats, Bars);
}

void LoopSamplerView::OnSliceSelected(wxCommandEvent &event)
{
  list<Slice *>::iterator i;

  for (i = Slices->begin(); i != Slices->end(); i++)
    if ((*i)->Selected)
      {
	if ((*i)->Invert)
	  InvertBtn->SetOn();
	else
	  InvertBtn->SetOff();

	wxString s;

	NoteKnob->SetValue((*i)->Note + 13);	
	s.Printf("%d", (*i)->Note);
	NoteLabel->SetLabel(s);

	PitchKnob->SetValue((int)((*i)->Pitch * 100));
	s.Printf("%d", (int)((*i)->Pitch * 100) - 100);
	PitchLabel->SetLabel(s);
	
	VolKnob->SetValue((int)((*i)->Volume * 100));
	s.Printf("%d", (int)((*i)->Volume * 100));
	VolLabel->SetLabel(s);

	AffectKnob->SetValue((*i)->AffectMidi);
	s.Printf("%d", (*i)->AffectMidi);
	AffectLabel->SetLabel(s);
	
	break;
      }
}

void LoopSamplerView::OnNote(wxScrollEvent &event)
{
  if (!Wave)
    return;

   list<Slice *>::iterator i;

   for (i = Slices->begin(); i != Slices->end(); i++)
     if ((*i)->Selected)
       {
	 (*i)->SetNote(NoteKnob->GetValue() - 13); // 13 == DO
	 
	 wxString s;
	 
	 s.Printf("%d", (*i)->Note);
	 NoteLabel->SetLabel(s);
	 break;
       } 
}

void LoopSamplerView::OnPitch(wxScrollEvent &event)
{
  if (!Wave)
    return;
  list<Slice *>::iterator i;
  
  for (i = Slices->begin(); i != Slices->end(); i++)
    if ((*i)->Selected)
      {
	(*i)->Pitch = PitchKnob->GetValue() / 100.f;
	
	wxString s;
	
	s.Printf("%d", PitchKnob->GetValue() - 100);
	PitchLabel->SetLabel(s);
	break;
      } 
}

void LoopSamplerView::OnVolume(wxScrollEvent &event)
{
  if (!Wave)
    return;
  list<Slice *>::iterator i;
  
  for (i = Slices->begin(); i != Slices->end(); i++)
    if ((*i)->Selected)
      {
	(*i)->Volume = VolKnob->GetValue() / 100.f;
	
	wxString s;
	
	s.Printf("%d", VolKnob->GetValue());
	VolLabel->SetLabel(s);
	break;
      } 
}

void LoopSamplerView::OnAffectMidi(wxScrollEvent &event)
{
  if (!Wave)
    return;
  list<Slice *>::iterator i;
  
  for (i = Slices->begin(); i != Slices->end(); i++)
    if ((*i)->Selected)
      {
	(*i)->AffectMidi = AffectKnob->GetValue();

	wxString s;
	
	s.Printf("%d", AffectKnob->GetValue());
	AffectLabel->SetLabel(s);
	break;
      } 
}






