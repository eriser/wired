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
  this->loopcount = 0;
  this->loopstart = 0;
  this->loopend = w->GetNumberOfFrames();
}

ASamplerSample::ASamplerSample(class AkaiSampler *as, akaiSample *smpL, akaiSample *smpR, wxString AkaiPrefix, unsigned long id)
{
  if (smpR != NULL)
  {
    if (smpL->getRate() != smpR->getRate())
      cerr << "[WiredSampler] error mixing sample, stereo tracks have not the same rate !" << endl;
    short *data = new short[smpL->getSize() + smpR->getSize()];
    for (int i = 0; i < smpL->getSize(); i++)
      data[i] = smpL->getSample()[i];
    for (int i = 0; i < smpR->getSize(); i++)
      data[smpL->getSize() + i] = smpR->getSample()[i];
    this->w = new WaveFile(data, smpL->getSize() + smpR->getSize(), 2, smpL->getRate());
    w->Filename = AkaiPrefix + smpL->getName().c_str() + "/" + smpR->getName().c_str();
    free(data);
  }
  else
  {
    this->w = new WaveFile(smpL->getSample(), smpL->getSize(), 2, smpL->getRate());
    w->Filename = AkaiPrefix + smpL->getName().c_str();
  }
  this->Position = smpL->getHeaders()->start;
  this->loopstart = smpL->getHeaders()->loops[0].loopStart - smpL->getHeaders()->loops[0].loopLen;
  this->loopend = smpL->getHeaders()->loops[0].loopStart;
  this->loopcount = smpL->getHeaders()->loops[0].loopTime;
  this->askg = NULL;
  this->aske = NULL;
  if (!id)
    id = as->sampleid++;
  else
    if (id > as->sampleid)
      as->sampleid = id + 1;
  this->id = id;
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
