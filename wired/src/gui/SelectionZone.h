/*
** Copyright (C) 2004, 2005 by WiReD Team
** Under the GNU General Public License
*/

#ifndef __SELECTION_ZONE_H__
#define __SELECTION_ZONE_H__

/*
** Zone rectangulaire dans le seqview permettant la selection
** multiple des patterns a la souris.
**
** Rectangular zone in the seqview allowing multiple patterns selection
** with the mouse.
*/

class SequencerView;
class ColoredLine;

#define STATE_VISIBLE			(0x1)
#define STATE_X_REVERSED		(0x2)
#define STATE_Y_REVERSED		(0x4)
#define ZONE_MARGIN			(0xA)		/* still unused */

class SelectionZone
{
  unsigned char				State;
  unsigned long				x_pos;
  unsigned long				y_pos;
  unsigned long				width;
  unsigned long				height;
  ColoredLine				*T;
  ColoredLine				*B;
  ColoredLine				*L;
  ColoredLine				*R;
  
 public:
  SelectionZone(SequencerView *S);
  SelectionZone(const unsigned long x, const unsigned long y,
		const unsigned long w, const unsigned long h, SequencerView *S);
  ~SelectionZone();
  int					IsVisible() { return (State & STATE_VISIBLE); }
  int					IsXReversed() { return (State & STATE_X_REVERSED); }
  int					IsYReversed() { return (State & STATE_Y_REVERSED); }
  void					SetZone(const unsigned long x, const unsigned long y,
						const unsigned long w, const unsigned long h);
  void					UpdateZone(long w, long h);
  void					Hide();
  unsigned long				GetZoneX() { return (x_pos); }
  unsigned long				GetZoneY() { return (y_pos); }
  unsigned long				GetZoneW() { return (width); }
  unsigned long				GetZoneH() { return (height); }
};

#endif/*__SELECTION_ZONE_H__*/
