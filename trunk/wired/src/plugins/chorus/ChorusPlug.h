// Copyright (C) 2005 by Wired Team
// Under the GNU General Public License

#ifndef _CHORUS_PLUGIN_
#define  _CHORUS_PLUGIN_

#define PLUGIN_NAME	"Chorus"

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
  long		Save(int fd);
  
  bool		IsAudio();
  bool		IsMidi();

  std::string	DefaultName() { return "Chorus"; }

  void		OnChorusTime(wxScrollEvent &e);  
  void		OnFeedback(wxScrollEvent &e);  
  void		OnDryWet(wxScrollEvent &event);
  void		OnChorusStage(wxScrollEvent &event);
  void		OnPaint(wxPaintEvent &event);

  wxBitmap	*GetBitmap();

  DelayL	delayLine[2];
  WaveLoop	*mods[2];
  Chorus	*chorus1;
  Chorus	*chorus2;


  double	BaseLength;
  double	ModDepth;
  double	Frequency;
  double	EffectMix;

 protected:
  wxBitmap	*bmp;   

  FaderCtrl	*FrequencyFader;
  FaderCtrl	*BaseLengthFader;
  FaderCtrl	*EffectMixFader;
  FaderCtrl	*ModDepthFader;
  wxImage	*img_fg;
  wxImage	*img_bg;
  wxBitmap	*TpBmp;




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
    Chorus_Time = 1,
    Chorus_Feedback,
    Chorus_Stage,
    Chorus_DryWet,
  };

/******** ChorusPlugin Implementation *********/

BEGIN_EVENT_TABLE(ChorusPlugin, wxWindow)
  EVT_COMMAND_SCROLL(Chorus_Time, ChorusPlugin::OnChorusTime)
  EVT_COMMAND_SCROLL(Chorus_Feedback, ChorusPlugin::OnFeedback)
  EVT_COMMAND_SCROLL(Chorus_Stage, ChorusPlugin::OnChorusStage)
  EVT_COMMAND_SCROLL(Chorus_DryWet, ChorusPlugin::OnDryWet)
  EVT_PAINT(ChorusPlugin::OnPaint)
END_EVENT_TABLE()

#define IMG_DL_BG	"plugins/chorus/chorus.png"
#define IMG_DL_BMP	"plugins/chorus/ChorusPlug.bmp"
#define IMG_DL_FADER_BG "plugins/chorus/fader_bg.png"
#define IMG_DL_FADER_FG	"plugins/chorus/fader_fg.png"
#define SINERAW		"plugins/chorus/sinewave.raw"

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)

#endif
