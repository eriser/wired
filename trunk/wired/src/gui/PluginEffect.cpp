
#include "PluginEffect.h"
#include <iostream>

using namespace std;

/* DO NOT MODIFY THIS FILE FOR MAKING A PLUGIN. JUST DERIVE FROM THAT CLASS */

PluginEffect::PluginEffect() 
{ 

}

PluginEffect::~PluginEffect() 
{ 

}

void		PluginEffect::Process(WaveFile &input, WaveFile &output, float gain, int channel, int effet)
{
  if (effet == 1)
	Gain(input, output, gain, channel);
   else  if (effet == 2)
	Normalize(input, output, channel);
}

void 		PluginEffect::Gain(WaveFile &input, WaveFile &output, 
							  float gain, int channel)
{
  wxString 				text;
  int                   nb_read;
  
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
}

void 		PluginEffect::Normalize(WaveFile &input, WaveFile &output, int channel)
{
  int                   nb_read;
  
  float * rw_buffer = new float [channel * WAVE_TEMP_SIZE];

  input.SetCurrentPosition(0);
  nb_read = input.ReadFloatF(rw_buffer);
    
  float	k = 0;
  while (nb_read)
  {
	for (int i=0; i < nb_read; i++)
	  if (k < rw_buffer[i])
	    k = rw_buffer[i];
	nb_read = input.ReadFloatF(rw_buffer);
  }
  
  input.SetCurrentPosition(0);
  nb_read = input.ReadFloatF(rw_buffer);
  
  while (nb_read)
  {
	for (int i=0; i < nb_read; i++)
	  {
		float norma = k - rw_buffer[i];
		rw_buffer[i] += norma;
	  }
    output.WriteFloatF(rw_buffer, nb_read);
    nb_read = input.ReadFloatF(rw_buffer);
  }
}

