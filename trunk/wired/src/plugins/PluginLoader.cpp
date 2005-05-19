// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include <iostream>
#include <dlfcn.h>
#include "PluginLoader.h"

PluginLoader::PluginLoader(WiredDSSI *PlugMgr, int MenuItemId)
{
	External = true;
	PluginMgr = PlugMgr;
	IdMenuItem = MenuItemId;
	ExternalPlug = PluginMgr->CreatePlugin(IdMenuItem);
	ExternalPlug->SetInfo(&InitInfo);
}

PluginLoader::PluginLoader(string filename) : 
  FileName(filename)
{
  External = false;
  handle = dlopen(filename.c_str(), RTLD_LAZY);
  if (!handle) 
    {
      cerr << "[PLUGLOADER] Error: Cannot open library: " << dlerror() << '\n';
      exit(1);
    }
  cout << "[PLUGLOADER] Loading symbol init...\n";

  init = (init_t) dlsym(handle, PLUG_INIT);
  if (!init) 
    {
      cerr << "[PLUGLOADER] Error: Cannot load symbol 'init': " << dlerror() << '\n';
      dlclose(handle);
      exit(1);
    }
  destroy = (destroy_t) dlsym(handle, PLUG_DESTROY);
  if (!destroy) 
    {
      cerr << "[PLUGLOADER] Error: Cannot load symbol 'destroy': " << dlerror() << '\n';
      dlclose(handle);
      exit(1);
    }  
  create = (create_t) dlsym(handle, PLUG_CREATE);
  if (!create) 
    {
      cerr << "[PLUGLOADER] Error: Cannot load symbol 'create': " << dlerror() << '\n';
      dlclose(handle);
      exit(1);
    }  
  InitInfo = init();
}

PluginLoader::~PluginLoader()
{
	if (External == false)
		dlclose(handle);
	else
		;
}

Plugin *PluginLoader::CreateRack(PlugStartInfo &info)
{
	if (External == false)
	  return (create(&info));
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

void PluginLoader::Unload()
{
	if (External == false)
		dlclose(handle);
}
