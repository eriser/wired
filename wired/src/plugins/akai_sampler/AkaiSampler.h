#ifndef __AKAISAMPLER_H__
#define __AKAISAMPLER_H__
#include<wx/wx.h>
#include "ASClavier.h"
#include "ASKey.h"
#include "Plugin.h"
#include "akai.h"
#include "DownButton.h"
#include "WaveFile.h"
#include "Polyphony.h"
#include "FaderCtrl.h"
#include "HoldButton.h"
#include "ASList.h"

#define IMG_SP_BMP	    "plugins/akaisampler/AkaiSampler.bmp"
#define IMG_SP_OPEN_UP      "plugins/akaisampler/open_up.png"
#define IMG_SP_OPEN_DOWN    "plugins/akaisampler/open_down.png"
#define IMG_SP_SAVE_UP      "plugins/akaisampler/save_up.png"
#define IMG_SP_SAVE_DOWN    "plugins/akaisampler/save_down.png"
#define IMG_SP_UPUP         "plugins/akaisampler/up_up.png"
#define IMG_SP_UPDO         "plugins/akaisampler/up_up.png"
#define IMG_SP_DOWNUP       "plugins/akaisampler/down_up.png"
#define IMG_SP_DOWNDO       "plugins/akaisampler/down_up.png"
#define IMG_SP_LED_OFF      "plugins/akaisampler/led_off.png"
#define IMG_SP_LED_ON       "plugins/akaisampler/led_on.png"
#define IMG_SP_FADER_BG     "plugins/akaisampler/fader_bg.png"
#define IMG_SP_FADER_FG     "plugins/akaisampler/fader_fg.png"

using namespace std;

#define PLUGIN_NAME "Sampler"

class ASamplerKey
{
  public:
    ASamplerKey(t_akaiSample *smp, float pitch = 1.f) :
      AkaiSample(smp), Pitch(pitch)
    {
      Wave = new WaveFile(smp->buffer, smp->size, 2, smp->rate);
      Wave->SetPitch(pitch);
      AkaiSample->end /= 2;
      AkaiSample->loop_start /= 2;
      AkaiSample->loop_len /= 2;
    }
    ~ASamplerKey()
    {
      delete Wave;
      free(AkaiSample);
    }

    t_akaiSample *AkaiSample;
    WaveFile     *Wave;
    float   Pitch;

  protected:

};

class ASamplerNote
{     
  public:
    ASamplerNote(int note, float vol, ASamplerKey *key, int delta, float **buf, int length)
      : Note(note), Volume(vol), Key(key), Delta(delta), Buffer(buf), Length(length),
      Position(key->AkaiSample->start / key->Wave->GetNumberOfChannels())
      {

      }
    ~ASamplerNote()
    {

    }

    int   Note;
    float   Volume;
    ASamplerKey  *Key;
    int   Delta;
    float   **Buffer;
    int   Length;
    long    Position;
};


class AkaiSampler : public Plugin
{
  public:
    AkaiSampler(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
    ~AkaiSampler();

    void   Stop();

    void   Load(int fd, long size);
    long   Save(int fd);

    void   SetBufferSize(long size);
    void   SetSamplingRate(double rate);


    void  Process(float **input, float **output, long sample_length);
    void   ProcessEvent(WiredEvent &event);
    void  OnScroll(wxScrollEvent &e);
    bool   IsInstrument(); 
    bool   IsAudio();
    bool   IsMidi();
    std::string DefaultName() { return "AkaiSampler"; }

    wxBitmap *GetBitmap();

  protected:
    /* Audio/Midi members */
    ASamplerKey    *Keys[127];
    Polyphony   Workshop;
    list<ASamplerNote *> Notes;
    t_akaiProgram   *AkaiProgram;

    float  Volume;
    unsigned int PolyphonyCount;
    double SamplingRate;


    void OnPaint(wxPaintEvent &event);
    void LoadProgram();
    void DeleteProgram();

    wxScrolledWindow      *swg;
    wxScrollBar           *sbh;
    ASClavier       *clavier;
    //wxBitmap *GetBitmap();
    wxBitmap *TpBmp;

    /* Graphic controls */
    wxBitmap *bmp;

    wxImage *sp_bg;
    wxImage *open_up;
    wxImage *open_down;
    wxImage *save_up;
    wxImage *save_down;
    wxImage *up_up;
    wxImage *up_down;
    wxImage *down_up;
    wxImage *down_down;

    wxImage *fader_bg;
    wxImage *fader_fg;

    DownButton *SaveBtn;
    DownButton *OpenBtn;
    wxStaticBitmap *MidiInBmp;
    StaticBitmap   *PianoBmp;

    FaderCtrl *VolumeFader;

    HoldButton *PolyUpBtn;
    HoldButton *PolyDownBtn;
    wxBitmap   *LedOff;
    wxBitmap   *LedOn; 
    wxBitmap   *BgBmp;
    ASList    *test;

    wxStaticText *PolyCountLabel;

    wxMutex    Mutex;

    void OnOpenFile(wxCommandEvent &event);
    void OnSaveFile(wxCommandEvent &event);
    void OnPolyUp(wxCommandEvent &event);
    void OnPolyDown(wxCommandEvent &event);
    //void OnPianoClickDown(wxCommandEvent &event);
    //void OnPianoClickUp(wxCommandEvent &event);
    void OnVolume(wxScrollEvent &event);
    //void OnPaint(wxPaintEvent &event);
    //void LoadProgram();
   // void DeleteProgram();


    DECLARE_EVENT_TABLE()
};

enum
{
  Sampler_PolyUp = 1,
  Sampler_PolyDown,
  Sampler_ShowOpt,
  Sampler_Play,
  Sampler_ToSeqTrack,
  Sampler_Save,
  Sampler_Open,
  Sampler_Invert,
  Sampler_LoFi,
  Sampler_Octave,
  Sampler_Pitch,
  Sampler_Volume,
  Sampler_Attack,
  Sampler_Decay,
  Sampler_Sustain,
  Sampler_Release,
  Sampler_Piano
};  

#endif
