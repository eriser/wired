#ifndef __PLUGINEFFECT_H__
#define __PLUGINEFFECT_H__

#include "WaveFile.h"
#include <wx/wx.h>
#include <string> 
#include <wx/textdlg.h>


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
