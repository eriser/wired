#ifndef __ASKEY_H__
#define __ASKEY_H__
#include <wx/wx.h>

class ASKey: public wxPanel
{
 public:
  ASKey(wxWindow *parent, wxWindowID id, const wxPoint& pos,
	const wxSize& size, bool pisBlack, wxString pnote, int note_code);

 bool selected;
 bool isBlack;
 wxString note;
 int code;

 void OnPaint(wxPaintEvent &);
 void OnLeftDown(wxMouseEvent &);
 void OnLeftUp(wxMouseEvent &);
 void OnEnterWindow(wxMouseEvent &);
 void OnLeaveWindow(wxMouseEvent &);
DECLARE_EVENT_TABLE()
};


#endif
