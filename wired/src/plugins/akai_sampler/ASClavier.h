// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __ASCLAVIER_H__
#define __ASCLAVIER_H__
#include <wx/wx.h>
#include <vector>
#include "ASKey.h"

using std::vector;

#define	BLACKASKEY_WIDTH 8 // 10	
#define WHITEASKEY_HEIGHT 40 // 60	
#define	NB_WHITEASKEY	56 //75
#define	BLACKASKEY_HEIGHT (WHITEASKEY_HEIGHT * 2 / 3)
#define ASCLAVIER_WIDTH	(BLACKASKEY_WIDTH * (NB_WHITEASKEY * 12 / 7))
#define	ASCLAVIER_HEIGHT (WHITEASKEY_HEIGHT + 10)

class ASClavier: public wxControl
{
  private:
  vector<ASKey *> keys;
  class AkaiSampler *as;
    
 public:
  ASClavier(wxWindow *parent, wxWindowID id, const wxPoint& pos,
	const wxSize& size, long style, class AkaiSampler *);

  void OnPaint(wxPaintEvent &);
  void OnKeyDown(wxMouseEvent &event);
  void OnKeyUp(wxMouseEvent &event);
  vector<ASKey *> GetKeys() { return keys; }

  DECLARE_EVENT_TABLE()
};


#endif
