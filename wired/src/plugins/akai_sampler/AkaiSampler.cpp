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
/*
  PlugPanel = new ASPlugPanel(this, wxPoint(150, 0), wxSize(GetSize().GetWidth() - 150, GetSize().GetHeight() - ASCLAVIER_HEIGHT - 5), wxTHICK_FRAME);

  ASEnvel *envel = new ASEnvel(wxString("Envelope"));
  PlugPanel->AddPlug(envel);
  */
  test = new ASList(this, -1, wxPoint(150, 0), wxSize(110, GetSize().GetHeight() - ASCLAVIER_HEIGHT - 5), "Samples");

  memset(Keys, 0x0, sizeof(ASamplerKey *) * 127);

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
  return TRUE;
}

bool AkaiSampler::IsMidi()
{
  return TRUE;
}

void AkaiSampler::Load(int fd, long size)
{

}

long AkaiSampler::Save(int fd)
{
  return (Plugin::Save(fd));
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
    wxString s = "Sample #";
    s << group->num;
    test->AddEntry(s, group);
    if (group->zone_sample[0])
      for (i = group->lowkey; i <= group->highkey; i++)
      {
        Keys[i] = new ASamplerKey(group->zone_sample[0], powf(2.f, static_cast<float>(i - group->lowkey) / 12.f));
        cout << "creating key: " << i << endl;
      }
  }
}


void AkaiSampler::DeleteProgram()
{
  if (AkaiProgram)
  {
    for (int i = 0; i < 127; i++)
    {
      if (Keys[i])
        delete Keys[i];
    }
    //delete 
    free(AkaiProgram);
    AkaiProgram = 0x0;
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

  if (!AkaiProgram)
  {
    Mutex.Unlock();
    return;
  }
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
          if (Keys[event.MidiData[1]])
          {
            ASamplerNote *n = new ASamplerNote(event.MidiData[1],
                event.MidiData[2] / 100.f,
                Keys[event.MidiData[1]],
                event.DeltaFrames,
                Workshop.GetFreeBuffer(),
                event.NoteLength);
            Notes.push_back(n);
            printf("[SAMPLER] Note added: %d\n", n->Note);
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
      cout << "device: " << mDevice << "; part: " << mPart << "; name: " << mName << "; filename: " << mFilename << endl;
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
    WIRED_MAKE_STR(info.UniqueId, "PLAS");
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
