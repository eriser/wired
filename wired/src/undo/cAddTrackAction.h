
#if !defined(ADD_TRACK_ACTION_H)
#define ADD_TRACK_ACTION_H

#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"

class SequencerGui;

extern SequencerGui          *SeqPanel;

class cAddTrackAction : public cAction 
{
 private:
  bool mTrackKindFlag;
  
 public:
  cAddTrackAction (bool kind)
    { mTrackKindFlag = kind; };
  
  ~cAddTrackAction ()
    {};
  
  virtual void Do ()
    { SeqPanel->AddTrack(mTrackKindFlag); NotifyActionManager(); };
  
  virtual void Redo ()
    { SeqPanel->AddTrack(mTrackKindFlag); };
  
  virtual void Undo ()
    { SeqPanel->RemoveTrack(); };
  
  virtual void Accept (cActionVisitor& visitor)
    { visitor.Visit (*this); };
};

#endif
