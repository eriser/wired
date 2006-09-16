// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004, 2005 by Wired Team
// Under the GNU General Public License

#include <iostream>
#include <math.h>
#include "PluginEffect.h"

using namespace				std;

/* DO NOT MODIFY THIS FILE FOR MAKING A PLUGIN. JUST DERIVE FROM THAT CLASS */

PluginEffect::PluginEffect() 
{ 

}

PluginEffect::~PluginEffect() 
{ 

}

void					PluginEffect::Process(WaveFile &input, WaveFile &output, float effect, int channel, int ind)
{
  if (ind == 1)
	Gain(input, output, effect, channel);
   else  if (ind == 2)
	Normalize(input, output, effect, channel);
}

void					PluginEffect::Gain(WaveFile &input, WaveFile &output, 
							   float gain, int channel)
{
  int					nb_read;
  
  float * rw_buffer = new float [channel * WAVE_TEMP_SIZE];
  nb_read = input.ReadFloatF(rw_buffer);
  for (int i=0; i < nb_read; i++)
	rw_buffer[i] = rw_buffer[i] * gain;
  while (nb_read)
  {
    output.WriteFloatF(rw_buffer, nb_read);
    nb_read = input.ReadFloatF(rw_buffer);
	for (int i=0; i < nb_read; i++)
	  rw_buffer[i] = rw_buffer[i] * gain;
  }
  delete[] rw_buffer;
}

void					PluginEffect::Normalize(WaveFile &input, WaveFile &output, float norma, int channel)
{
  int					nb_read;
  float					*rw_buffer = new float [channel * WAVE_TEMP_SIZE];
  float					k = 0;

  input.SetCurrentPosition(0);
  nb_read = input.ReadFloatF(rw_buffer);
  while (nb_read)
  {
	for (int i=0; i < nb_read; i++)
	  if (k < rw_buffer[i])
	    k = rw_buffer[i];
	nb_read = input.ReadFloatF(rw_buffer);
  }
  norma = fabs(norma - k); 

  input.SetCurrentPosition(0);
  nb_read = input.ReadFloatF(rw_buffer);
  while (nb_read)
  {
	for (int i=0; i < nb_read; i++)
		rw_buffer[i] += norma;
    output.WriteFloatF(rw_buffer, nb_read);
    nb_read = input.ReadFloatF(rw_buffer);
  }
  delete[] rw_buffer;

}

