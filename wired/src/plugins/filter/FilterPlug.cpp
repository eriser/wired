// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "FilterPlug.h"
#include "midi.h"
#include <unistd.h>
#include <sstream>
#include <stdlib.h>

/******** FilterPlugin Implementation *********/

BEGIN_EVENT_TABLE(FilterPlugin, wxWindow)
  EVT_BUTTON(Filter_Bypass, FilterPlugin::OnBypass)
  EVT_COMMAND_SCROLL(Filter_Cutoff, FilterPlugin::OnCutoff)
  EVT_COMMAND_SCROLL(Filter_Res, FilterPlugin::OnResonance)
  EVT_BUTTON(Filter_Select, FilterPlugin::OnSelect)

  EVT_BUTTON(Filter_LP, FilterPlugin::OnLPSelect)
  EVT_BUTTON(Filter_BP, FilterPlugin::OnBPSelect)
  EVT_BUTTON(Filter_HP, FilterPlugin::OnHPSelect)
  EVT_BUTTON(Filter_Notch, FilterPlugin::OnNotchSelect)
  EVT_BUTTON(Filter_NotchBar, FilterPlugin::OnNotchBarSelect)

  EVT_PAINT(FilterPlugin::OnPaint)
END_EVENT_TABLE()

FilterPlugin::FilterPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo), Bypass(false)
{
  Init();

  wxImage *tr_bg = 
    new wxImage(GetDataDir() + wxString(IMG_FL_BG), 
		wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  delete tr_bg;
  bmp = new wxBitmap(GetDataDir() + wxString(IMG_FL_BMP), 
		     wxBITMAP_TYPE_BMP); 
  liquid_on = new wxImage(GetDataDir() + wxString(IMG_LIQUID_ON),
			  wxBITMAP_TYPE_PNG);
  liquid_off = new wxImage(GetDataDir() + wxString(IMG_LIQUID_OFF), 
			   wxBITMAP_TYPE_PNG);
  Liquid = new StaticBitmap(this, -1, wxBitmap(liquid_on), wxPoint(22, 25));

  bypass_on = new wxImage(GetDataDir() + wxString(IMG_BYPASS_ON),
			  wxBITMAP_TYPE_PNG);
  bypass_off = new wxImage(GetDataDir() + wxString(IMG_BYPASS_OFF), 
			   wxBITMAP_TYPE_PNG);
  BypassBtn = new DownButton(this, Filter_Bypass, 
			     wxPoint(21, 58), 
			     wxSize(bypass_on->GetWidth(), 
				    bypass_on->GetHeight()), bypass_off, 
			     bypass_on);

  hp_on = new wxImage(GetDataDir() + wxString(IMG_FL_HP), 
		      wxBITMAP_TYPE_PNG);
  hp_off = new wxImage(GetDataDir() + wxString(IMG_FL_HP_UP), 
		       wxBITMAP_TYPE_PNG);
  lp_on = new wxImage(GetDataDir() + wxString(IMG_FL_LP), 
		      wxBITMAP_TYPE_PNG);
  lp_off = new wxImage(GetDataDir() + wxString(IMG_FL_LP_UP), 
		       wxBITMAP_TYPE_PNG);
  bp_on = new wxImage(GetDataDir() + wxString(IMG_FL_BP),
		      wxBITMAP_TYPE_PNG);
  bp_off = new wxImage(GetDataDir() + wxString(IMG_FL_BP_UP),
		       wxBITMAP_TYPE_PNG);
  notch_on = new wxImage(GetDataDir() + wxString(IMG_FL_NOTCH), 
			 wxBITMAP_TYPE_PNG);
  notch_off = 
    new wxImage(GetDataDir() + wxString(IMG_FL_NOTCH_UP), 
		wxBITMAP_TYPE_PNG);
  notchbar_on = 
    new wxImage(GetDataDir() + wxString(IMG_FL_NOTCHBAR), 
		wxBITMAP_TYPE_PNG);
  notchbar_off = 
    new wxImage(GetDataDir() + wxString(IMG_FL_NOTCHBAR_UP), 
		wxBITMAP_TYPE_PNG);


  LpBtn = new DownButton(this, Filter_LP, wxPoint(70, 43),
			 wxSize(lp_off->GetWidth(), lp_off->GetHeight()),
			 lp_off, lp_on);
  LpBtn->SetOn();
  BpBtn = new DownButton(this, Filter_BP, wxPoint(81, 47),
			 wxSize(bp_off->GetWidth(), bp_off->GetHeight()),
			 bp_off, bp_on);
  HpBtn = new DownButton(this, Filter_HP, wxPoint(92, 47),
			 wxSize(hp_off->GetWidth(), hp_off->GetHeight()),
			 hp_off, hp_on);
  NotchBarBtn = new DownButton(this, Filter_NotchBar, wxPoint(104, 43),
			       wxSize(notchbar_off->GetWidth(), 
				      notchbar_off->GetHeight()),
			       notchbar_off, notchbar_on);
  NotchBtn = new DownButton(this, Filter_Notch, wxPoint(93, 69),
			    wxSize(notch_off->GetWidth(), 
				   notch_off->GetHeight()),
			    notch_off, notch_on);
  
  img_bg = new wxImage(GetDataDir() + wxString(IMG_FL_FADER_BG), 
		       wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(GetDataDir() + wxString(IMG_FL_FADER_FG), 
		       wxBITMAP_TYPE_PNG);
  
  CutoffFader = new FaderCtrl(this, Filter_Cutoff, img_bg, img_fg, 1, 
			      SIZE_CUTOFF, &Cutoff, true, wxPoint(118, 12),
			      wxSize(img_bg->GetWidth(), img_bg->GetHeight()), 
			      this, GetPosition() + wxPoint(103, 25));
  ResFader = new FaderCtrl(this, Filter_Res, img_bg, img_fg, 0, SIZE_RES, &Res,
			   true, wxPoint(153, 12), 
			   wxSize(img_bg->GetWidth(), img_bg->GetHeight()), 
			   this, GetPosition() + wxPoint(138, 25));
  wxImage** imgs;

  imgs = new wxImage*[4];
  imgs[0] = 
    new wxImage(GetDataDir() + wxString(IMG_FL_KNOB_LP));
  imgs[1] = 
    new wxImage(GetDataDir() + wxString(IMG_FL_KNOB_BP));
  imgs[2] = 
    new wxImage(GetDataDir() + wxString(IMG_FL_KNOB_HP));
  imgs[3] = 
    new wxImage(GetDataDir() + wxString(IMG_FL_KNOB_NOTCH));
  
  FilterSelect = new StaticPosKnob(this, Filter_Select, 4, imgs, 15, 0, 3, 0, 
				   wxPoint(68, 7), wxDefaultSize);
  delete imgs[0];
  delete imgs[1];
  delete imgs[2];
  delete imgs[3];
  delete imgs;
  
  SetBackgroundColour(wxColour(237, 237, 237));

  MidiCutoff[0] = M_CONTROL;
  MidiCutoff[1] = 0xA;
  MidiRes[0] = M_CONTROL;
  MidiRes[1] = 0xB;
  MidiBypass[0] = -1;
  MidiBypass[1] = -1;

  UpdateBypass = false;
  UpdateCutoff = false;
  UpdateRes = false;
  
  Connect(Filter_Bypass, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&FilterPlugin::OnBypassController);    
  Connect(Filter_Cutoff, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&FilterPlugin::OnCutoffController);
  Connect(Filter_Res, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&FilterPlugin::OnResController);    
}

FilterPlugin::~FilterPlugin()
{
  delete bmp;
  delete TpBmp;
  delete img_bg;
  delete img_fg;
  delete bypass_on;
  delete bypass_off;
  delete liquid_on;
  delete liquid_off;
  delete hp_on;
  delete hp_off;  
  delete lp_on;
  delete lp_off;  
  delete bp_on;
  delete bp_off;  
  delete notch_on;
  delete notch_off; 
  delete notchbar_on;
  delete notchbar_off; 
}

void FilterPlugin::Init()
{
  Mutex.Lock();

  Cutoff = SIZE_CUTOFF;
  Res = 0.f;

  memset(Coefs, 0, sizeof (float) * FILTER_SIZE);  
  memset(History[0], 0, sizeof (float) * FILTER_SIZE);  
  memset(History[1], 0, sizeof (float) * FILTER_SIZE);  

  SetFilter(filter_lp, Cutoff, Res);

  Mutex.Unlock();
}

void FilterPlugin::Load(int fd, long size)
{
  int filter;

  Mutex.Lock();

  if (read(fd, &filter, sizeof (filter)) <= 0)
    {
      cout << "[FILTERPLUG] Error while loading patch !" << endl;
      return;
    }
  if (read(fd, &Cutoff, sizeof (Cutoff)) <= 0)
    {
      cout << "[FILTERPLUG] Error while loading patch !" << endl;
      return;
    }
  if (read(fd, &Res, sizeof (Res)) <= 0)
    {
      cout << "[FILTERPLUG] Error while loading patch !" << endl;
      return;
    }
  if (read(fd, &Bypass, sizeof (Bypass)) <= 0)
    {
      cout << "[FILTERPLUG] Error while loading patch !" << endl;
      return;
    }
  read(fd, MidiBypass, sizeof (int[2]));
  read(fd, MidiCutoff, sizeof (int[2]));
  read(fd, MidiRes, sizeof (int[2]));

  FilterSelect->SetValue(filter);
  CutoffFader->SetValue((int)Cutoff);
  ResFader->SetValue(int(Res * 100));

  Mutex.Unlock();

  wxCommandEvent e;
  OnSelect(e);
}
 
long FilterPlugin::Save(int fd)
{
  Mutex.Lock();

  long size;
  int filter = FilterSelect->GetValue();

  size = write(fd, &filter, sizeof (filter));
  size += write(fd, &Cutoff, sizeof (Cutoff));
  size += write(fd, &Res, sizeof (Res));
  size += write(fd, &Bypass, sizeof (Bypass));
  size += write(fd, MidiBypass, sizeof (int[2]));
  size += write(fd, MidiCutoff, sizeof (int[2]));
  size += write(fd, MidiRes, sizeof (int[2]));

  Mutex.Unlock();

  return (size);
}



void	 	FilterPlugin::Load(WiredPluginData& Datas)
{
  int 	filter;
  char	*buffer;
  
  Mutex.Lock();
  
  buffer = strdup(Datas.LoadValue(wxString(STR_FILTER, *wxConvCurrent)));
  if (buffer != NULL)
    filter = atoi(buffer);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_CUTOFF, *wxConvCurrent)));
  if (buffer != NULL)
    Cutoff = strtof(buffer, NULL);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_RESONANCE, *wxConvCurrent)));
  if (buffer != NULL)
    Res = strtof(buffer, NULL);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_BYPASS, *wxConvCurrent)));
  if (buffer != NULL)
    Bypass = atoi(buffer);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_BYPASS1, *wxConvCurrent)));
  if (buffer != NULL)
    MidiBypass[0] = atoi(buffer);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_BYPASS2, *wxConvCurrent)));
  if (buffer != NULL)
    MidiBypass[1] = atoi(buffer);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_CUTOFF1, *wxConvCurrent)));
  if (buffer != NULL)
    MidiCutoff[0] = atoi(buffer);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_CUTOFF2, *wxConvCurrent)));
  if (buffer != NULL)
	  MidiCutoff[1] = atoi(buffer);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_RESONANCE1, *wxConvCurrent)));
  if (buffer != NULL)
    MidiRes[0] = atoi(buffer);
  free(buffer);
  buffer = strdup(Datas.LoadValue(wxString(STR_MIDI_RESONANCE2, *wxConvCurrent)));
  if (buffer != NULL)
    MidiRes[1] = atoi(buffer);
  free(buffer);
  
  FilterSelect->SetValue(filter);
  CutoffFader->SetValue((int)Cutoff);
  ResFader->SetValue(int(Res * 100));
  
  Mutex.Unlock();
  
  wxCommandEvent e;
	OnSelect(e);
}

void	 FilterPlugin::Save(WiredPluginData& Datas)
{
  std::ostringstream 	oss;
  
  Mutex.Lock();
  oss << FilterSelect->GetValue();
  Datas.SaveValue(wxString(STR_FILTER), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << Cutoff;
  Datas.SaveValue(wxString(STR_CUTOFF), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << Res;
  Datas.SaveValue(wxString(STR_RESONANCE), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << Bypass;
  Datas.SaveValue(wxString(STR_BYPASS), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << MidiBypass[0];
  Datas.SaveValue(wxString(STR_MIDI_BYPASS1), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << MidiBypass[1];
  Datas.SaveValue(wxString(STR_MIDI_BYPASS2), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << MidiCutoff[0];
  Datas.SaveValue(wxString(STR_MIDI_CUTOFF1), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << MidiCutoff[1];
  Datas.SaveValue(wxString(STR_MIDI_CUTOFF2), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << MidiRes[0];
  Datas.SaveValue(wxString(STR_MIDI_RESONANCE1), (char *)oss.str().c_str());
  oss.seekp(ios_base::beg);
  oss << MidiRes[1];
  Datas.SaveValue(wxString(STR_MIDI_RESONANCE2), (char *)oss.str().c_str());
  Mutex.Unlock();
}

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)

void FilterPlugin::SetCoeffs(double b0, double b1, double b2, 
			     double a0, double a1, double a2)
{
  if (a0)
    {
      Coefs[0] = (float)(b0 / a0);
      Coefs[1] = (float)(b1 / a0);
      Coefs[2] = (float)(b2 / a0);
      Coefs[3] = (float)(-a1 / a0);
      Coefs[4] = (float)(-a2 / a0);
      
      if (IS_DENORMAL(Coefs[0]))
	Coefs[0] = 0.f;
      if (IS_DENORMAL(Coefs[1]))
	Coefs[1] = 0.f;
      if (IS_DENORMAL(Coefs[2]))
	Coefs[2] = 0.f;
      if (IS_DENORMAL(Coefs[3]))
	Coefs[3] = 0.f;
      if (IS_DENORMAL(Coefs[4]))
	Coefs[4] = 0.f;
    }
  else
    {
      memset(Coefs, 0, sizeof (float) * FILTER_SIZE);
      Coefs[0] = 1.f;
    }
}

void FilterPlugin::SetFilter(int type, float cutoff, float resonance)
{
  static const double dbGain = 24;
  static const double A = pow(10, dbGain / 40);
  static const double beta = sqrt(A + A);
  
  double omega = 2 * M_PI * Cutoff * SamplePeriod;
  double sn = sin(omega);
  double cs = cos(omega);
  double mcs = 1 - cs;
  double alpha = sn * sinh(M_LN2 / 2 * exp(-3 * Res / 100.f) * omega / sn);

  if (IS_DENORMAL(omega))
    omega = 0.0;
  if (IS_DENORMAL(sn))
    sn = 0.0;
  if (IS_DENORMAL(cs))
    cs = 0.0;
  if (IS_DENORMAL(cs))
    cs = 0.0;
  if (IS_DENORMAL(mcs))
    mcs = 0.0;

  Reamp = 1;
  switch (type)
    {
    case filter_lp:
      SetCoeffs(mcs / 2, mcs, mcs / 2, 1 + alpha, -2 * cs, 1 - alpha);
      break;
      
    case filter_bp:
      Reamp = 1; //6;
      SetCoeffs(alpha, 0, -alpha, 1 + alpha, - 2 * cs, 1 - alpha);
      break;

    case filter_notch:
      Reamp = 0.01f;//1
      SetCoeffs(1, -2 * cs, 1, 1 + alpha, -2 * cs, 1 - alpha);
      break;

    case filter_peq:
      Reamp = 0.5f;
      SetCoeffs(1 + (alpha * A), -2 * cs, 1 - (alpha * A), 1 + (alpha /A), -2 * cs, 
	       1 - (alpha /A));
      break;
      
    case filter_hp:
      Reamp = 0.01f;//0.15f;
      SetCoeffs(A * ((A + 1) + (A - 1) * cs + beta * sn),
		-2 * A * ((A - 1) + (A + 1) * cs),
		A * ((A + 1) + (A - 1) * cs - beta * sn),
		(A + 1) - (A - 1) * cs + beta * sn,
		2 * ((A - 1) - (A + 1) * cs),
		(A + 1) - (A - 1) * cs - beta * sn);
      break;
    }  
}

void FilterPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  char chan;
  
  float out;
  int   j;

  static const float anti_denormal = 1e-18; 
  
  Mutex.Lock();
  
  if (!Bypass)
    {
      for (chan = 0; chan < 2; chan++)
	{
	  for (i = 0; i < sample_length; i++)
	    {
	      History[chan][2] = History[chan][1];
	      History[chan][1] = History[chan][0];
	      History[chan][0] = input[chan][i];
	      
	      out = 0.f;
	      for (j = 0; j < FILTER_SIZE; j++)
		{
		  out += History[chan][j] * Coefs[j];
		  if (IS_DENORMAL(out))
		    {
		      out += anti_denormal; 
		      out -= anti_denormal;
		    }
		}
	      History[chan][4] = History[chan][3];
	      History[chan][3] = out;
	      
	      out *= Reamp;
	      
	      if (IS_DENORMAL(out))
		{
		  out = 0.f;
		}
	      output[chan][i] = out;
	    } 
	  for (j = 0; j < FILTER_SIZE; j++)     
	    {
	      if (IS_DENORMAL(History[chan][j]))
		History[chan][j] = 0.f;
	    }
	}		  
    }
  else
    {
      memcpy(output[0], input[0], sample_length * sizeof(float));
      memcpy(output[1], input[1], sample_length * sizeof(float));
    }

  Mutex.Unlock();
}

void FilterPlugin::ProcessEvent(WiredEvent &event)
{
  Mutex.Lock();

  if ((MidiCutoff[0] == event.MidiData[0]) && 
      (MidiCutoff[1] == event.MidiData[1]))
    {
      float step = SIZE_CUTOFF / 127.f;

      Cutoff = event.MidiData[2] * step;
      if (Cutoff < 1.f)
	Cutoff = 1.f;
      SetFilter(FilterSelect->GetValue(), Cutoff, Res / 100.f);

      UpdateCutoff = true;
      AskUpdate();
    }
  else if ((MidiRes[0] == event.MidiData[0]) && 
	   (MidiRes[1] == event.MidiData[1]))
    {
      float step = 100.f / 127.f;
      
      Res = (event.MidiData[2] * step) / 100.f;
      SetFilter(FilterSelect->GetValue(), Cutoff, Res / 100.f);

      UpdateRes = true;
      AskUpdate();
    }
  else if ((MidiBypass[0] == event.MidiData[0]) && 
	   (MidiBypass[1] == event.MidiData[1]))
    {
      if (event.MidiData[2])
	Bypass = true;
      else
	Bypass = false;
      
      UpdateBypass = true;
      AskUpdate();
    }
  
  Mutex.Unlock();
}

void FilterPlugin::Update()
{
  Mutex.Lock();

  if (UpdateCutoff)
    {
      CutoffFader->SetValue((long)Cutoff);
      UpdateCutoff = false;
    }
  if (UpdateRes)
    {
      //ResFader->SetValue((long)(Res * 100.f));
      ResFader->SetValue((long)Res);
      UpdateRes = false;      
    }
  if (UpdateBypass)
    {
      if (Bypass)
	{
	  BypassBtn->SetOn();
	  Liquid->SetBitmap(wxBitmap(liquid_off));
	}
      else
	{
	  BypassBtn->SetOff();
	  Liquid->SetBitmap(wxBitmap(liquid_on));
	}
      UpdateBypass = false;
    }
  
  Mutex.Unlock();
}

void FilterPlugin::CheckExistingControllerData(int MidiData[3])
{
  if ((MidiBypass[0] == MidiData[0]) && (MidiBypass[1] == MidiData[1]))
    MidiBypass[0] = -1;
  else if ((MidiCutoff[0] == MidiData[0]) && (MidiCutoff[1] == MidiData[1]))
    MidiCutoff[0] = -1;
  else if ((MidiRes[0] == MidiData[0]) && (MidiRes[1] == MidiData[1]))
    MidiRes[0] = -1;
}

void FilterPlugin::ApplyFilter()
{
  Mutex.Lock();
  
  memset(Coefs, 0, sizeof (float) * FILTER_SIZE);  
  memset(History[0], 0, sizeof (float) * FILTER_SIZE);  
  memset(History[1], 0, sizeof (float) * FILTER_SIZE);  
  
  SetFilter(FilterSelect->GetValue(), Cutoff, Res / 100.f);
  
  Mutex.Unlock();     
}

void FilterPlugin::OnLPSelect(wxCommandEvent &e)
{
  if (!LpBtn->GetOn())
    LpBtn->SetOn();
  else
    {
      FilterSelect->SetValue(0);
      if (BpBtn->GetOn())
	BpBtn->SetOff();
      else if (HpBtn->GetOn())
	HpBtn->SetOff();
      else if (NotchBtn->GetOn())
	{
	  NotchBtn->SetOff();
	  NotchBarBtn->SetOff();
	}
      ApplyFilter();
    }
}

void FilterPlugin::OnBPSelect(wxCommandEvent &e)
{
  if (!BpBtn->GetOn())
    BpBtn->SetOn();
  else
    {
      FilterSelect->SetValue(1);
      if (LpBtn->GetOn())
	LpBtn->SetOff();
      else if (HpBtn->GetOn())
	HpBtn->SetOff();
      else if (NotchBtn->GetOn())
	{
	  NotchBtn->SetOff();
	  NotchBarBtn->SetOff();
	}
      ApplyFilter();
    }
}

void FilterPlugin::OnHPSelect(wxCommandEvent &e)
{
  if (!HpBtn->GetOn())
    HpBtn->SetOn();
  else
    {
      FilterSelect->SetValue(2);
      if (LpBtn->GetOn())
	LpBtn->SetOff();
      else if (BpBtn->GetOn())
	BpBtn->SetOff();
      else if (NotchBtn->GetOn())
	{
	  NotchBtn->SetOff();
	  NotchBarBtn->SetOff();
	}
      ApplyFilter();
    }
}

void FilterPlugin::OnNotchSelect(wxCommandEvent &e)
{
  if (!NotchBtn->GetOn())
    NotchBtn->SetOn();
  else
    {
      NotchBarBtn->SetOn();
      FilterSelect->SetValue(3);
      if (LpBtn->GetOn())
	LpBtn->SetOff();
      else if (BpBtn->GetOn())
	BpBtn->SetOff();
      else if (HpBtn->GetOn())
	HpBtn->SetOff();
      ApplyFilter();
    }
}

void FilterPlugin::OnNotchBarSelect(wxCommandEvent &e)
{
  if (!NotchBarBtn->GetOn())
    NotchBarBtn->SetOn();
  else
    {
      NotchBtn->SetOn();
      FilterSelect->SetValue(3);
      if (LpBtn->GetOn())
	LpBtn->SetOff();
      else if (BpBtn->GetOn())
	BpBtn->SetOff();
      else if (HpBtn->GetOn())
	HpBtn->SetOff();
      ApplyFilter();
    }
}

void FilterPlugin::OnBypassController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiBypass[0] = midi_data[0];
      MidiBypass[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void FilterPlugin::OnCutoffController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiCutoff[0] = midi_data[0];
      MidiCutoff[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void FilterPlugin::OnResController(wxMouseEvent &event)
{
  int *midi_data;

  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      Mutex.Lock();

      CheckExistingControllerData(midi_data);      
      MidiRes[0] = midi_data[0];
      MidiRes[1] = midi_data[1];

      Mutex.Unlock();
    }
  delete midi_data;
}

void FilterPlugin::OnBypass(wxCommandEvent &e)
{
  Mutex.Lock();

  Bypass = BypassBtn->GetOn();
  if (Bypass)
    Liquid->SetBitmap(wxBitmap(liquid_off));
  else
    Liquid->SetBitmap(wxBitmap(liquid_on));	      

  Mutex.Unlock();
}

void FilterPlugin::OnSelect(wxCommandEvent &e)
{
  int f;

  ApplyFilter();
  f = FilterSelect->GetValue();

  if (f == 0) 
    {
      LpBtn->SetOn();
      BpBtn->SetOff();
      HpBtn->SetOff();
      NotchBtn->SetOff();
      NotchBarBtn->SetOff();
    }
  else if (f == 1)
    {
      BpBtn->SetOn();
      LpBtn->SetOff();
      HpBtn->SetOff();
      NotchBtn->SetOff();
      NotchBarBtn->SetOff();
    }
  else if (f == 2)
    {
      HpBtn->SetOn();
      BpBtn->SetOff();
      LpBtn->SetOff();
      NotchBtn->SetOff();
      NotchBarBtn->SetOff();
    }
  else if (f == 3)
    {
      NotchBtn->SetOn();
      NotchBarBtn->SetOn();
      HpBtn->SetOff();
      BpBtn->SetOff();
      LpBtn->SetOff();
    }
}

bool FilterPlugin::IsAudio()
{
  return (true);
}

bool FilterPlugin::IsMidi()
{
  return (true);
}

wxBitmap *FilterPlugin::GetBitmap()
{
  return (bmp);
}

void FilterPlugin::OnCutoff(wxScrollEvent &e)
{
  Mutex.Lock();

  //Cutoff = CutoffFader->GetValue();
  SetFilter(FilterSelect->GetValue(), Cutoff, Res / 100.f);

  //cout << "Cutoff: " << Cutoff << "; Res: " << Res << endl;

  Mutex.Unlock();
}
  
void FilterPlugin::OnResonance(wxScrollEvent &e)
{
  Mutex.Lock();

  //Res = ResFader->GetValue() / 100.f;
  SetFilter(FilterSelect->GetValue(), Cutoff, Res);

  //cout << "Cutoff: " << Cutoff << "; Res: " << Res << endl;

  Mutex.Unlock();
}

void FilterPlugin::OnPaint(wxPaintEvent &event)
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

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "FILT");
    info.Name = PLUGIN_NAME;
    info.Type = ePlugTypeEffect;
    info.UnitsX = 1;
    info.UnitsY = 1;
    info.Version = 1;

    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new FilterPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}
