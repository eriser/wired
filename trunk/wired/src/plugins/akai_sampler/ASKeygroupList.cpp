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

ASamplerSample::ASamplerSample(class AkaiSampler *as, t_akaiSample *smpL, t_akaiSample *smpR, wxString AkaiPrefix, unsigned long id)
{
  if (smpR != NULL)
  {
    if (smpL->rate != smpR->rate)
      cerr << "[WiredSampler] error mixing sample, stereo tracks have not the same rate !" << endl;
    short *data = new short[smpL->size + smpR->size];
    for (int i = 0; i < smpL->size; i++)
      data[i] = smpL->buffer[i];
    for (int i = 0; i < smpR->size; i++)
      data[smpL->size + i] = smpR->buffer[i];
    this->w = new WaveFile(data, smpL->size + smpR->size, 2, smpL->rate);
    w->Filename = AkaiPrefix + smpL->name + "/" + smpR->name;
    free(data);
    this->Position = smpL->start;
    this->loopstart = smpL->loop_len;
    this->loopend = smpL->loop_start;
  }
  else
  {
    this->w = new WaveFile(smpL->buffer, smpL->size, 2, smpL->rate);
    w->Filename = AkaiPrefix + smpL->name;
    this->Position = smpL->start;
    this->loopstart = smpL->loop_len;
    this->loopend = smpL->loop_start;
  }
  this->askg = NULL;
  this->aske = NULL;
  if (!id)
    id = as->sampleid++;
  else
    if (id > as->sampleid)
      as->sampleid = id + 1;
  this->id = id;
  this->loopcount = smpL->loop_times;
  if (loopcount == 9999)
    loopcount = -1;
  if (loopend > w->GetNumberOfFrames())
    loopend = w->GetNumberOfFrames();
  this->as = as;
}

ASamplerSample::~ASamplerSample()
{
  if (w)
    delete w;
  if (aske)
  {
    as->PlugPanel->RemovePlugin(aske);
    delete aske;
  }
  if (askg)
    delete askg;
  for (vector<ASPlugin *>::iterator i = effects.begin(); i != effects.end(); i++)
  {
    as->PlugPanel->RemovePlugin((*i));
    delete (*i);
  }
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
      i = as->Keygroups.erase(i);
      return;
    }
  }
}
