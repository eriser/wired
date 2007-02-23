// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <wx/string.h>
#include <wx/wx.h>

#include "WiredDocument.h"

#define PATTERN_DRAG_SCROLL_UNIT	(MEASURE_WIDTH + 1)
#define PATTERN_NAME_WIDTH		(42)
#define PATTERN_NAME_HEIGHT		(10)
#define PATTERN_NAME_MARGINS		(3)
#define PATTERN_NAME_OFFSET		(2)
#define PATTERN_MOVE_BOX_SIZE		(4)
#define PATTERN_MASK_SELECTED		(1)
#define PATTERN_MASK_DRAGGED		(2)
#define PATTERN_MASK_TOGGLED		(4)
#define PATTERN_AUTO_SCROLL_BOX		(42)

class					WaveFile;
class					MidiEvent;

/**
 * creating a pattern.
 */
class					Pattern : public wxWindow, public WiredDocument
{
  
  /**
   * Still unused,it'll to be avoid wrong way scroll during a pattern dragging.
   * The default value is 0. 
   */    
  double							xdrag;		
  
  /**
   * The default value is 0.
   */
  double							ydrag;		

 protected:

  virtual void				OnClick(wxMouseEvent &e);
  virtual void				OnLeftUp(wxMouseEvent &e);
  virtual void				OnDoubleClick(wxMouseEvent &e);
  virtual void				OnRightClick(wxMouseEvent &e);
  virtual void				OnMotion(wxMouseEvent &e);

  /**
   * moving pattern.
   * \param Motion a double,checking if pattern changed its position.
   */
  void								XMove(double Motion);

  /**
   * Setting and drawing a pattern.
   * \param a wxPaintDC,dc.
   * \param a wxSize,the positions.
   */
  void								DrawName(wxPaintDC &dc, const wxSize &s);

  /**
   * The beginning of pattern.
   */
  double							Position;

  /**
   * The end of pattern.
   */
  double							EndPosition;

  /**
   * The length of pattern.
   */
  double							Length;

  /**
   * The Index of track.
   */
  unsigned long				TrackIndex;

  /**
   * The state of pattern, selected, dragged and/or toggled (see PATTERN_MASK_*)
   */
  unsigned char				StateMask;
  unsigned long							TrackFrom;

  /**
   * To measure the position of pattern.
   */
  wxPoint							m_pos;

  /**
   * To measure the size of pattern.
   */
  wxSize							m_size;

  /** 
   * To measure the position which clicked.
   */
  wxPoint							m_click;

  /**
   * Name of pattern.
   */
  wxString							Name;

  /**
   * Setting a color of a pattern.
   */ 
  wxColour							PenColor;
 
  /**
   * The background of pattern.
   */
  wxColour							BrushColor;

  /**
   * Next merged pattern.
   */
  Pattern							*NextMergedPattern;

  /**
   * Previous merged pattern.
   */
  Pattern							*PrevMergedPattern;

 public:

  /**
   * The information of pattern.
   * \param pos a double,the beginning position of pattern.
   * \param endpos a  double,the end position of pattern.
   * \param trackindex a long,the index of track.
   */
  Pattern(WiredDocument *parent, wxString name, double pos, double endpos, long trackindex);
//  Pattern(const Pattern& copy){*this = copy;};
  virtual ~Pattern();


  // WiredDocument implementation
  void				Save();
  void				Load(SaveElementArray data);


  /**
   * To modify the pattern.
   * \param newpos a double,the new beginning position of pattern.
   * \param newendpos a double,the new end position of pattern.
   * \param newtrackindex a long,the new index of track.
   * \param newlength a double,the new length of pattern.
   */
  void								Modify(double newpos = -1, double newendpos = -1, 
								       long newtrackindex = -1, double newlength = -1);

  /**
   * To measure a position,and updated it.
   */
  void								UpdateMeasure();

  /**
   * To update a position of pattern and its size of pattern.
   */
  virtual void							Update();
  
  /**
   * Setting the selected of pattern.
   * \param sel a bool,If it selected or not.
   */
  virtual void							SetSelected(bool sel);
  virtual void							OnBpmChange() {};

  /**
   * Setting the color of pen.
   * \param c a wxcolour,a color.
   */
  virtual void							SetDrawColour(wxColour c) { PenColor = c; };
  virtual Pattern						*CreateCopy(double pos) = 0x0;

  /**
   * Getting a measure of a position of pattern.
   * \return returns a wxPoint,a measure of position.
   */
  wxPoint							GetMPosition() { return (m_pos); };

  /**
   * Setting the measure of a position of pattern.
   * \param a wxPoint,the position.
   */
  void								SetMPosition(wxPoint p) { m_pos = p; };

  /**
   * Setting the position.
   * \param a double,the beginning of position.
   * \param a long,the index of the track.
   * \return returns a wxPoint,the measure of position.
   */
  wxPoint							SetPos(double newpos, long track);

  /**
   * Getting the size of pattern.
   * \return returns a wxSize,the measure of the size of the pattern.
   */
  wxSize							GetSize() { return (m_size); };

  /**
   * Setting a size of pattern.
   * \param a wxSize,a size of pattern.
   */
  void								SetSize(wxSize s) { m_size = s; };

  /**
   * Getting a width of pattern.
   * \param a double,a beginning of position.
   * \return returns a int.
   */ 
  int									GetXPos(double pos);

  /**
   * If selected a pattern.
   * \return returns a bool,if selected then it returns true.
   */
  bool								IsSelected() { return (StateMask & PATTERN_MASK_SELECTED); };

  /**
   * Setting a position of pattern.
   * \param a double,Setting the positon.
   */
  double							SetPosition(double p) { Position = p; };

  /**
   * Getting the beginning position.
   * \return returns a double,the posion.
   */  
  double							GetPosition() { return (Position); };

  /**
   * Getting the end of position.
   * \return returns a double,the end of position.
   */ 
  double							GetEndPosition() { return (EndPosition); };

  /**
   * Getting a name of pattern.
   * \return returns a wxString,the name of pattern.
   */
  wxString						GetName() { return (Name); };
  
  /**
   * Setting the name of pattern.
   * \param a wxString,the name of pattern.
   */
  void								SetName(wxString n) { Name = n; };

  /**
   * Getting a index of track.
   * \return returns a long,the index of track.
   */ 
  long								GetTrackIndex() { return (TrackIndex); };

  /**
   * Setting the index of track.
   * \param a long,the index of track.
   */
  void								SetTrackIndex(long t) { TrackIndex = t; };

  /**
   * Sets the next merged pattern.
   * \param points to the next merged pattern.
   */
  void								SetNextMergedPattern(Pattern *p) { NextMergedPattern = p; };

  /**
   * Sets the previous merged pattern.
   * \param points to the previous merged pattern.
   */
  void								SetPrevMergedPattern(Pattern *p) { PrevMergedPattern = p; };

  /**
   * Gets the next merged pattern.
   * \return returns pointer to the next merged pattern.
   */
  Pattern							*GetNextMergedPattern() { return NextMergedPattern; };

  /**
   * Gets the previous merged pattern.
   * \return returns pointer to the previous merged pattern.
   */
  Pattern							*GetPrevMergedPattern() { return PrevMergedPattern; };
  
  /**
   * Getting the end of position.
   * \return returns a double,the end of position.
   */
  double							GetEndPos();


  void								Dump();
//  virtual Pattern			operator=(const Pattern& right) = 0;
};

#endif
