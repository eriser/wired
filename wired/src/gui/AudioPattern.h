// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __AUDIOPATTERN_H__
#define __AUDIOPATTERN_H__

#include "Pattern.h"
#include "WaveView.h"
#include "WriteWaveFile.h"
#include "Channel.h"

class AudioPattern: public Pattern, public WaveDrawer
{
 public:
  AudioPattern(double pos, double endpos, long trackindex);
  AudioPattern(double pos, WaveFile *w, long trackindex);
  ~AudioPattern();
  
  float					**GetBlock(long block);
  void					Update();
  void					SetSelected(bool sel);
  bool					PrepareRecord(int type);
  void					StopRecord();
  void					GetRecordBuffer();
  void					OnBpmChange();
  void					SetDrawing();
  void					SetWave(WaveFile *w);
  void					SetDrawColour(wxColour c);

  Pattern				*CreateCopy(double pos);
  Channel				*InputChan;

  long					LastBlock;
  string				FileName;

 protected:
  void					Init();
  void					OnClick(wxMouseEvent &e);
  void					OnDoubleClick(wxMouseEvent &e);
  void					OnRightClick(wxMouseEvent &e);
  void					OnMotion(wxMouseEvent &e);
  void					OnPaint(wxPaintEvent &e);  
  void					OnSize(wxSizeEvent &e);  
  void					SetSize(wxSize s);
  void					OnHelp(wxMouseEvent &event);
  
  WriteWaveFile				*RecordWave;
//  DECLARE_EVENT_TABLE()
};

#endif/*__AUDIOPATTERN_H__*/
