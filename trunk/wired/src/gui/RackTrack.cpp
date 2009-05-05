// Copyright (C) 2004-2009 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "RackTrack.h"
#include "Mixer.h"
#include "MixerGui.h"
#include "HelpPanel.h"
#include "OptionPanel.h"
#include "Sequencer.h"

/********************   Class RackTrack   ********************/

RackTrack::RackTrack(Rack *parent, int index)
  : WiredDocument(wxT("RackTrack"), (WiredDocument*)parent)
{
  wxString s;

  Parent = parent;
  Index = index;
  Units = 0;
  CurrentBuffer = 0x0;
  s.Printf(_("Rack %d"), index + 1);

  Output = Mix->AddChannel(false, true, true);
  ChanGui = MixerPanel->AddChannel(Output, s);
}

RackTrack::~RackTrack()
{
  DeleteAllRacks();
  RemoveChannel();
}

void				RackTrack::RemoveChannel()
{
  MixerPanel->RemoveChannel(ChanGui);
  Mix->RemoveChannel(Output);
}

Plugin*				RackTrack::CreateRack(PlugStartInfo &startinfo, PluginLoader *p)
{
  int xx, yy, xpos, ypos;
  static int num = 31000;
  Plugin *plug;

  xpos = Parent->GetXPos(Index);
  ypos = GetYPos();

  Parent->CalcScrolledPosition(xpos, ypos, &xx, &yy);
  startinfo.Pos = wxPoint(xx, yy);
  startinfo.Size = wxSize(p->InitInfo.UnitsX * UNIT_W, p->InitInfo.UnitsY * UNIT_H);
  startinfo.saveCenter = saveCenter;
  startinfo.parent = this;
  plug = p->CreateRack(startinfo);

  if (!plug)
    return (NULL);

  if (p->InitInfo.UnitsX > Units)
    Units = p->InitInfo.UnitsX;

  // Plug initialization
  plug->SetId(num++);
  plug->SetBufferSize(Audio->SamplesPerBuffer);
  plug->SetSamplingRate(Audio->SampleRate);
  plug->Init();
  plug->SetHelpMode(HelpWin->IsShown());

  if (Seq->Playing)
    plug->Play();

  wxChar str[128];
  wxSnprintf(str, 128, wxT("%d"), ++RackCount);
  plug->Name = plug->DefaultName() + wxT(" ") + str;
  AddRack(plug);
  Parent->ResizeTracks();
  Parent->SetScrolling();
  return (plug);
}

void				RackTrack::SetSelected(Plugin *plugin)
{
  /*
  //Dump();
  Racks.remove(SelectedPlugin);
  //Dump();
  SelectedPlugin->Hide();
  SeqPanel->RemoveReferenceTo(SelectedPlugin);
  //delete SelectedPlugin;
  Parent->ResizeTracks();
  Parent->SetScrolling();
  */
  SelectedPlugin = plugin;
}

void				RackTrack::AddRack(Plugin* plug)
{
  wxMutexLocker			locker(SeqMutex);

  Racks.push_back(plug);
}

void				RackTrack::RemoveSelectedRack()
{
  Racks.remove(SelectedPlugin);

  SelectedPlugin->Hide();
  SeqPanel->RemoveReferenceTo(SelectedPlugin);

  Parent->ResizeTracks();
}

void				RackTrack::DeleteAllRacks()
{
  list<Plugin *>::iterator j;

  for (j = Racks.begin(); j != Racks.end(); j++)
    {
      OptPanel->ClosePlug(*j);
      SeqPanel->RemoveReferenceTo(*j);
      delete *j;
    }
  Units = 0;
  Racks.clear();
}

int					RackTrack::GetYPos()
{
  int u = 0;
  list<Plugin *>::iterator i;

  for (i = Racks.begin(); i != Racks.end(); i++)
    u += (*i)->InitInfo->UnitsY;
  return (u * (UNIT_H + UNIT_S));
}

void				RackTrack::Dump()
{
  cout << "  Dumping RackTrack at adress " << this << endl;
  cout << "    int Units : " << Units << endl;
  cout << "    Rack* Parent : " << Parent << endl;
  cout << "    int index : " << Index << endl;
  cout << "    Channel Output : " << Output << endl;
  cout << "    ChannelGui* ChanGui : " << ChanGui << endl;
  cout << "    float **CurrentBuffer : " << CurrentBuffer << endl;
  DumpPlugins();
  cout << "  End Dumping RackTrack" << endl;
}

void				RackTrack::DumpPlugins()
{
  list<Plugin *>::iterator iterRack;

  for (iterRack = Racks.begin(); iterRack != Racks.end(); iterRack++)
    {
      cout << "    Dumping Plugin at adress " << (*iterRack) << endl;
      cout << "      Position x : "  << (*iterRack)->GetPosition().x << endl;
      cout << "      Position y : "  << (*iterRack)->GetPosition().y << endl;
      cout << "      Virtual size x : " << (*iterRack)->GetVirtualSize().x << endl;
      cout << "      Virtual size y : " << (*iterRack)->GetVirtualSize().y << endl;
      cout << "    End dumping Plugin" << endl;
    }
}

void				RackTrack::Save()
{
}

void				RackTrack::Load(SaveElementArray data)
{

}
