// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#ifndef		__WWHAWHA_H__
#define		__WWHAWHA_H__

#include	"../redist/Plugin.h"

#include	<math.h>
#include	<wx/wxprec.h>
#ifndef		WX_PRECOMP
#include	<wx/wx.h>
#endif

#include	"Plugin.h"
#include	"FaderCtrl.h"
#include	"HintedFader.h"
#include	"DownButton.h"


#define		PLUGIN_NAME	"Wahwah"

/* Desssin */
#define		IMG_WW_BG	"plugins/wahwah/wah-wah_bg.png"
#define		IMG_WW_BMP	"plugins/wahwah/ReverbPlug.bmp"
#define		IMG_WW_FADER_BG	"plugins/wahwah/fader_bg.png"
#define		IMG_WW_FADER_FG	"plugins/wahwah/fader_button.png"

/* Initialisation */
#define		DEFAULT_FREQ		1.5
#define		DEFAULT_FLOAT_FREQ	(float)1.5
#define		DEFAULT_STARTPHASE	0
#define		DEFAULT_DEPTH		0.7
#define		DEFAULT_FLOAT_DEPTH	(float)0.7
#define		DEFAULT_FREQOFS		0.3
#define		DEFAULT_FLOAT_FREQOFS	(float)0.3
#define		DEFAULT_RES		2.5
#define		DEFAULT_FLOAT_RES	(float)2.5

/* Gestion des parametres */
#define		FROM_GUI_FREQ(X)	((float)X) / 10
#define		TO_GUI_FREQ(X)		(long)(X * 10)
#define		FROM_GUI_DEPTH(X)	((float)X) / 100
#define		TO_GUI_DEPTH(X)		(long)(X * 100)
#define		FROM_GUI_FREQOFS(X)	((float)X) / 100
#define		TO_GUI_FREQOFS(X)	(long)(X * 100)
#define		FROM_GUI_RES(X)		((float)X) / 10
#define		TO_GUI_RES(X)		(long)(X * 10)

#define		mCurRate		44110.f
#define		mCurChannel		1

#ifndef M_PI
#define M_PI            3.14159265358979323846  /* pi */
#endif

typedef struct s_plugParams
{
  float freq;
  float startphase;
  float depth;
  float freqofs;
  float res;
}	       t_plugParams;

#define STR_FREQUENCY "Frequency"
#define STR_START_PHASE "StartPhase"
#define STR_DEPTH "Depth"
#define STR_FREQUENCY_OFS "FrequencyOFS"
#define STR_RESOLUTION "Resolution"

#define IMG_LIQUID_ON	"plugins/reverb/liquid-cristal_play.png"
#define IMG_LIQUID_OFF	"plugins/reverb/liquid-cristal_stop.png"
#define IMG_BYPASS_ON	"plugins/reverb/bypass_button_down.png"
#define IMG_BYPASS_OFF	"plugins/reverb/bypass_button_up.png"

 
class Wahwah
{
  public :

  Wahwah(){};
  ~Wahwah(){};

  float freq;
  float startphase;
  float depth;
  float freqofs;
  float res;

  /* Effet Wahwah */
  float phase;
  float lfoskip;
  unsigned long skipcount;
  float xn1, xn2, yn1, yn2;
  float b0, b1, b2, a0, a1, a2;

  void		SetValues(const t_plugParams &params);
  void		SetValues(float paramFreq, float paramStartPhase, float paramDepth, float Freqofs, float res);
  t_plugParams	*GetValues();
  bool		NewTrackSimpleMono();
  bool		ProcessSimpleMono(float *inbuffer, float *outbuffer, long len);

};

class EffectWahwah : public Plugin
{
  /* From api */

 public:
  EffectWahwah(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~EffectWahwah(){};
  void	 Init();

  void	 Process(float **input, float **output, long sample_length);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  
  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Wahwah"; }

  void	OnFrequency(wxScrollEvent &e);
  void	OnStartPhase(wxScrollEvent &e);
  void	OnDepth(wxScrollEvent &e);
  void	OnFreqOfs(wxScrollEvent &e);
  void	OnRes(wxScrollEvent &e);
  void	OnPaint(wxPaintEvent &event);
  void	OnBypass(wxCommandEvent &e);
  void	Load(int fd, long size);
  long	Save(int fd);
  void	Load(WiredPluginData& Datas);
  void	Save(WiredPluginData& Datas);

  wxBitmap	*GetBitmap();
  float		Wide;

 private:

  Wahwah	LeftChannel;
  Wahwah	RightChannel;

 protected:

  wxBitmap	*bmp;   

  bool		Bypass;

  FaderCtrl	*FreqFader;
  FaderCtrl	*StartPhaseFader;
  FaderCtrl	*DepthFader;
  FaderCtrl	*FreqOfsFader;
  FaderCtrl	*ResFader;
  DownButton	*BypassBtn;

  wxImage	*img_fg;
  wxImage	*img_bg;
  wxImage	*bypass_on;
  wxImage	*bypass_off;
  wxImage	*liquid_on;
  wxImage	*liquid_off;
  
  StaticBitmap	*Liquid;
  wxBitmap	*TpBmp;

  wxMutex	WahwahMutex;

  DECLARE_EVENT_TABLE()  

};

enum
  {
    Wahwah_Bypass,
    Wahwah_Frequency,
    Wahwah_StartPhase,
    Wahwah_Depth,
    Wahwah_FreqOfs,
    Wahwah_Res,
  };

#endif
