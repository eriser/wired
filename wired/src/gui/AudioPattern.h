// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __AUDIOPATTERN_H__
#define __AUDIOPATTERN_H__

#include "Pattern.h"
#include "WaveDrawer.h"

class					WriteWaveFile;
class					Channel;

class					AudioPattern: public Pattern, public WaveDrawer
{
 public:
  AudioPattern(double pos, double endpos, long trackindex);
  AudioPattern(double pos, WaveFile *w, long trackindex);
	AudioPattern(const AudioPattern& copy){*this = copy;};
  ~AudioPattern();
  
  float					**GetBlock(long block);
  void					Update();
  void					SetSelected(bool sel);
  bool					PrepareRecord(int type);
  void					StopRecord();
  void					GetRecordBuffer();
  void					OnBpmChange();
  void					SetDrawing();
  void					SetFullWave(WaveFile *w);
  void					SetWave(WaveFile *w);
  void					SetDrawColour(wxColour c);
  void					Split(double pos);
  void					OnDirectEdit();
  Pattern				*CreateCopy(double pos);
  
  AudioPattern				operator=(const AudioPattern& right);

  Channel				*InputChan;
  long					LastBlock;
  wxString				FileName;

 private:
  void					Init(WaveFile *w);

 protected:
  void					OnClick(wxMouseEvent &e);
  void					OnLeftUp(wxMouseEvent &e);
  void					OnDoubleClick(wxMouseEvent &e);
  void					OnRightClick(wxMouseEvent &e);
  void					OnMotion(wxMouseEvent &e);
  void					OnPaint(wxPaintEvent &e);  
  void					OnSize(wxSizeEvent &e);  
  void					SetSize(wxSize s);
  void					OnHelp(wxMouseEvent &event);
  
  WriteWaveFile				*RecordWave;
};

#endif/*__AUDIOPATTERN_H__*/
