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

vector <ASamplerKeygroup *> Keygroups;
unsigned long keygroupid = 1;

static PlugInitInfo info;

BEGIN_EVENT_TABLE(AkaiSampler, wxWindow)
  EVT_PAINT(AkaiSampler::OnPaint)
  EVT_BUTTON(Sampler_Open, AkaiSampler::OnOpenFile)
  EVT_BUTTON(Sampler_Save, AkaiSampler::OnSaveFile)
  EVT_BUTTON(Sampler_PolyUp, AkaiSampler::OnPolyUp)
  EVT_BUTTON(Sampler_PolyDown, AkaiSampler::OnPolyDown)
  EVT_BUTTON(Sampler_SampleBt, AkaiSampler::OnSampleButton)
  EVT_BUTTON(Sampler_KgroupBt, AkaiSampler::OnKgroupButton)
  EVT_BUTTON(Sampler_EffectBt, AkaiSampler::OnEffectButton)
  EVT_LEFT_DOWN(AkaiSampler::OnKeyDown)
  EVT_LEFT_UP(AkaiSampler::OnKeyUp)
  EVT_COMMAND_SCROLL(Sampler_Volume, AkaiSampler::OnVolume)
END_EVENT_TABLE()

  AkaiSampler::AkaiSampler(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
: Plugin(startinfo, initinfo), PolyphonyCount(7), Volume(1.f), AkaiProgram(0x0)
{
  wxImage *tr_bg =
    new wxImage(string(GetDataDir() + string(IMG_SP_BMP)).c_str(), 
        wxBITMAP_TYPE_BMP);
  if (tr_bg)
    TpBmp = new wxBitmap(tr_bg);


  PlugPanel = new ASPlugPanel(this, wxPoint(150, 0), wxSize(GetSize().GetWidth() - 150, GetSize().GetHeight() - ASCLAVIER_HEIGHT - 5), wxTHICK_FRAME, this);

  //Keygroups = new ASKeygroupList("Keygroups");
  Samples = new ASSampleList("Samples");

  clavier = new ASClavier(this, -1, wxPoint(GetSize().GetWidth() - ASCLAVIER_WIDTH, GetSize().GetHeight() - ASCLAVIER_HEIGHT),
      wxSize(ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT),
      wxSIMPLE_BORDER, this);

  PlugPanel->AddPlug(Samples);
  //PlugPanel->AddPlug(Keygroups);
  PlugPanel->ShowPlugin(Samples);

  //memset(Keys, 0x0, sizeof(ASamplerKey *) * 127);

  /* Graphic control initialization */

  open_up = new wxImage(string(GetDataDir() + string(IMG_SP_OPEN_UP)).c_str(),
      wxBITMAP_TYPE_PNG);
  open_down = new wxImage(string(GetDataDir() + string(IMG_SP_OPEN_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  save_up = new wxImage(string(GetDataDir() + string(IMG_SP_SAVE_UP)).c_str(), wxBITMAP_TYPE_PNG);
  save_down = new wxImage(string(GetDataDir() + string(IMG_SP_SAVE_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  up_up = new wxImage(string(GetDataDir() + string(IMG_SP_UPUP)).c_str(), wxBITMAP_TYPE_PNG);
  up_down = new wxImage(string(GetDataDir() + string(IMG_SP_UPDO)).c_str(), wxBITMAP_TYPE_PNG);
  down_up = new wxImage(string(GetDataDir() + string(IMG_SP_DOWNUP)).c_str(), wxBITMAP_TYPE_PNG);
  down_down = new wxImage(string(GetDataDir() + string(IMG_SP_DOWNDO)).c_str(), wxBITMAP_TYPE_PNG);
  fader_bg = new wxImage(string(GetDataDir() + string(IMG_SP_FADER_BG)).c_str(),wxBITMAP_TYPE_PNG);
  fader_fg = new wxImage(string(GetDataDir() + string(IMG_SP_FADER_FG)).c_str(),wxBITMAP_TYPE_PNG);

  wxImage *sample_up = new wxImage(string(GetDataDir() + string(IMG_SP_SAMPLE_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *sample_down = new wxImage(string(GetDataDir() + string(IMG_SP_SAMPLE_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *kg_up = new wxImage(string(GetDataDir() + string(IMG_SP_KGROUP_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *kg_down = new wxImage(string(GetDataDir() + string(IMG_SP_KGROUP_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *effect_up = new wxImage(string(GetDataDir() + string(IMG_SP_EFFECT_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *effect_down = new wxImage(string(GetDataDir() + string(IMG_SP_EFFECT_DOWN)).c_str(), wxBITMAP_TYPE_PNG);

  OpenBtn = new DownButton(this, Sampler_Open,
      wxPoint(2, 2), wxSize(28, 28), open_up, open_down, true);
  SaveBtn = new DownButton(this, Sampler_Save,
      wxPoint(2, 32), wxSize(28, 28), save_up, save_down, true);

  btsample = new DownButton(this, Sampler_SampleBt, wxPoint(125, 5), wxSize(20, 20), new wxImage(sample_up->Scale(20, 20)), new wxImage(sample_down->Scale(20, 20)), true);
  btkgroup = new DownButton(this, Sampler_KgroupBt, wxPoint(125, 30), wxSize(20, 20), new wxImage(kg_up->Scale(20, 20)), new wxImage(kg_down->Scale(20, 20)), true);
  bteffect = new DownButton(this, Sampler_EffectBt, wxPoint(125, 55), wxSize(20, 20), new wxImage(effect_up->Scale(20, 20)), new wxImage(effect_down->Scale(20, 20)), true);

  /* Toolbar Haut */

  wxString s;

  s.Printf("%d", PolyphonyCount);
  PolyCountLabel = new wxStaticText(this, -1, s, wxPoint(6, 162), wxSize(-1, 12));
  PolyCountLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  PolyCountLabel->SetLabel(s);

  PolyUpBtn = new HoldButton(this, Sampler_PolyUp, wxPoint(16, 160), wxSize(11, 8),
      up_up, up_down);
  PolyDownBtn = new HoldButton(this, Sampler_PolyDown, wxPoint(16, 170), wxSize(11, 8),
      down_up, down_down);

  wxImage *img_led_off = new wxImage(string(GetDataDir() + string(IMG_SP_LED_OFF)).c_str(), wxBITMAP_TYPE_PNG);
  if (img_led_off)
    LedOff = new wxBitmap(img_led_off);

  wxImage *img_led_on = new wxImage(string(GetDataDir() + string(IMG_SP_LED_ON)).c_str(), wxBITMAP_TYPE_PNG);
  if (img_led_on)
    LedOn = new wxBitmap(img_led_on);

  MidiInBmp = new wxStaticBitmap(this, -1, *LedOff, wxPoint(12, 145));

  /* Envelope */

  VolumeFader = new FaderCtrl(this, Sampler_Volume, fader_bg, fader_fg, 0, 127, 100,
      wxPoint(2, 62), wxSize(28, 76));

  AkaiPrefix = _T("");
}

AkaiSampler::~AkaiSampler()
{
  //delete BgBmp;
  //delete bmp;

  delete open_up;
  delete open_down;
  delete save_up;
  delete save_down;
  delete up_up;
  delete up_down;
  delete down_up;
  delete down_down;
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
  int i;


  /**** DEBUG ONLY ****/
  /**/
  char *buffer = (char *)malloc(size);
  long count = read(fd, buffer, size);
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
  /**/
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
      path = path.SubString(pos, path.size() - pos);
      int opos = path.Find('/', true);
      wxString name = path.SubString(opos + 1, path.Len());
      path = path.SubString(1, opos - 1);
      AkaiPrefix = "[AKAI]";
      AkaiPrefix += dev.c_str();
      AkaiPrefix += ':';
      AkaiPrefix += ((char)part + 64);
      AkaiPrefix += '/';
      AkaiPrefix += path.c_str();
      AkaiPrefix += '/';
      cerr << "device: " << dev << "; part: " << part << "; path: " << path << "; filename: " << name << endl;
      cerr << "AkaiPrefix: " << AkaiPrefix << endl;
      t_akaiSample *sample = akaiGetSampleByName((char *)dev.c_str(), part, (char *)path.c_str(), (char *)name.c_str());
      if (sample)
      {
        ass = new ASamplerSample(sample, AkaiPrefix, id);
        free(sample);
        ASPlugin *p = new ASLoop(ASLoop::GetFXName() + " #0 for " + name);
        p->SetSample(ass);
        ass->AddEffect(p);
        PlugPanel->AddPlug(p);
      }
    }
    else
      ass = new ASamplerSample(new WaveFile(string(str), true), id);
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
    ASamplerKeygroup *askg = new ASamplerKeygroup(lokey, hikey, id);
    Keygroups.push_back(askg);
    count += read(fd, &len, sizeof(len));
    if (len)
    {
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
      ASamplerSample *ass = new ASamplerSample(group->zone_sample[0], AkaiPrefix);
      Samples->List->AddEntry(group->zone_sample[0]->name, (void *)ass);
      ASamplerKeygroup *askg = new ASamplerKeygroup(group->lowkey, group->highkey);
      Keygroups.push_back(askg);
      ass->SetKeygroup(askg);
      askg->SetSample(ass);
      ASPlugin *p = new ASLoop(ASLoop::GetFXName() + " #0 for " + group->zone_sample[0]->name);
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
  return NULL;
}

void AkaiSampler::Process(float **input, float **output, long sample_length)
{
  //long i;
  //int chan;
  
  cerr << "Process start" << endl;
  Mutex.Lock();

  /*  if (!AkaiProgram)
      {
      Mutex.Unlock();
      return;
      }
      */
  list<ASamplerNote *>::iterator i;
  ASamplerNote *n;
  long length, end, idx, chan;

  cerr << "Process note" << endl;
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
  cerr << "pos < endtotest" << endl;
      length = sample_length - n->Delta;
      end = (long)(endtotest / n->Key->Pitch);
      if (end < (length))
        length = end - n->Delta;
      else
        end = 0;
      n->Key->ass->GetSample()->SetPitch(n->Key->Pitch);
      long pos = n->Position;
  cerr << "read" << endl;
      n->Key->ass->GetSample()->Read(n->Buffer, n->Position, length, n->Delta, &(n->Position));

  cerr << "vol" << endl;
      if (n->Volume != 1.f)
        for (chan = 0; chan < 2; chan++)
          for (idx = n->Delta; idx < length; idx++)
            n->Buffer[chan][idx] *= n->Volume;

  cerr << "plugs" << endl;
      vector<ASPlugin *> p = n->Key->ass->GetEffects();
      for (vector<ASPlugin*>::iterator i = p.begin(); i != p.end(); i++)
        (*i)->Process(n->Buffer, 2, pos, length);
  cerr << "ok c fini" << endl;
      if (n->Delta)
        n->Delta = 0;
    }
  }

  cerr << "on mixe" << endl;
  Workshop.GetMix(output);

  cerr << "deletion" << endl;
  // Suppression des notes termin~Aées
  for (i = Notes.begin(); i != Notes.end();)
  {
    long endtotest;
    endtotest = (*i)->Key->ass->GetSample()->GetNumberOfFrames();
    if (((*i)->Key->looping) && ((*i)->Key->ass->GetLoopEnd() < endtotest))
      endtotest = (*i)->Key->ass->GetLoopEnd();
    if ((*i)->Position >= endtotest)
    {
  cerr << "haha pos > end" << endl;
      if (((*i)->Key->ass->GetLoopCount() > (*i)->Key->loops) || ((*i)->Key->ass->GetLoopCount() == -1))
      {
  cerr << "je loop" << endl;
        (*i)->Position = (*i)->Key->ass->GetLoopStart();
        (*i)->Key->looping = true;
        (*i)->Key->loops++;
        i++;
      }
      else
      {
  cerr << "je delete " << endl;
        Workshop.SetFreeBuffer((*i)->Buffer);
        delete *i;
        i = Notes.erase(i);
      }
    }
    else
    {
      i++;
  cerr << "c good on delete pas a po fini " << endl;
    }
  }
  cerr << "mutex bientot unlock" << endl;
  Mutex.Unlock();
  cerr << "THE END" << endl;

}

void AkaiSampler::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  /*
     memDC.SelectObject(*TpBmp);
     wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
     while (upd)
     {
     dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(),
     wxCOPY, FALSE);
     upd++;
     }
     */

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
      Mutex.Unlock();

      MidiInBmp->SetBitmap(*LedOff);
    }
    else
    {
      MidiInBmp->SetBitmap(*LedOn);

      Mutex.Lock();

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
  //  printf("[SAMPLER] Got midi in : %2x %2x %2x\n", event.MidiData[0], event.MidiData[1], event.MidiData[2]);
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

void AkaiSampler::OnPolyUp(wxCommandEvent &event)
{
  wxString s;

  if (PolyphonyCount < 256)
  {
    Mutex.Lock();

    PolyphonyCount++;
    Workshop.SetPolyphony(PolyphonyCount);
    s.Printf("%d", PolyphonyCount);
    PolyCountLabel->SetLabel(s);

    Mutex.Unlock();
  }
}

void AkaiSampler::OnPolyDown(wxCommandEvent &event)
{
  wxString s;

  if (PolyphonyCount > 1)
  {
    Mutex.Lock();

    PolyphonyCount--;
    Workshop.SetPolyphony(PolyphonyCount);
    s.Printf("%d", PolyphonyCount);
    PolyCountLabel->SetLabel(s);

    Mutex.Unlock();
  }
}

void AkaiSampler::OnSampleButton(wxCommandEvent &event)
{
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
      aske = new ASKeygroupEditor(wxString(_T("Keygroup editor for ")) + e->GetName());
      aske->SetSample(ass);
      ass->SetKgEditor(aske);
      PlugPanel->AddPlug(aske);
    }
    PlugPanel->ShowPlugin(aske);
  }
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
        {
          cout << "Cmp " << EFFECTSNAMES[0] << " with " << (*i)->Name.Left(EFFECTSNAMES[0].Len()) << endl;
          if ((*i)->Name.Left(EFFECTSNAMES[0].Len()) == EFFECTSNAMES[0])
            count++;
        }
        printf("Count = %d\n", count);
        s << EFFECTSNAMES[0];
        s << " #";
        s << count;
        s << " for " ;
        s << e->GetName();
        p = new ASEnvel(s);
        break;
      case 2:
        // Looping
        for (vector<ASPlugin *>::iterator i = plugs.begin(); i != plugs.end(); i++)
        {
          cout << "Cmp " << EFFECTSNAMES[1] << " with " << (*i)->Name.Left(EFFECTSNAMES[1].Len()) << endl;
          if ((*i)->Name.Left(EFFECTSNAMES[1].Len()) == EFFECTSNAMES[1])
            count++;
        }
        printf("Count = %d\n", count);
        s << EFFECTSNAMES[1];
        s << " #";
        s << count;
        s << " for " ;
        s << e->GetName();
        p = new ASLoop(s);
        break;
      default:
        p = NULL;
    }
    if (p)
    {
      p->SetSample(ass);
      ass->AddEffect(p);
      PlugPanel->AddPlug(p);
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
    PlugPanel->ShowPlugin(p[event.GetId() - NB_EFFECTS - 2]);
  }
}

void AkaiSampler::OnEffectButton(wxCommandEvent &event)
{
  ASListEntry *e = Samples->List->GetSelected();
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

void AkaiSampler::OnKeyUp(wxMouseEvent &event)
{
}

void AkaiSampler::OnKeyDown(wxMouseEvent &event)
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

