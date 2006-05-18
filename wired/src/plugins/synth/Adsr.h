// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef ADSR_H_
# define ADSR_H_

class Adsr
{
 private:
  float Attack;
  float Decay;
  float Sustain;
  float Release;
 public:
  Adsr();
  ~Adsr();
  float GetAttack() { return Attack; }
  float GetDecay() { return Decay; }
  float GetSustain() { return Sustain; }
  float GetRelease() { return Release; }
  void SetAttack(float a) { Attack = a; }
  void SetDecay(float d) { Decay = d; }
  void SetSustain(float s) { Sustain = s; }
  void SetRelease(float r) { Release = r; }
};

#endif // ADSR_H_
