#include "AkaiSampler.h"
#include "midi.h"
#include <stdio.h>
#include <math.h>
#include <wx/progdlg.h>
#include "ASPlugPanel.h"
#include "ASEnvel.h"
#include "Settings.h"

static PlugInitInfo info;

  BEGIN_EVENT_TABLE(AkaiSampler, wxWindow)
  EVT_PAINT(AkaiSampler::OnPaint)
  EVT_BUTTON(Sampler_Open, AkaiSampler::OnOpenFile)
  EVT_BUTTON(Sampler_Save, AkaiSampler::OnSaveFile)
  EVT_BUTTON(Sampler_PolyUp, AkaiSampler::OnPolyUp)
  EVT_BUTTON(Sampler_PolyDown, AkaiSampler::OnPolyDown)
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

  clavier = new ASClavier(this, -1, wxPoint(GetSize().GetWidth() - ASCLAVIER_WIDTH, GetSize().GetHeight() - ASCLAVIER_HEIGHT),
      wxSize(ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT),
      wxSIMPLE_BORDER, this);

  PlugPanel = new ASPlugPanel(this, wxPoint(150, 0), wxSize(GetSize().GetWidth() - 150, GetSize().GetHeight() - ASCLAVIER_HEIGHT - 5), wxTHICK_FRAME, this);

  Samples = new ASSampleList("Samples");
  Keygroups = new ASKeygroupList("Keygroups");

  PlugPanel->AddPlug(Samples);
  PlugPanel->AddPlug(Keygroups);

  //ASEnvel *envel = new ASEnvel("Envelope");
  //PlugPanel->AddPlug(envel);

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

  OpenBtn = new DownButton(this, Sampler_Open,
      wxPoint(2, 2), wxSize(28, 28), open_up, open_down, true);
  SaveBtn = new DownButton(this, Sampler_Save,
      wxPoint(2, 32), wxSize(28, 28), save_up, save_down, true);

  /* Toolbar Haut */

  wxString s;

  s.Printf("%d", PolyphonyCount);
  PolyCountLabel = new wxStaticText(this, -1, s, wxPoint(80, 10), wxSize(-1, 12));
  PolyCountLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  PolyCountLabel->SetLabel(s);

  PolyUpBtn = new HoldButton(this, Sampler_PolyUp, wxPoint(108, 10), wxSize(11, 8),
      up_up, up_down);
  PolyDownBtn = new HoldButton(this, Sampler_PolyDown, wxPoint(108, 21), wxSize(11, 8),
      down_up, down_down);

  wxImage *img_led_off = new wxImage(string(GetDataDir() + string(IMG_SP_LED_OFF)).c_str(), wxBITMAP_TYPE_PNG);
  if (img_led_off)
    LedOff = new wxBitmap(img_led_off);

  wxImage *img_led_on = new wxImage(string(GetDataDir() + string(IMG_SP_LED_ON)).c_str(), wxBITMAP_TYPE_PNG);
  if (img_led_on)
    LedOn = new wxBitmap(img_led_on);

  MidiInBmp = new wxStaticBitmap(this, -1, *LedOff, wxPoint(70, 10));

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
	    wxString dev(s2.SubString(0, s2.Find(':')));
	    s2 = s2.SubString(s2.Find(':') + 1, s2.size() - s2.Find(':'));
	    wxString path = s2.SubString(10, s2.size() - 10);
	    int pos = path.Find('/');
	    int part = path.SubString(0, pos).c_str()[0] - 64;
	    path = path.SubString(pos, path.size() - pos);
	    int opos = path.Find('/', true);
	    wxString name = path.SubString(opos, path.size() - opos);
	    path = path.SubString(1, opos - 2);
      wxString prefix = s.SubString(6, opos);
      cout << "device: " << dev << "; part: " << part << "; path: " << path << "; filename: " << name << endl;
      cout << "AkaiPrefix: " << AkaiPrefix << endl;
	    t_akaiSample *sample = akaiGetSampleByName((char *)dev.c_str(), part, (char *)path.c_str(), (char *)name.c_str());
	    if (sample != NULL)
        ass = new ASamplerSample(sample, prefix, id);
      free(sample);
    }
    else
      ass = new ASamplerSample(new WaveFile(string(str)), id);
    if (ass)
    {
      cerr << "[WiredSampler] Loaded wav " << wxString(_T(str)) << endl;
      Samples->List->AddEntry(smpname, (void *)ass);
    }
    else
      cerr << "[WiredSampler] Error loading wav " << wxString(_T(str)) << endl;
    free(str);
  }
  count += read(fd, &nbent, sizeof(nbent));
  cerr << "[WiredSampler] Loading " << nbent << " keygroups..." << endl;
  for (int i = 0; i < nbent; i++)
  {
    long len;
    char *str;

    count += read(fd, &len, sizeof(len));
    str = (char *)malloc(len + 1);
    count += read(fd, str, len);
    str[len] = 0;
    wxString kgname(_T(str));
    free(str);
    cerr << "[WiredSampler] Loaded keygroup " << kgname << endl;
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
    Keygroups->List->AddEntry(kgname, (void *)askg);
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
    count += write(fd, &len, sizeof(len));
    cerr << "[WiredSampler] Saving ID " << len << endl;
    str = (char *)ass->GetSample()->Filename.c_str();
    len = strlen(str);
    cerr << "[WiredSampler] Saving wavefile " << ass->GetSample()->Filename << endl;
    count += write(fd, &len, sizeof(len));
    count += write(fd, str, len);
  }
  entries = Keygroups->List->GetEntries();
  nbent = entries.size();
  count += write(fd, &nbent, sizeof(nbent));
  cerr << "[WiredSampler] Saving " << nbent << " keygroups..." << endl;
  for (vector<ASListEntry *>::iterator i = entries.begin(); i != entries.end(); i++)
  {
    long len;
    char *str;

    cerr << "[WiredSampler] Saving keygroup " << (*i)->GetName() << endl;
    ASamplerKeygroup *askg = (ASamplerKeygroup *)(*i)->GetEntry();
    str = (char *)(*i)->GetName().c_str();
    len = strlen(str);
    count += write(fd, &len, sizeof(len));
    count += write(fd, str, len);
    cerr << "[WiredSampler] Saving ID " << len << endl;
    len = askg->GetID();
    count += write(fd, &len, sizeof(len));
    len = askg->GetLowKey();
    cerr << "[WiredSampler] Saving lokey " << len << endl;
    count += write(fd, &len, sizeof(len));
    len = askg->GetHighKey();
    cerr << "[WiredSampler] Saving hikey " << len << endl;
    count += write(fd, &len, sizeof(len));
    ASamplerSample *smp = askg->GetSample();
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
      wxString samplename = "AkaiSample #";
      samplename << group->num;
      wxString groupname = "AkaiKeygroup #";
      groupname << group->num;
      ASamplerSample *ass = new ASamplerSample(group->zone_sample[0], AkaiPrefix);
      Samples->List->AddEntry(group->zone_sample[0]->name, (void *)ass);
      ASamplerKeygroup *askg = new ASamplerKeygroup(group->lowkey, group->highkey);
      Keygroups->List->AddEntry(groupname, (void *)askg);
      ass->SetKeygroup(askg);
      askg->SetSample(ass);
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

  // Processing des notes
  for (i = Notes.begin(); i != Notes.end(); i++)
  {
    n = *i;
    if (n->Position < n->Key->Wave->GetNumberOfFrames())
    {
      length = sample_length - n->Delta;
      end = (long)((n->Key->Wave->GetNumberOfFrames()) / n->Key->Pitch);
      if (end < (length))
        length = end - n->Delta;
      else
        end = 0;
      n->Key->Wave->SetPitch(n->Key->Pitch);
      n->Key->Wave->Read(n->Buffer, n->Position, length, n->Delta, &(n->Position));

      if (n->Volume != 1.f)
        for (chan = 0; chan < 2; chan++)
          for (idx = n->Delta; idx < length; idx++)
            n->Buffer[chan][idx] *= n->Volume;

      if (n->Delta)
        n->Delta = 0;
    }
  }

  Workshop.GetMix(output);

  // Suppression des notes termin~Aées
  for (i = Notes.begin(); i != Notes.end();)
  {
    if ((*i)->Position >= (*i)->Key->Wave->GetNumberOfFrames())
    {
      Workshop.SetFreeBuffer((*i)->Buffer);
      delete *i;
      i = Notes.erase(i);
    }
    else
      i++;
  }

  Mutex.Unlock();

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
          ASamplerKeygroup *askg = Keygroups->FindKeygroup(event.MidiData[1]);
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
      AkaiPrefix += mName.c_str();
      AkaiPrefix += '/';
      cout << "device: " << mDevice << "; part: " << mPart << "; name: " << mName << "; filename: " << mFilename << endl;
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
