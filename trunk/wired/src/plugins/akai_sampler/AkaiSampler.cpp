// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "AkaiSampler.h"
#include "midi.h"
#include <stdio.h>
#include <math.h>
#include <wx/wx.h>
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

static int			gl_WSId;

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
    : Plugin(startinfo, initinfo), PolyphonyCount(8), Volume(100.f), AkaiProgram(0x0),
      WiredDocument(wxT("AkaySampler"), startinfo.parent)
{

  if (startinfo.saveCenter)
    saveCenter = startinfo.saveCenter;
  else
    cerr << "[AKAISAMPLER] could not get access to save center for some reason..." << endl << "[AKAISAMPLER] Saving might be difficult" << endl;
  setId(saveCenter->RegisterId());

  sampleid = 0;
  keygroupid = 0;
  sp_bg = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_BG)), 
      wxBITMAP_TYPE_PNG);
  if (sp_bg)
  {
    BgBmp = new wxBitmap(*sp_bg);
    delete sp_bg;
  }

  bmp = new wxBitmap(wxString(GetDataDir() + wxString(IMG_SP_BMP)), wxBITMAP_TYPE_BMP);  

  PlugPanel = new ASPlugPanel(this, wxPoint(149, 8), wxSize(642, 120),//GetSize().GetWidth() - 150, GetSize().GetHeight() - ASCLAVIER_HEIGHT - 5), 
            wxTHICK_FRAME , this);

  Samples = new ASSampleList(this, _("Samples"));

  clavier = new ASClavier(this, -1, wxPoint(23, 142), //GetSize().GetWidth() - ASCLAVIER_WIDTH, GetSize().GetHeight() - ASCLAVIER_HEIGHT),
          wxSize(ASCLAVIER_WIDTH, ASCLAVIER_HEIGHT),
          wxSIMPLE_BORDER, this);

  PlugPanel->AddPlug(Samples);
  PlugPanel->ShowPlugin(Samples);

  open_up = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_OPEN_UP)),
      wxBITMAP_TYPE_PNG);
  open_down = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_OPEN_DOWN)), wxBITMAP_TYPE_PNG);
  save_up = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_SAVE_UP)), wxBITMAP_TYPE_PNG);
  save_down = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_SAVE_DOWN)), wxBITMAP_TYPE_PNG);
  fader_bg = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_FADER_BG)),wxBITMAP_TYPE_PNG);
  fader_fg = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_FADER_FG)),wxBITMAP_TYPE_PNG);

  wxImage *sample_up = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_SAMPLE_UP)), wxBITMAP_TYPE_PNG);
  wxImage *sample_down = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_SAMPLE_DOWN)), wxBITMAP_TYPE_PNG);
  wxImage *kg_up = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_KGROUP_UP)), wxBITMAP_TYPE_PNG);
  wxImage *kg_down = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_KGROUP_DOWN)), wxBITMAP_TYPE_PNG);
  wxImage *effect_up = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_EFFECT_UP)), wxBITMAP_TYPE_PNG);
  wxImage *effect_down = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_EFFECT_DOWN)), wxBITMAP_TYPE_PNG);

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

  s.Printf(wxT("%d"), PolyphonyCount);
  PolyCountLabel = new wxStaticText(this, -1, s, wxPoint(76, 100), wxSize(-1, 12));
  PolyCountLabel->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));
  PolyCountLabel->SetLabel(s);

  wxImage** imgs;
  imgs = new wxImage*[3];
  imgs[0] = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_POLY_1)));
  imgs[1] = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_POLY_2)));
  imgs[2] = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_POLY_3)));

  PolyKnob = new CycleKnob(this, Sampler_Poly, 3, imgs, 10, 1, 256, 8,
      wxPoint(97, 97), wxDefaultSize);

  delete imgs[0];
  delete imgs[1];
  delete imgs[2];
  delete imgs;

  wxImage *img_led_off = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_LED_OFF)), wxBITMAP_TYPE_PNG);
  if (img_led_off)
    LedOff = new wxBitmap(*img_led_off);

  wxImage *img_led_on = new wxImage(wxString(GetDataDir() + wxString(IMG_SP_LED_ON)), wxBITMAP_TYPE_PNG);
  if (img_led_on)
    LedOn = new wxBitmap(*img_led_on);

  MidiInBmp = new wxStaticBitmap(this, -1, *LedOff, wxPoint(2, 180));

  /* Envelope */

  VolumeFader = new FaderCtrl(this, Sampler_Volume, fader_bg, fader_fg, 0, 127, &Volume, false, 
      wxPoint(11, 21), wxSize(15, 102));

  AkaiPrefix = _T("");

  MidiVolume[0] = M_CONTROL;
  MidiVolume[1] = 0x7;

  UpdateMidi = false;
  UpdateVolume = false;

  // let's build a _customFileName for autosaving patch :
  wxString customFN_orig = WIRED_SAMPLER_SAVE_PATCH;
  wxString ext;

  gl_WSId++; //WiredSampler counter
  _customFileName = customFN_orig.BeforeLast('.');
  ext = customFN_orig.AfterLast('.');
  _customFileName << gl_WSId << wxT(".") << ext;
  while (wxFileName::FileExists(_customFileName))
  {
	gl_WSId++;
	_customFileName = customFN_orig.BeforeLast('.');
	_customFileName << gl_WSId << wxT(".") << ext;
  } 
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

wxString	AkaiSampler::GetName()
{
  return (PLUGIN_NAME);
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
      wxString name = wxString(group->zone_sample[0]->name, *wxConvCurrent);
      if (group->zone_sample[1])
        name << wxT("/") << wxString(group->zone_sample[1]->name, *wxConvCurrent);
      ASamplerSample *ass = new ASamplerSample(this, group->zone_sample[0], group->zone_sample[1], AkaiPrefix);
      Samples->List->AddEntry(name, (void *)ass);
      ASamplerKeygroup *askg = new ASamplerKeygroup(this, group->lowkey, group->highkey);
      Keygroups.push_back(askg);
      ass->SetKeygroup(askg);
      askg->SetSample(ass);
      ASPlugin *p = new ASLoop(this, ASLoop::GetFXName() + _(" #0 for ") + wxString(group->zone_sample[0]->name, *wxConvCurrent));
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

  // Suppression des notes termin~A?es
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

      // Suppression des notes termin~A?es
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
            //printf("FindKeyGroup: %p\n", askg);
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
              //printf("[SAMPLER] Note added: %d\n", n->Note);
            }
          }
        }
      }
      else
        cout << "[SAMPLER] Max polyphony reached" << endl;

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
  vector<wxString> exts;
  exts.push_back(_("xml\tWiredSampler xml patch file (*.xml)"));
  
  wxString selfile = OpenFileLoader(_("Load Patch"), &exts);
  if (!selfile.empty())
    {
      LoadPatch(selfile);
	  _customFileName = selfile;
    }
  else
    cout << "[WiredSampler] could not open patch file" << endl;
  cout << "OnOpenFile(): end" << endl;
}

void	AkaiSampler::LoadPatch(wxString filename)
{
  SaveElementArray	data;
  int			dataIt;
  wxString		s;

  data = AskData(filename);
  std::cerr << "[WiredSampler] loading : " << filename.mb_str() << std::endl;

  //nettoyer le patch

  for(dataIt = 0; dataIt < data.GetCount(); dataIt++)
  {
	if(data[dataIt]->getKey() == wxT("samples"))
	  LoadSamples(data[dataIt]);
	else if(data[dataIt]->getKey() == wxT("keygroups"))
	  LoadKeygroups(data[dataIt]);
  }
}

void	AkaiSampler::LoadSamples(SaveElement *data)
{
  SaveElementArray	samplesData;

  int			dataIt;

  unsigned long		id;
  wxString		filename;
  wxString		name;
  long			loop_start;
  long			loop_end;
  int			loop_count;
  wxString		s;
  ASamplerSample	*sample;
  WaveFile		*w;

  std::cerr << "Load Samples" << std::endl;

  samplesData = data->getChildren();

  for(dataIt = 0; dataIt < samplesData.GetCount(); dataIt++)
  {
	filename = samplesData[dataIt]->getValue();
	name = samplesData[dataIt]->getAttribute(wxT("name"));
	loop_count = samplesData[dataIt]->getAttributeInt(wxT("loop_count"));
	s.clear();
	s = samplesData[dataIt]->getAttribute(wxT("loop_start"));
	s.ToLong(&loop_start);
	s.clear();
	s = samplesData[dataIt]->getAttribute(wxT("loop_end"));
	s.ToLong(&loop_end);
	s.clear();
	s = samplesData[dataIt]->getAttribute(wxT("id"));
	s.ToULong(&id);
	std::cerr << "data loaded : id " << id << " name " << name.mb_str() << std::endl;
	std::cerr << "loop_start " << loop_start << " loop_end " << loop_end << std::endl;
	std::cerr << "loop_count " << loop_count << std::endl;


	w = new WaveFile(filename, true);
	sample = new ASamplerSample(this, w, id); 

	Samples->List->AddEntry(name, sample);
	sample->SetLoopCount(loop_count);
	sample->SetLoopStart(loop_start);
	sample->SetLoopEnd(loop_end);

	if (id >= sampleid)
	  sampleid = id + 1;
  }
}

void	AkaiSampler::LoadKeygroups(SaveElement *data)
{
  SaveElementArray	keygroupsData;
  int			dataIt;

  int		lo;
  int		hi;
  unsigned long	id;
  unsigned long	sampleId;
  wxString	s;

  ASamplerKeygroup	*keygroup;
  ASamplerSample	*sample;

  keygroupsData = data->getChildren();

  std::cerr << "[WiredSampler] LoadKeygroups" << std::endl;
  std::cerr << "[WiredSampler] " << keygroupsData.GetCount() << " keygroups" << std::endl;
  
  for(dataIt = 0; dataIt < keygroupsData.GetCount(); dataIt++)
    {
      lo = keygroupsData[dataIt]->getAttributeInt(wxT("lokey"));
      hi = keygroupsData[dataIt]->getAttributeInt(wxT("hikey"));
      s.clear();
      s = keygroupsData[dataIt]->getAttribute(wxT("id"));
      s.ToULong(&id);
      s.clear();
      s = keygroupsData[dataIt]->getAttribute(wxT("sample_id"));
      s.ToULong(&sampleId);

      std::cerr << "data loaded : lo = " << lo << " hi = " << hi << std::endl;
      std::cerr << "       sample_id = " << sampleId << " id = " << id << std::endl;

      sample = Samples->GetSampleById(sampleId);
      if(!sample)
	std::cerr << "sample not found : id : " << sampleId << std::endl;
      else
	{
	  keygroup = new ASamplerKeygroup(this, lo, hi, id);
	  
	  sample->SetKeygroup(keygroup);

	  keygroup->SetSample(sample);
	  keygroup->SetLowKey(lo);
	  keygroup->SetHighKey(hi);
	  
	  Keygroups.push_back(keygroup);
	}
    }

}

void	AkaiSampler::Load(SaveElementArray data)
{
  int	i;

  for( i = 0 ; i < data.GetCount() ; i++ )
	if (data[i]->getKey() == wxT("reference"))
	  _customFileName = data[i]->getValue();
  if(wxFileName::FileExists(_customFileName))
	LoadPatch(_customFileName);
  else
	cerr << "[WiredSampler] patch not found : '" << _customFileName.mb_str() << "'" << endl;
}

void AkaiSampler::OnSaveFile(wxCommandEvent &event)
{
  vector<wxString>	exts;
  wxFileName		filename;
  wxString			str_filename;
  
  exts.push_back(_("xml\tWiredSampler xml patch file (*.xml)"));
  wxString selfile = SaveFileLoader(_("Save Patch"), &exts);
  if (selfile.Right(4) != wxString(wxT(".xml")) || selfile.Right(4) != wxString(wxT(".XML")))
	selfile += wxT(".xml");
  filename.Assign(selfile);
  if (filename.IsOk())
  {
	cout << "[WIREDSAMPLER] will try to write this file : '" << selfile.mb_str() << "'" <<endl;
	SaveXmlPatch(selfile);
	_customFileName = selfile;
	cout << "[WIREDSAMPLER] Calling savecenter to write the file" << endl;
	saveCenter->SaveOneDocument(this, _customFileName);
	//SavePatch(wxString(GetName()) << wxT("/patch"), selfile);      
  }
  else
    cout << "[WiredSampler] could not open save file" << endl;
  cout << "OnSavePatch(): end" << endl;
}

void	AkaiSampler::Save()
{  
  SaveXmlPatch(_customFileName);
}

void	AkaiSampler::SaveXmlPatch(wxString filename)
{
  SaveSamples(filename);
  SaveKeygroups(filename);
}

void	AkaiSampler::SaveSamples(wxString filename)
{
  SaveElement		*samplesData = new SaveElement();
  SaveElement		*sampleData;
  SaveElement		*effectData;

  vector<ASListEntry *>			SamplesListEntries;
  vector<ASListEntry *>::iterator	SamplesListIt;

  ASamplerSample		*sample; 

  vector<ASPlugin *>		effects;
  vector<ASPlugin *>::iterator	effectsIt;
  
  wxString	s;

  samplesData->setKey(wxT("samples"));

  SamplesListEntries = Samples->List->GetEntries();
  
  for (SamplesListIt = SamplesListEntries.begin(); 
       SamplesListIt != SamplesListEntries.end(); 
       SamplesListIt++)
  {
    sampleData = new SaveElement();

    sample = (ASamplerSample *)(*SamplesListIt)->GetEntry();

    sampleData->setKey(wxT("sample"));
    sampleData->setValue(sample->GetSample()->Filename);

    s.clear();
    s << sample->GetID();
    sampleData->addAttribute(wxT("id"), s);

    sampleData->addAttribute(wxT("name"), 
			     (*SamplesListIt)->GetName());

    s.clear();
    s << sample->GetLoopStart();
    sampleData->addAttribute(wxT("loop_start"), s);

    s.clear();
    s << sample->GetLoopCount();
    sampleData->addAttribute(wxT("loop_count"), s);

    s.clear();
    s << sample->GetLoopEnd();
    sampleData->addAttribute(wxT("loop_end"), s);

    effects = sample->GetEffects();
    for (effectsIt = effects.begin(); 
	 effectsIt != effects.end(); 
	 effectsIt++)
    {
      effectData = new SaveElement();
      effectData->setKey(wxT("effect"));
      effectData->addAttribute(wxT("type"),
			       (*effectsIt)->GetType());
      effectData->addAttribute(wxT("name"),
			       (*effectsIt)->Name);

      //first we have to implement SavePatch() in every plugin
      //      effectData->addChild((*effectIt)->SavePatch());
    }
    samplesData->addChildren(sampleData);
  }
  saveDocData(samplesData, filename);
}

void	AkaiSampler::SaveKeygroups(wxString filename)
{
  SaveElement	*keygroupsData;
  SaveElement	*keygroupData;

  vector<ASamplerKeygroup*>::iterator	keygroupsIt;

  ASamplerSample	*sample;
  wxString		s;

  keygroupsData = new SaveElement();
  keygroupsData->setKey(wxT("keygroups"));


  for (keygroupsIt = Keygroups.begin(); 
       keygroupsIt != Keygroups.end(); 
       keygroupsIt++)
  {
    keygroupData = new SaveElement();

    keygroupData->setKey(wxT("keygroup"));

    s.clear();
    s << (*keygroupsIt)->GetID();
    keygroupData->addAttribute(wxT("id"), s);
    keygroupData->addAttribute(wxT("lokey"), 
			       (*keygroupsIt)->GetLowKey());
    keygroupData->addAttribute(wxT("hikey"), 
			       (*keygroupsIt)->GetHighKey());
    
    sample = (*keygroupsIt)->GetSample();
    if (sample)
      {
	s.clear();
	s << sample->GetID();
	keygroupData->addAttribute(wxT("sample_id"), s);
      }
    keygroupsData->addChildren(keygroupData);
  }
  saveDocData(keygroupsData, filename);

}

void AkaiSampler::OnVolume(wxScrollEvent &event)
{
  Mutex.Lock();
  float RealVolume = VolumeFader->GetValue() / 127.f;
  Workshop.SetVolume(RealVolume);

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
    s.Printf(wxT("%d"), PolyphonyCount);
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
      aske = new ASKeygroupEditor(this, wxString(_("Keygroup editor for ")) + e->GetName());
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
        s << wxT(" #");
        s << count;
        s << _(" for ") ;
        s << e->GetName();
        p = new ASEnvel(this, s);
        break;
      case 2:
        // Looping
        for (vector<ASPlugin *>::iterator i = plugs.begin(); i != plugs.end(); i++)
          if ((*i)->GetType() == EFFECTSNAMES[1])
            count++;
        s << EFFECTSNAMES[1];
        s << wxT(" #");
        s << count;
        s << _(" for ") ;
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
    menu->Append(num++, _("New effect"), newplug);
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

/******** Main and mandatory library functions *********/

extern "C"
{
  PlugInitInfo init()
  {
    WIRED_MAKE_STR(info.UniqueId, "PLWS");
    info.Name = PLUGIN_NAME;
    info.Type = ePlugTypeInstrument;
    info.UnitsX = 4;
    info.UnitsY = 2;
    info.Version = 1;

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
