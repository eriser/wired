// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __PLUGINLOADER_H__
#define __PLUGINLOADER_H__

#include "Plugin.h"
#include "../dssi/WiredExternalPluginMgr.h"
#include <wx/wx.h>
#include <wx/dynlib.h>

using namespace std;

#define COMPARE_IDS(x, y) ((x[0] == y[0]) && (x[1] == y[1]) && (x[2] == y[2]) && (x[3] == y[3]))

//((*it)->InitInfo.Id[0] == plugin.Id[0]) && ((*it)->InitInfo.Id[1] == plugin.Id[1]) &&
//	    ((*it)->InitInfo.Id[2] == plugin.Id[2]) && ((*it)->InitInfo.Id[3] == plugin.Id[3])

/**
 * Loads/Unloads plugins.
 * Be carefull, it will be modified soon with the new plugin's API implementation !!!
 */
class				PluginLoader
{
 public:

/**
 *  This constructor takes a plugin's path.
 * \param filename a wxString, the plugin's path.
 */
  PluginLoader(wxString filename);

/**
 * This contructor takes 3 params.
 * \param PlugMgr a WiredExternalPluginMgr pointer, it manages all external plugins in Wired.
 * \param MenuItemId an int, it's a plugin's identifier...
 * \param info a PlugStartInfo, it handles some information about plugin.
 */
  PluginLoader(WiredExternalPluginMgr *PlugMgr, int MenuItemId, PlugStartInfo &info);

/**
 * This contructor takes 2 params.
 * \param PlugMgr manager a WiredExternalPluginMgr pointer, it manages all external plugins in Wired.
 * \param UniqueId an unsigned long, it's a plugin's identifier.
 */
  PluginLoader(WiredExternalPluginMgr *PlugMgr, unsigned long UniqueId);
/**
 * This contructor takes 2 params.
 * \param PlugMgr manager a WiredExternalPluginMgr pointer, it manages all external plugins in Wired.
 * \param IdPlugin it's TEH plugin's identifier.
 */
  PluginLoader(WiredExternalPluginMgr *PlugMgr, PlugStartInfo &info, int IdPlugin);

/**
 * The main destructor.
 */
  ~PluginLoader();

  /**
   * Init basics vars
   */
  void				Init(WiredExternalPluginMgr* PlugMgr);

/**
 * Called when the host initializes the plugin's shared library (usually at startup).
 * \param info a PlugStartInfo, plugin informations. Needed to create the plugin.
 * \return a Plugin, The new plugin which has been instantied.
 */
  Plugin			*CreateRack(PlugStartInfo &info);

/**
 * Called when the host needs to destroy an instance of the plugin.
 * \param p a Plugin pointer, the plugin to destroy.
 */
  void				Destroy(Plugin *p);

/**
 * Called the wxDynamicLibrary (handle attribute) is the plugin has been loaded.
 * \return a bool, TRUE if the plugin bas been loaded, FALSE if not.
 */
  bool				IsLoaded();

/**
 * The plugin's path.
 */
  wxString			FileName;

/**
 * The plugin's identifier.
 */
  int				Id;

/**
 * The plugin's informations.
 * A (*init_t) typedef.
 */
  PlugInitInfo			InitInfo;

 private:

/**
 * The pointer to the plugin library
 */
  wxDynamicLibrary		handle;

/**
 * The plugin's init symbol.
 */
  init_t			init;

/**
 * The plugin's create symbol.
 */
  create_t			create;

/**
 * The plugin's destroy symbol.
 */
  destroy_t			destroy;

/**
 * The plugin is an external one?
 */
  bool				External;

/**
 * The DSSI plugin's API.
 */
  WiredDSSIGui			*ExternalPlug;

/**
 * The external plugin's manager.
 */
  WiredExternalPluginMgr	*PluginMgr;

/**
 * The plugin's identifier.
 */
  int				IdMenuItem;
};

#endif
