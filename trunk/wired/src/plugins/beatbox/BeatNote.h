#ifndef __BEATNOTE_H__
#define __BEATNOTE_H__

#define NB_PARAMS	6

#define LEV		0
#define VEL		1
#define PIT		2
#define PAN		3
#define STA		4
#define END		5

class BeatBoxChannel;

class BeatNote
{
 public:
  BeatNote(unsigned int numchan, double pos, unsigned int state, double bpos)
    { 
      Selected = false;
      NumChan = numchan;
      State = state;
      Position = pos;
      BarPos = bpos;
      Params[LEV] = 1.0f;
      Params[VEL] = (float)(state / 4.f);
      Params[PIT] = 1.0f;
      Params[PAN] = 0.5f;
      Params[STA] = 0.0f;
      Params[END] = 1.0f;
      
    }
  BeatNote(BeatNote* note)
    {
      Selected = note->Selected;
      NumChan = note->NumChan;
      State = note->State;
      Position = note->Position;
      BarPos = note->BarPos;
      
      for (int i = 0; i < NB_PARAMS; i++)
	Params[i] = note->Params[i];
    }
  ~BeatNote() {}
  
  unsigned int  State;
  bool		Selected;
  unsigned int	NumChan;
  double	Position;
  double	BarPos;
  bool		Reversed;
  float		Params[NB_PARAMS];
};

class BeatNoteToPlay
{
 public:
  BeatNoteToPlay(float vel, unsigned long delta, 
		 int numchan);		//Midi notes ctor()
  BeatNoteToPlay(BeatNote* bn, unsigned long delta)	//ctor()from note
    {
      NumChan = bn->NumChan;
      Reversed = bn->Reversed;
      Delta = delta;
      OffSet = 0;
      SEnd = 0;
      Buffer = 0x0;
      for (int i = 0; i < NB_PARAMS; i++)
	Params[i] = bn->Params[i];
    }
  BeatNoteToPlay(void);
  ~BeatNoteToPlay()
    {
    }
  
  unsigned int	NumChan;
  float		Params[NB_PARAMS];
  float		Pan[2];
  bool		Reversed;
  float**	Buffer;
  unsigned long Delta;
  long		OffSet;
  long		SEnd;
};

#endif//__BEATNOTE_H__
