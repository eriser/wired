// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#if !defined(ADD_TRACK_ACTION_H)
#define ADD_TRACK_ACTION_H

#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"
#include "../gui/SequencerGui.h"
#include "../sequencer/Track.h"

#define	HISTORY_LABEL_ADD_TRACK_ACTION	_("AddTrackAction")

class				SequencerGui;

extern SequencerGui		*SeqPanel;

class				cAddTrackAction : public cAction
{
 private:
  trackType			mTrackKindFlag;
  Track*			trackCreated;

 public:
  cAddTrackAction (trackType kind)
    { mTrackKindFlag = kind; };
  cAddTrackAction(const cAddTrackAction& copy)
    { *this = copy; };
  ~cAddTrackAction ()
    {};

  virtual void			Do ()
    { trackCreated = SeqPanel->CreateTrack(mTrackKindFlag); NotifyActionManager(); };

  virtual void			Redo ()
    { trackCreated = SeqPanel->CreateTrack(mTrackKindFlag); };

  virtual void			Undo ()
    {
      SeqPanel->DeleteTrack(trackCreated);
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
