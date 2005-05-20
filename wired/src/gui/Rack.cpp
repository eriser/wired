// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include			"SeqTrack.h"
#include			"Track.h"
#include			"Rack.h"
#include			"SequencerGui.h"
#include			"HelpPanel.h"
#include			"AudioEngine.h"
#include			"OptionPanel.h"
#include			"MixerGui.h"
#include			"MainWindow.h"
#include			"cImportMidiAction.h"
#include			"../sequencer/Sequencer.h"
#include			"../mixer/Mixer.h"
#include			"../redist/Plugin.h"
#include			"../plugins/PluginLoader.h"

extern wxMutex		SeqMutex;
int					RackCount = 0;


/********************   Class RackTrack   ********************/

RackTrack::RackTrack(Rack *parent, int index)
  : Parent(parent), Index(index)
{
  Units = 0;
  CurrentBuffer = 0x0;
  wxString s;
  s.Printf("Rack %d", index + 1);
  Output = Mix->AddStereoOutputChannel(true);
  ChanGui = MixerPanel->AddChannel(Output, s);
}

RackTrack::~RackTrack()
{
  RemoveChannel();
}

void				RackTrack::RemoveChannel()
{
  //cout << "removing changui" << endl;
  MixerPanel->RemoveChannel(ChanGui);
  Mix->RemoveChannel(Output);
}

Plugin*				RackTrack::AddRack(PlugStartInfo &startinfo, PluginLoader *p, Plugin *connect_to)
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

  if (Seq->Playing)
    plug->Play();

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

void				RackTrack::RemoveRack()
{
	//Dump();
	Plugin* plugin = Racks.back();
	Racks.pop_back();
  	//Dump();
	plugin->Hide();
	delete plugin;
	Parent->ResizeTracks();
	Parent->SetScrolling();
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

/********************   Class Rack   ********************/

Rack::Rack(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
	   const wxSize& size) :
  wxScrolledWindow(parent, id, pos, size, wxSUNKEN_BORDER), WasDragging(false)
{
	SetScrollRate(10, 10);
	SetVirtualSize(760, 180);  
	selectedPlugin = 0x0;
	selectedTrack = 0x0;
	fd_copy = -1;
	is_cut = false;
	InitContextMenu();
}

Rack::~Rack()
{
  t_ListRackTrack::iterator i;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)
    delete *i;

  if(fd_copy != -1)
    {
      close(fd_copy);
      if(wxRemoveFile("/tmp/.tmpccp") == false)
	cout << "error supression\n" <<endl;
      
    }
}

void				Rack::InitContextMenu()
{
	menu = new wxMenu();
	submenu = new wxMenu();
	instr_menu = new wxMenu();
	effects_menu  = new wxMenu();

	menu->Append(ID_MENU_ADD, _T("Add"), submenu);
	submenu->Append(ID_INSTR_MENU, _T("&Instruments"), instr_menu);
	submenu->Append(ID_EFFECTS_MENU, _T("&Effects"), effects_menu);
	menu->Append(ID_MENU_CUT, _T("Cut"));
	menu->Append(ID_MENU_COPY, _T("Copy"));
	menu->Append(ID_MENU_PASTE, _T("Paste"), false);
	menu->AppendSeparator();
	menu->Append(ID_MENU_DELETE, _T("Delete"));
	menu->Enable(ID_MENU_PASTE, false);

	AddPlugToMenu();

	Connect(ID_MENU_CUT, wxEVT_COMMAND_MENU_SELECTED, 
		(wxObjectEventFunction)(wxEventFunction)
		(wxCommandEventFunction)&Rack::OnCutClick);
	Connect(ID_MENU_COPY, wxEVT_COMMAND_MENU_SELECTED, 
		(wxObjectEventFunction)(wxEventFunction)
		(wxCommandEventFunction)&Rack::OnCopyClick);
	Connect(ID_MENU_PASTE, wxEVT_COMMAND_MENU_SELECTED, 
		(wxObjectEventFunction)(wxEventFunction)
		(wxCommandEventFunction)&Rack::OnPasteClick);
	Connect(ID_MENU_DELETE, wxEVT_COMMAND_MENU_SELECTED, 
		(wxObjectEventFunction)(wxEventFunction)
		(wxCommandEventFunction)&Rack::OnDeleteClick);	
}

void				Rack::ConnectPluginChangeParamEventHandler(RackTrack *rackTrack)
{
	t_ListPluginIterator	iter;
	
	for (iter = rackTrack->Racks.begin(); iter != rackTrack->Racks.end(); iter++)
	{
		cout << "One connection done" << endl;
		(*iter)->Connect(wxID_ANY, wxEVT_LEFT_UP, (wxObjectEventFunction) (wxEventFunction) 
			(wxMouseEventFunction)&Rack::OnPluginParamChange);
	}
}

t_RackTrackPlugin*	Rack::AddRackAndChannel(PlugStartInfo &startinfo, PluginLoader *p)
{
	t_RackTrackPlugin	*result;
	RackTrack			*t;
	Plugin				*tmp;

	result = new t_RackTrackPlugin();
	t = new RackTrack(this, RackTracks.size());
	tmp = t->AddRack(startinfo, p);
	cout << "AddRackAndChannel(PlugStartInfo &startinfo, PluginLoader *p)" << endl;
 	ConnectPluginChangeParamEventHandler(t);
	SeqMutex.Lock(); 
	RackTracks.push_back(t);
	SeqMutex.Unlock();
  	result->rackTrack = t;
  	result->plugin = tmp;
  	ResizeTracks();
	SetScrolling();
	return (result);
}

Plugin*				Rack::AddSelectedRackAndChannel(PlugStartInfo &startinfo, PluginLoader *p)
{
	t_RackTrackPlugin*	rackTrackPlugin;
	
	rackTrackPlugin = AddRackAndChannel(startinfo, p);
	selectedTrack = rackTrackPlugin->rackTrack;
	selectedPlugin = rackTrackPlugin->plugin;
	delete rackTrackPlugin;
}

bool				Rack::RemoveTrack(int index)
{
	t_ListRackTrack::const_iterator		iter;
	int									cptRacks;

	if (index > (RackTracks.size() - 1) || index < 0) return false;					//TODO: Exception handling
	for (iter = RackTracks.begin(), cptRacks = 0; cptRacks < index; iter++, cptRacks++) ;
	RemoveRackAndChannel(iter);
	return true;
}

bool				Rack::RemoveTrack(const RackTrack* rackTrack)
{
	t_ListRackTrack::const_iterator	iter;
	unsigned int					cpt;

	for (iter = RackTracks.begin(), cpt = 0; (*iter) != rackTrack && iter != RackTracks.end(); iter++, cpt++) ;
	if (iter == RackTracks.end())
		return false;										//TODO Exception handling
	RemoveRackAndChannel(iter);
	return true;
}

void				Rack::RemoveRackAndChannel(t_ListRackTrack::const_iterator iter)
{
	//wxMutexLocker lock(SeqMutex);
	SeqMutex.Lock();
	UpdateConnectedSeqTracksFromDeletedRacks(iter);
	(*iter)->RemoveRack();
	(*iter)->RemoveChannel();
	RackTracks.remove(*iter);
	ResizeTracks();
	SetScrolling();
	SeqMutex.Unlock();
}

void				Rack::UpdateConnectedSeqTracksFromDeletedRacks(t_ListRackTrack::const_iterator	iterRackTrack)
{
	vector<Track *>::const_iterator	iter;
	
	for (iter = Seq->Tracks.begin(); iter != Seq->Tracks.end(); iter++)
	{
		if ((*iter)->TrackOpt->ConnectedRackTrack == (*iterRackTrack))
		{
			(*iter)->TrackOpt->ConnectedRackTrack = 0;
			(*iter)->TrackOpt->Connected = 0;
		}
	}
}

bool				Rack::RemoveSelectedRackAndChannel()
{
	if (selectedTrack)
		if (RemoveTrack(selectedTrack) == true)
		{
			selectedTrack = 0;
			selectedPlugin = 0;
			return true;
		}
	return false;
}

void				Rack::SetScrolling()
{
	int x, y;
	t_ListRackTrack::iterator i;

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
	//Dump();
}

int					Rack::GetXPos(int index)
{
  t_ListRackTrack::iterator i;
  int u = 0;

  for (i = RackTracks.begin(); 
       (i != RackTracks.end()) && ((*i)->Index != index); i++)
    u += (*i)->Units;
  return (u * (UNIT_W + UNIT_S));
}  

void				Rack::DeleteAllRacks()
{
  t_ListRackTrack::iterator i;
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

void				Rack::DeleteRack(Plugin *plug)
{
	t_ListRackTrack::iterator i;
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

void				Rack::SetSelected(Plugin *p)
{
  t_ListRackTrack::iterator i;
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

void				Rack::HandleMouseEvent(Plugin *plug, wxMouseEvent *event)
{
	t_ListRackTrack::iterator i;
	t_ListRackTrack::iterator k;
	list<Plugin *>::iterator j;
	list<Plugin *>::iterator l;
	new_x = 0;
	new_y = 0;
	int tmp_x = 0;
	int tmp_y = 0;
	int x , y = 0;
	int xx, yy = 0;

	if (event->GetEventType() == wxEVT_MOUSEWHEEL)
    {
	    int x, y, y1, y2, y3;
     
	    GetVirtualSize(0x0, &y1);
	    GetSize(0x0, &y2);
	    GetViewStart(&x, &y3);
	    if (y1 > y2)
		{	  
			if (event->GetWheelRotation() > 0)
			    y = -1;
			else
			    y = 1;
			Scroll(x, y3 + y);
		}
    }
	else if (event->Dragging() && event->LeftIsDown())
    {
    	tmp_x = event->GetPosition().x + plug->GetPosition().x - OldX;
	    tmp_y = event->GetPosition().y + plug->GetPosition().y - OldY;
	    if(tmp_x < 0)
			tmp_x = 0;
	    if(tmp_y < 0)
			tmp_y = 0;
	    plug->Move(wxPoint(tmp_x, tmp_y));
	    WasDragging = true;
    }
  
	if(event->LeftDown())
	{
    	OldX = event->GetPosition().x;
	    OldY = event->GetPosition().y;

    	Plugin *oldplug = selectedPlugin;	 
	    SetSelected(plug);
	    if (oldplug)
			oldplug->Refresh();
       
	    new_x = (event->GetPosition().x + plug->GetPosition().x);
    	new_y = (event->GetPosition().y + plug->GetPosition().y);
    }
    
	if(event->RightDown())
    {
    	SetSelected(plug);
	    wxPoint p(event->GetPosition().x + plug->GetPosition().x, event->GetPosition().y + plug->GetPosition().y);
	    PopupMenu(menu, p.x, p.y);
    }   
	else if(event->LeftUp() && WasDragging)
    {
		new_x = (event->GetPosition().x + plug->GetPosition().x);
	    new_y = (event->GetPosition().y + plug->GetPosition().y);
	    if(!DndGetDest(k, l, new_x, new_y, plug))
	    {
			DeleteRack(plug);
			AddTrack(plug);
	    }
	    ResizeTracks();
	    WasDragging = false;
    }   
}

void				Rack::AddPlugToMenu()
{
	vector<PluginLoader *>::iterator i;
	int Id = 20100;
	
	for(i = LoadedPluginsList.begin(); i != LoadedPluginsList.end() ; i++)
    {
    	if ((*i)->InitInfo.Type == PLUG_IS_INSTR)
		{
			Id++;
			instr_menu->Append((*i)->Id, ((*i)->InitInfo.Name).c_str());
		}
    	else
		{
			Id++;
			effects_menu->Append((*i)->Id, ((*i)->InitInfo.Name).c_str());
		}
    }
}

void				Rack::HandlePaintEvent(Plugin *plug, wxPaintEvent *event)
{
	int xx, yy;
	

	CalcScrolledPosition(0, 0, &xx, &yy);
	if (selectedPlugin == 0x0)
		return;
	if (selectedPlugin == plug)
    {
    	wxPaintDC dc(selectedPlugin);
	    PrepareDC(dc);
	    dc.SetPen(wxPen(wxColour(255,0,0), 3, wxSOLID));
	    dc.SetBrush(*wxTRANSPARENT_BRUSH);
	    dc.DrawRectangle(0 - xx, 0 - yy, selectedPlugin->GetSize().x, selectedPlugin->GetSize().y);
    }
}

bool				Rack::DndGetDest(t_ListRackTrack::iterator &k,  list<Plugin *>::iterator &l, int &new_x, int &new_y, Plugin *plug)
{
  int pos_x = 0;
  int pos_y = 0;
  int xx, yy;

  CalcScrolledPosition(0, 0, &xx, &yy);  

  for (k= RackTracks.begin(); k != RackTracks.end(); k++){
    pos_x = (pos_x + (*k)->Units * UNIT_W);
    if((((pos_x + xx)- ((*k)->Units * UNIT_W)) < new_x) && (new_x < (pos_x + xx)) )
      {
	for(l = (*k)->Racks.begin(); l != (*k)->Racks.end(); l++){
	  pos_y = pos_y + (*l)->InitInfo->UnitsY * UNIT_H;
	  if((((pos_y + yy) - ((*l)->InitInfo->UnitsY * UNIT_H)) < new_y) && (new_y < (pos_y + yy)))
	    {
	      if((*l) == plug)
		return true;
	      DeleteRack(plug);
	      if((new_y + yy) < (((*l)->InitInfo->UnitsY * UNIT_H)/ 2))
		  DndInsert(k, l, plug);
	      else
		{
		  l++;
		  DndInsert(k, l, plug);
		}
	      UpdateUnitXSize();
	      return true;
	    }
	}
	break;
      }
  }
   return false;
}  

inline void			Rack::DndInsert(t_ListRackTrack::iterator &k,  list<Plugin *>::iterator &l, Plugin *plug)
{
  list<Plugin *>::iterator debug;
  
  (*k)->Racks.insert(l, plug);
  SetScrolling();
}

void				Rack::UpdateUnitXSize()
{
  t_ListRackTrack::iterator		i;
  list<Plugin *>::iterator		j;

  
  for(i = RackTracks.begin(); i != RackTracks.end(); i++)
    {
      for(j = (*i)->Racks.begin(); j != (*i)->Racks.end() ; j++)
	{
	  if((*j)->InitInfo->UnitsX > (*i)->Units)
	    {
	      (*i)->Units = (*j)->InitInfo->UnitsX;
	      break;
	    }
	}
    }
}

inline void			Rack::OnDeleteClick()
{
	vector<PluginLoader *>::iterator	k;
  
	if (selectedPlugin)
    {
    	for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
			if (COMPARE_IDS((*k)->InitInfo.UniqueId, selectedPlugin->InitInfo->UniqueId))
			{
		    	cout << "[MAINWIN] Destroying plugin: " << selectedPlugin->Name << endl;
		    	cActionManager::Global().AddEffectAction(&StartInfo, *k, false);
				return;
			}
    }
}

inline void			Rack::OnCutClick()
{
  char file[12] ;

  if(selectedPlugin == 0x0)
    return;
    
  copy_plug = selectedPlugin;
  strcpy(file, "/tmp/.tmpccp");
  if(fd_copy != -1)
    close(fd_copy);
  fd_copy = open(file,  O_CREAT | O_TRUNC | O_RDWR, 0644);
  
  if(fd_copy < 0)
    cout << "[RACKPANEL] Error creating tmp file"<< endl;
  
  else{
    fd_size = copy_plug->Save(fd_copy);
    menu->Enable(ID_MENU_PASTE, true);
    is_cut = true;
  }

  
}

inline void			Rack::OnCopyClick()
{
  char file[12] ;

  if(selectedPlugin == 0x0)
    return;
  
  copy_plug = selectedPlugin;
  strcpy(file, "/tmp/.tmpccp");
  if(fd_copy != -1)
    close(fd_copy);
  fd_copy = open(file,  O_CREAT | O_TRUNC | O_RDWR, 0644);
  
  if(fd_copy < 0)
     cout << "[RACKPANEL] Error creating tmp file" << endl;
   
  else{
    fd_size = copy_plug->Save(fd_copy);
    menu->Enable(ID_MENU_PASTE, true);
  }  
}

inline void			Rack::OnPasteClick()
{
  vector<PluginLoader *>::iterator	k;
  Plugin				*tmp;
  PluginLoader				*p = 0x0;
  vector<PluginLoader *>::iterator	l;
  
  for (k = LoadedPluginsList.begin(); k != LoadedPluginsList.end(); k++)
    if (COMPARE_IDS((*k)->InitInfo.UniqueId, copy_plug->InitInfo->UniqueId))
      {
	  p = *k;
	  break;
      }
   if (p)
    {
      if(fd_copy >= 0){
	cout << fd_copy << endl;
	if(lseek(fd_copy,0,SEEK_SET) != -1){
	  tmp = AddToSelectedTrack(StartInfo, p);
	  tmp->Load(fd_copy, fd_size);
	  cout << "[RACKPANEL] plugin pasted" <<endl;
	}
      }       
    }

   if (is_cut && copy_plug)
    {
      //RemoveChild(copy_plug);
      cout << "CUTTTTTTTTTTT" << endl;
    }
  
}

void				Rack::HandleKeyEvent(Plugin *plug, wxKeyEvent *event)
{
  if (event->GetKeyCode() == WXK_DOWN)
    {
      //cout << "[RACKPANEL] key received: down arrow" << endl;
    }
}

inline void			Rack::ResizeTracks()
{
	int xx, yy, xpos, ypos, k;
	t_ListRackTrack::iterator i;
	list<Plugin *>::iterator j;

	xx = 0;
	for (k = 0, i = RackTracks.begin(); i != RackTracks.end(); i++, k++)
    {
    	yy = 0;
	    (*i)->Index = k;
	    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
		{
			if ((*j)->HasView())
			{
				CalcScrolledPosition(xx * (UNIT_W + UNIT_S), yy * (UNIT_H + UNIT_S), &xpos, &ypos);
				(*j)->SetPosition(wxPoint(xpos, ypos));
				yy += (*j)->InitInfo->UnitsY;
			}
		}
    	xx += (*i)->Units;
    }
}

RackTrack*			Rack::GetRackTrack(Plugin *plug)
{
  t_ListRackTrack::iterator i;
  list<Plugin *>::iterator j;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)  
    {
      for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)  
	if (*j == plug)
	  return (*i);
    }
  return (0x0);
}

void				Rack::OnPluginParamChange(wxMouseEvent &event)
{
	cout << "OnPluginParamChange(wxMouseEvent &event)" << endl;
	cActionManager::Global().AddChangeParamsEffectAction(0, true);
}

void				Rack::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
    	cout << "In OnHelp" << endl;
      wxString s("This is the Rack view of Wired, where you can add plugins. Plugins are organized into tracks vertically, meaning the first plugin in a column will send its output signal to the plugin below it. You can connect sequencer tracks to plugins");
      HelpWin->SetText(s);
    }
}

void				Rack::OnClick(wxMouseEvent &event)
{
	cout << "In OnClick" << endl;
	//selectedTrack = 0x0;
	//selectedPlugin = 0x0;
}

void				Rack::Dump()
{
	t_ListRackTrack::const_iterator	iter;
		
	cout << "Dumping Rack" << endl;
	for (iter = RackTracks.begin(); iter != RackTracks.end(); iter++)
		if (*iter != 0)
			(*iter)->Dump();
	if (selectedTrack)
		selectedTrack->Dump();
	cout << "  selectedPlugin at adress " << selectedPlugin << endl;
	cout << "  Virtual size x : " << m_virtualSize.x << endl;
	cout << "  Virtual size y : " << m_virtualSize.y << endl;
	cout << "  height : " << m_height << endl;
	cout << "  width : " << m_width << endl;
	cout << "End dumping Rack" << endl;
}	

// Don't know if this methodes should be used

RackTrack* 			Rack::AddTrack()
{
  RackTrack *t;

  SeqMutex.Lock(); 
  t = new RackTrack(this, RackTracks.size());
  RackTracks.push_back(t);
  SeqMutex.Unlock();      
  return (t);
}

void				Rack::OnPaint(wxPaintEvent &event)
{

}

void				Rack::RemoveFromSelectedTrack()
{
	if (selectedTrack)
    {
    	// ??? RemoveRack is enougth ??? What about RemoveChannel ??? And RackTracks.pop ???
    	SeqMutex.Lock();
	    selectedTrack->RemoveRack();
    	SeqMutex.Unlock();
    }
	else
		RemoveTrack();
}

void 				Rack::RemoveTrack()
{
	SeqMutex.Lock();
	RackTrack* rackTrack = RackTracks.back();
	rackTrack->RemoveRack();
	rackTrack->RemoveChannel();
	RackTracks.pop_back();
	SeqMutex.Unlock();
	SetScrolling();
}

Plugin*				Rack::AddToSelectedTrack(PlugStartInfo &startinfo, PluginLoader *p)
{
  //if (!selectedTrack && (RackTracks.size() > 0))
  //  selectedTrack = *(RackTracks.begin());
  cout << "AddToSelectedTrack(PlugStartInfo &startinfo, PluginLoader *p)" << endl;
  Plugin *tmp;
  if (selectedTrack)
    {
      tmp = selectedTrack->AddRack(startinfo, p, selectedTrack->Racks.back());
    }
  else
    tmp = AddTrack(startinfo, p);
  return tmp;
}

void 				Rack::AddTrack(Plugin *p)
{
  RackTrack *t;

  t = new RackTrack(this, RackTracks.size());

  t->Units = p->InitInfo->UnitsX;
 
  SeqMutex.Lock(); 

  t->Racks.insert(t->Racks.begin(),p);
	cout << "AddTrack(Plugin *p)" << endl;
 	ConnectPluginChangeParamEventHandler(t);
  RackTracks.push_back(t);

  SeqMutex.Unlock();      

  SetScrolling();
}

Plugin*				Rack::AddTrack(PlugStartInfo &startinfo, PluginLoader *p)
{
	RackTrack *t;
	Plugin *tmp;

	t = new RackTrack(this, RackTracks.size());
	tmp = t->AddRack(startinfo, p);

	cout << "Good AddTrack" << endl;
	ConnectPluginChangeParamEventHandler(t);
	SeqMutex.Lock(); 
	RackTracks.push_back(t);
	SeqMutex.Unlock();
	if (tmp->HasView())
		SetScrolling();
	return tmp;
}


// Events loop (Static events)
BEGIN_EVENT_TABLE(Rack, wxScrolledWindow)
  //  EVT_PAINT(Rack::OnPaint)
  EVT_ENTER_WINDOW(Rack::OnHelp)
//  EVT_LEFT_DOWN(Rack::OnClick)
//  EVT_RIGHT_DOWN(Rack::OnClick)
//  EVT_LEFT_UP(Rack::OnClick)
END_EVENT_TABLE()
