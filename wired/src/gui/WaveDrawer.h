// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __WAVE_DRAWER_H__
#define __WAVE_DRAWER_H__

#include <wx/wx.h>
#include "WaveFile.h"

class				WaveDrawer
{
 protected:
  long				NumberOfChannels;
  float				**Data;
  long				*DrawData;
  wxSize			size;
  wxBitmap			*Bmp;
  wxMemoryDC			memDC;
  bool				FullDraw;
  bool				UseSettings;

  long				StartWavePos;
  long				EndWavePos;
  wxColour			PenColor;
  wxColour			BrushColor;
  WaveFile			*Wave;   

  virtual void			OnPaint(wxPaintDC &dc, wxSize s, wxRegionIterator &region);
  
 public:
  WaveDrawer(const wxSize& s, bool fulldraw = false, bool use_settings = true);
  virtual ~WaveDrawer();
  
  virtual void			SetDrawing(wxSize s);
  virtual void			SetWave(float **data, unsigned long frame_length, long channel_count, wxSize s);
  virtual void			SetWave(WaveFile *w, wxSize s);
  virtual void			SetSize(wxSize s);
  virtual void			SetSize(int x, int y);
  void				SetPenColor(wxColour p) { PenColor = p; }
  wxColour			GetPenColor() { return (PenColor); }
  void				SetBrushColor(wxColour b) { BrushColor = b; }
  void				SetColors(wxColour p, wxColour b) { PenColor = p; BrushColor = b; }
  void				SetWaveFile(WaveFile *wav) { Wave = wav; }
  void				SetStartWavePos(long p) { StartWavePos = p; }
  long				GetStartWavePos() { return (StartWavePos); }
  void				SetEndWavePos(long p) { EndWavePos = p; }
  long				GetEndWavePos() { return (EndWavePos); }
  WaveFile			*GetWaveFile() { return (Wave); }
};

#endif/*__WAVE_DRAWER_H__*/
