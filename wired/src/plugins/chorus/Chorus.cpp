// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Chorus.h"
#include <iostream>

Chorus :: Chorus(StkFloat baseDelay, string dirpath)
{
  delayLine_[0].setMaximumDelay( (unsigned long) (baseDelay * 1.414) + 2);
  delayLine_[0].setDelay( baseDelay );
  delayLine_[1].setMaximumDelay( (unsigned long) (baseDelay * 1.414) + 2);
  delayLine_[1].setDelay( baseDelay );
  baseLength_ = baseDelay;

  // Concatenate the STK rawwave path to the rawwave file
  mods_[0] = new WvLoop( (dirpath + "plugins/chorus/sinewave.raw").c_str(), 
			   true );
  mods_[1] = new WvLoop( (dirpath + "plugins/chorus/sinewave.raw").c_str(), 
			   true );
  mods_[0]->setFrequency(0.2);
  mods_[1]->setFrequency(0.222222);
  modDepth_ = 0.05;
  effectMix_ = 0.5;
  this->clear();
}



void	Chorus :: setBaseLength(StkFloat baseDelay)
{
  delayLine_[0].setMaximumDelay( (unsigned long) (baseDelay * 1.414) + 2);
  delayLine_[0].setDelay( baseDelay );
  delayLine_[1].setMaximumDelay( (unsigned long) (baseDelay * 1.414) + 2);
  delayLine_[1].setDelay( baseDelay );
  baseLength_ = baseDelay;
  /*
  // Concatenate the STK rawwave path to the rawwave file
  mods_[0] = new WvLoop( (dirpath + "sinewave.raw").c_str(), true );
  mods_[1] = new WvLoop( (dirpath + "sinewave.raw").c_str(), true );
  mods_[0]->setFrequency(0.2);
  mods_[1]->setFrequency(0.222222);
  modDepth_ = 0.05;
  effectMix_ = 0.5;
  */
  this->clear();
}




Chorus :: ~Chorus()
{
  delete mods_[0];
  delete mods_[1];
}

void Chorus :: clear()
{
  delayLine_[0].clear();
  delayLine_[1].clear();
  lastOutput_[0] = 0.0;
  lastOutput_[1] = 0.0;
}

void Chorus :: setModDepth(StkFloat depth)
{
  modDepth_ = depth;
}

void Chorus :: setModFrequency(StkFloat frequency)
{
  mods_[0]->setFrequency(frequency);
  mods_[1]->setFrequency(frequency * 1.1111);
}

StkFloat Chorus :: tick(StkFloat input)
{
  delayLine_[0].setDelay( baseLength_ * 0.707 * (1.0 + mods_[0]->tick()) );
  delayLine_[1].setDelay( baseLength_  * 0.5 *  (1.0 - mods_[1]->tick()) );
  lastOutput_[0] = input * (1.0 - effectMix_);
  lastOutput_[0] += effectMix_ * delayLine_[0].tick(input);
  lastOutput_[1] = input * (1.0 - effectMix_);
  lastOutput_[1] += effectMix_ * delayLine_[1].tick(input);
  return Effect::lastOut();
}

StkFloat *Chorus :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Effect::tick( vector, vectorSize );
}

StkFrames& Chorus :: tick( StkFrames& frames, unsigned int channel )
{
  return Effect::tick( frames, channel );
}
