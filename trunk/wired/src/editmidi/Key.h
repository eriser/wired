#ifndef __KEY_H__
#define __KEY_H__
#include <wx/wx.h>

class Key: public wxPanel
{
 public:
  Key(wxWindow *parent, wxWindowID id, const wxPoint& pos,
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