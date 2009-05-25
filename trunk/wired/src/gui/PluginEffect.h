// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __PLUGINEFFECT_H__
#define __PLUGINEFFECT_H__

#include "WaveFile.h"
#include <wx/wx.h>
#include <wx/textdlg.h>
#include <wx/cmdproc.h>

class		PluginLoader;
class		Plugin;
typedef struct s_PlugStartInfo			PlugStartInfo;

/********************   class CreateEffectAction   ********************/

class		CreateEffectAction : public wxCommand
{
private:
	PluginLoader			*mPluginLoader;			// Contexte
	PlugStartInfo			*mStartInfo;			// Contexte

	Plugin*				_created;
  
public:
	CreateEffectAction (wxString& label, PlugStartInfo* startInfo, PluginLoader * plugin);
	~CreateEffectAction () {};

	bool			Do ();
	bool			Undo ();
};


/********************   class PluginEffect   ********************/

class PluginEffect 
{
 public:
  PluginEffect();
  ~PluginEffect();

  void		Process(WaveFile &input, WaveFile &output, float gain, int channel, int effet);
  void 		Gain(WaveFile &input, WaveFile &output, float gain, int channel);
  void 		Normalize(WaveFile &input, WaveFile &output, float norma, int channel);
};

#endif
