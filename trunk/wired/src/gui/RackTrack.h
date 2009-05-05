// Copyright (C) 2004-2009 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __RACKTRACK_H__
#define __RACKTRACK_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <list>

#include "Channel.h"
#include "ChannelGui.h"
#include "WiredDocument.h"
#include "Plugin.h"
#include "PluginLoader.h"

#include "Rack.h"

class Rack;

class RackTrack : public WiredDocument
{
 private:
  void				RemoveChannel();
  void				DeleteAllRacks();

public:
    RackTrack(Rack *parent, int index);
    virtual ~RackTrack();

  // WiredDocument implementation
  void				Save();
  void				Load(SaveElementArray data);

  void				AddRack(Plugin* plug);
  Plugin*			CreateRack(PlugStartInfo &startinfo, PluginLoader *p);
  void				DeleteRack(Plugin *plug);
  int				GetYPos();
  void				RemoveSelectedRack();
  void				SetSelected(Plugin *plugin);
  int				NbRacks(){return Racks.size();};
  RackTrack			operator=(const RackTrack& right);
	//void				SetSelected(Plugin *plugin);
	//int				NbRacks(){return Racks.size();};
  // Debug - Shows member variables
  // Pas du tout objet, mais plus simple pour l'instant (ca permet de ne pas changer
  // l'API du plugin qui va de tte maniere l'etre pour les DSSI)
  void				Dump();
  void				DumpPlugins();
  Plugin*			SelectedPlugin;
  int				Units;
  Rack*				Parent;
  int				Index;
  std::list<Plugin *>		Racks;
  Channel*			Output;
  ChannelGui*			ChanGui;
  float**			CurrentBuffer;
};

#endif // __RACKTRACK_H__
