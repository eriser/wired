#ifndef _ASKEYGROUPLIST_H_
#define _ASKEYGROUPLIST_H_

#include <wx/wx.h>
#include <math.h>
#include "ASPlugin.h"
#include "Plugin.h"
#include "ASList.h"
#include "WaveFile.h"
#include "akai.h"

using namespace std;

#define IMAGE_BT_ADD_KEYGROUP     "plugins/akaisampler/add.png"
#define IMAGE_BT_DEL_KEYGROUP    "plugins/akaisampler/del.png"

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
      this->Position = 0;
      if (!id)
        id = sampleid++;
      else
        if (id > sampleid)
          sampleid = id + 1;
      this->id = id;
    }

    ASamplerSample(t_akaiSample *smp, wxString AkaiPrefix, unsigned long id = 0)
    {
      this->w = new WaveFile(smp->buffer, smp->size, 2, smp->rate);
      w->Filename = AkaiPrefix + smp->name;
      this->askg = NULL;
      this->Position = smp->start / 2;
      if (!id)
        id = sampleid++;
      else
        if (id > sampleid)
          sampleid = id + 1;
      this->id = id;
    }

    void SetKeygroup(ASamplerKeygroup *askg) { this->askg = askg; }
    ASamplerKeygroup *GetKeygroup() { return askg; }
    WaveFile *GetSample() { return w; }
    long GetPosition() { return Position; }
    unsigned long GetID() { return id; }
  private:
    WaveFile *w;
    ASamplerKeygroup *askg;
    long Position;
    unsigned long id;
};

class ASamplerKey
{
  public:
    ASamplerKey(WaveFile *w, long pos = 0, float pitch = 1.f) :
      Pitch(pitch)
    {
      Wave = w;
      Position = pos;
    }

    ~ASamplerKey()
    {
    }

    WaveFile  *Wave;
    float   Pitch;
    long Position;

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
      return new ASamplerKey(smp->GetSample(), smp->GetPosition(), powf(2.f, static_cast<float>(num - lokey) / 12.f)); 
    }
    int GetLowKey() { return lokey; }
    int GetHighKey() { return hikey; }
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
    void OnAddKeygroup(wxCommandEvent &);
    void OnDelKeygroup(wxCommandEvent &);
    void OnResize(wxSizeEvent &);
    ASamplerKeygroup *FindKeygroup(int key);
    vector <ASListEntry *> GetEntries() { return List->GetEntries(); }
    ASList *List;

  DECLARE_EVENT_TABLE()
};

extern ASKeygroupList *Keygroups;

enum 
{
  ASKeygroupList_AddKeygroup = 1234,
  ASKeygroupList_DelKeygroup,
};

#endif
