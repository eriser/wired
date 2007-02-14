// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "Osc.h"

static const float pi2 = 2 * M_PI;
static const float sqrt2 = sqrtf(2);

Oscillator::Oscillator() : 
  Volume(1), Freq(0), Phase(0), 
  Transpose(0), Fine(0)
{ }

Oscillator::~Oscillator()
{ }

SineOscillator::SineOscillator()
{ }

SineOscillator::~SineOscillator()
{ }

void SineOscillator::Accumulate(float **output, unsigned long sample_length)
{
  float frequency = Freq; // FIXME handle fine and stuff ..
  float delta = pi2 * frequency / SampleRate; 
  float inc;

  for (unsigned long i = 0; i < sample_length; i++)
    {
      inc = Volume * sinf(Phase);
      output[0][i] += inc;
      output[1][i] += inc;
      Phase += delta;
    }
}

SquareOscillator::SquareOscillator()
{ }

SquareOscillator::~SquareOscillator()
{ }

void SquareOscillator::Accumulate(float **output, unsigned long sample_length)
{ }

TriangleOscillator::TriangleOscillator()
{ }

TriangleOscillator::~TriangleOscillator()
{ }

void TriangleOscillator::Accumulate(float **output, unsigned long sample_length)
{ }

NoisePseudoOscillator::NoisePseudoOscillator()
{ }

NoisePseudoOscillator::~NoisePseudoOscillator()
{ }

void NoisePseudoOscillator::Accumulate(float **output, unsigned long sample_length)
{ 
  // FIXME pink noise would actually be better
  for (unsigned long i = 0; i < sample_length; i++)
    {
      output[0][i] += (Volume * rand()) / RAND_MAX;
      output[1][i] += (Volume * rand()) / RAND_MAX;
    }
}

