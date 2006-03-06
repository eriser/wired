// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#ifndef _CHORUS_PLUGIN_
#define  _CHORUS_PLUGIN_

#define PLUGIN_NAME	"Chorus"

//Names used t stre and retreive parameters
#define STR_BASE_LENGHT _("BaseLength")
#define STR_MODE_DEPTH _("ModDepth")
#define STR_FREQUENCY _("Frequency")
#define STR_EFFECT_MIX _("EffectMix")

static PlugInitInfo info;

class		ChorusPlugin: public Plugin
{
 public:
		ChorusPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
		~ChorusPlugin();

  void		Init();
  void		Process(float **input, float **output, long sample_length);
  void		CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);

  void		Load(int fd, long size);
  void		Load(WiredPluginData& Datas);
  long		Save(int fd);
  void		Save(WiredPluginData& Datas);
  
  bool		IsAudio();
  bool		IsMidi();

  std::string	DefaultName() { return "Chorus"; }

  void		OnChorusTime(wxScrollEvent &e);  
  void		OnFeedback(wxScrollEvent &e);  
  void		OnDryWet(wxScrollEvent &event);
  void		OnChorusStage(wxScrollEvent &event);
  void		OnPaint(wxPaintEvent &event);
  void		OnBypass(wxCommandEvent &e);

  wxBitmap	*GetBitmap();

  DelayL	delayLine[2];
  WaveLoop	*mods[2];
  Chorus	*chorus1;
  Chorus	*chorus2;


  float		BaseLength;
  float		ModDepth;
  float		Frequency;
  float		EffectMix;

 protected:
  bool		Bypass;

  wxBitmap	*bmp;   

  FaderCtrl	*FrequencyFader;
  FaderCtrl	*BaseLengthFader;
  FaderCtrl	*EffectMixFader;
  FaderCtrl	*ModDepthFader;
  wxImage	*img_fg;
  wxImage	*img_bg;
  wxBitmap	*TpBmp;

  wxImage	*bypass_on;
  wxImage	*bypass_off;
  wxImage	*liquid_on;
  wxImage	*liquid_off;

  StaticBitmap	*Liquid;
  DownButton	*BypassBtn;
  float		*DelayBuffer;
  float		*BufStart[2];
  float		*BufEnd[2];
  float		*BufPtr[2];

  wxMutex	ChorusMutex;

  void		AllocateMem();

  DECLARE_EVENT_TABLE()
};

enum
  {
    Chorus_Bypass = 1,
    Chorus_Time,
    Chorus_Feedback,
    Chorus_Stage,
    Chorus_DryWet,
  };

/******** ChorusPlugin Implementation *********/

BEGIN_EVENT_TABLE(ChorusPlugin, wxWindow)
     EVT_BUTTON(Chorus_Bypass, ChorusPlugin::OnBypass)
     EVT_COMMAND_SCROLL(Chorus_Time, ChorusPlugin::OnChorusTime)
     EVT_COMMAND_SCROLL(Chorus_Feedback, ChorusPlugin::OnFeedback)
     EVT_COMMAND_SCROLL(Chorus_Stage, ChorusPlugin::OnChorusStage)
     EVT_COMMAND_SCROLL(Chorus_DryWet, ChorusPlugin::OnDryWet)
     EVT_PAINT(ChorusPlugin::OnPaint)
END_EVENT_TABLE()

#define IMG_DL_BG	"plugins/chorus/chorus_bg.png"
#define IMG_DL_BMP	"plugins/chorus/ChorusPlug.bmp"
#define IMG_DL_FADER_BG "plugins/chorus/fader_bg.png"
#define IMG_DL_FADER_FG	"plugins/chorus/fader_button.png"
#define IMG_LIQUID_ON	"plugins/chorus/liquid-cristal_play.png"
#define IMG_LIQUID_OFF	"plugins/chorus/liquid-cristal_stop.png"
#define IMG_BYPASS_ON	"plugins/chorus/bypass_button_down.png"
#define IMG_BYPASS_OFF	"plugins/chorus/bypass_button_up.png"
#define SINERAW		"plugins/chorus/sinewave.raw"

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)

#endif
