#include "FilterPlug.h"
#include <unistd.h>

/******** FilterPlugin Implementation *********/

BEGIN_EVENT_TABLE(FilterPlugin, wxWindow)
  EVT_COMMAND_SCROLL(Filter_Cutoff, FilterPlugin::OnCutoff)
  EVT_COMMAND_SCROLL(Filter_Res, FilterPlugin::OnResonance)
#ifdef FRANCIS
  EVT_COMMAND_SCROLL(Filter_Select, FilterPlugin::OnSelect)
#endif
  EVT_PAINT(FilterPlugin::OnPaint)
END_EVENT_TABLE()

FilterPlugin::FilterPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
#ifdef MOOG
#define SIZE_CUTOFF 100
#define SIZE_RES    400
  f = 1.f;
  fb = 0.f;
#endif

#ifdef FRANCIS
#define SIZE_CUTOFF 2205
#define SIZE_RES    100
#endif
 
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_FL_BG)).c_str(), wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_FL_BMP)).c_str(), wxBITMAP_TYPE_BMP); 

  img_bg = new wxImage(string(GetDataDir() + string(IMG_FL_FADER_BG)).c_str(), wxBITMAP_TYPE_PNG );
  img_fg = new wxImage(string(GetDataDir() + string(IMG_FL_FADER_FG)).c_str(), wxBITMAP_TYPE_PNG );
  
  CutoffFader = new 
    FaderCtrl(this, Filter_Cutoff, img_bg, img_fg, 0, SIZE_CUTOFF, 100,
	      wxPoint(18, 8)/*wxPoint(GetSize().x / 2, 10)*/, wxSize(22,78));
  ResFader = new 
    FaderCtrl(this, Filter_Res, img_bg, img_fg, 0, SIZE_RES, 0,
	      wxPoint(142, 8)/*wxPoint(GetSize().x / 2 + 40, 10)*/,
 wxSize(22,78));

#ifdef FRANCIS
  wxImage *knob_bg = new wxImage(string(GetDataDir() + string(IMG_FL_KNOB_BG)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *knob_fg = new wxImage(string(GetDataDir() + string(IMG_FL_KNOB_FG)).c_str(), wxBITMAP_TYPE_PNG);
  FilterSelect = new KnobCtrl(this, Filter_Select, knob_bg, knob_fg, 0, 4, 0, 1,
			      wxPoint(GetSize().x / 2 - 16, 26), wxSize(23, 23));
#endif
  
  SetBackgroundColour(wxColour(237, 237, 237));
}

void FilterPlugin::Init()
{
#ifdef FRANCIS
  Cutoff = 127.f;
  Res = 0.f;

  memset(Coefs, 0, sizeof (float) * FILTER_SIZE);  
  memset(History[0], 0, sizeof (float) * FILTER_SIZE);  
  memset(History[1], 0, sizeof (float) * FILTER_SIZE);  

  SetFilter(filter_lp, Cutoff, Res);
#endif

#ifdef MOOG
  Cutoff = 1.f;
  Res = 0.f;

  Lin1 = Lin2 = Lin3 = Lin4 = Lout1 = Lout2 = Lout3 = Lout4 = 0.f;
  Rin1 = Rin2 = Rin3 = Rin4 = Rout1 = Rout2 = Rout3 = Rout4 = 0.f;
#endif
}

void FilterPlugin::Load(int fd, long size)
{
  int filter;

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
  FilterSelect->SetValue(filter);
  CutoffFader->SetValue((int)Cutoff);
  ResFader->SetValue(int(Res * 100));

  memset(Coefs, 0, sizeof (float) * FILTER_SIZE);  
  memset(History[0], 0, sizeof (float) * FILTER_SIZE);  
  memset(History[1], 0, sizeof (float) * FILTER_SIZE);  

  SetFilter(filter, Cutoff, Res);
}
 
long FilterPlugin::Save(int fd)
{
  long size;
  int filter = FilterSelect->GetValue();

  size = write(fd, &filter, sizeof (filter));
  size += write(fd, &Cutoff, sizeof (Cutoff));
  size += write(fd, &Res, sizeof (Res));
  return (size);
}

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)
//#define IS_DENORMAL(f) (f)

#ifdef FRANCIS

void FilterPlugin::SetCoeffs(double b0, double b1, double b2, double a0, double a1, double a2)
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
      Coefs[0] = 1;
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
  double alpha = sn * sinh(M_LN2 / 2 * exp(-3 * Res) * omega / sn);

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
      Reamp = 6;
      SetCoeffs(alpha, 0, -alpha, 1 + alpha, - 2 * cs, 1 - alpha);
      break;

    case filter_notch:
      SetCoeffs(1, -2 * cs, 1, 1 + alpha, -2 * cs, 1 - alpha);
      break;

    case filter_peq:
      Reamp = 0.5f;
      SetCoeffs(1 + (alpha * A), -2 * cs, 1 - (alpha * A), 1 + (alpha /A), -2 * cs, 
	       1 - (alpha /A));
      break;
      
    case filter_hp:
      Reamp = 0.15f;
      SetCoeffs(A * ((A + 1) + (A - 1) * cs + beta * sn),
	       -2 * A * ((A - 1) + (A + 1) * cs),
	       A * ((A + 1) + (A - 1) * cs - beta * sn),
	       (A + 1) - (A - 1) * cs + beta * sn,
	       2 * ((A - 1) - (A + 1) * cs),
	       (A + 1) - (A - 1) * cs - beta * sn);
      break;
    }  
}

#endif

void FilterPlugin::Process(float **input, float **output, long sample_length)
{
  long i;
  char chan;

#ifdef FRANCIS
  float out;
  int   j;

  static const float anti_denormal = 1e-18; 

  for (chan = 0; chan < 2; chan++)
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
		
	output[chan][i] = out;
      }
#endif

#ifdef MOOG
  f = Cutoff * 1.16;
  fb = Res * (1.0 - 0.15 * f * f);

  chan = 0;
  for (i = 0; i < sample_length; i++)
    {
      input[chan][i] -= Lout4 * fb;
      input[chan][i] *= 0.35013 * (f * f) * (f * f);
      Lout1 = input[chan][i] + 0.3 * Lin1 + (1 - f) * Lout1; // Pole 1
      Lin1 = input[chan][i];
      Lout2 = Lout1 + 0.3 * Lin2 + (1 - f) * Lout2;  // Pole 2
      Lin2 = Lout1;
      Lout3 = Lout2 + 0.3 * Lin3 + (1 - f) * Lout3;  // Pole 3
      Lin3 = Lout2;
      Lout4 = Lout3 + 0.3 * Lin4 + (1 - f) * Lout4;  // Pole 4
      Lin4 = Lout3;
      output[chan][i] = Lout4;
      if (!isfinite(output[chan][i]))
	output[chan][i] = 0.f;       
      /*      if (IS_DENORMAL(output[chan][i]))
	{
	  cout << "[SEQ] DENORMAL !!!! " <<output[chan][i] << endl;
	  output[chan][i] = 0.f;
	}
      cout << "[PLUG] INPUT Left: " << input[chan][i] << endl;
      cout << "[PLUG] Left: " << output[chan][i] << endl;*/
    }

  chan = 1;
  for (i = 0; i < sample_length; i++)
    {
      input[chan][i] -= Rout4 * fb;
      input[chan][i] *= 0.35013 * (f * f) * (f * f);
      Rout1 = input[chan][i] + 0.3 * Rin1 + (1 - f) * Rout1; // Pole 1
      Rin1 = input[chan][i];
      Rout2 = Rout1 + 0.3 * Rin2 + (1 - f) * Rout2;  // Pole 2
      Rin2 = Rout1;
      Rout3 = Rout2 + 0.3 * Rin3 + (1 - f) * Rout3;  // Pole 3
      Rin3 = Rout2;
      Rout4 = Rout3 + 0.3 * Rin4 + (1 - f) * Rout4;  // Pole 4
      Rin4 = Rout3;
      output[chan][i] = Rout4;
      if (!isfinite(output[chan][i]))
	output[chan][i] = 0.f;
      /*if (IS_DENORMAL(output[chan][i]))
	{
	  cout << "[SEQ] DENORMAL !!!! " <<output[chan][i] << endl;
	  output[chan][i] = 0.f;
	}
            cout << "[PLUG] INPUT Right: " << input[chan][i] << endl;
	      cout << "[PLUG] Right: " << output[chan][i] << endl;*/
    }

  if (IS_DENORMAL(Lout1))
    Lout1 = 0.f;
  if (IS_DENORMAL(Lout2))
    Lout2 = 0.f;
  if (IS_DENORMAL(Lout3))
    Lout3 = 0.f;
  if (IS_DENORMAL(Lout4))
    Lout4 = 0.f;
  if (IS_DENORMAL(Lin1))
    Lin1 = 0.f;
  if (IS_DENORMAL(Lin2))
    Lin2 = 0.f;
  if (IS_DENORMAL(Lin3))
    Lin3 = 0.f;
  if (IS_DENORMAL(Lin4))
    Lin4 = 0.f;

  if (IS_DENORMAL(Rout1))
    Rout1 = 0.f;
  if (IS_DENORMAL(Rout2))
    Rout2 = 0.f;
  if (IS_DENORMAL(Rout3))
    Rout3 = 0.f;
  if (IS_DENORMAL(Rout4))
    Rout4 = 0.f;
  if (IS_DENORMAL(Rin1))
    Rin1 = 0.f;
  if (IS_DENORMAL(Rin2))
    Rin2 = 0.f;
  if (IS_DENORMAL(Rin3))
    Rin3 = 0.f;
  if (IS_DENORMAL(Rin4))
    Rin4 = 0.f;
#endif
}

FilterPlugin::~FilterPlugin()
{
  delete bmp;
}

#ifdef FRANCIS
void FilterPlugin::OnSelect(wxScrollEvent &e)
{
  memset(Coefs, 0, sizeof (float) * FILTER_SIZE);  
  memset(History[0], 0, sizeof (float) * FILTER_SIZE);  
  memset(History[1], 0, sizeof (float) * FILTER_SIZE);  

  SetFilter(FilterSelect->GetValue(), Cutoff, Res);
}
#endif

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
#ifdef FRANCIS
  Cutoff = CutoffFader->GetValue();
  SetFilter(FilterSelect->GetValue(), Cutoff, Res);
#endif

#ifdef MOOG
  Cutoff = CutoffFader->GetValue() / 100.f;

  if (Cutoff <= 0.f)
    Cutoff = 0.01f;
#endif
  cout << "Cutoff: " << Cutoff << "; Res: " << Res << endl;
}
  
void FilterPlugin::OnResonance(wxScrollEvent &e)
{
#ifdef FRANCIS
  Res = ResFader->GetValue() / 100.f;
  SetFilter(FilterSelect->GetValue(), Cutoff, Res);
#endif

#ifdef MOOG
  Res = ResFader->GetValue() / 100.f;
#endif

  cout << "Cutoff: " << Cutoff << "; Res: " << Res << endl;
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
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
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








