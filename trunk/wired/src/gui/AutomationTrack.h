// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __AUTOMATIONTRACK_H__
#define __AUTOMATIONTRACK_H__

#include <vector>
#include "Pattern.h"
#include "WiredDocument.h"

class MidiEvent;

typedef std::vector<MidiEvent * > pMidiEventVector;

//! Pattern specialisation for automation track.
class AutomationPattern : public Pattern
{
 private:
  WiredDocument    *_documentParent;
  pMidiEventVector _events;

 public:
  AutomationPattern  (WiredDocument *parent);
  ~AutomationPattern ();

  void    Save          ();
  void    Load          (SaveElementArray data);
  void    Update        ();
  void    SetSelected   (bool sel);
  void    OnBpmChang    ();
  void    SetDrawColour (wxColour c);
  Pattern *CreateCopy   (double pos);
  void    SetSize       (wxSize s);

 protected:
  void OnClick             (wxMouseEvent &e);
  void OnLeftUp            (wxMouseEvent &e);
  void OnDoubleClick       (wxMouseEvent &e);
  void OnRightClick        (wxMouseEvent &e);
  void OnMotion            (wxMouseEvent &e);
  /*
  void OnDeleteClick       (wxCommandEvent &e);
  void OnMoveToCursorClick (wxCommandEvent &e);
  void OnPaint             (wxPaintEvent &e);
  void OnHelp              (wxMouseEvent &event);
  */
};

#endif /* __AUTOMATIONTRACK_H__ */
