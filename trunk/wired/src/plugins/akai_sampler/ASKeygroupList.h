#ifndef _ASKEYGROUPLIST_H_
#define _ASKEYGROUPLIST_H_

#include <wx/wx.h>
#include <math.h>
#include "ASPlugin.h"
#include "Plugin.h"
#include "ASList.h"
#include "WaveFile.h"
#include "akai.h"
#include "ASKeygroupEditor.h"

using namespace std;

class ASamplerKeygroup;

class ASamplerSample
{
  public:
    ASamplerSample(class AkaiSampler *as, WaveFile *w, unsigned long id = 0);
    ASamplerSample(class AkaiSampler *as, t_akaiSample *smp, wxString AkaiPrefix, unsigned long id = 0);
    ~ASamplerSample();
    void SetKeygroup(ASamplerKeygroup *askg) { this->askg = askg; }
    ASamplerKeygroup *GetKeygroup() { return askg; }
    void SetKgEditor(class ASKeygroupEditor *aske) { this->aske = aske; }
    class ASKeygroupEditor *GetKgEditor() { return aske; }
    WaveFile *GetSample() { return w; }
    long GetPosition() { return Position; }
    unsigned long GetID() { return id; }
    int GetLoopCount() { return loopcount; }
    long GetLoopStart() { return loopstart; }
    long GetLoopEnd() { return loopend; }
    void SetLoopCount(int count) { loopcount = count; }
    void SetLoopStart(long start) { loopstart = start; }
    void SetLoopEnd(long end) { loopend = end; }
    void AddEffect(ASPlugin *effect) { this->effects.push_back(effect); }
    vector<ASPlugin *> GetEffects() { return effects; }
  private:
    WaveFile *w;
    ASamplerKeygroup *askg;
    ASKeygroupEditor *aske;
    long Position;
    unsigned long id;
    long loopstart;
    long loopend;
    int loopcount;
    vector<ASPlugin *> effects;
    class AkaiSampler *as;
};

class ASamplerKey
{
  public:
    ASamplerKey(ASamplerSample *ass, float pitch = 1.f) :
      Pitch(pitch)
    {
      this->ass = ass;
      loops = 0;
      looping = false;
    }

    ~ASamplerKey()
    {
    }

    ASamplerSample *ass;
    float   Pitch;
    int loops;
    bool looping;
  protected:
};

class ASamplerKeygroup
{
  public:
    ASamplerKeygroup(class AkaiSampler *as, int lo, int hi, unsigned long id = 0);
    ~ASamplerKeygroup();
    void SetSample(ASamplerSample *s) { smp = s; }
    ASamplerSample *GetSample() { return smp; }
    int HasKey(int key) { return ((key >= lokey) && (key <= hikey)); }
    unsigned long GetID() { return id; }
    ASamplerKey *GetKey(int num)
    {
      if (!smp)
        return NULL;
      return new ASamplerKey(smp, powf(2.f, static_cast<float>(num - lokey) / 12.f)); 
    }
    int GetLowKey() { return lokey; }
    int GetHighKey() { return hikey; }
    void SetLowKey(int lokey) { this->lokey = lokey; }
    void SetHighKey(int hikey) { this->hikey = hikey; }
  private:
    ASamplerSample *smp;
    int lokey;
    int hikey;
    unsigned long id;
    class AkaiSampler *as;
};

#endif
