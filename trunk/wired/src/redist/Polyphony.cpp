#include "Polyphony.h"

#define BUFSIZE 4096

Polyphony::Polyphony()
{
  buffer_size = BUFSIZE;
  Volume = 1.f;
}

Polyphony::~Polyphony()
{  
  list<Poly *>::iterator i;
  for (i = Voices.begin(); i != Voices.end(); i++)
    {
      delete (*i)->Buffer[0];
      delete (*i)->Buffer[1];
      delete (*i)->Buffer;
      delete *i;
    }      
}

void Polyphony::SetVolume(float vol)
{
  Volume = vol;
}

void Polyphony::SetPolyphony(int p)
{
  int count = p - Voices.size();
  int j;
  Poly *voice;

  printf("[POLYPHONY] Setting polyphony to: %d\n", p);
  fflush(stdout);  
  PolyphonyCount = p;
  for (j = 0; j < count; j++)
    {
      voice = new Poly;
      voice->IsFree = true;
      voice->Buffer = new float*[2];
      voice->Buffer[0] = new float[buffer_size];
      voice->Buffer[1] = new float[buffer_size];
      memset(voice->Buffer[0], 0, buffer_size * sizeof(float));
      memset(voice->Buffer[1], 0, buffer_size * sizeof(float));
      Voices.push_back(voice);
    }
}

#include <stdio.h>

void Polyphony::SetBufferSize(int size)
{
  buffer_size = size;
  printf("[POLYPHONY] Buffer size: %d\n", buffer_size);
  fflush(stdout);
  //  OnChangeSettings();
}

float **Polyphony::GetFreeBuffer()
{
  list<Poly *>::iterator i;
  for (i = Voices.begin(); i != Voices.end(); i++)
    if ((*i)->IsFree)     
      {
	(*i)->IsFree = false;
	return (*i)->Buffer;
      }
  printf("[POLYPHONY] Returning No Buffer !!!\n");
  fflush(stdout);  
  return (0x0);
}

void Polyphony::SetFreeBuffer(float **f)
{
  list<Poly *>::iterator i;
  for (i = Voices.begin(); i != Voices.end(); i++)
    if ((*i)->Buffer == f) 
      {
	(*i)->IsFree = true;
	return;
      }
}

void Polyphony::OnChangeSettings()
{
  list<Poly *>::iterator i;
  for (i = Voices.begin(); i != Voices.end(); i++)
    {
      delete (*i)->Buffer[0];
      delete (*i)->Buffer[1];
      delete (*i)->Buffer;
      delete *i;
    }     
  SetPolyphony(PolyphonyCount);
}

void Polyphony::GetMix(float **output)
{
  list<Poly *>::iterator v;
  unsigned long		i;

  for (i = 0; i < buffer_size; i++)
    {
      for (v = Voices.begin(); v != Voices.end(); v++)
	if (!((*v)->IsFree))
	  {	      
	    output[0][i] += (*v)->Buffer[0][i];
	    output[1][i] += (*v)->Buffer[1][i];
	  }
      output[0][i] *= Volume;
      output[1][i] *= Volume;
    }
}
