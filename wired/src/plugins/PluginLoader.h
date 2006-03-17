// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __PLUGINLOADER_H__
#define __PLUGINLOADER_H__

#include "Plugin.h"
#include "../dssi/WiredExternalPluginMgr.h"
#include <wx/wx.h>
using namespace std;

#define COMPARE_IDS(x, y) ((x[0] == y[0]) && (x[1] == y[1]) && (x[2] == y[2]) && (x[3] == y[3]))

//((*it)->InitInfo.Id[0] == plugin.Id[0]) && ((*it)->InitInfo.Id[1] == plugin.Id[1]) &&
//	    ((*it)->InitInfo.Id[2] == plugin.Id[2]) && ((*it)->InitInfo.Id[3] == plugin.Id[3])

class				PluginLoader
{
 public:
  PluginLoader(wxString filename);
  PluginLoader(WiredExternalPluginMgr *PlugMgr, int MenuItemId, PlugStartInfo &info);
  PluginLoader(WiredExternalPluginMgr *PlugMgr, unsigned long UniqueId);
  ~PluginLoader();

  void				Unload();
  Plugin			*CreateRack(PlugStartInfo &info);
  void				Destroy(Plugin *p);

  wxString			FileName;
  int				Id;
  PlugInitInfo			InitInfo;

 private:
  void				*handle;
  init_t			init;
  create_t			create;
  destroy_t			destroy;
  bool				External;
  WiredDSSIGui		*ExternalPlug;
  WiredExternalPluginMgr			*PluginMgr;
  int				IdMenuItem;
};

#endif
