#include <wx/wx.h>
#include "ASKeygroupList.h"
#include "ASSampleList.h"
#include "AkaiSampler.h"

ASamplerSample::ASamplerSample(class AkaiSampler *as, WaveFile *w, unsigned long id)
{
  this->as = as;
  this->w = w;
  this->askg = NULL;
  this->aske = NULL;
  this->Position = 0;
  if (!id)
    id = as->sampleid++;
  else
    if (id > as->sampleid)
      as->sampleid = id + 1;
  this->id = id;
  this->loopcount = -1;
  this->loopstart = 0;
  this->loopend = w->GetNumberOfFrames();
}

ASamplerSample::ASamplerSample(class AkaiSampler *as, t_akaiSample *smp, wxString AkaiPrefix, unsigned long id)
{
  this->w = new WaveFile(smp->buffer, smp->size, 2, smp->rate);
  w->Filename = AkaiPrefix + smp->name;
  this->askg = NULL;
  this->aske = NULL;
  this->Position = smp->start / 2;
  if (!id)
    id = as->sampleid++;
  else
    if (id > as->sampleid)
      as->sampleid = id + 1;
  this->id = id;
  this->loopcount = smp->loop_times;
  if (loopcount == 9999)
    loopcount = -1;
  this->loopstart = smp->loop_len / 2;
  this->loopend = smp->loop_start / 2;
  if (loopend > w->GetNumberOfFrames())
    loopend = w->GetNumberOfFrames();
  cout << "[WiredSampler] AKAI Sample Loaded, loopcount= " << loopcount << " loopstart= " << loopstart << " loopend= " << loopend << endl;
  this->as = as;
}

ASamplerSample::~ASamplerSample()
{
  if (w)
    delete w;
  if (aske)
    delete aske;
  if (askg)
    delete askg;
  for (vector<ASPlugin *>::iterator i = effects.begin(); i != effects.end(); i++)
    delete (*i);
  effects.clear();
}

ASamplerKeygroup::ASamplerKeygroup(class AkaiSampler *as, int lo, int hi, unsigned long id)
{
  this->as = as;
  lokey = lo;
  hikey = hi;
  smp = NULL;
  if (!id)
    id = as->keygroupid++;
  else
    if (id > as->keygroupid)
      as->keygroupid = id + 1;
  this->id = id;
}


ASamplerKeygroup::~ASamplerKeygroup()
{
  for (vector<ASamplerKeygroup *>::iterator i = as->Keygroups.begin(); i != as->Keygroups.end(); i++)
  {
    if ((*i) == this)
    {
      as->Keygroups.erase(i);
      return;
    }
  }
}
