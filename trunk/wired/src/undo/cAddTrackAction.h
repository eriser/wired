// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#if !defined(ADD_TRACK_ACTION_H)
#define ADD_TRACK_ACTION_H

#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"
#include "SequencerGui.h"

#define	HISTORY_LABEL_ADD_TRACK_ACTION	_("AddTrackAction")

class				SequencerGui;

extern SequencerGui		*SeqPanel;

class				cAddTrackAction : public cAction 
{
 private:
  bool				mTrackKindFlag;
  
 public:
  cAddTrackAction (bool kind)
    { mTrackKindFlag = kind; };
  cAddTrackAction(const cAddTrackAction& copy)
    { *this = copy; };
  ~cAddTrackAction ()
    {};
  
  virtual void			Do ()
    { SeqPanel->AddTrack(mTrackKindFlag); NotifyActionManager(); };
  
  virtual void			Redo ()
    { SeqPanel->AddTrack(mTrackKindFlag); };
  
  virtual void			Undo ()
    { 
    	SeqPanel->RemoveTrack();
    	//SeqPanel->DeleteSelectedTrack();
    };
  
  virtual void			Accept (cActionVisitor& visitor)
    { visitor.Visit (*this); };
  // Returns History label wxString
  virtual const wxString	getHistoryLabel()
  {return HISTORY_LABEL_ADD_TRACK_ACTION;};
  				
  cAddTrackAction		operator=(const cAddTrackAction& right)
    {if (this != &right) mTrackKindFlag = right.mTrackKindFlag;return *this;}
};

#endif
