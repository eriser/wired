#ifndef __FILTERPLUG_H__
#define __FILTERPLUG_H__

#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"
#include "KnobCtrl.h"

#define PLUGIN_NAME	"Filter"

#define IMG_FL_BG	"plugins/filter/filter.png"
#define IMG_FL_BMP	"plugins/filter/FilterPlug.bmp"
#define IMG_FL_FADER_BG	"plugins/filter/fader_bg.png"
#define IMG_FL_FADER_FG	"plugins/filter/fader_fg.png"
#define IMG_FL_KNOB_BG	"plugins/filter/knob_bg.png"
#define IMG_FL_KNOB_FG	"plugins/filter/knob_fg.png"

static PlugInitInfo info;

#define FRANCIS

class FilterPlugin: public Plugin
{
 public:
  FilterPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~FilterPlugin();

  void	 Init();
  void	 Process(float **input, float **output, long sample_length);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  void	 Load(int fd, long size);
  long   Save(int fd);
  
  void  SetSamplingRate(double rate)
    { 
#ifdef FRANCIS
      SamplePeriod = 1.f / rate;
#endif
    }
  
  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Filter"; }

#ifdef FRANCIS
  void SetFilter(int type, float cutoff, float resonance);
  void OnSelect(wxScrollEvent &e);  

#endif

  void OnButtonClick(wxCommandEvent &e); 
  void OnCutoff(wxScrollEvent &e);  
  void OnResonance(wxScrollEvent &e);  
  void OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();
  float		Cutoff;
  float		Res;
  
 protected:
  wxBitmap *bmp;   

  FaderCtrl *CutoffFader;
  FaderCtrl *ResFader;
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;

  DECLARE_EVENT_TABLE()  

#ifdef FRANCIS

  KnobCtrl  *FilterSelect;

#define FILTER_SIZE	5		// 3 inputs, 2 outputs
  float	History[2][FILTER_SIZE];
  float	Coefs[FILTER_SIZE];
  float	Reamp;
  float SamplePeriod;

  void  SetCoeffs(double b0, double b1, double b2, double a0, double a1, double a2);
#endif

#ifdef MOOG
  float Lout1;
  float Lout2;
  float Lout3;
  float Lout4;
  float Lin1;
  float Lin2;
  float Lin3;
  float Lin4;

  float Rout1;
  float Rout2;
  float Rout3;
  float Rout4;
  float Rin1;
  float Rin2;
  float Rin3;
  float Rin4;

  double f;
  double fb;
#endif
};

enum
  {
    Filter_Cutoff = 1,
    Filter_Res,
    Filter_Select
  };

#ifdef FRANCIS
enum
  {
    filter_lp = 0,
    filter_bp,
    filter_notch,
    filter_hp,
    filter_peq,
    
    filter_count
  };

#endif

#endif
