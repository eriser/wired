#ifndef __BEATBUTTON_H__
#define __BEATBUTTON_H__

#include <iostream>
#include <wx/wx.h>

#define ID_POS		0
#define ID_STATE	1
#define ID_X		2

#define ID_UNCLICKED	0
#define ID_VLOW		1
#define ID_LOW		2
#define ID_MEDIUM	3
#define ID_HIGH		4
#define ID_VHIGH	5

#define MAX_BITMAPS	6


#define MEDIUM_POS	11
#define MEDIUM_SIZE	10
#define BTN_SIZE	28

#define EVT_MOTION_OUT	31133113
#define BEATBUTTON_ID	13377331

#define GET_STATE(state, x, y)	{					\
      if ( ((x > MEDIUM_POS) && (x < MEDIUM_POS + MEDIUM_SIZE)) &&	\
	 ((y > MEDIUM_POS) && (y < MEDIUM_POS + MEDIUM_SIZE)) )		\
	state = ID_MEDIUM;						\
      else if (x < BTN_SIZE/2)	{					\
					if (y < BTN_SIZE/2)		\
					  state = ID_HIGH;		\
					else				\
					  state = ID_VLOW;		\
				}					\
      else			{					\
					if (y < BTN_SIZE/2)		\
					  state = ID_VHIGH;		\
					else				\
					  state = ID_LOW;		\
				}					\
  	}

class BeatButton : public wxWindow
{
 public:
  BeatButton(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
	     const wxSize& size, wxBitmap** bitmaps, unsigned int pos);
  ~BeatButton();
  unsigned int* GetData() { return Data; }
  
  void OnPaint(wxPaintEvent &event);
  void OnMouseEvent(wxMouseEvent &event);
  void OnMotion(wxMouseEvent &event);
  void OnEnter(wxMouseEvent &event);
  void OnLeave(wxMouseEvent &event);
  void OnRightDownEvent(wxMouseEvent &event);
  void SetState(unsigned int state);
  unsigned int GetState() { return Data[ID_STATE]; }
 protected:
  wxBitmap**	Bitmaps;
  unsigned int	Data[3];
DECLARE_EVENT_TABLE()

};

#endif//__BEATBUTTON_H__
