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

/*
#define IMAGE_BT_ADD_KEYGROUP     "plugins/akaisampler/add.png"
#define IMAGE_BT_DEL_KEYGROUP    "plugins/akaisampler/del.png"
#define IMAGE_BT_EDIT_KEYGROUP    "plugins/akaisampler/assign.png"
*/

class ASamplerKeygroup;

extern unsigned long sampleid;
extern unsigned long keygroupid;

class ASamplerSample
{
  public:
    ASamplerSample(WaveFile *w, unsigned long id = 0)
    {
      this->w = w;
      this->askg = NULL;
      this->aske = NULL;
      this->Position = 0;
      if (!id)
        id = sampleid++;
      else
        if (id > sampleid)
          sampleid = id + 1;
      this->id = id;
      this->loopcount = -1;
      this->loopstart = 0;
      this->loopend = w->GetNumberOfFrames();
      this->effect = NULL;
    }

    ASamplerSample(t_akaiSample *smp, wxString AkaiPrefix, unsigned long id = 0)
    {
      this->w = new WaveFile(smp->buffer, smp->size, 2, smp->rate);
      w->Filename = AkaiPrefix + smp->name;
      this->askg = NULL;
      this->aske = NULL;
      this->Position = smp->start / 2;
      if (!id)
        id = sampleid++;
      else
        if (id > sampleid)
          sampleid = id + 1;
      this->id = id;
      this->loopcount = smp->loop_times;
      this->loopstart = smp->loop_start / 2;
      this->loopend = smp->loop_len / 2;
      this->effect = NULL;
    }

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
    void SetEffect(ASPlugin *effect) { this->effect = effect; }
    ASPlugin *GetEffect() { return effect; }
  private:
    WaveFile *w;
    ASamplerKeygroup *askg;
    ASKeygroupEditor *aske;
    long Position;
    unsigned long id;
    long loopstart;
    long loopend;
    int loopcount;
    ASPlugin *effect;
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
    ASamplerKeygroup(int lo, int hi, unsigned long id = 0)
    {
      lokey = lo;
      hikey = hi;
      smp = NULL;
      if (!id)
        id = keygroupid++;
      else
        if (id > keygroupid)
          keygroupid = id + 1;
      this->id = id;
    }
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
};

class ASKeygroupList : public ASPlugin
{
  public:
    ASKeygroupList(wxString Name);
    ~ASKeygroupList();
    wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
//    void OnAddKeygroup(wxCommandEvent &);
//    void OnDelKeygroup(wxCommandEvent &);
//    void OnEditKeygroup(wxCommandEvent &);
    void OnResize(wxSizeEvent &);
    ASamplerKeygroup *FindKeygroup(int key);
    vector <ASListEntry *> GetEntries() { return List->GetEntries(); }
    ASList *List;

  DECLARE_EVENT_TABLE()
};

extern ASKeygroupList *Keygroups;

/*
enum 
{
  ASKeygroupList_AddKeygroup = 1234,
  ASKeygroupList_DelKeygroup,
  ASKeygroupList_EditKeygroup,
};
*/

#endif
