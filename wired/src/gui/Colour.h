// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __COLOUR_H__
#define __COLOUR_H__

/*
** Merci d'aligner vos codes couleurs sur trois chiffres :)
*/

#define CL_WAVE_DRAW			wxColour( 88, 106, 226)
#define CL_MIDI_DRAW			wxColour(140, 185, 214)
#define CL_SEQ_BACKGROUND		wxColour(142, 142, 169)
#define CL_SEQ_FOREGROUND		wxColour(242, 242, 255)

#define CL_PATTERN_NORM			CL_WAVE_DRAW
#define CL_PATTERN_SEL			wxColour( 69,  42, 142)
#define CL_PATTERN_NAME			wxColour(  4,   2,  42)
#define CL_PATTERN_NAME_BRUSH		wxColour(251, 252, 253)

#define CL_WAVEDRAWER_BRUSH		wxColour(251, 252, 253)
#define CL_WAVEDRAWER_BRUSH_SEL		wxColour(151, 142, 169)

#define CL_MIDIDRAWER_BRUSH		wxColour(251, 252, 253)
#define CL_MIDIDRAWER_BRUSH_SEL		wxColour(151, 142, 169)

/*
** Rulers
*/

#define CL_RULER_BACKGROUND		wxColour(237, 237, 242)
#define CL_RULER_FOREGROUND		wxColour( 10,  10,  42)
#define CL_RULER_PATTERNNUM		wxColour(  0,   0,  24)

/*
** Sequencer colours
*/

#define CL_DEFAULT_BACKGROUND		wxColour(184, 184, 184)
#define CL_DEFAULT_FOREGROUND		wxColour(242, 242, 242)
#define CL_SEQVIEW_BACKGROUND		wxColour(242, 242, 255)
#define CL_SEQVIEW_FOREGROUND		wxColour( 59,  65,  70)
#define CL_SEQVIEW_MES			wxColour(142, 155, 166)
#define CL_SEQVIEW_BAR			wxColour(184, 204, 242)
#define CL_TRKVIEW_CONTOUR		wxColour(202, 207, 211)
#define CL_TRKVIEW_BACKGROUND		wxColour(109, 122, 130)
#define CL_CURSORZ_HEAD_PLAY		wxColour( 88, 108, 242)
#define CL_CURSORZ_HEAD_LEFT		wxColour(142, 142, 151)
#define CL_CURSORZ_HEAD_RIGHT		wxColour(124, 124, 142)
#define CL_CURSORZ_HEAD_END		wxColour( 51,  69,  84)
#define CL_CURSORZ_HEAD_NAME		wxColour(250, 252, 255)

#define CL_CURSORZ_LINE_GREEN		wxColour( 24, 242,  42)
#define CL_CURSORZ_LINE_RED		wxColour(242,  24,  42)
#define CL_CURSORZ_LINE_DARK		wxColour( 24,  24,  42)

#define CL_DEFAULT_SEQ_BRUSH		wxColour(142, 142, 242)
#define CL_DEFAULT_SEQ_PEN		wxColour(  4,   2,  42)

#define CL_SELECTED_ZONE		wxColour( 42,  42, 242)

/*
** Option panel
*/

#define CL_OPTION_TOOLBAR		wxColour(100, 101, 203)//wxColour( 68,  54, 163)

/*
** Wave editor
*/

#define CL_WAVEEDITOR_BACKGROUND	wxColour(242, 242, 255)

#endif/*__COLOUR_H__*/
