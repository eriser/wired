#ifndef OSC_H_
# define OSC_H_

#include <math.h>
#include <strings.h>
#include <assert.h>
#include <stdlib.h>

#define SAMPLE_PERIOD                   (1.f / 44100.f)

class Oscillator
{
 protected:
  float Volume;
  float Freq;
  float Phase;
  float Transpose;
  float Fine;
  unsigned long SampleRate;
 public:
  Oscillator();
  virtual ~Oscillator();
  virtual void Accumulate(float **, unsigned long) = 0;
  
  float GetVolume() { return Volume; }
  float GetFreq() { return Freq; }
  float GetPhase() { return Phase; }
  float GetTranspose() { return Transpose; }
  float GetFine() { return Fine; }
  unsigned long GetRate() { return SampleRate; }
  
  void SetVolume(float v) { Volume = v; }
  void SetFreq(float f) { Freq = f; }
  void SetPhase(float p) { Phase = p; }
  void SetTranspose(float t) { Transpose = t; }
  void SetFine(float f) { Fine = f; }
  void SetSampleRate(unsigned long s) { assert(s != 0); SampleRate = s; }
};

class SineOscillator : public Oscillator
{
 public:
  SineOscillator();
  ~SineOscillator();
  virtual void Accumulate(float **, unsigned long);
};

class SquareOscillator : public Oscillator
{
 public:
  SquareOscillator();
  ~SquareOscillator();
  virtual void Accumulate(float **, unsigned long);
};

class TriangleOscillator : public Oscillator
{
 public:
  TriangleOscillator();
  ~TriangleOscillator();
  virtual void Accumulate(float **, unsigned long);
};

class NoisePseudoOscillator : public Oscillator
{
 public:
  NoisePseudoOscillator();
  ~NoisePseudoOscillator();
  virtual void Accumulate(float **, unsigned long);
};

#endif // OSC_H_
