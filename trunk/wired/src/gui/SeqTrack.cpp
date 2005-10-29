// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include "SeqTrack.h"
#include "MainWindow.h"
#include "SequencerGui.h"
#include "Colour.h"
#include "Settings.h"
#include "ChannelGui.h"
#include "HelpPanel.h"
#include "ChoiceButton.h"
#include "Rack.h"
#include "Plugin.h"
#include "DownButton.h"
#include "VUMCtrl.h"
#include "../midi/MidiInDevice.h"
#include "../midi/MidiThread.h"
#include "../sequencer/Sequencer.h"

// Counts number of Audio and MIDI tracks created yet
int				AudioTrackCount = 0;
int				MidiTrackCount = 0;

SeqTrack::SeqTrack(long index, wxWindow *parent, 
		   const wxPoint& pos = wxDefaultPosition, 
		   const wxSize& size = wxDefaultSize, bool audio)
  : wxControl(parent, -1, pos, size)
{
  wxString					s;
  //  wxTextAttr					attr;  

  Index = index;
  ChanGui = 0X0;
  Connected = 0x0;
  ConnectedRackTrack = 0x0;
  IsAudio = audio;
  DeviceId = -1;
  Record = false;
  Mute = false;
  VuValue = 0;

  SetBackgroundColour(CL_RULER_BACKGROUND);
  
  if (audio)
    s.Printf("Audio %d", ++AudioTrackCount);
  else
    s.Printf("MIDI %d", ++MidiTrackCount);
  
  /*
    if (Seq->Tracks[Index - 1]->Output)
    MixerPanel->SetLabelByChan(Seq->Tracks[Index - 1]->Output, s);
    else
    cout << "SEQTRACK helas pas d output" << endl;
  */
  Text = new wxTextCtrl(this, SeqTrack_OnNameChange, s, wxPoint(6, 8), 
			wxSize(TRACK_WIDTH - 38, 18), wxTE_PROCESS_ENTER);
  Text->SetFont(wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL));

  wxImage *rec_up = new wxImage(string(WiredSettings->DataDir + string(REC_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *rec_down = 
    new wxImage(string(WiredSettings->DataDir + string(REC_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *mute_up = new wxImage(string(WiredSettings->DataDir + string(MUTE_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *mute_down = new wxImage(string(WiredSettings->DataDir + string(MUTE_DOWN)).c_str(), wxBITMAP_TYPE_PNG);

  RecBtn = new DownButton(this, SeqTrack_Record, wxPoint(6, 30), wxSize(25, 16), 
			  rec_up, rec_down);
  MuteBtn = new DownButton(this, SeqTrack_Mute, wxPoint(34, 30), wxSize(25, 16),
			   mute_up, mute_down);
  Image = new ChoiceButton(this, SeqTrack_ConnectTo, wxPoint(62, 30), wxSize(24, 16), "");
  
  Image->Connect(SeqTrack_ConnectTo, wxEVT_ENTER_WINDOW, 
		 (wxObjectEventFunction)(wxEventFunction) 
		 (wxMouseEventFunction)&SeqTrack::OnConnectToHelp);

  DeviceBox = new wxChoice(this, SeqTrack_DeviceChoice, wxPoint(5, 50), wxSize(TRACK_WIDTH - 38, 22), 
			   0, 0x0);
  DeviceBox->SetFont(wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL));
  FillChoices();
  DeviceBox->SetSelection(0);

  DeviceBox->Connect(SeqTrack_DeviceChoice, wxEVT_ENTER_WINDOW, 
		     (wxObjectEventFunction)(wxEventFunction) 
		     (wxMouseEventFunction)&SeqTrack::OnDeviceHelp);

  wxImage *green = new wxImage(string(WiredSettings->DataDir + string(VUM_GREEN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *orange = new wxImage(string(WiredSettings->DataDir + string(VUM_ORANGE)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *red = new wxImage(string(WiredSettings->DataDir + string(VUM_RED)).c_str(), wxBITMAP_TYPE_PNG);

  Vu = new VUMCtrl(this, -1, 100, green, orange, red,wxPoint(TRACK_WIDTH - 28, 8), wxSize(16, 64));
  Vu->SetValue(0);
  menu = 0x0;
  Selected = false;
  wxWindow::SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

SeqTrack::~SeqTrack()
{
  if (menu)
    delete menu;
}

void					SeqTrack::OnConnectToHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("Click on this button to show the list of instruments and effects you can connect your track to.");
      HelpWin->SetText(s);
    }
}

void					SeqTrack::OnDeviceHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("Click on this box to select the device you would like to record from.");
      HelpWin->SetText(s);
    }
}


void					SeqTrack::FillChoices()
{
  wxString				s;
  vector<long>::iterator		i;

  DeviceBox->Clear();
  DeviceBox->Append(wxString("None"));
  DeviceBox->SetSelection(0);
  if (IsAudio)
    {
      for (i = WiredSettings->InputChannels.begin(); i != WiredSettings->InputChannels.end(); i++)
	{
	  s.Printf("Input %d", (int)((*i) + 1));
	  DeviceBox->Append(s);
	}
    }
  else
    {
      for (i = WiredSettings->MidiIn.begin(); i != WiredSettings->MidiIn.end(); i++)
	{
	  s.Printf("Midi In %d", (int)((*i) + 1));
	  DeviceBox->Append(s);
	}
    }
}

void					SeqTrack::OnConnectTo(wxCommandEvent &event)
{
  list<RackTrack *>::iterator		i;
  list<Plugin *>::iterator		j;
  long					k = 1000;
  
  OnClick(event);
  if (RackPanel->RackTracks.size() <= 0)
    return;
  if (menu)
    delete menu;
  menu = new wxMenu();  
  menu->Append(NONE_SELECTED_ID, "None");
  Connect(NONE_SELECTED_ID, wxEVT_COMMAND_MENU_SELECTED, 
	  (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&SeqTrack::OnConnectSelected);
  for (i = RackPanel->RackTracks.begin(); i != RackPanel->RackTracks.end(); i++)
    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++, k++)
      {
	if ((IsAudio && (*j)->IsAudio()) ||
	    (!IsAudio && (*j)->IsMidi()))
	  {
	    menu->Append(k, (*j)->Name.c_str());
	    Connect(k, wxEVT_COMMAND_MENU_SELECTED, 
		    (wxObjectEventFunction)(wxEventFunction)
		    (wxCommandEventFunction)&SeqTrack::OnConnectSelected);
	  }
      }
  wxPoint p(Image->GetPosition());
  PopupMenu(menu, p.x, p.y);
}

void					SeqTrack::ConnectTo(Plugin *plug)
{
  if (!plug)
    {
      Connected = 0x0;
      ConnectedRackTrack = 0x0;
      //Label->SetLabel("No Instrument");
      Image->SetImage(0x0);
    }
  else
    {
      Connected = plug;
      ConnectedRackTrack = RackPanel->GetRackTrack(plug);
      // Initialisation du plugin
      plug->Init();
      //Label->SetLabel(plug->Name.c_str());
      if (plug)
	Image->SetImage(plug->GetBitmap());
    }
  Image->Refresh();
}

void					SeqTrack::OnConnectSelected(wxCommandEvent &event)
{
  list<RackTrack *>::iterator		i;
  list<Plugin *>::iterator		j;
  long					k = 1000;
  wxMutexLocker				m(SeqMutex);

  if (event.GetId() == NONE_SELECTED_ID)
    {
      ConnectTo(0x0);
      return;
    }
  for (i = RackPanel->RackTracks.begin(); i != RackPanel->RackTracks.end(); i++)
    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++, k++)
      {
	if (k == event.GetId())
	  {
	    ConnectTo(*j);
	    return;
	  }
      }
}

void					SeqTrack::OnPaint(wxPaintEvent &WXUNUSED(event))
{
  wxPaintDC				dc(this);
  wxSize s;	
#define BORDER				(3)

  PrepareDC(dc);
  s = GetSize();
  dc.SetPen(*wxMEDIUM_GREY_PEN); 
  dc.SetBrush(wxBrush(CL_RULER_BACKGROUND, wxTRANSPARENT));//*wxLIGHT_GREY_BRUSH); 
  dc.DrawRoundedRectangle(0, 0, s.x - BORDER, s.y, 3);
  
  if (Selected)
    dc.SetPen(wxPen(CL_WAVE_DRAW, 3, wxSOLID)); 
  else
    dc.SetPen(wxPen(wxColor(141, 153, 170), 2, wxSOLID)); 
  dc.SetBrush(CL_RULER_BACKGROUND);//*wxLIGHT_GREY_BRUSH); 
  dc.DrawRoundedRectangle(1, 1, s.x - 3 - BORDER, s.y - 2, 3);
}

void					SeqTrack::OnClick(wxCommandEvent &WXUNUSED(event))
{
  //printf("what 4 funciton ?\n");
  SeqPanel->UnselectTracks();
  SetSelected(true);
}

void					SeqTrack::OnMouseClick(wxMouseEvent &e)
{
  m_click.x = e.m_x;
  m_click.y = e.m_y;
  SeqPanel->UnselectTracks();
  SetSelected(true);
}

void					SeqTrack::OnMotion(wxMouseEvent &e)
{
  long					z;
  long					h;

  if (Selected && e.Dragging())
    {
      SeqMutex.Lock();
      z = ((e.m_y - m_click.y) / (h = (long) (TRACK_HEIGHT * SeqPanel->VertZoomFactor)));
      if (z < 0)
	if (GetPosition().y < 0)
	  {
	    SeqPanel->ScrollTrackList(-1);
	    SeqPanel->ChangeSelectedTrackIndex(-1);
	  }
	else
	  SeqPanel->ChangeSelectedTrackIndex(z);
      else
	if (z > 0)
	  if (GetPosition().y >= SeqPanel->TrackView->GetClientSize().y - h)
	    {
	      SeqPanel->ChangeSelectedTrackIndex(1);
	      SeqPanel->ScrollTrackList(1);
	    }
	  else
	    SeqPanel->ChangeSelectedTrackIndex(z);
      SeqMutex.Unlock();
    }
}

void					SeqTrack::SetSelected(bool sel)
{
  Selected = sel;
  Refresh();
}

void					SeqTrack::OnNameChange(wxCommandEvent& event)
{
  if (ChanGui)
    ChanGui->SetLabel(Text->GetValue());
}

void					SeqTrack::SetName(const wxString& name)
{
  Text->SetValue(name);
  if (ChanGui)
    ChanGui->SetLabel(name);
}

void					SeqTrack::OnDeviceChoice(wxCommandEvent &WXUNUSED(event))
{
  int					k = 0;

  if (IsAudio)
    {
      vector<long>::iterator		i;

      for (i = WiredSettings->InputChannels.begin(); i != WiredSettings->InputChannels.end(); 
	   i++, k++)
	if (k == DeviceBox->GetSelection() - 1)
	  {
	    DeviceId = *i;
	    return;
	  }
    }
  else
    {
      vector<MidiInDevice *>::iterator	i;

      for (i = MidiEngine->MidiInDev.begin(); i != MidiEngine->MidiInDev.end(); i++, k++)
	if (k == DeviceBox->GetSelection() - 1)
	  {
	    DeviceId = (*i)->id;
	    return;
	  }
    }
}

void					SeqTrack::OnRecordClick(wxCommandEvent &WXUNUSED(event))
{
  if (RecBtn->GetOn())
    {
      Record = true;
      if (Seq->Playing && Seq->Recording)
	{
	  SeqMutex.Lock();
	  Seq->PrepareTrackForRecording(Seq->Tracks[Index - 1]);
	  SeqMutex.Unlock();
	}
    }
  else
    Record = false;
}

void					SeqTrack::OnMuteClick(wxCommandEvent &WXUNUSED(event))
{
  if (MuteBtn->GetOn())
    Mute = true;
  else
    Mute = false;
}
 
void					SeqTrack::SetVuValue(long value)
{
  Vu->SetValue(value);
}

void					SeqTrack::SetVuValue()
{
  Vu->SetValue(VuValue);
}

void					SeqTrack::SetRecording(bool rec)
{
  Record = rec;
  if (rec)
    RecBtn->SetOn();
  else
    RecBtn->SetOff();
}

void					SeqTrack::SetMute(bool mut)
{
  Mute = mut;
  if (mut)
    MuteBtn->SetOn();
  else
    MuteBtn->SetOff();
}

void					SeqTrack::SetDeviceId(long devid)
{
  int					k = 1; // + 1 for the "None" parameter

  DeviceId = devid; 
  if (IsAudio)
    {
      vector<long>::iterator		i;

      for (i = WiredSettings->InputChannels.begin(); i != WiredSettings->InputChannels.end(); 
	   i++, k++)
	if (*i == devid)
	  {
	    DeviceBox->SetSelection(k);
	    return;
	  }
    }
  else
    {
      vector<MidiInDevice *>::iterator	i;

      for (i = MidiEngine->MidiInDev.begin(); i != MidiEngine->MidiInDev.end(); i++, k++)
	if ((*i)->id == devid)
	  {
	    DeviceBox->SetSelection(k);
	    return;
	  }
    }
}

BEGIN_EVENT_TABLE(SeqTrack, wxControl)
  EVT_COMMAND(SeqTrack_ConnectTo, wxEVT_COMMAND_BUTTON_CLICKED, SeqTrack::OnConnectTo)
  EVT_COMMAND(SeqTrack_OnClick, wxEVT_COMMAND_BUTTON_CLICKED, SeqTrack::OnClick)
  EVT_MENU(SeqTrack_ConnectSelected, SeqTrack::OnConnectSelected)
  EVT_TEXT_ENTER(SeqTrack_OnNameChange, SeqTrack::OnNameChange)
  EVT_CHOICE(SeqTrack_DeviceChoice, SeqTrack::OnDeviceChoice)
  EVT_PAINT(SeqTrack::OnPaint)
  EVT_MOTION(SeqTrack::OnMotion)
  EVT_LEFT_DOWN(SeqTrack::OnMouseClick)
  EVT_BUTTON(SeqTrack_Record, SeqTrack::OnRecordClick)
  EVT_BUTTON(SeqTrack_Mute, SeqTrack::OnMuteClick)
END_EVENT_TABLE()

