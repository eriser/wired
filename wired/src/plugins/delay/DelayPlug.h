#ifndef __DELAYPLUG_H__
#define __DELAYPLUG_H__

#include <math.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Plugin.h"
#include "FaderCtrl.h"
#include "StaticBitmap.h"
#include "DownButton.h"

#define PLUGIN_NAME	"Delay"

#define IMG_DL_BG	"plugins/delay/delay_bg.png"
#define IMG_DL_BMP	"plugins/delay/DelayPlug.bmp"
#define IMG_DL_FADER_BG	"plugins/delay/fader_bg.png"
#define IMG_DL_FADER_FG	"plugins/delay/fader_button.png"
#define IMG_LIQUID_ON	"plugins/delay/liquid-cristal_play.png"
#define IMG_LIQUID_OFF	"plugins/delay/liquid-cristal_stop.png"
#define IMG_BYPASS_ON	"plugins/delay/bypass_button_down.png"
#define IMG_BYPASS_OFF	"plugins/delay/bypass_button_up.png"

#define MAX_TIME	5000

class DelayPlugin: public Plugin
{
 public:
  DelayPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~DelayPlugin();

  void	 Init();
  void	 Process(float **input, float **output, long sample_length);
  void	 ProcessEvent(WiredEvent &event);
  void	 CreateGui(wxWindow *rack, wxPoint &pos, wxSize &size);
  
  void	 Load(int fd, long size);
  long	 Save(int fd);

  bool	 IsAudio();
  bool	 IsMidi();

  std::string DefaultName() { return "Delay"; }

  void	 OnBypass(wxCommandEvent &e);  
  void	 OnDelayTime(wxScrollEvent &e);  
  void   OnFeedback(wxScrollEvent &e);  
  void   OnDryWet(wxScrollEvent &event);
  void   OnPaint(wxPaintEvent &event);
  void	 OnBypassController(wxMouseEvent &event);
  void	 OnTimeController(wxMouseEvent &event);
  void	 OnFeedbackController(wxMouseEvent &event);
  void	 OnDryWetController(wxMouseEvent &event);

  wxBitmap	*GetBitmap();

  float		DelayTime;
  float		Feedback;
  float		WetLevel;
  float		DryLevel;
  
 protected:
  bool		Bypass;

  int		MidiBypass[2];
  int		MidiTime[2];
  int		MidiFeedback[2];
  int		MidiDryWet[2];

  wxBitmap *bmp;   

  FaderCtrl *TimeFader;
  FaderCtrl *FeedbackFader;
  FaderCtrl *DryWetFader;
  wxImage *img_fg;
  wxImage *img_bg;
  wxBitmap *TpBmp;
  wxImage *bypass_on;
  wxImage *bypass_off;
  wxImage *liquid_on;;
  wxImage *liquid_off;;
  StaticBitmap *Liquid;
  DownButton *BypassBtn;

  float *DelayBuffer;
  float *BufStart[2];
  float *BufEnd[2];
  float *BufPtr[2];

  wxMutex DelayMutex;

  void AllocateMem();
  void CheckExistingControllerData(int MidiData[3]);

  DECLARE_EVENT_TABLE()  
};

enum
  {
    Delay_Bypass = 1,
    Delay_Time,
    Delay_Feedback,
    Delay_DryWet,
  };

#endif
