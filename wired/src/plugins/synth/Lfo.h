// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef LFO_H_
# define LFO_H_

class Lfo
{
 private:
  float OscTime;
  float DelayTime;
  float Frequency;
  float Rate;
  float Amount;
  float Delay;

 public:
  Lfo();
  ~Lfo();
  float GetRate() { return Rate; }
  float GetAmount() { return Amount; }
  float GetDelay() { return Delay; }
  float Compute();
  void SetRate(float r) { Rate = r; }
  void SetAmount(float a) { Amount = a; }
  void SetDelay(float d) { Delay = d; }
};

#endif // LFO_H_
