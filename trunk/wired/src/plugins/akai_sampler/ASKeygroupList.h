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

class ASamplerSample
{
  public:
    ASamplerSample(WaveFile *w)
    {
      this->w = w;
      this->askg = NULL;
      this->Position = 0;
    }

    ASamplerSample(t_akaiSample *smp)
    {
      this->w = new WaveFile(smp->buffer, smp->size, 2, smp->rate);
      this->askg = NULL;
      this->Position = smp->start / 2;
    }

    void SetKeygroup(ASamplerKeygroup *askg) { this->askg = askg; }
    ASamplerKeygroup *GetKeygroup() { return askg; }
    WaveFile *GetSample() { return w; }
    long GetPosition() { return Position; }
  private:
    WaveFile *w;
    ASamplerKeygroup *askg;
    long Position;
};

class ASamplerKey
{
  public:
    ASamplerKey(WaveFile *w, long pos = 0, float pitch = 1.f) :
      Pitch(pitch)
    {
      Wave = w->Clone();
      Wave->SetPitch(pitch);
      Position = pos;
    }

    ~ASamplerKey()
    {
      delete Wave;
    }

    WaveFile  *Wave;
    float   Pitch;
    long Position;

  protected:
};

class ASamplerKeygroup
{
  public:
    ASamplerKeygroup(int lo, int hi) { lokey = lo; hikey = hi; smp = NULL; }
    void SetSample(ASamplerSample *s) { smp = s; }
    int HasKey(int key) { return ((key >= lokey) && (key <= hikey)); }
    ASamplerKey *GetKey(int num)
    {
      if (!smp)
        return NULL;
      return new ASamplerKey(smp->GetSample(), smp->GetPosition(), powf(2.f, static_cast<float>(num - lokey) / 12.f)); 
    }
  private:
    ASamplerSample *smp;
    int lokey;
    int hikey;
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
