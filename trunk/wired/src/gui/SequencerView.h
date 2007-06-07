// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __SEQUENCERVIEW_H__
#define __SEQUENCERVIEW_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "SequencerGui.h"
#include "Pattern.h"
#include "SelectionZone.h"
#include "AccelCenter.h"
#include "../save/WiredDocument.h"

// 80.6 ? any idea where to find a better value for the horiz pixel/track time ratio ?
#define	HORIZ_SEQ_RATIO	80.6
class				SequencerGui;
class				SelectionZone;
class				AccelCenter;

class				SequencerView: public wxWindow, public WiredDocument
{
  friend class			SequencerGui;
  friend class			Pattern;

 private:
  void				OnClick(wxMouseEvent &e);
  void				OnMotion(wxMouseEvent &e);
  void				OnLeftUp(wxMouseEvent &e);
  void				OnRightClick(wxMouseEvent &event);
  void				OnPaint(wxPaintEvent &event);
  void				OnHelp(wxMouseEvent &event);
  void				SelectZonePatterns(bool shift);

  unsigned long			TotalWidth;
  unsigned long			TotalHeight;
  long				XScroll;
  long				YScroll;
  SelectionZone			*TheZone;
  AccelCenter			*HAxl;
  AccelCenter			*VAxl;

 public:
  SequencerView(SequencerGui *parent, const wxPoint &pos, const wxSize &size);
  ~SequencerView();

  // WiredDocument implementation
  void				Save();
  void				Load(SaveElementArray data);

  void				SetXScroll(long x, long range, long seqwidth);
  void				SetXScrollValue(long X);
  long				GetXScroll();
  void				SetYScroll(long y, long range, long seqwidth);
  void				SetYScrollValue(long Y);
  long				GetYScroll();
  void				AutoScroll(double xmove, double ymove);
  void				AutoXScroll(double xmove);
  void				AutoXScrollBackward(long accel_type = 0);
  void				AutoXScrollForward(long accel_type = 0);
  void				AutoXScrollReset();
  void				AutoYScroll(double ymove);
  void				AutoYScrollBackward(long accel_type = 0);
  void				AutoYScrollForward(long accel_type = 0);
  void				AutoYScrollReset();
  unsigned long			GetTotalWidth();
  void				SetTotalWidth(unsigned long w);
  unsigned long			GetTotalHeight();
  void				SetTotalHeight(unsigned long h);
  void				Drop(int x, int y, wxString file);

 protected:
  void				DrawMeasures(wxDC &dc);
  void				DrawTrackLines(wxDC &dc);

  DECLARE_EVENT_TABLE()
};

#endif // __SEQUENCERVIEW_H__
