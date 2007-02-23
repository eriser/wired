// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __AUDIOPATTERN_H__
#define __AUDIOPATTERN_H__

#include "Pattern.h"
#include "WaveDrawer.h"

#include "WiredDocument.h"

class					WriteWaveFile;
class					Channel;

class					AudioPattern: public Pattern, public WaveDrawer
{
 private:
  WiredDocument*			_documentParent;

 public:
  Channel*				InputChan;
  long					LastBlock;
  wxString				FileName;
  WaveFile				*wavefile;
 public:
  AudioPattern(WiredDocument *parent, double pos, double endpos, long trackindex);
  AudioPattern(WiredDocument *parent, double pos, WaveFile *w, long trackindex);
  ~AudioPattern();

  AudioPattern				operator=(const AudioPattern& right);

 private:
  void					Init(WaveFile *w, WiredDocument* parent);

 public:  
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
  void					Split(double pos);
  void					Merge(Pattern *pattern);
  void					OnDirectEdit();
  inline WaveFile*			GetWaveFile(){return Wave;};
 Pattern				*CreateCopy(double pos);
  
  // WiredDocument implementation
  void					Save();
  void					Load(SaveElementArray data);

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
