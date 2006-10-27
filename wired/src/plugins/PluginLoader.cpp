// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <iostream>
#include "PluginLoader.h"
#include <config.h>

PluginLoader::PluginLoader(WiredExternalPluginMgr *PlugMgr, int MenuItemId, PlugStartInfo &info)
{
	External = true;
	PluginMgr = PlugMgr;
	IdMenuItem = MenuItemId;
	ExternalPlug = PluginMgr->CreatePlugin(IdMenuItem, info);
	ExternalPlug->SetInfo(&InitInfo);
	//ExternalPlug->SetVirtualSize(400, 100);
}

PluginLoader::PluginLoader(WiredExternalPluginMgr *PlugMgr, unsigned long UniqueId)
{
	External = true;
	PluginMgr = PlugMgr;
	//IdMenuItem = MenuItemId;
	ExternalPlug = PluginMgr->CreatePlugin(UniqueId);
	ExternalPlug->SetInfo(&InitInfo);
}

PluginLoader::PluginLoader(wxString filename) : 
  FileName(filename), handle(wxT(LIB_DIR) + filename)
{
  External = false;

  // if failed, try loading filename without PREFIX base
  // (wx load /usr/lib, /usr/local/lib/, ...)
  if (!handle.IsLoaded())
    {
      cout << "[PLUGLOADER] Warning : " <<
	wxString(wxT(LIB_DIR) + filename).mb_str()
	   << " can't be loaded" << endl;
      handle.Load(filename);
    }

  // check if the library is correctly loaded
  if (handle.IsLoaded())
    {
      // check all mandatory symbols
      cout << "[PLUGLOADER] Loading symbol init..." << endl;

      init = (init_t) handle.GetSymbol(PLUG_INIT);
      if (!init) 
	{
	  cerr << "[PLUGLOADER] Error: Cannot load symbol : " << PLUG_INIT << endl;
	  handle.Unload();
	  return ;
	}
      destroy = (destroy_t) handle.GetSymbol(PLUG_DESTROY);
      if (!destroy) 
	{
	  cerr << "[PLUGLOADER] Error: Cannot load symbol : " << PLUG_DESTROY << endl;
	  handle.Unload();
	  return ;
	}  
      create = (create_t) handle.GetSymbol(PLUG_CREATE);
      if (!create) 
	{
	  cerr << "[PLUGLOADER] Error: Cannot load symbol : " << PLUG_CREATE << endl;
	  handle.Unload();
	  return ;
	}  

      // get unique info from plugin (id, name, version, size, ..)
      InitInfo = init();

      // check version of API
      if (InitInfo.Version != WIRED_CURRENT_VERSION_API)
	{
	  cerr << "[PLUGLOADER] Error: Cannot load plugin " << filename.mb_str() 
	       << ", it has deprecated version of API " << endl;
	  handle.Unload();
	  return ;
	}
    }
  else
    cerr << "[PLUGLOADER] Error: Cannot open library : " << filename.mb_str() << endl;
}

PluginLoader::~PluginLoader()
{

}

bool	PluginLoader::IsLoaded()
{
  return (handle.IsLoaded());
}

Plugin *PluginLoader::CreateRack(PlugStartInfo &info)
{
	if (External == false)
	  return (create(&info));
	ExternalPlug->SetInfo(&info);
	
	return (Plugin*) ExternalPlug;
}

void PluginLoader::Destroy(Plugin *todel)
{
	if (External == false)
		destroy(todel);
	else
	{
		PluginMgr->DestroyPlugin(ExternalPlug);
		ExternalPlug = NULL;
	}
}
