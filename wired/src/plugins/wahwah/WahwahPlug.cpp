// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#include "WahwahPlug.h"

#define lfoskipsamples 30

static PlugInitInfo info;

BEGIN_EVENT_TABLE(EffectWahwah, wxWindow)
  EVT_COMMAND_SCROLL(Wahwah_Frequency, EffectWahwah::OnFrequency)
  EVT_COMMAND_SCROLL(Wahwah_StartPhase, EffectWahwah::OnStartPhase)
  EVT_COMMAND_SCROLL(Wahwah_Depth, EffectWahwah::OnDepth)
  EVT_COMMAND_SCROLL(Wahwah_FreqOfs, EffectWahwah::OnFreqOfs)
  EVT_COMMAND_SCROLL(Wahwah_Res, EffectWahwah::OnRes)
  EVT_PAINT(EffectWahwah::OnPaint)
END_EVENT_TABLE()

EffectWahwah::EffectWahwah(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  cout << "WahWah - Construction" << endl;
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_RV_BG)).c_str(), wxBITMAP_TYPE_PNG);
  TpBmp = new wxBitmap(tr_bg);
  
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_RV_BMP)).c_str(), wxBITMAP_TYPE_BMP);
  img_bg = new wxImage(string(GetDataDir() + string(IMG_RV_KNOB_BG)).c_str(),wxBITMAP_TYPE_PNG);
  img_fg = new wxImage(string(GetDataDir() + string(IMG_RV_KNOB_FG)).c_str(),wxBITMAP_TYPE_PNG);
  
  FreqFader = new FaderCtrl(this, Wahwah_Frequency, img_bg, img_fg, 0, TO_GUI_FREQ(4.0), TO_GUI_FREQ(DEFAULT_FREQ),
			    wxPoint(8, 8), wxSize(22, 78));
  StartPhaseFader = new FaderCtrl(this, Wahwah_StartPhase, img_bg, img_fg, 0, 1, DEFAULT_STARTPHASE,
				  wxPoint(35, 8), wxSize(22, 78));
  DepthFader = new FaderCtrl(this, Wahwah_Depth, img_bg, img_fg, 0, 100, TO_GUI_DEPTH(DEFAULT_DEPTH),
			     wxPoint(72, 8), wxSize(22, 78));
  FreqOfsFader = new FaderCtrl(this, Wahwah_FreqOfs, img_bg, img_fg, 0, 100, TO_GUI_FREQOFS(DEFAULT_FREQOFS),
			     wxPoint(100, 8), wxSize(22, 78));
  ResFader = new FaderCtrl(this, Wahwah_Res, img_bg, img_fg, 0, TO_GUI_RES(10.0), TO_GUI_RES(DEFAULT_RES),
			   wxPoint(142, 8), wxSize(22, 78));
  SetBackgroundColour(wxColour(237, 237, 237));
}

void		EffectWahwah::Init()
{
  // TODO 
  // FreqFader->GetValue();
  // FreqFader->SetValue(TO_GUI_FREQ(DEFAULT_FREQ));
  // cout << "after setValue" << endl;
  // StartPhaseFader->SetValue(DEFAULT_STARTPHASE);
  // DepthFader->SetValue(TO_GUI_DEPTH(DEFAULT_DEPTH));
  // FreqOfsFader->SetValue(TO_GUI_FREQOFS(DEFAULT_FREQOFS));
  // ResFader->SetValue(TO_GUI_RES(DEFAULT_RES));
  // Should not segfault

  cout << "WahWah - Initialisation" << endl;
  LeftChannel.SetValues(DEFAULT_FLOAT_FREQ, DEFAULT_STARTPHASE, DEFAULT_FLOAT_DEPTH, 
			DEFAULT_FLOAT_FREQOFS, DEFAULT_FLOAT_RES);
  RightChannel.SetValues(DEFAULT_FLOAT_FREQ, DEFAULT_STARTPHASE, DEFAULT_FLOAT_DEPTH, 
			DEFAULT_FLOAT_FREQOFS, DEFAULT_FLOAT_RES);
  LeftChannel.NewTrackSimpleMono();
  
  RightChannel.NewTrackSimpleMono();
}

bool		EffectWahwah::IsAudio()
{
  return (true);
}

bool		EffectWahwah::IsMidi()
{
  return (true);
}

wxBitmap*	EffectWahwah::GetBitmap()
{
  return (bmp);
}

void		EffectWahwah::OnFrequency(wxScrollEvent &e)
{
  LeftChannel.freq = FROM_GUI_FREQ(FreqFader->GetValue());
  LeftChannel.lfoskip = LeftChannel.freq * 2 * M_PI / mCurRate;
  RightChannel.freq = LeftChannel.freq;
  RightChannel.lfoskip = RightChannel.freq * 2 * M_PI / mCurRate;
  cout << "Frequency: " << LeftChannel.freq << endl;
}

void		EffectWahwah::OnStartPhase(wxScrollEvent &e)
{
  float	shouldDo;

  shouldDo = (float) StartPhaseFader->GetValue();
  if (shouldDo == 0)
    {
      LeftChannel.startphase = M_PI;
      LeftChannel.phase = M_PI;
      RightChannel.startphase = M_PI;
      RightChannel.phase = M_PI;
    }
  else
    {
      LeftChannel.startphase = M_PI;
      LeftChannel.phase = M_PI;
      RightChannel.startphase = 0;
      RightChannel.phase = 0;
    }
  cout << "Start Phase: " << LeftChannel.startphase << endl;
}

void		EffectWahwah::OnDepth(wxScrollEvent &e)
{
  LeftChannel.depth = FROM_GUI_DEPTH(DepthFader->GetValue());
  RightChannel.depth = LeftChannel.depth;
  cout << "Depth: " << LeftChannel.depth << endl;
}

void		EffectWahwah::OnFreqOfs(wxScrollEvent &e)
{
  LeftChannel.freqofs = FROM_GUI_FREQOFS(FreqOfsFader->GetValue());
  RightChannel.freqofs = LeftChannel.freqofs;
  cout << "Frequency OFS : " << LeftChannel.freqofs << endl;
}

void		EffectWahwah::OnRes(wxScrollEvent &e)
{
  LeftChannel.res = FROM_GUI_RES(ResFader->GetValue());
  RightChannel.res = LeftChannel.res;
  cout << "Res: " << LeftChannel.res << endl;
}

void		EffectWahwah::OnPaint(wxPaintEvent &event)
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
}

void		EffectWahwah::Load(int fd, long size)
{
  t_plugParams	params;

  if (read (fd, &params, size) <= 0)
    {
      cout << "[WAHWAHPLUG] Error while loading patch !" << endl;
    }
  else
    {
      ResFader->SetValue((int)(TO_GUI_RES(params.res)));
      FreqOfsFader->SetValue((int)(TO_GUI_FREQOFS(params.freqofs)));
      FreqFader->SetValue((int)(TO_GUI_FREQ(params.freq)));
      StartPhaseFader->SetValue((int)(params.startphase * M_PI));
      DepthFader->SetValue((int)(TO_GUI_DEPTH(params.depth)));
      LeftChannel.SetValues(params);
      RightChannel.SetValues(params);
    }
}

long		EffectWahwah::Save(int fd)
{
  long retSize;
  t_plugParams	*params;
  
  params = LeftChannel.GetValues();
  retSize = write (fd, &(*params), sizeof(t_plugParams));
  return (retSize);
}

void		EffectWahwah::Process(float **input, float **output, long sample_length) 
{
  LeftChannel.ProcessSimpleMono(input[0], output[0], sample_length);
  RightChannel.ProcessSimpleMono(input[1], output[1], sample_length);
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "Wah");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new EffectWahwah(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}


/********** WahWah **********/

void		Wahwah::SetValues(float paramFreq, float paramStartPhase, float paramDepth, 
				  float paramFreqofs, float paramRes)
{
  freq = paramFreq;
  startphase = paramStartPhase;
  depth = paramDepth;
  freqofs = paramFreqofs;
  res = paramRes;
}

void		Wahwah::SetValues(const t_plugParams &params)
{
  freq = params.freq;
  freqofs = params.freqofs;
  startphase = params.startphase;
  depth = params.depth;
  res = params.res;
}
 
t_plugParams	*Wahwah::GetValues()
{
  t_plugParams	*returnValue = new t_plugParams;

  returnValue->freq = freq;
  returnValue->freqofs = freqofs;
  returnValue->startphase = startphase;
  returnValue->depth = depth;
  returnValue->res = res;
  return (returnValue);
}

bool		Wahwah::NewTrackSimpleMono()
{
   lfoskip = freq * 2 * M_PI / mCurRate;
   skipcount = 0;
   xn1 = 0;
   xn2 = 0;
   yn1 = 0;
   yn2 = 0;
   b0 = 0;
   b1 = 0;
   b2 = 0;
   a0 = 0;
   a1 = 0;
   a2 = 0;
   phase = startphase;
   return true;
}

bool		Wahwah::ProcessSimpleMono(float *inbuffer, float *outbuffer, long len)
{
   float frequency, omega, sn, cs, alpha;
   float in, out;

   for (long i = 0; i < len; i++) {
      in = inbuffer[i];
      
      if ((skipcount++) % lfoskipsamples == 0) {
         frequency = (1 + cos(skipcount * lfoskip + phase)) / 2;
         frequency = frequency * depth * (1 - freqofs) + freqofs;
         frequency = exp((frequency - 1) * 6);
         omega = M_PI * frequency;
         sn = sin(omega);
         cs = cos(omega);
         alpha = sn / (2 * res);
         b0 = (1 - cs) / 2;
         b1 = 1 - cs;
         b2 = (1 - cs) / 2;
         a0 = 1 + alpha;
         a1 = -2 * cs;
         a2 = 1 - alpha;
      };
      out = (b0 * in + b1 * xn1 + b2 * xn2 - a1 * yn1 - a2 * yn2) / a0;
      xn2 = xn1;
      xn1 = in;
      yn2 = yn1;
      yn1 = out;
      
      // Prevents clipping
      if (out < -1.0)
         out = float(-1.0);
      else if (out > 1.0)
         out = float(1.0);
      
      outbuffer[i] = (float) out;
   }

   return true;
}
