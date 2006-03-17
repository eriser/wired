#ifndef __COMPRESSORPLUG_H__
#define __COMPRESSORPLUG_H__

#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"

#define IMG_CP_BG	wxT("plugins/compressor/compressor.png")
#define IMG_CP_BMP	wxT("plugins/compressor/CompressorPlug.bmp")
#define IMG_CP_FADER_BG	wxT("plugins/compressor/fader_bg.png")
#define IMG_CP_FADER_FG	wxT("plugins/compressor/fader_fg.png")

#define PLUGIN_NAME	wxT("Compressor")

class CompressorPlugin: public Plugin
{
 public:
  CompressorPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~CompressorPlugin();

  void	 Init();
  void	 SetSamplingRate(double rate) { /*SamplingRate = rate; }*/Nyquist = rate / 2; }
  

  void	 Process(float **input, float **output, long sample_length);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  
  bool	 IsAudio();
  bool	 IsMidi();

  wxString DefaultName() { return wxT("Compressor"); }

  void OnButtonClick(wxCommandEvent &e); 
  void OnTarget(wxScrollEvent &e);  
  void OnResonance(wxScrollEvent &e);  
  void OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();
  float		Target;
  float		Res;
  
 protected:
  wxBitmap *bmp;   

  FaderCtrl *TargetFader;
  FaderCtrl *ResFader;
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;

  /*  
  double Threshold;
  double Slope;
  double LookAhead;
  double WindowTime;
  double Attack;
  double Release;
  double AttackTime;
  double ReleaseTime;

  int	LookOffset;
  int	LookCount;
  */

float SamplingRate;

  float gain;
  float NGI;
  float NG[2];
  float MaxGain;
  float Level[2];
  float NGLevel[2];
  float Gate;
  float Attacka;
  float Attackb;
  float Decaya;
  float Decayb;
  float NGa;
  float NGb;  
  float Nyquist;
  
  DECLARE_EVENT_TABLE()  

};

enum
  {
    Compressor_Target = 1,
    Compressor_Res
  };

#endif
