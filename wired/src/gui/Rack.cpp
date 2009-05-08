// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <wx/stdpaths.h>
#include	"SeqTrack.h"
#include	"Track.h"
#include	"Rack.h"
#include	"SequencerGui.h"
#include	"HelpPanel.h"
#include	"AudioEngine.h"
#include	"OptionPanel.h"
#include	"MixerGui.h"
#include	"MainWindow.h"
#include	"cImportMidiAction.h"
#include	"Sequencer.h"
#include	"Mixer.h"
#include	"Plugin.h"
#include	"PluginLoader.h"
#include	"debug.h"
int		RackCount = 0;

#ifdef __DEBUG__
Plugin*		gl_lastDeletedPlug = 0;
#endif


/********************   Class Rack   ********************/

Rack::Rack(wxWindow* parent, wxWindowID id, const wxPoint& pos,
	   const wxSize& size) :
  wxScrolledWindow(parent, id, pos, size, wxSUNKEN_BORDER),
  WiredDocument(wxT("Rack"), NULL)
{
  WasDragging = false;
  SetScrollRate(10, 10);
  SetVirtualSize(760, 180);
  InitContextMenu();
  copy_plug = NULL;
  filePath.Printf(wxT("/tmp/.tmpccp"));
  CleanChildren();
}

Rack::~Rack()
{
  CleanChildren();

  if (tmpFile.IsOpened())
    {
      tmpFile.Close();
      if (wxRemoveFile(filePath) == false)
	cout << "[Rack] deleting error" << endl;
    }

  if (copy_plug)
    delete copy_plug;
  cout << "[Rack] End destructor" << endl;
}

void				Rack::CleanChildren()
{
  DeleteAllTracks();
  selectedTrack = NULL;
  selectedPlugin = NULL;
  is_cut = false;
}

void				Rack::InitContextMenu()
{
  menu = new wxMenu();
  submenu = new wxMenu();
  instr_menu = new wxMenu();
  effects_menu  = new wxMenu();

  menu->Append(ID_MENU_ADD, _("Add"), submenu);
  submenu->Append(ID_INSTR_MENU, _("&Instruments"), instr_menu);
  submenu->Append(ID_EFFECTS_MENU, _("&Effects"), effects_menu);
#ifndef DEBUG_1561088
  menu->Append(ID_MENU_CUT, _("Cut"));
  menu->Append(ID_MENU_COPY, _("Copy"));
  menu->Append(ID_MENU_PASTE, _("Paste"), false);
  menu->Enable(ID_MENU_PASTE, false);
#endif
  menu->AppendSeparator();
  menu->Append(ID_MENU_DELETE, _("Delete"));

  /* XXX
  result = new t_RackTrackPlugin();
  t = new RackTrack(this, RackTracks.size());
  tmp = t->AddRack(startinfo, p);
  ConnectPluginChangeParamEventHandler(t);
  SeqMutex.Lock();
  RackTracks.push_back(t);
  SeqMutex.Unlock();
  result->rackTrack = t;
  result->plugin = tmp;
  ResizeTracks();
  SetScrolling();
  SeqPanel->RefreshConnectMenu();
  */
  AddPlugToMenu();

#ifndef DEBUG_1561088
  Connect(ID_MENU_CUT, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&Rack::OnCutClick);
  Connect(ID_MENU_COPY, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&Rack::OnCopyClick);
  Connect(ID_MENU_PASTE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&Rack::OnPasteClick);
#endif
  Connect(ID_MENU_DELETE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)
	  (wxCommandEventFunction)&Rack::OnDeleteClick);
  //return (result);
}

bool				Rack::RemoveRackTrack(const RackTrack* rackTrack)
{
  t_ListRackTrack::const_iterator	iter;

  for (iter = RackTracks.begin(); iter != RackTracks.end(); iter++)
    if ((*iter) != rackTrack)
      {
	wxMutexLocker		locked(SeqMutex);

	delete (*iter);
	RackTracks.remove(*iter);
	ResizeTracks();
	return true;
      }
  return false;
}

bool				Rack::RemoveRackTrack(unsigned int index)
{
  t_ListRackTrack::const_iterator	iter;
  unsigned int				cptRacks;

  if (index > (RackTracks.size() - 1) || index < 0)
    return false;
  for (iter = RackTracks.begin(), cptRacks = 0; cptRacks < index; iter++, cptRacks++)
    ;
  RemoveRackTrack(*iter);
  return true;
}

void 				Rack::RemoveLastRackTrack()
{
  if (RackTracks.size() > 0)
    RemoveRackTrack(RackTracks.size() - 1);
}

bool				Rack::RemoveSelectedRackTrack()
{
  if (selectedTrack)
    if (RemoveRackTrack(selectedTrack) == true)
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

void				Rack::DeleteAllTracks()
{
  t_ListRackTrack::iterator i;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)
    delete *i;

  ResizeTracks();

  selectedTrack = NULL;
  selectedPlugin = NULL;
}

bool				Rack::DeleteRack(Plugin *plug, bool eraseit)
{
  t_ListRackTrack::iterator i;
  list<Plugin *>::iterator j, k;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)
    for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
      if (*j == plug)
	{
	  OptPanel->ClosePlug(plug);
	  (*i)->Racks.erase(j);
	  if (eraseit)
	    plug->Hide();
	  (*i)->Units = 0;
	  for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	    {
	      if ((*j)->InitInfo->UnitsX > (*i)->Units)
		(*i)->Units = (*j)->InitInfo->UnitsX;
	    }
	  // if we removed the last rack of the RackTrack
	  if ((*i)->Racks.size() == 0)
	    {
	      delete (*i);
	      RackTracks.erase(i);
	    }
	  selectedPlugin = 0x0;
	  ResizeTracks();
	  SeqPanel->RemoveReferenceTo(plug);
#ifdef __DEBUG__
	  cout << "[RACK] plugin to be erased = " << plug << endl;
#endif
           //toto TODO XXX *will* cause segfault at exit (for ladspa and dssi)
	   //is it deleted somewhere else ? where is that pointer ??
	   //and why "awatch gl_lastDeletedPlug" or "awatch (Plugin*)0x......" don't work in gdb ? :(
	   //am i doing it wrong... ?
	   //do we really need to delete it btw ?
          /*
	   *if (eraseit)
	   *{
	   *  delete plug;
	   *  plug = 0x0;
	   *}
           */
#ifdef __DEBUG__
	  gl_lastDeletedPlug = plug;
#endif
	  return true;
	}
  cerr << "[RACK] did not find plugin to delete" << endl;
  return false;
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
	      selectedTrack->SetSelected(selectedPlugin);
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
	// if IsAudio() is false (plugins can't receive audio): we don't allow chaining
	if(plug->IsAudio() && !DndGetDest(k, l, new_x, new_y, plug))
	{
	  DeleteRack(plug, false);
	  AddLoadedRack(plug);
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
      if ((*i)->InitInfo.Type == ePlugTypeInstrument)
	{
	  Id++;
	  instr_menu->Append((*i)->Id, ((*i)->InitInfo.Name).c_str());
	}
      else if ((*i)->InitInfo.Type == ePlugTypeEffect)
	{
	  Id++;
	  effects_menu->Append((*i)->Id, ((*i)->InitInfo.Name).c_str());
	}
    }
}

// called from OnPaint()'s plugins (via HostCallback)
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

// we got new_x and new_y which are mouse dropped positions
bool				Rack::DndGetDest(t_ListRackTrack::iterator &k,
						 list<Plugin *>::iterator &l,
						 int &new_x,
						 int &new_y,
						 Plugin *plug)
{
  int pos_x = 0;
  int pos_y = 0;
  int xx, yy;

  CalcScrolledPosition(0, 0, &xx, &yy);

  for (k = RackTracks.begin(); k != RackTracks.end(); k++)
    {
      // for each RackTrack, we look x coord and width of the Rack
    pos_x = (pos_x + (*k)->Units * UNIT_W);
    if((((pos_x + xx)- ((*k)->Units * UNIT_W)) < new_x) && (new_x < (pos_x + xx)) )
      {
	// for each Rack linked to the RackTrack, we look y coord and height of the Rack
	for(l = (*k)->Racks.begin(); l != (*k)->Racks.end(); l++)
	  {
	  pos_y = pos_y + (*l)->InitInfo->UnitsY * UNIT_H;
	  if((((pos_y + yy) - ((*l)->InitInfo->UnitsY * UNIT_H)) < new_y) && (new_y < (pos_y + yy)))
	    {
	      if((*l) == plug)
		return true;
	      DeleteRack(plug, false);
	      l++;
	      DndInsert(k, l, plug);
	      UpdateUnitXSize();
	      return true;
	    }
	}
	break;
      }
  }
   return false;
}

inline void			Rack::DndInsert(t_ListRackTrack::iterator &k,
						list<Plugin *>::iterator &l,
						Plugin *plug)
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

void				Rack::DeleteSelectedRack()
{
  if (selectedPlugin)
    DeleteRack(selectedPlugin);
}

inline void			Rack::OnDeleteClick()
{
  wxCommand*			cmd;

  cmd = new DeleteRackAction(_("deleted rack"), selectedPlugin);
  UndoRedo->Submit(cmd);
}

inline void			Rack::OnCutClick()
{
  if(selectedPlugin == 0x0)
    return;

  copy_plug = selectedPlugin;
  if (tmpFile.IsOpened())
    tmpFile.Close();

  if (!tmpFile.Exists(filePath))
    tmpFile.Create(filePath);
  if (!tmpFile.Exists(filePath))
    cout << "[RACKPANEL] Error creating tmp file"<< endl;
  else
    {
      tmpFile.Open(filePath, wxFile::read_write);

      if (!tmpFile.IsOpened())
	cout << "[RACKPANEL] Error opening tmp file"<< endl;
      else
	{
	  // TODO : replace line below when remaking plug-in API giving wxFile arg instead of fd
	  fd_size = copy_plug->Save(tmpFile.fd());
	  menu->Enable(ID_MENU_PASTE, true);
	  is_cut = true;
	}
    }
}

inline void			Rack::OnCopyClick()
{
  if(selectedPlugin == 0x0)
    return;

  copy_plug = selectedPlugin;

  if (tmpFile.IsOpened())
    tmpFile.Close();

  if (!tmpFile.Exists(filePath))
    tmpFile.Create(filePath);
  if (!tmpFile.Exists(filePath))
    cout << "[RACKPANEL] Error creating tmp file"<< endl;
  else
    {
      tmpFile.Open(filePath, wxFile::read_write);

      if (!tmpFile.IsOpened())
	cout << "[RACKPANEL] Error opening tmp file"<< endl;
      else
	{
	  // TODO : replace line below when remaking plug-in API giving wxFile arg instead of fd
	  fd_size = copy_plug->Save(tmpFile.fd());
	  menu->Enable(ID_MENU_PASTE, true);
	}
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

      if (tmpFile.IsOpened())
	{
	  cout << tmpFile.fd() << endl;
	  if (tmpFile.Seek(0, wxFromStart) != wxInvalidOffset)
	    {
	      tmp = AddToSelectedTrack(StartInfo, p);
	      tmp->Load(tmpFile.fd(), fd_size);
	      cout << "[RACKPANEL] plugin pasted" <<endl;
	    }
	}
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
	  CalcScrolledPosition(xx * (UNIT_W + UNIT_S), yy * (UNIT_H + UNIT_S), &xpos, &ypos);
	  (*j)->SetPosition(wxPoint(xpos, ypos));
	  yy += (*j)->InitInfo->UnitsY;
	}
      xx += (*i)->Units;
    }
  SetScrolling();
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

Plugin*				Rack::FindPlugin(wxString name)
{
  t_ListRackTrack::iterator i;
  list<Plugin *>::iterator j;

  for (i = RackTracks.begin(); i != RackTracks.end(); i++)
    {
      for (j = (*i)->Racks.begin(); j != (*i)->Racks.end(); j++)
	if ((*j)->Name == name)
	  return (*j);
    }
  return (NULL);
}

void				Rack::OnPluginParamChange(wxMouseEvent &event)
{
	//cActionManager::Global().AddChangeParamsEffectAction(0, true);
}

void				Rack::OnHelp(wxMouseEvent &event)
{
  if (HelpWin == NULL)
      return;
  if (HelpWin->IsShown())
    {
      //cout << "In OnHelp" << endl;
      if (selectedPlugin)
    	{
	  wxString s(selectedPlugin->GetHelpString());
	  HelpWin->SetText(s);
    	}
      else
    	{
	  wxString s(_("This is the Rack view of Wired, where you can add plugins. Plugins are organized into tracks vertically, meaning the first plugin in a column will send its output signal to the plugin below it. You can connect sequencer tracks to plugins"));
	  HelpWin->SetText(s);
    	}
    }
}

void				Rack::OnClick(wxMouseEvent &event)
{

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
#if wxCHECK_VERSION(2, 8, 0)
  cout << "  height : unknown" << endl;
  cout << "  width : unknown" << endl;
#else
  cout << "  height : " << m_height << endl;
  cout << "  width : " << m_width << endl;
#endif
  cout << "End dumping Rack" << endl;
}

Plugin*				Rack::AddToSelectedTrack(PlugStartInfo &startinfo, PluginLoader *p)
{
  Plugin *tmp;

  if (selectedTrack)
    tmp = selectedTrack->CreateRack(startinfo, p);
  else
    tmp = AddNewRack(startinfo, p);
  return tmp;
}

bool 				Rack::AddLoadedRack(Plugin *plug)
{
  RackTrack			*track;

  if ( plug )
    {
      track = CreateRackTrack();
      track->Units = plug->InitInfo->UnitsX;
      track->AddRack(plug);
      ResizeTracks();
      plug->Show();
      return true;
    }
  return false;
}

RackTrack*			Rack::CreateRackTrack()
{
  wxMutexLocker			locked(SeqMutex);
  RackTrack*			rackTrack;

  rackTrack = new RackTrack(this, RackTracks.size());
  RackTracks.push_back(rackTrack);
  return (rackTrack);
}

Plugin*				Rack::AddNewRack(PlugStartInfo &startinfo, PluginLoader *p)
{
  Plugin *loaded;

  loaded = CreateRackTrack()->CreateRack(startinfo, p);
  SeqPanel->RefreshConnectMenu();
  ResizeTracks();
  return loaded;
}

void				Rack::SetAudioConfig(long bufferSize, double sampleRate)
{
  list<RackTrack *>::iterator	itRackTrack;
  list<Plugin *>::iterator	itPlugin;

  for (itRackTrack = RackTracks.begin();
       itRackTrack != RackTracks.end(); itRackTrack++)
    for (itPlugin = (*itRackTrack)->Racks.begin();
	 itPlugin != (*itRackTrack)->Racks.end(); itPlugin++)
      {
	(*itPlugin)->SetBufferSize(bufferSize);
	(*itPlugin)->SetSamplingRate(sampleRate);
      }
}

void				Rack::Save()
{
  // not used right now
  saveDocData(new SaveElement(wxT("RackTrackNumber"), (int)RackTracks.size()));

  // save all racktracks
  t_ListRackTrack::iterator	itRackTrack;
  std::list<Plugin *>::iterator	it;
  int				no;

  no = 0;
  for (itRackTrack = RackTracks.begin(); itRackTrack != RackTracks.end(); itRackTrack++)
    {
      for (it = (*itRackTrack)->Racks.begin(); it != (*itRackTrack)->Racks.end(); it++)
	{
	  SaveElement*			saved = new SaveElement(wxT("RackPlugin"), (int)(*itRackTrack)->Racks.size());

	  saved->addAttribute(wxT("Track"), no);
	  saved->addAttribute(wxT("Name"), (*it)->Name);
	  saved->addAttribute(wxT("PlugName"), (*it)->InitInfo->Name);
	  saved->addAttribute(wxT("UniqueId"), wxString((*it)->InitInfo->UniqueId,*wxConvCurrent));
	  // toto : on pourrait ecrire qqchose la
	  // saved->setValue(wxT("beatbox"));
	  saveDocData(saved);
	}
      no++;
    }
}

void				Rack::Load(SaveElementArray data)
{
  // load racktracks
  vector<PluginLoader*>::iterator	it;
  unsigned int				i;

  for (i = 0; i < data.GetCount(); i++)
    if (data[i]->getKey() == wxT("RackTrackNumber"))
      for (; i > 0; i--)
	CreateRackTrack();
  for (i = 0; i < data.GetCount(); i++)
    {
    if (data[i]->getKey() == wxT("RackPlugin"))
      {
#ifdef __DEBUG__
	cout << "== Rackplugin" << endl;
	cout << "Loaded Track:: " << data[i]->getAttributeInt(wxT("Track")) << endl;
	cout << "Loaded Name: " << data[i]->getAttribute(wxT("Name")).mb_str() << endl;
	cout << "Loaded PlugName: " << data[i]->getAttribute(wxT("PlugName")).mb_str() << endl;
      	cout << "Loaded UniqueId: " << data[i]->getAttribute(wxT("UniqueId")).mb_str() << endl;
#endif
	SelectTrackFromNumber(data[i]->getAttributeInt(wxT("Track")));
	MainWin->CreatePluginFromUniqueId(data[i]->getAttribute(wxT("UniqueId")),
					  data[i]->getAttribute(wxT("PlugName")));
      }
  }
}

void				Rack::SelectTrackFromNumber(int no)
{
  t_ListRackTrack::iterator	it;
  int				i;

  for (i = 0, it = RackTracks.begin(); it != RackTracks.end(); it++, i++)
    {
      if (i == no)
	{
	  selectedTrack = *it;
	  return;
	}
    }
  selectedTrack = NULL;
}

// Events loop (Static events)
BEGIN_EVENT_TABLE(Rack, wxScrolledWindow)
  EVT_ENTER_WINDOW(Rack::OnHelp)
//  EVT_LEFT_DOWN(Rack::OnClick)
//  EVT_RIGHT_DOWN(Rack::OnClick)
//  EVT_LEFT_UP(Rack::OnClick)
END_EVENT_TABLE()

/********************   class CreateRackAction   ********************/

CreateRackAction::CreateRackAction(wxString& label, PlugStartInfo* startInfo, PluginLoader* plugLoader)
: wxCommand(true, label)
{
  mPluginLoader = plugLoader;
  mStartInfo = startInfo;
}

bool CreateRackAction::Do ()
{
  if (mPluginLoader)
    {
      _created = RackPanel->AddNewRack(*mStartInfo, mPluginLoader);
      return true;
    }
  return false;
}

bool CreateRackAction::Undo ()
{
  RackPanel->DeleteRack( _created, true );
  return true;
}

/********************   class DeleteRackAction   ********************/

DeleteRackAction::DeleteRackAction(wxString label, Plugin* plug)
  : wxCommand(true, label)
{
  _deleted = plug;
}

DeleteRackAction::~DeleteRackAction()
{
  RackPanel->DeleteRack( _deleted, true );  
}

bool DeleteRackAction::Do ()
{
  return RackPanel->DeleteRack( _deleted, false );  
}

bool DeleteRackAction::Undo ()
{
  if ( RackPanel->AddLoadedRack( _deleted ) == false )
    return false;
  RackPanel->SetSelected( _deleted );
  return true;
}
