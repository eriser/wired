#include "BeatNote.h"
#include "BeatBoxChannel.h"

inline void CalcPan(float pan, float *panvals)
{
  float tmp;
  if (pan < 0.5)
    {
      panvals[0] = 1.0f;
      tmp = pan * 2;
      panvals[1] = tmp;
    }
  else if (pan > 0.5)
    {
      panvals[1] = 1.0f;
      tmp = (1.0 - pan) * 2;
      panvals[0] = tmp;
    }
  else
    {
      panvals[0] = 1.0f;
      panvals[1] = 1.0f;
    }
}

BeatNoteToPlay::BeatNoteToPlay(float vel, unsigned long delta, 
			       int numchan, float* params)
			       
{
  NumChan = numchan;
  Delta = delta;
  
  for (int i = 0; i < NB_PARAMS; i++)
    Params[i] = params[i];
  Params[VEL] = vel;
  
  CalcPan(Params[PAN], Pan);
      
  Reversed = false;
  
  Buffer = 0x0;
  
  OffSet = 0;
  SEnd = 0;
  /*
    if (c->Wave)
    {
    OffSet = static_cast<unsigned long>
    (floor(c->Wave->GetNumberOfFrames() * Start));
    SEnd = static_cast<unsigned long>
    (floor(c->Wave->GetNumberOfFrames() * End));
    }
  */
}

BeatNoteToPlay::BeatNoteToPlay(BeatBoxChannel* chan)
{
  NumChan = chan->Id;
  for (int i = 0; i < NB_PARAMS; i++)
    Params[i] = chan->Params[i];
  Reversed = chan->Reversed;
  CalcPan(Params[PAN], Pan);
  Buffer = 0x0;
  OffSet = 0;
  SEnd = 0;
  
  if (chan->Wave)
    {
      OffSet = static_cast<unsigned long>
	(floor(chan->Wave->GetNumberOfFrames() * Params[STA]));
      SEnd = static_cast<unsigned long>
	(floor(chan->Wave->GetNumberOfFrames() * Params[END]));
    }
  
}
