#include <math.h>
#include "BeatNote.h"
#include "BeatBoxChannel.h"

BeatNoteToPlay::BeatNoteToPlay(float vel, unsigned long delta, 
			       int numchan)
			       
{
  Params[VEL] = vel;
  
  NumChan = numchan;
  Delta = delta;
  Buffer = 0x0;
  OffSet = 0;
  SEnd = 0;
}

BeatNoteToPlay::BeatNoteToPlay(void)
{
  Delta = 0;
  Buffer = 0x0;
  OffSet = 0;
  SEnd = 0;
}
