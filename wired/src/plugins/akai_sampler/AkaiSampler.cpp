#include "AkaiSampler.h"
#include "midi.h"
#include <stdio.h>
#include <math.h>
#include <wx/progdlg.h>
#include "ASPlugPanel.h"
#include "Settings.h"

// ICI METTRE TOUS LES EFFETS
#include "ASEnvel.h"
#include "ASLoop.h"

#define NB_EFFECTS 2
static wxString EFFECTSNAMES[NB_EFFECTS] = {
  ASEnvel::GetFXName(),
  ASLoop::GetFXName(),
};
// FIN DES EFFETS

static PlugInitInfo info;

  BEGIN_EVENT_TABLE(AkaiSampler, wxWindow)
  EVT_PAINT(AkaiSampler::OnPaint)
  EVT_BUTTON(Sampler_Open, AkaiSampler::OnOpenFile)
  EVT_BUTTON(Sampler_Save, AkaiSampler::OnSaveFile)
  EVT_BUTTON(Sampler_Poly, AkaiSampler::OnPolyphony)
  EVT_BUTTON(Sampler_SampleBt, AkaiSampler::OnSampleButton)
  EVT_BUTTON(Sampler_KgroupBt, AkaiSampler::OnKgroupButton)
  EVT_BUTTON(Sampler_EffectBt, AkaiSampler::OnEffectButton)
  EVT_COMMAND_SCROLL(Sampler_Volume, AkaiSampler::OnVolume)
END_EVENT_TABLE()

  AkaiSampler::AkaiSampler(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
: Plugin(startinfo, initinfo), PolyphonyCount(8), Volume(1.f), AkaiProgram(0x0)
{

  sampleid = 0;
  keygroupid = 0;
  sp_bg = new wxImage(string(GetDataDir() + string(IMG_SP_BG)).c_str(), 
      wxBITMAP_TYPE_PNG);
  if (sp_bg)
  {
    BgBmp = new wxBitmap(sp_bg);
    delete sp_bg;
  }

  bmp = new wxBitmap(string(GetDataDir() + string(IMG_SP_BMP)).c_str(), wxBITMAP_TYPE_BMP);  

  PlugPanel = new ASPlugPanel(this, wxPoint(149, 8), wxSize(642, 120),//GetSize().GetWidth() - 150, GetSize().GetHeight() - ASCLAVIER_HEIGHT - 5), 
            wxTHICK_FRAME , this);

  Samples = new ASSampleList(this, "Samples");

  clavier = new ASClavier(this, -1, wxPoint(23, 142), //GetSize().GetWidth() - ASCLAVIER_WIDTH, GetSize().GetHeight() - ASCLAVIER_HEIGHT),
          wxSize(ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT),
          wxSIMPLE_BORDER, this);

  PlugPanel->AddPlug(Samples);
  PlugPanel->ShowPlugin(Samples);

  open_up = new wxImage(string(GetDataDir() + string(IMG_SP_OPEN_UP)).c_str(),
      wxBITMAP_TYPE_PNG);
  open_down = new wxImage(string(GetDataDir() + string(IMG_SP_OPEN_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  save_up = new wxImage(string(GetDataDir() + string(IMG_SP_SAVE_UP)).c_str(), wxBITMAP_TYPE_PNG);
  save_down = new wxImage(string(GetDataDir() + string(IMG_SP_SAVE_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  fader_bg = new wxImage(string(GetDataDir() + string(IMG_SP_FADER_BG)).c_str(),wxBITMAP_TYPE_PNG);
  fader_fg = new wxImage(string(GetDataDir() + string(IMG_SP_FADER_FG)).c_str(),wxBITMAP_TYPE_PNG);

  wxImage *sample_up = new wxImage(string(GetDataDir() + string(IMG_SP_SAMPLE_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *sample_down = new wxImage(string(GetDataDir() + string(IMG_SP_SAMPLE_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *kg_up = new wxImage(string(GetDataDir() + string(IMG_SP_KGROUP_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *kg_down = new wxImage(string(GetDataDir() + string(IMG_SP_KGROUP_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *effect_up = new wxImage(string(GetDataDir() + string(IMG_SP_EFFECT_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *effect_down = new wxImage(string(GetDataDir() + string(IMG_SP_EFFECT_DOWN)).c_str(), wxBITMAP_TYPE_PNG);

  OpenBtn = new DownButton(this, Sampler_Open,
      wxPoint(63, 10), wxSize(27, 30), open_up, open_down, true);
  SaveBtn = new DownButton(this, Sampler_Save,
      wxPoint(63, 45), wxSize(27, 30), save_up, save_down, true);

  btsample = new DownButton(this, Sampler_SampleBt, wxPoint(122, 6), wxSize(21, 22), sample_up,
      sample_down, false);
  btsample->SetOn();
  btkgroup = new DownButton(this, Sampler_KgroupBt, wxPoint(122, 28), wxSize(21, 22), 
      kg_up, kg_down, false);
  bteffect = new DownButton(this, Sampler_EffectBt, wxPoint(122, 51), wxSize(21, 22), 
      effect_up, effect_down, false);

  wxString s;

  s.Printf("%d", PolyphonyCount);
  PolyCountLabel = new wxStaticText(this, -1, s, wxPoint(76, 100), wxSize(-1, 12));
  PolyCountLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  PolyCountLabel->SetLabel(s);

  wxImage** imgs;
  imgs = new wxImage*[3];
  imgs[0] = new wxImage(_T(string(GetDataDir() + string(IMG_SP_POLY_1)).c_str()));
  imgs[1] = new wxImage(_T(string(GetDataDir() + string(IMG_SP_POLY_2)).c_str()));
  imgs[2] = new wxImage(_T(string(GetDataDir() + string(IMG_SP_POLY_3)).c_str()));

  PolyKnob = new CycleKnob(this, Sampler_Poly, 3, imgs, 10, 1, 256, 8,
      wxPoint(97, 97), wxDefaultSize);

  delete imgs[0];
  delete imgs[1];
  delete imgs[2];
  delete imgs;

  wxImage *img_led_off = new wxImage(string(GetDataDir() + string(IMG_SP_LED_OFF)).c_str(), wxBITMAP_TYPE_PNG);
  if (img_led_off)
    LedOff = new wxBitmap(img_led_off);

  wxImage *img_led_on = new wxImage(string(GetDataDir() + string(IMG_SP_LED_ON)).c_str(), wxBITMAP_TYPE_PNG);
  if (img_led_on)
    LedOn = new wxBitmap(img_led_on);

  MidiInBmp = new wxStaticBitmap(this, -1, *LedOff, wxPoint(2, 180));

  /* Envelope */

  VolumeFader = new FaderCtrl(this, Sampler_Volume, fader_bg, fader_fg, 0, 127, 100,
      wxPoint(11, 21), wxSize(15, 102));

  AkaiPrefix = _T("");

  MidiVolume[0] = M_CONTROL;
  MidiVolume[1] = 0x7;

  UpdateMidi = false;
  UpdateVolume = false;
}

AkaiSampler::~AkaiSampler()
{
  delete BgBmp;
  delete bmp;
  Keygroups.clear();
  delete open_up;
  delete open_down;
  delete save_up;
  delete save_down;
  delete fader_bg;
  delete fader_fg;

}

void AkaiSampler::Stop()
{
  // arreter les notes en cours
}


bool AkaiSampler::IsInstrument()
{
  return (true);
}


bool AkaiSampler::IsAudio()
{
  return false;
}

bool AkaiSampler::IsMidi()
{
  return true;
}

void AkaiSampler::Load(int fd, long size)
{

  long count;
  /**** DEBUG ONLY ****/
  /**
    int i;
    char *buffer = (char *)malloc(size);
    count = read(fd, buffer, size);
    printf("[WiredSampler] -- SAVE FILE DUMP --\n");
    for (i = 0; i < count ; i++)
    {
    if (!(i & 15))
    printf("[WiredSampler] %08X   ", i);
    printf(" %02X ", buffer[i] & 0xFF);
    if (!((i + 1) & 15))
    {
    printf("   ");
    for (int j = i & 15; j >= 0; j--)
    if ((buffer[i - j] >= 32) && (buffer[i - j] < 127))
    printf("%c", buffer[i - j]);
    else
    printf(".");
    printf("\n");
    }
    }
    if (i & 15)
    {
    for (int j = i; j & 15; j++)
    printf("    ");
    printf("   ");
    for (int j = i & 15; j > 0; j--)
    if ((buffer[count - j] >= 32) && (buffer[count - j] < 127))
    printf("%c", buffer[count - j]);
    else
    printf(".");
    printf("\n");
    }
    printf("[WiredSampler] -- END OF DUMP --\n");
    free(buffer);
    lseek(fd, -count, SEEK_CUR);
  **/
  /**** END OF DEBUG ****/


  long nbent;
  count = 0;
  count += read(fd, &nbent, sizeof(nbent));
  cerr << "[WiredSampler] Loading " << nbent << " samples..." << endl;
  for (int i = 0; i < nbent; i++)
  {
    long len;
    char *str;

    count += read(fd, &len, sizeof(len));
    str = (char *)malloc(len + 1);
    count += read(fd, str, len);
    str[len] = 0;
    wxString smpname(_T(str));
    free(str);
    cerr << "[WiredSampler] Loaded sample " << smpname << endl;
    unsigned long id;
    count += read(fd, &id, sizeof(id));
    cerr << "[WiredSampler] Loaded ID " << id << endl;
    count += read(fd, &len, sizeof(len));
    str = (char *)malloc(len + 1);
    count += read(fd, str, len);
    str[len] = 0;
    wxString s(_T(str));
    wxString s2;
    ASamplerSample *ass = NULL; 
    if (s.StartsWith("[AKAI]", &s2))
    {
      wxString dev(s2.SubString(0, s2.Find(':') - 1));
      s2 = s2.SubString(s2.Find(':') + 1, s2.Len());
      wxString path = s2; //.SubString(10, s2.size() - 10);
      int pos = path.Find('/');
      int part = path.SubString(0, pos).c_str()[0] - 64;
      path = path.SubString(pos + 1, path.size() - pos);
      //int opos = path.Find('/', true);
      int opos = path.Find('/');
      wxString name = path.SubString(opos + 1, path.Len());
      path = path.SubString(0, opos - 1);
      AkaiPrefix = "[AKAI]";
      AkaiPrefix += dev.c_str();
      AkaiPrefix += ':';
      AkaiPrefix += ((char)part + 64);
      AkaiPrefix += '/';
      AkaiPrefix += path.c_str();
      AkaiPrefix += '/';
      cerr << "device: " << dev << "; part: " << part << "; path: " << path << "; filename: " << name << endl;
      cerr << "AkaiPrefix: " << AkaiPrefix << endl;
      if ((opos = name.Find('/')) != -1)
      {
        cout << "Dual akai Sample found, getting the 2 samples and mixing it..." << endl;
        wxString smp1 = name.SubString(0, opos - 1);
        wxString smp2 = name.SubString(opos + 1, name.Len());
        t_akaiSample *sampleL = akaiGetSampleByName((char *)dev.c_str(), part, (char *)path.c_str(), (char *)smp1.c_str());
        t_akaiSample *sampleR = akaiGetSampleByName((char *)dev.c_str(), part, (char *)path.c_str(), (char *)smp2.c_str());
        if (sampleL && sampleR)
        {
          ass = new ASamplerSample(this, sampleL, sampleR, AkaiPrefix, id);
          free(sampleL);
          free(sampleR);
        }
      }
      else
      {
        t_akaiSample *sample = akaiGetSampleByName((char *)dev.c_str(), part, (char *)path.c_str(), (char *)name.c_str());
        if (sample)
        {
          ass = new ASamplerSample(this, sample, NULL, AkaiPrefix, id);
          free(sample);
        }
      }
    }
    else
      ass = new ASamplerSample(this, new WaveFile(string(str), true), id);
    if (ass)
    {
      cerr << "[WiredSampler] Loaded wav " << wxString(_T(str)) << endl;
      Samples->List->AddEntry(smpname, (void *)ass);
    }
    else
      cerr << "[WiredSampler] Wavefile " << wxString(_T(str)) << " can't be loaded skipping..." << endl;
    free(str);
    count += read(fd, &len, sizeof(len));
    if (ass)
    {
      cerr << "[WiredSampler] Setting loop_count to " << len << endl;
      ass->SetLoopCount(len);
    }
    count += read(fd, &len, sizeof(len));
    if (ass)
    {
      cerr << "[WiredSampler] Setting loop_start to " << len << endl;
      ass->SetLoopStart(len);
    }
    count += read(fd, &len, sizeof(len));
    if (ass)
    {
      cerr << "[WiredSampler] Setting loop_end to " << len << endl;
      ass->SetLoopEnd(len);
    }
    int nbplug;
    count += read(fd, &nbplug, sizeof(nbplug));
    cerr << "[WiredSampler] Loading " << nbplug << " plugins..." << endl;
    for (int j = 0; j < nbplug ; j++)
    {
      count += read(fd, &len, sizeof(len));
      str = (char *)malloc(len + 1);
      count += read(fd, str, len);
      str[len] = 0;
      wxString type(str);
      free(str);
      count += read(fd, &len, sizeof(len));
      str = (char *)malloc(len + 1);
      count += read(fd, str, len);
      str[len] = 0;
      wxString name(str);
      free(str);
      cerr << "[WiredSampler] Creating effect type " << type << " name " << name << endl;
      ASPlugin *p = NULL;
      count += read(fd, &len, sizeof(len));
      count += len;
      if (type == EFFECTSNAMES[0])
        p = new ASEnvel(this, name);
      else if (type == EFFECTSNAMES[1])
        p = new ASLoop(this, name);
      if (p)
      {
        p->SetSample(ass);
        ass->AddEffect(p);
        PlugPanel->AddPlug(p);
        cerr << "[WiredSampler] Loading " << len << " bytes of data for this plugin" << endl;
        p->Load(fd, len);
      }
      else
      {
        cerr << "[WiredSampler] Plugin type unknown, can't load it !" << endl;
        lseek(fd, len, SEEK_CUR);
      }
    }
  }
  count += read(fd, &nbent, sizeof(nbent));
  cerr << "[WiredSampler] Loading " << nbent << " keygroups..." << endl;
  for (int i = 0; i < nbent; i++)
  {
    long len;
    char *str;

    unsigned long id;
    count += read(fd, &id, sizeof(id));
    cerr << "[WiredSampler] Loaded ID " << id << endl;
    long lokey;
    count += read(fd, &lokey, sizeof(lokey));
    cerr << "[WiredSampler] Loaded low key " << lokey << endl;
    long hikey;
    count += read(fd, &hikey, sizeof(hikey));
    cerr << "[WiredSampler] Loaded high key " << hikey << endl;
    ASamplerKeygroup *askg = new ASamplerKeygroup(this, lokey, hikey, id);
    Keygroups.push_back(askg);
    count += read(fd, &len, sizeof(len));
    cerr << "[WiredSampler] Need to associate with sample " << len << endl;
    vector<ASListEntry *> v = Samples->List->GetEntries();
    for (vector<ASListEntry *>::iterator i = v.begin(); i != v.end(); i++)
    {
      ASamplerSample *ass = (ASamplerSample *)(*i)->GetEntry();
      if (ass->GetID() == len)
      {
        cerr << "[WiredSampler] Sample found, associating..." << endl;
        ass->SetKeygroup(askg);
        askg->SetSample(ass);
        break;
      }
    }
  }
  cerr << "[WiredSampler] Config loaded, total " << count << " bytes read (of " << size << ")." << endl;
}

long AkaiSampler::Save(int fd)
{
  long count = 0;

  vector<ASListEntry *> entries = Samples->List->GetEntries();
  long nbent = entries.size();
  count += write(fd, &nbent, sizeof(nbent));
  cerr << "[WiredSampler] Saving " << nbent << " samples..." << endl;
  for (vector<ASListEntry *>::iterator i = entries.begin(); i != entries.end(); i++)
  {
    long len;
    char *str;

    cerr << "[WiredSampler] Saving sample " << (*i)->GetName() << endl;
    ASamplerSample *ass = (ASamplerSample *)(*i)->GetEntry();
    str = (char *)(*i)->GetName().c_str();
    len = strlen(str);
    count += write(fd, &len, sizeof(len));
    count += write(fd, str, len);
    len = ass->GetID();
    cerr << "[WiredSampler] Saving ID " << len << endl;
    count += write(fd, &len, sizeof(len));
    str = (char *)ass->GetSample()->Filename.c_str();
    len = strlen(str);
    cerr << "[WiredSampler] Saving wavefile " << ass->GetSample()->Filename << endl;
    count += write(fd, &len, sizeof(len));
    count += write(fd, str, len);
    len = ass->GetLoopCount();
    cerr << "[WiredSampler] Saving loop_count " << len << endl;
    count += write(fd, &len, sizeof(len));
    len = ass->GetLoopStart();
    cerr << "[WiredSampler] Saving loop_start " << len << endl;
    count += write(fd, &len, sizeof(len));
    len = ass->GetLoopEnd();
    cerr << "[WiredSampler] Saving loop_end " << len << endl;
    count += write(fd, &len, sizeof(len));
    cerr << "[WiredSampler] Saving effects..." << endl;
    vector<ASPlugin *> effects = ass->GetEffects();
    nbent = effects.size();
    count += write(fd, &nbent, sizeof(nbent));
    cerr << "[WiredSampler] Saving " << nbent << " effects..." << endl;
    for (vector<ASPlugin *>::iterator j = effects.begin(); j != effects.end(); j++)
    {
      str = (char *)(*j)->GetType().c_str();
      len = strlen(str);
      cerr << "[WiredSampler] Saving plugin type " << str << endl;
      count += write(fd, &len, sizeof(len));
      count += write(fd, str, len);
      str = (char *)(*j)->Name.c_str();
      len = strlen(str);
      cerr << "[WiredSampler] Saving plugin name " << str << endl;
      count += write(fd, &len, sizeof(len));
      count += write(fd, str, len);
      unsigned long pos = lseek(fd, 0, SEEK_CUR);
      count += write(fd, &len, sizeof(len));
      cerr << "[WiredSampler] Saving plugin data " << endl;
      len = (*j)->Save(fd);
      unsigned long pos2 = lseek(fd, 0, SEEK_CUR);
      lseek(fd, pos, SEEK_SET);
      cerr << "[WiredSampler] plugin data len " << len << endl;
      write(fd, &len, sizeof(len));
      lseek(fd, pos2, SEEK_SET);
      count += len;
    }
    
  }
  nbent = Keygroups.size();
  count += write(fd, &nbent, sizeof(nbent));
  cerr << "[WiredSampler] Saving " << nbent << " keygroups..." << endl;
  for (vector<ASamplerKeygroup*>::iterator i = Keygroups.begin(); i != Keygroups.end(); i++)
  {
    long len;
    char *str;

    len = (*i)->GetID();
    cerr << "[WiredSampler] Saving ID " << len << endl;
    count += write(fd, &len, sizeof(len));
    len = (*i)->GetLowKey();
    cerr << "[WiredSampler] Saving lokey " << len << endl;
    count += write(fd, &len, sizeof(len));
    len = (*i)->GetHighKey();
    cerr << "[WiredSampler] Saving hikey " << len << endl;
    count += write(fd, &len, sizeof(len));
    ASamplerSample *smp = (*i)->GetSample();
    if (!smp)
      len = 0;
    else
      len = smp->GetID();
    cerr << "[WiredSampler] Saving associated sample " << len << endl;
    count += write(fd, &len, sizeof(len));
  }
  cerr << "[WiredSampler] Config saved, total " << count << " bytes written." << endl;
  return count;
}

void AkaiSampler::LoadProgram()
{
  t_akaiKeygrp *group;
  t_list *elem;
  int i;

  for (elem = AkaiProgram->keygrp; elem; elem = elem->next)
  {
    group = elt(elem, t_akaiKeygrp *);
    cout << "Num: " << group->num << endl;
    if (group->zone_sample[0])
    {
      wxString name = group->zone_sample[0]->name;
      if (group->zone_sample[1])
        name << "/" << group->zone_sample[1]->name;
      ASamplerSample *ass = new ASamplerSample(this, group->zone_sample[0], group->zone_sample[1], AkaiPrefix);
      Samples->List->AddEntry(name, (void *)ass);
      ASamplerKeygroup *askg = new ASamplerKeygroup(this, group->lowkey, group->highkey);
      Keygroups.push_back(askg);
      ass->SetKeygroup(askg);
      askg->SetSample(ass);
      ASPlugin *p = new ASLoop(this, ASLoop::GetFXName() + " #0 for " + group->zone_sample[0]->name);
      p->SetSample(ass);
      ass->AddEffect(p);
      PlugPanel->AddPlug(p);
    }
  }
}


void AkaiSampler::DeleteProgram()
{
  if (AkaiProgram)
  {
    //delete 
    free(AkaiProgram);
    AkaiProgram = 0x0;
    AkaiPrefix = _T("");
  }
}

void AkaiSampler::SetBufferSize(long size)
{
  Workshop.SetBufferSize(size);
  Workshop.SetPolyphony(PolyphonyCount);
}

void AkaiSampler::SetSamplingRate(double rate)
{
  SamplingRate = rate;
}

wxBitmap *AkaiSampler::GetBitmap()
{
  return (bmp);
}

void AkaiSampler::Process(float **input, float **output, long sample_length)
{
  Mutex.Lock();

  list<ASamplerNote *>::iterator i;
  ASamplerNote *n;
  long length, end, idx, chan;

  // Processing des notes
  for (i = Notes.begin(); i != Notes.end(); i++)
  {
    n = *i;
    long endtotest;
    endtotest = n->Key->ass->GetSample()->GetNumberOfFrames();
    if ((n->Key->looping) && (n->Key->ass->GetLoopEnd() < endtotest))
      endtotest = n->Key->ass->GetLoopEnd();
    if (n->Position < endtotest)
    {
      length = sample_length - n->Delta;
      if (endtotest <= (n->Position + length))
        length = endtotest - n->Position;
      n->Key->ass->GetSample()->SetPitch(n->Key->Pitch);
      long pos = n->Position;
      n->Key->ass->GetSample()->Read(n->Buffer, n->Position, length, n->Delta, &(n->Position));
      if ((length < sample_length) && ((n->Key->ass->GetLoopCount() > n->Key->loops) || (n->Key->ass->GetLoopCount() == -1)))
      {
        n->Position = n->Key->ass->GetLoopStart();
        n->Key->ass->GetSample()->Read(n->Buffer, n->Position, sample_length - length, length, &n->Position);
        n->Key->looping = true;
        n->Key->loops++;
        length = sample_length;
      }
      else
      {
        for (chan = 0; chan < 2; chan++)
          for (idx = length; idx < sample_length; idx++)
            n->Buffer[chan][idx] = 0.f;
        length = sample_length;
      }

      if (n->Volume != 1.f)
        for (chan = 0; chan < 2; chan++)
          for (idx = n->Delta; idx < length; idx++)
            n->Buffer[chan][idx] *= n->Volume;

      vector<ASPlugin *> p = n->Key->ass->GetEffects();
      for (vector<ASPlugin*>::iterator i = p.begin(); i != p.end(); i++)
        (*i)->Process(n->Buffer, 2, pos, length);
      if (n->Delta)
        n->Delta = 0;
    }
  }

  Workshop.GetMix(output);

  // Suppression des notes termin~Aées
  for (i = Notes.begin(); i != Notes.end();)
  {
    long endtotest;
    endtotest = (*i)->Key->ass->GetSample()->GetNumberOfFrames();
    if (((*i)->Key->looping) && ((*i)->Key->ass->GetLoopEnd() < endtotest))
      endtotest = (*i)->Key->ass->GetLoopEnd();
    if ((*i)->Position >= endtotest)
    {
      if (((*i)->Key->ass->GetLoopCount() > (*i)->Key->loops) || ((*i)->Key->ass->GetLoopCount() == -1))
      {
        (*i)->Position = (*i)->Key->ass->GetLoopStart();
        (*i)->Key->looping = true;
        (*i)->Key->loops++;
        i++;
      }
      else
      {
        Workshop.SetFreeBuffer((*i)->Buffer);
        delete *i;
        i = Notes.erase(i);
      }
    }
    else
    {
      i++;
    }
  }
  Mutex.Unlock();

}

void AkaiSampler::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);

  memDC.SelectObject(*BgBmp);
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
  while (upd)
  {
    dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(),
        wxCOPY, FALSE);
    upd++;
  }

  Plugin::OnPaintEvent(event);
}

void AkaiSampler::ProcessEvent(WiredEvent &event)
{
  if ((event.MidiData[0] == M_NOTEON1) || (event.MidiData[0] == M_NOTEON2))
  {
    if (!event.MidiData[2])
    {
      Mutex.Lock();

      // Suppression des notes termin~Aées
      list<ASamplerNote *>::iterator i;
      for (i = Notes.begin(); i != Notes.end(); i++)
      {
        if ((*i)->Note == event.MidiData[1])
        {
          Workshop.SetFreeBuffer((*i)->Buffer);
          delete *i;
          Notes.erase(i);
          break;
        }
      }
      MidiInOn = false;
      UpdateMidi = true;
      AskUpdate();

      Mutex.Unlock();
    }
    else
    {
      Mutex.Lock();

      MidiInOn = true;
      UpdateMidi = true;
      AskUpdate();

      if (Notes.size() < PolyphonyCount)
      {
        if ((event.MidiData[1] >= 0) && (event.MidiData[1] <= 127))
        {
          ASamplerKeygroup *askg = NULL;
          for (vector <ASamplerKeygroup *>::iterator i = Keygroups.begin(); (!askg) && (i != Keygroups.end()); i++)
            if ((*i)->HasKey(event.MidiData[1]))
              askg = (*i);
          if (askg)
          {
            printf("FindKeyGroup: %p\n", askg);
            ASamplerKey *key = askg->GetKey(event.MidiData[1]);
            if (key)
            {
              ASamplerNote *n = new ASamplerNote(event.MidiData[1],
                  event.MidiData[2] / 100.f,
                  key,
                  event.DeltaFrames,
                  Workshop.GetFreeBuffer(),
                  event.NoteLength);
              Notes.push_back(n);
              printf("[SAMPLER] Note added: %d\n", n->Note);
            }
          }
        }
      }
      else
        printf("[SAMPLER] Max polyphony reached\n");

      Mutex.Unlock();
    }
  }
  else
    ProcessMidiControls(event);
  //  printf("[SAMPLER] Got midi in : %2x %2x %2x\n", event.MidiData[0], event.MidiData[1], event.MidiData[2]);
}

void AkaiSampler::ProcessMidiControls(WiredEvent &event)
{
  Mutex.Lock();

  if ((MidiVolume[0] == event.MidiData[0]) && (MidiVolume[1] == event.MidiData[1]))
  {
    Volume = event.MidiData[2] / 100.f;
    Workshop.SetVolume(Volume);

    UpdateVolume = true;
    AskUpdate();
  }

  Mutex.Unlock();
}

void AkaiSampler::Update()
{
  Mutex.Lock();

  if (UpdateMidi)
  {
    UpdateMidi = false;
    if (MidiInOn)
      MidiInBmp->SetBitmap(*LedOn);
    else
      MidiInBmp->SetBitmap(*LedOff);
  }
  if (UpdateVolume)
  {
    UpdateVolume = false;
    VolumeFader->SetValue((int)(Volume * 100.f)); 
  }      

  Mutex.Unlock();
}

void AkaiSampler::OnOpenFile(wxCommandEvent &event)
{
  //  FileLoader *dlg = new FileLoader(this, -1, "Load AKAI patch", true, false, NULL);
  string s = OpenFileLoader("Load AKAI program", 0x0, true);
  if (!s.empty()) //dlg->ShowModal() == wxID_OK)
  {
    string filename = s; //dlg->GetSelectedFile();
    //dlg->Destroy();

    wxProgressDialog *Progress = new wxProgressDialog("Loading AKAI program",
        "Please wait...",
        100, this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT
        | wxPD_REMAINING_TIME);
    Progress->Update(1);
    try
    {
      DeleteProgram();

      Progress->Update(10);
      string mDevice, mFilename, mName;
      int mPart;

      mDevice = filename.substr(0, filename.find(":", 0));
      filename = filename.substr(filename.find(":", 0) + 1,
          filename.size() - filename.find(":", 0));
      mFilename = filename.substr(10, filename.size() - 10);
      int pos = mFilename.find("/", 0);
      mPart = mFilename.substr(0, pos).c_str()[0] - 64;
      mFilename = mFilename.substr(pos, mFilename.size() - pos);
      int opos = 0;
      while ((pos = mFilename.find("/", opos)) != string::npos)
        opos = pos + 1;
      mName = mFilename.substr(opos, mFilename.size() - opos);
      mFilename = mFilename.substr(1, opos - 2);
      AkaiPrefix = "[AKAI]";
      AkaiPrefix += mDevice.c_str();
      AkaiPrefix += ':';
      AkaiPrefix += ((char)mPart + 64);
      AkaiPrefix += '/';
      AkaiPrefix += mFilename.c_str();
      AkaiPrefix += '/';
      cout << "device: " << mDevice << "; part: " << mPart << "; name: " << mName << "; path: " << mFilename << endl;
      cout << "AkaiPrefix: " << AkaiPrefix << endl;
      Progress->Update(20);
      if (!(AkaiProgram = akaiLoadProgram((char *)mDevice.c_str(), mPart,
              (char *)mFilename.c_str(),
              (char *)mName.c_str())))
        cout << "[SAMPLER] Cannot open AKAI program !" << endl;
      else
        LoadProgram();

      Progress->Update(99);
    }
    catch (...)
    {
      cout << "[SAMPLER] Cannot open AKAI program !" << endl;
    }
    delete Progress;
  }
  //  else
  //  dlg->Destroy();
}

void AkaiSampler::OnVolume(wxScrollEvent &event)
{
  Mutex.Lock();

  Volume = VolumeFader->GetValue() / 100.f;
  Workshop.SetVolume(Volume);

  Mutex.Unlock();
}

void AkaiSampler::OnPolyphony(wxCommandEvent &event)
{
  wxString s;

  if (PolyphonyCount < 256)
  {
    Mutex.Lock();

    PolyphonyCount = PolyKnob->GetValue();;
    Workshop.SetPolyphony(PolyphonyCount);
    s.Printf("%d", PolyphonyCount);
    PolyCountLabel->SetLabel(s);

    Mutex.Unlock();
  }
}

void AkaiSampler::OnSampleButton(wxCommandEvent &event)
{
  if (btkgroup->GetOn())
    btkgroup->SetOff();
  if (bteffect->GetOn())
    bteffect->SetOff();
  if (!btsample->GetOn())
    btsample->SetOn();

  PlugPanel->ShowPlugin(Samples);
}

void AkaiSampler::OnKgroupButton(wxCommandEvent &event)
{
  ASListEntry *e = Samples->List->GetSelected();
  if (e)
  {
    ASamplerSample *ass = (ASamplerSample *)(e->GetEntry());
    ASKeygroupEditor *aske = ass->GetKgEditor();
    ASamplerKeygroup *askg = ass->GetKeygroup();
    if (!aske)
    {
      aske = new ASKeygroupEditor(this, wxString(_T("Keygroup editor for ")) + e->GetName());
      aske->SetSample(ass);
      ass->SetKgEditor(aske);
      PlugPanel->AddPlug(aske);
    }
    if (btsample->GetOn())
      btsample->SetOff();
    if (bteffect->GetOn())
      bteffect->SetOff();
    if (!btkgroup->GetOn())
      btkgroup->SetOn();
    PlugPanel->ShowPlugin(aske);
  }
  else
    btkgroup->SetOff();
}

void AkaiSampler::OnAddEffect(wxCommandEvent &event)
{
  ASListEntry *e = Samples->List->GetSelected();
  if (e)
  {
    ASamplerSample *ass = (ASamplerSample *)(e->GetEntry());
    ASPlugin *p;
    vector <ASPlugin *> plugs = ass->GetEffects();
    int count = 0;
    wxString s;
    switch (event.GetId())
    {
      case 1:
        // Enveloppe
        for (vector<ASPlugin *>::iterator i = plugs.begin(); i != plugs.end(); i++)
          if ((*i)->GetType() == EFFECTSNAMES[0])
            count++;
        s << EFFECTSNAMES[0];
        s << " #";
        s << count;
        s << " for " ;
        s << e->GetName();
        p = new ASEnvel(this, s);
        break;
      case 2:
        // Looping
        for (vector<ASPlugin *>::iterator i = plugs.begin(); i != plugs.end(); i++)
          if ((*i)->GetType() == EFFECTSNAMES[1])
            count++;
        s << EFFECTSNAMES[1];
        s << " #";
        s << count;
        s << " for " ;
        s << e->GetName();
        p = new ASLoop(this, s);
        break;
      default:
        p = NULL;
    }
    if (p)
    {
      p->SetSample(ass);
      ass->AddEffect(p);
      PlugPanel->AddPlug(p);

      bteffect->SetOn();
      if (btkgroup->GetOn())
        btkgroup->SetOff();
      if (btsample->GetOn())
        btsample->SetOff();

      PlugPanel->ShowPlugin(p);
    }
  }
}

void AkaiSampler::OnSelectEffect(wxCommandEvent &event)
{
  ASListEntry *e = Samples->List->GetSelected();
  if (e)
  {
    ASamplerSample *ass = (ASamplerSample *)(e->GetEntry());
    vector <ASPlugin *> p = ass->GetEffects();

    bteffect->SetOn();
    if (btkgroup->GetOn())
      btkgroup->SetOff();
    if (btsample->GetOn())
      btsample->SetOff();

    PlugPanel->ShowPlugin(p[event.GetId() - NB_EFFECTS - 2]);
  }
}

void AkaiSampler::OnEffectButton(wxCommandEvent &event)
{
  ASListEntry *e = Samples->List->GetSelected();

  bteffect->SetOff();
  if (e)
  {
    ASamplerSample *ass = (ASamplerSample *)(e->GetEntry());
    vector <ASPlugin *> p = ass->GetEffects();
    wxMenu *menu = new wxMenu();
    wxMenu *newplug = new wxMenu();
    for (int k = 1; k <= NB_EFFECTS; k++)
    {
      newplug->Append(k, EFFECTSNAMES[k - 1]);
      Connect(k, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction)(wxEventFunction)
          (wxCommandEventFunction)&AkaiSampler::OnAddEffect);
    }
    int num = NB_EFFECTS + 1;
    menu->Append(num++, "New effect", newplug);
    for (vector<ASPlugin *>::iterator i = p.begin(); i != p.end(); i++)
    {
      menu->Append(num, (*i)->Name);
      Connect(num++, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction)(wxEventFunction)
          (wxCommandEventFunction)&AkaiSampler::OnSelectEffect);
    }
    wxPoint pt(((wxWindow *)event.GetEventObject())->GetPosition());
    PopupMenu(menu, pt.x, pt.y);
  }
}

void AkaiSampler::OnSaveFile(wxCommandEvent &event)
{
}

/******** Main and mandatory library functions *********/

extern "C"
{
  PlugInitInfo init()
  {
    WIRED_MAKE_STR(info.UniqueId, "PLWS");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_INSTR;
    info.UnitsX = 4;
    info.UnitsY = 2;

    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new AkaiSampler(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }
}

