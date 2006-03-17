#include "DelayPlug.h"
#include "midi.h"
#include <sstream>

static PlugInitInfo info;

BEGIN_EVENT_TABLE(DelayPlugin, wxWindow)
  EVT_BUTTON(Delay_Bypass, DelayPlugin::OnBypass)
  EVT_COMMAND_SCROLL(Delay_Time, DelayPlugin::OnDelayTime)
  EVT_COMMAND_SCROLL(Delay_Feedback, DelayPlugin::OnFeedback)
  EVT_COMMAND_SCROLL(Delay_DryWet, DelayPlugin::OnDryWet)
  EVT_PAINT(DelayPlugin::OnPaint)
END_EVENT_TABLE()

DelayPlugin::DelayPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo), Bypass(false)
{
  BufStart[0] = 0x0;
  BufStart[1] = 0x0;
  Init();

  wxImage *tr_bg = 
    new wxImage(GetDataDir() + wxString(IMG_DL_BG), wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);

  liquid_on = new wxImage(GetDataDir() + wxString(IMG_LIQUID_ON), wxBITMAP_TYPE_PNG);
  liquid_off = new wxImage(GetDataDir() + wxString(IMG_LIQUID_OFF), wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));

  bypass_on = new wxImage(GetDataDir() + wxString(IMG_BYPASS_ON), wxBITMAP_TYPE_PNG);
  bypass_off = new wxImage(GetDataDir() + wxString(IMG_BYPASS_OFF), wxBITMAP_TYPE_PNG);
  BypassBtn = new DownButton(this, Delay_Bypass, wxPoint(21, 58), 
			     wxSize(bypass_on->GetWidth(), bypass_on->GetHeight()), bypass_off, bypass_on);

  bmp = new wxBitmap(GetDataDir() + wxString(IMG_DL_BMP),
		     wxBITMAP_TYPE_BMP); 
  img_bg = new wxImage(GetDataDir() + wxString(IMG_DL_FADER_BG),
		       wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(GetDataDir() + wxString(IMG_DL_FADER_FG), 
		       wxBITMAP_TYPE_PNG );
  TimeFader = new FaderCtrl(this, Delay_Time, img_bg, img_fg, 0, MAX_TIME, 
			    &DelayTime, true, wxPoint(73, 11), 
			    wxSize(img_bg->GetWidth(), img_bg->GetHeight()),
			    this, GetPosition() + wxPoint(58, 25));
  FeedbackFader = new FaderCtrl(this, Delay_Feedback, img_bg, img_fg, 0, 100, 
				&Feedback, true, wxPoint(110, 11),
				wxSize(img_bg->GetWidth(), 
				       img_bg->GetHeight()),
				this, GetPosition() + wxPoint(95, 35));
  DryWetFader = new FaderCtrl(this, Delay_DryWet, img_bg, img_fg, 0, 100, 
			      &WetLevel, true, wxPoint(149, 11), 
			      wxSize(img_bg->GetWidth(), img_bg->GetHeight()),
			      this, GetPosition() + wxPoint(135, 35));
  SetBackgroundColour(wxColour(237, 237, 237));

  // Midi automation defaults
  MidiTime[0] = M_CONTROL;
  MidiTime[1] = 0xA;
  MidiFeedback[0] = M_CONTROL;
  MidiFeedback[1] = 0xB;
  MidiDryWet[0] = M_CONTROL;
  MidiDryWet[1] = 0xC;
  MidiBypass[0] = -1;
  MidiBypass[1] = -1;
  
  Connect(Delay_Bypass, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&DelayPlugin::OnBypassController);    
  Connect(Delay_Time, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&DelayPlugin::OnTimeController); 
  Connect(Delay_Feedback, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&DelayPlugin::OnFeedbackController);    
  Connect(Delay_DryWet, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&DelayPlugin::OnDryWetController);    
}

DelayPlugin::~DelayPlugin()
{
  delete TpBmp;
  delete bmp;
  delete img_bg;
  delete img_fg;

  delete bypass_on;
  delete bypass_off;
  delete liquid_on;
  delete liquid_off;
}

void DelayPlugin::Init()
{
  DelayMutex.Lock();

  DelayTime = 1000;
  // Feedback = 0.5f;
//   DryLevel = 0.5f;
//   WetLevel = 0.5f;
  
  Feedback = 50;
  DryLevel = 50;
  WetLevel = 50;

  AllocateMem();

  DelayMutex.Unlock();

}

void DelayPlugin::AllocateMem()
{
  for (int i = 0; i < 2; i++)
    if (BufStart[i])
      delete BufStart[i];

  int len = (int)(DelayTime * 44100.f / 1000.f);
  if (len >= 0)
    {
      for (int i = 0; i < 2; i++)
	{
	  BufStart[i] = new float[len];
	  if (BufStart[i] == 0x0)
	    cout << "[DELAYPLUG] Error allocating memory" << endl;
	  
	  BufEnd[i] = BufStart[i] + len;
	  BufPtr[i] = BufStart[i];
	  
	  memset(BufPtr[i], 0, len * sizeof(float));
	}
    }
}

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)

void DelayPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  int chan;
  float out[2];

  DelayMutex.Lock();

  if (!Bypass)
    for (chan = 0; chan < 2; chan++)
      {
	for (i = 0; i < sample_length; i++)
	  {
	    out[chan] = *BufPtr[chan];
	    
	    output[chan][i] = DryLevel / 100.f * input[chan][i] + 
	      WetLevel / 100.f * out[chan];
	    
	    *BufPtr[chan] = input[chan][i] + Feedback / 100.f * out[chan];
	    
	    if (++(BufPtr[chan]) >= BufEnd[chan])
	      BufPtr[chan] = BufStart[chan];
	  }
      }
  else
    {
      memcpy(output[0], input[0], sample_length * sizeof(float));
      memcpy(output[1], input[1], sample_length * sizeof(float));
    }

  DelayMutex.Unlock();
}

void DelayPlugin::ProcessEvent(WiredEvent &event)
{
  DelayMutex.Lock();

  if ((MidiTime[0] == event.MidiData[0]) && (MidiTime[1] == event.MidiData[1]))
    {
      float step = MAX_TIME / 127.f;

      DelayTime = event.MidiData[2] * step;
      TimeFader->SetValue((long)DelayTime);
    }
  else if ((MidiFeedback[0] == event.MidiData[0]) && (MidiFeedback[1] == event.MidiData[1]))
    {
      float step = 100.f / 127.f;
      
      Feedback = event.MidiData[2] * step;
      FeedbackFader->SetValue((long)Feedback);
      Feedback /= 100.f;
    }
  else if ((MidiDryWet[0] == event.MidiData[0]) && (MidiDryWet[1] == event.MidiData[1]))
    {
      float step = 100.f / 127.f;
      
      DryWetFader->SetValue((long)(event.MidiData[2] * step)); 
      DryLevel = (100 - DryWetFader->GetValue()) / 100.f;
      WetLevel = DryWetFader->GetValue() / 100.f;
    }
  else if ((MidiBypass[0] == event.MidiData[0]) && (MidiBypass[1] == event.MidiData[1]))
    {
      if (event.MidiData[2])
	{
	  BypassBtn->SetOn();
	  Bypass = true;
	  // *** Known bug : something generates a X async reply 
	  Liquid->SetBitmap(wxBitmap(liquid_off));
	}
      else
	{
	  BypassBtn->SetOff();
	  Bypass = false;
	  // *** Known bug : something generates a X async reply 
	  Liquid->SetBitmap(wxBitmap(liquid_on));
	}
    }

  DelayMutex.Unlock();
}

void DelayPlugin::Load(int fd, long size)
{
  DelayMutex.Lock();

  if (size)
    {
      read(fd, &DelayTime, sizeof (DelayTime));
      read(fd, &Feedback, sizeof (Feedback));
      read(fd, &DryLevel, sizeof (DryLevel));
      read(fd, &WetLevel, sizeof (WetLevel));
      read(fd, MidiBypass, sizeof (int[2]));
      read(fd, MidiTime, sizeof (int[2]));
      read(fd, MidiFeedback, sizeof (int[2]));
      read(fd, MidiDryWet, sizeof (int[2]));
    }

  DelayMutex.Unlock();
}

long DelayPlugin::Save(int fd)
{
  long size;

  size = write(fd, &DelayTime, sizeof (DelayTime));
  size += write(fd, &Feedback, sizeof (Feedback));
  size += write(fd, &DryLevel, sizeof (DryLevel));
  size += write(fd, &WetLevel, sizeof (WetLevel));
  size += write(fd, MidiBypass, sizeof (int[2]));
  size += write(fd, MidiTime, sizeof (int[2]));
  size += write(fd, MidiFeedback, sizeof (int[2]));
  size += write(fd, MidiDryWet, sizeof (int[2]));
  
  return (size);
}

void DelayPlugin::Load(WiredPluginData& Datas)
{
	char		*buffer;
	
	DelayMutex.Lock();
		
	buffer = strdup(Datas.LoadValue(wxString(STR_DELAY_TIME, *wxConvCurrent)));
	if (buffer != NULL)
		DelayTime = strtof(buffer, NULL);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_FEEDBACK, *wxConvCurrent)));
	if (buffer != NULL)
		Feedback = strtof(buffer, NULL);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_DRY_LEVEL, *wxConvCurrent)));
	if (buffer != NULL)
		DryLevel = strtof(buffer, NULL);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_WET_LEVEL, *wxConvCurrent)));
	if (buffer != NULL)
		WetLevel = strtof(buffer, NULL);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_BYPASS1, *wxConvCurrent)));
	if (buffer != NULL)
		MidiBypass[0] = atoi(buffer);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_BYPASS2, *wxConvCurrent)));
	if (buffer != NULL)
		MidiBypass[1] = atoi(buffer);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_TIME1, *wxConvCurrent)));
	if (buffer != NULL)
		MidiTime[0] = atoi(buffer);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_TIME2, *wxConvCurrent)));
	if (buffer != NULL)
		MidiTime[1] = atoi(buffer);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_FEEDBACK1, *wxConvCurrent)));
	if (buffer != NULL)
		MidiFeedback[0] = atoi(buffer);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_FEEDBACK2, *wxConvCurrent)));
	if (buffer != NULL)
		MidiFeedback[1] = atoi(buffer);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_DRY_WET1, *wxConvCurrent)));
	if (buffer != NULL)
		MidiDryWet[0] = atoi(buffer);
	free(buffer);	
	buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_DRY_WET2, *wxConvCurrent)));
	if (buffer != NULL)
		MidiDryWet[1] = atoi(buffer);
	free(buffer);
	
	DelayMutex.Unlock();
}

void DelayPlugin::Save(WiredPluginData& Datas)
{
	std::ostringstream 	oss;

	oss << DelayTime;
	Datas.SaveValue(wxString(STR_DELAY_TIME, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << Feedback;
	Datas.SaveValue(wxString(STR_FEEDBACK, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << DryLevel;
	Datas.SaveValue(wxString(STR_DRY_LEVEL, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << WetLevel;
	Datas.SaveValue(wxString(STR_WET_LEVEL, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiBypass[0];
	Datas.SaveValue(wxString(STR_MIDI_BYPASS1, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiBypass[1];
	Datas.SaveValue(wxString(STR_MIDI_BYPASS2, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiTime[0];
	Datas.SaveValue(wxString(STR_MIDI_TIME1, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiTime[1];
	Datas.SaveValue(wxString(STR_MIDI_TIME2, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiFeedback[0];
	Datas.SaveValue(wxString(STR_MIDI_FEEDBACK1, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiFeedback[1];
	Datas.SaveValue(wxString(STR_MIDI_FEEDBACK2, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiDryWet[0];
	Datas.SaveValue(wxString(STR_MIDI_DRY_WET1, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
	oss << MidiDryWet[1];
	Datas.SaveValue(wxString(STR_MIDI_DRY_WET2, *wxConvCurrent), (char *)oss.str().c_str());
	oss.seekp(ios_base::beg);
}

bool DelayPlugin::IsAudio()
{
  return (true);
}

bool DelayPlugin::IsMidi()
{
  return (true);
}

wxBitmap *DelayPlugin::GetBitmap()
{
  return (bmp);
}

void DelayPlugin::OnBypass(wxCommandEvent &e)
{
  DelayMutex.Lock();

  Bypass = BypassBtn->GetOn();
  if (Bypass)
    Liquid->SetBitmap(wxBitmap(liquid_off));
  else
    Liquid->SetBitmap(wxBitmap(liquid_on));	      

  DelayMutex.Unlock();
}

void DelayPlugin::OnDelayTime(wxScrollEvent &WXUNUSED(e))
{
  DelayMutex.Lock();

//   DelayTime = TimeFader->GetValue();
  AllocateMem();

  DelayMutex.Unlock();

  //  cout << "Time: " << DelayTime << endl;
}
  
void DelayPlugin::OnFeedback(wxScrollEvent &WXUNUSED(e))
{
//   Feedback = FeedbackFader->GetValue() / 100.f;
  //cout << "Feedback: " << Feedback << endl;
}

void DelayPlugin::OnDryWet(wxScrollEvent &WXUNUSED(e))
{
//   DryLevel = (100 - DryWetFader->GetValue()) / 100.f;
//   WetLevel = DryWetFader->GetValue() / 100.f;
  //cout << "DryLevel: " << DryLevel << "; WetLevel: " << WetLevel << endl;
}

void DelayPlugin::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*TpBmp);    
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }
  Plugin::OnPaintEvent(event);
}

void DelayPlugin::CheckExistingControllerData(int MidiData[3])
{
  if ((MidiBypass[0] == MidiData[0]) && (MidiBypass[1] == MidiData[1]))
    MidiBypass[0] = -1;
  else if ((MidiTime[0] == MidiData[0]) && (MidiTime[1] == MidiData[1]))
    MidiTime[0] = -1;
  else if ((MidiFeedback[0] == MidiData[0]) && (MidiFeedback[1] == MidiData[1]))
    MidiFeedback[0] = -1;
  else if ((MidiDryWet[0] == MidiData[0]) && (MidiDryWet[1] == MidiData[1]))
    MidiDryWet[0] = -1;
}

void DelayPlugin::OnBypassController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      DelayMutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiBypass[0] = midi_data[0];
      MidiBypass[1] = midi_data[1];

      DelayMutex.Unlock();
    }
  delete midi_data;
}

void DelayPlugin::OnTimeController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      DelayMutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiTime[0] = midi_data[0];
      MidiTime[1] = midi_data[1];

      DelayMutex.Unlock();
    }
  delete midi_data;
}

void DelayPlugin::OnFeedbackController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      DelayMutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiFeedback[0] = midi_data[0];
      MidiFeedback[1] = midi_data[1];

      DelayMutex.Unlock();
    }
  delete midi_data;
}

void DelayPlugin::OnDryWetController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      DelayMutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiDryWet[0] = midi_data[0];
      MidiDryWet[1] = midi_data[1];

      DelayMutex.Unlock();
    }
  delete midi_data;
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "DELA");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new DelayPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








