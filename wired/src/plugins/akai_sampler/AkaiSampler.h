// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

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
#include "CycleKnob.h"
#include "ASSampleList.h"
#include "ASKeygroupList.h"
#include "WiredDocument.h"

#define IMG_SP_BMP	    wxT("plugins/akaisampler/WiredSampler.bmp")
#define IMG_SP_BG	    wxT("plugins/akaisampler/sampler_bg.png")
#define IMG_SP_OPEN_UP      wxT("plugins/akaisampler/sampler_open_up.png")
#define IMG_SP_OPEN_DOWN    wxT("plugins/akaisampler/sampler_open_down.png")
#define IMG_SP_SAVE_UP      wxT("plugins/akaisampler/sampler_save_up.png")
#define IMG_SP_SAVE_DOWN    wxT("plugins/akaisampler/sampler_save_down.png")

#define IMG_SP_POLY_1       wxT("plugins/akaisampler/sampler_polyknob_1.png")
#define IMG_SP_POLY_2       wxT("plugins/akaisampler/sampler_polyknob_2.png")
#define IMG_SP_POLY_3       wxT("plugins/akaisampler/sampler_polyknob_3.png")

#define IMG_SP_LED_OFF      wxT("plugins/akaisampler/sampler_midi_off.png")
#define IMG_SP_LED_ON       wxT("plugins/akaisampler/sampler_midi_on.png")

#define IMG_SP_FADER_BG     wxT("plugins/akaisampler/sampler_fader_bg.png")
#define IMG_SP_FADER_FG     wxT("plugins/akaisampler/sampler_fader_button.png")

#define IMG_SP_SAMPLE_UP    wxT("plugins/akaisampler/sampler_samples_up.png")
#define IMG_SP_SAMPLE_DOWN  wxT("plugins/akaisampler/sampler_samples_down.png")
#define IMG_SP_KGROUP_UP    wxT("plugins/akaisampler/sampler_keygr_up.png")
#define IMG_SP_KGROUP_DOWN  wxT("plugins/akaisampler/sampler_keygr_down.png")
#define IMG_SP_EFFECT_UP    wxT("plugins/akaisampler/sampler_effects_up.png")
#define IMG_SP_EFFECT_DOWN  wxT("plugins/akaisampler/sampler_effects_down.png")

using namespace std;

#define PLUGIN_NAME wxT("WiredSampler")
#define WIRED_SAMPLER_SAVE_PATCH	wxT("WiredSampler/patch.xml")
#define  WIRED_SAMPLER_PATCH_EXT	wxT(".xml")

class ASamplerNote
{     
  public:
    ASamplerNote(int note, float vol, ASamplerKey *key, int delta, float **buf, int length)
      : Note(note), Volume(vol), Key(key), Delta(delta), Buffer(buf), Length(length), Position(key->ass->GetPosition())
      {
      }
    ~ASamplerNote()
    {
      delete Key;
    }

    int   Note;
    float   Volume;
    ASamplerKey  *Key;
    int   Delta;
    float   **Buffer;
    int   Length;
    long    Position;
};

class AkaiSampler : public Plugin, public WiredDocument
{
  public:
    AkaiSampler(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
    ~AkaiSampler();

    void   Stop();

    void	Load(SaveElementArray data);
    void	LoadSamples(SaveElement *data);
    void	LoadKeygroups(SaveElement *data);
    void	LoadPatch(wxString filename);

    void	Save();
    void	SaveSamples(wxString filename = WIRED_SAMPLER_SAVE_PATCH);
    void	SaveKeygroups(wxString filename = WIRED_SAMPLER_SAVE_PATCH);
	void	SaveXmlPatch(wxString filename = WIRED_SAMPLER_SAVE_PATCH);
    //SavePatch declared in WiredDocument.h


    void   SetBufferSize(long size);
    void   SetSamplingRate(double rate);


    void   Process(float **input, float **output, long sample_length);
    void   ProcessEvent(WiredEvent &event);
    void   OnScroll(wxScrollEvent &e);

    void   Update();

    bool   IsInstrument(); 
    bool   IsAudio();
    bool   IsMidi();
    wxString DefaultName() { return wxT("WiredSampler"); }

    wxBitmap *GetBitmap();

    int sampleid;
    int keygroupid;
    vector<ASamplerKeygroup *> Keygroups;
    ASSampleList *Samples;
    ASPlugPanel *PlugPanel;

  protected:
    /* Audio/Midi members */
//    ASamplerKey    *Keys[127];
    Polyphony   Workshop;
    list<ASamplerNote *> Notes;
    t_akaiProgram   *AkaiProgram;
    wxString        AkaiPrefix;

    float  Volume;
    unsigned int PolyphonyCount;
    double SamplingRate;

    void OnPaint(wxPaintEvent &event);
    void LoadProgram();
    void DeleteProgram();

    /* Graphic controls */

    wxScrolledWindow      *swg;
    wxScrollBar           *sbh;
    ASClavier		  *clavier;

    wxBitmap *BgBmp;
    wxBitmap *bmp;
    wxBitmap *LedOff;
    wxBitmap *LedOn; 

    wxImage *sp_bg;
    wxImage *open_up;
    wxImage *open_down;
    wxImage *save_up;
    wxImage *save_down;

    wxImage *fader_bg;
    wxImage *fader_fg;

    DownButton *SaveBtn;
    DownButton *OpenBtn;
    wxStaticBitmap *MidiInBmp;
    StaticBitmap   *PianoBmp;

    FaderCtrl *VolumeFader;

    CycleKnob *PolyKnob;

    ASamplerSample *ass;

    wxStaticText *PolyCountLabel;

    wxMutex    Mutex;

    DownButton *btsample;
    DownButton *btkgroup;
    DownButton *bteffect;

    int		MidiVolume[2];

    bool	MidiInOn;
    bool	UpdateMidi;
    bool	UpdateVolume;

    void ProcessMidiControls(WiredEvent &event);
    
    void OnSampleButton(wxCommandEvent &event);
    void OnKgroupButton(wxCommandEvent &event);
    void OnEffectButton(wxCommandEvent &event);
    void OnSelectEffect(wxCommandEvent &event);
    void OnAddEffect(wxCommandEvent &event);

    void OnOpenFile(wxCommandEvent &event);
    void OnSaveFile(wxCommandEvent &event);
    //void OnPianoClickDown(wxCommandEvent &event);
    //void OnPianoClickUp(wxCommandEvent &event);
    void OnPolyphony(wxCommandEvent &event);
    void OnVolume(wxScrollEvent &event);
    //void OnPaint(wxPaintEvent &event);
    //void LoadProgram();
   // void DeleteProgram();

	wxString	GetName();
	wxString	_customFileName;

    DECLARE_EVENT_TABLE()
};

enum
{
  Sampler_Poly,
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
  Sampler_SampleBt,
  Sampler_KgroupBt,
  Sampler_EffectBt,
};  

#endif
