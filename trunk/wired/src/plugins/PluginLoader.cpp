// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "PluginLoader.h"

#include <iostream>

PluginLoader::PluginLoader(string filename) : 
  FileName(filename)
{
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
  dlclose(handle);
}

Plugin *PluginLoader::CreateRack(PlugStartInfo &info)
{
  return (create(&info));
}

void PluginLoader::Destroy(Plugin *todel)
{
  destroy(todel);
}

void PluginLoader::Unload()
{
  dlclose(handle);
}
