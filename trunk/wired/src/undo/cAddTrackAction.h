
#if !defined(ADD_TRACK_ACTION_H)
#define ADD_TRACK_ACTION_H

#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"

#define	HISTORY_LABEL_ADD_TRACK_ACTION	"AddTrackAction"

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
    { 
    	SeqPanel->RemoveTrack();
    	//SeqPanel->DeleteSelectedTrack();
    };
  
  virtual void Accept (cActionVisitor& visitor)
    { visitor.Visit (*this); };
  virtual std::string		getHistoryLabel()				// Returns History label string
  				{return HISTORY_LABEL_ADD_TRACK_ACTION;};
};

#endif
