// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "Rack.h"
#include "SequencerGui.h"
#include "HelpPanel.h"
#include "AudioEngine.h"
#include "OptionPanel.h"

extern wxMutex SeqMutex;
int	RackCount = 0;

BEGIN_EVENT_TABLE(Rack, wxScrolledWindow)
  EVT_ENTER_WINDOW(Rack::OnHelp)
  EVT_LEFT_DOWN(Rack::OnClick)
  EVT_LEFT_UP(Rack::OnClick)
END_EVENT_TABLE()

RackTrack::RackTrack(Rack *parent, int index)
  : Parent(parent), Index(index)
{
  Units = 0;
  CurrentBuffer = 0x0;
  wxString s;
  s.Printf("Rack %d", index);
  Output = Mix->AddStereoOutputChannel(s);
}

RackTrack::~RackTrack()
{
  
  Mix->RemoveChannel(Output);
}

void RackTrack::RemoveChannel()
{
  Mix->RemoveChannel(Output);
}

Plugin *RackTrack::AddRack(PlugStartInfo &startinfo, PluginLoader *p, Plugin *connect_to)
{
  int xx, yy, xpos, ypos;
  static int num = 31000;
  Plugin *plug;

  xpos = Parent->GetXPos(Index);
  ypos = GetYPos();

  /*  if (connect_to)
    {
      if (Units - connect_to->GetUnitsX() >= plug->GetUnitsX())
	{
	  xpos += connect_to->GetUnitsX() * UNIT_W + UNIT_S;
	  ypos -= connect_to->GetUnitsY() * UNIT_H + UNIT_S;
	}
	}*/
  Parent->CalcScrolledPosition(xpos, ypos, &xx, &yy);

  startinfo.Pos = wxPoint(xx, yy);  
  startinfo.Size = wxSize(p->InitInfo.UnitsX * UNIT_W, p->InitInfo.UnitsY * UNIT_H); 
  plug = p->CreateRack(startinfo);

  if (p->InitInfo.UnitsX > Units)
    Units = p->InitInfo.UnitsX;

  // Plug initialization

  plug->SetId(num++);
  plug->SetBufferSize(Audio->SamplesPerBuffer);
  plug->SetSamplingRate(Audio->SampleRate);
  plug->Init();

  plug->SetHelpMode(HelpWin->IsShown());

  char str[128];
  
  sprintf(str, "%d", ++RackCount);

  plug->Name = plug->DefaultName() + " " + str;

  SeqMutex.Lock();
  Racks.push_back(plug);
  SeqMutex.Unlock();

  Parent->ResizeTracks();
  Parent->SetScrolling();
  return (plug);
}

void RackTrack::RemoveRack()
{
  Plugin* plugin = Racks.back();
  Racks.pop_back();
  plugin->Hide();
  delete plugin;
  Parent->ResizeTracks();
  Parent->SetScrolling();
}

void RackTrack::DeleteAllRacks()
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

int RackTrack::GetYPos()
{
  int u = 0;
  list<Plugin *>::iterator i;

  for (i = Racks.begin(); i != Racks.end(); i++)
    u += (*i)->InitInfo->UnitsY;
  return (u * (UNIT_H + UNIT_S));
}

Rack::Rack(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
	   const wxSize& size) :
  wxScrolledWindow(parent, id, pos, size, wxSUNKEN_BORDER), WasDragging(false)
{
  SetScrollRate(10, 10);
  SetVirtualSize(760, 180);  
  selectedPlugin = 0x0;
  selectedTrack = 0x0;
}

Rack::~Rack()
{
  list<RackTrack *>::iterator i;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)
    delete *i;
}

void Rack::AddTrack(PlugStartInfo &startinfo, PluginLoader *p)
{
  RackTrack *t;

  t = new RackTrack(this, RackTracks.size());
  t->AddRack(startinfo, p);

  SeqMutex.Lock(); 
  RackTracks.push_back(t);
  SeqMutex.Unlock();      

  SetScrolling();
}

void Rack::RemoveTrack()
{
  SeqMutex.Lock();
  RackTrack* rackTrack = RackTracks.back();
  rackTrack->RemoveRack();
  rackTrack->RemoveChannel();
  RackTracks.pop_back();
  SeqMutex.Unlock();
  SetScrolling();
}

RackTrack *Rack::AddTrack()
{
  RackTrack *t;

  SeqMutex.Lock(); 
  t = new RackTrack(this, RackTracks.size());
  RackTracks.push_back(t);
  SeqMutex.Unlock();      
  return (t);
}

void Rack::AddToSelectedTrack(PlugStartInfo &startinfo, PluginLoader *p)
{
  //if (!selectedTrack && (RackTracks.size() > 0))
  //  selectedTrack = *(RackTracks.begin());
  if (selectedTrack)
    {
      selectedTrack->AddRack(startinfo, p, selectedTrack->Racks.back());
    }
  else
    AddTrack(startinfo, p);
}

void Rack::RemoveFromSelectedTrack()
{
  if (selectedTrack)
    {
      SeqMutex.Lock();
      selectedTrack->RemoveRack();
      SeqMutex.Unlock();
    }
  else
    RemoveTrack();
}

void Rack::SetScrolling()
{
  int x, y;
  list<RackTrack *>::iterator i;

  x = 0; //GetXPos(RackTracks.size());
  y = 0;  
  for (i = RackTracks.begin(); i != RackTracks.end(); i++)
    {
      x += (*i)->Units;
      if ((*i)->GetYPos() > y)
	y = (*i)->GetYPos();
    }
  x *= UNIT_W + UNIT_S;
  SetVirtualSize(x, y);  
}

int Rack::GetXPos(int index)
{
  list<RackTrack *>::iterator i;
  int u = 0;

  for (i = RackTracks.begin(); 
       (i != RackTracks.end()) && ((*i)->Index != index); i++)
    u += (*i)->Units;
  return (u * (UNIT_W + UNIT_S));
}  

void Rack::DeleteAllRacks()
{
  list<RackTrack *>::iterator i;
  list<Plugin *>::iterator j;
  
  /*  while (!RackTracks.empty())
    {
      for (i = RackTracks.begin(); i != RackTracks.end(); i++)
	{
	  for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	    {
	      DeleteRack(*j);
	      break;
	    }
	  break;
	}
	}
  */
  
  for (i = RackTracks.begin(); i != RackTracks.end(); i++)
    {
      (*i)->DeleteAllRacks();
      delete *i;
    }
  RackTracks.clear();
  ResizeTracks();
  SetScrolling();

  /*  for (i = RackTracks.begin(); i != RackTracks.end(); i++)  
    {
      for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	delete *j;
      delete *i;
    }
    RackTracks.clear(); */
  selectedTrack = 0x0;
  selectedPlugin = 0x0;
}

void Rack::DeleteRack(Plugin *plug)
{
  list<RackTrack *>::iterator i;
  list<Plugin *>::iterator j, k;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)  
    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
      if (*j == plug)
	{
	  OptPanel->ClosePlug(plug);
	  (*i)->Racks.erase(j);
	  (*i)->Units = 0;
	  for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	    {
	      if ((*j)->InitInfo->UnitsX > (*i)->Units)
		(*i)->Units = (*j)->InitInfo->UnitsX;
	    }
	  if ((*i)->Racks.size() == 0)
	    {
	      delete (*i);
	      RackTracks.erase(i);
	      selectedTrack = 0x0;
	    }
	  selectedPlugin = 0x0;
	  ResizeTracks();
	  SetScrolling();
	  SeqPanel->RemoveReferenceTo(plug);
	  return;
	}
}

void Rack::SetSelected(Plugin *p)
{
  list<RackTrack *>::iterator i;
  list<Plugin *>::iterator j;

  if (!p)
    {
      selectedPlugin = 0x0;
      selectedTrack = 0x0;
    }
  else if (selectedPlugin != p)
    {
      selectedPlugin = p;
      for (i = RackTracks.begin(); i != RackTracks.end(); i++)  
	for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	  if (*j == p)
	    {
	      selectedTrack = *i;
	      return;
	    }
    }
}

void Rack::HandleMouseEvent(Plugin *plug, wxMouseEvent *event)
{
  list<RackTrack *>::iterator i;
  list<RackTrack *>::iterator k;
  list<Plugin *>::iterator j;
  list<Plugin *>::iterator l;
  int pos_x = 0;
  int pos_y = 0;
  int new_x = 0;
  int new_y = 0;
  int flag = 0;

  if (event->GetEventType() == wxEVT_MOUSEWHEEL)
    {
      int x, y, y1, y2, y3;

      GetVirtualSize(0x0, &y1);
      GetSize(0x0, &y2);
      GetViewStart(&x, &y3);
      if (y1 > y2)
	{	  
	  if (event->GetWheelRotation() > 0)
	    y =  -1;
	  else
	    y = 1;
	  Scroll(x, y3 + y);
	}
    }
  else if (event->Dragging())
    {
      plug->Move(wxPoint(event->GetPosition().x + plug->GetPosition().x - OldX, event->GetPosition().y + plug->GetPosition().y - OldY));      
      WasDragging = true;
    }
  else if (event->LeftIsDown())
    {
      OldX = event->GetPosition().x;
      OldY = event->GetPosition().y;
      SetSelected(plug);
    }
    
  else if(event->LeftUp() && WasDragging)
    {
      WasDragging = false;
      SeqMutex.Lock();

      new_x = (event->GetPosition().x + plug->GetPosition().x);
      new_y = (event->GetPosition().y + plug->GetPosition().y);
      for (i = RackTracks.begin(); i != RackTracks.end(); i++)
	{
	  pos_x = pos_x + (*i)->Units * UNIT_W ;
	  if(((pos_x - ((*i)->Units * UNIT_W)) < new_x) && (new_x < pos_x) )
	    {
	      for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
		{
		  pos_y = pos_y + (*j)->InitInfo->UnitsY * UNIT_H;
		  if(((pos_y - ((*j)->InitInfo->UnitsY * UNIT_H)) < new_y) && (new_y < pos_y))
		    {
		   
		      for (k = RackTracks.begin(); (k != RackTracks.end()) && (flag == 0); k++)  
			{
			  for (l = (*k)->Racks.begin(); l != (*k)->Racks.end(); l++)
			    {
			      if (*l == plug)
				{
				  //(*k)->Racks.erase(l);
				  if((*k)->Racks.size() == 0)
				    {
				      //delete *k;
				      //  RackTracks.erase(k);
				      flag = 1;
				    }		
				  break;
				}
			    }
			}
		      if(plug != *j)
			{
			  //(*i)->Racks.insert(j,plug); 
			  ResizeTracks();
			}
		      break;
		    }
		  else
		    ResizeTracks(); 
		}
	      break;
	    }
	  else
	    ResizeTracks(); 
	  
	}
      SeqMutex.Unlock();
    }
}

void Rack::HandleKeyEvent(Plugin *plug, wxKeyEvent *event)
{
  if (event->GetKeyCode() == WXK_DOWN)
    {
      //cout << "[RACKPANEL] key received: down arrow" << endl;
    }
}

void Rack::ResizeTracks()
{
  int xx, yy, xpos, ypos, k;
  list<RackTrack *>::iterator i;
  list<Plugin *>::iterator j;

  xx = 0;
  for (k = 0, i = RackTracks.begin(); i != RackTracks.end(); i++, k++)  
    {
      yy = 0;
      (*i)->Index = k;
      for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	{
	  CalcScrolledPosition(xx * (UNIT_W + UNIT_S), yy * (UNIT_H + UNIT_S), &xpos, &ypos);
	  (*j)->SetPosition(wxPoint(xpos, ypos));
	  yy += (*j)->InitInfo->UnitsY;
	}
      xx += (*i)->Units;
      }
}

RackTrack *Rack::GetRackTrack(Plugin *plug)
{
  list<RackTrack *>::iterator i;
  list<Plugin *>::iterator j;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)  
    {
      for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)  
	if (*j == plug)
	  return (*i);
    }
  return (0x0);
}

void Rack::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("This is the Rack view of Wired, where you can add plugins. Plugins are organized into tracks vertically, meaning the first plugin in a column will send its output signal to the plugin below it. You can connect sequencer tracks to plugins");
      HelpWin->SetText(s);
    }
}

void Rack::OnClick(wxMouseEvent &event)
{
  selectedTrack = 0x0;
  selectedPlugin = 0x0;
}
