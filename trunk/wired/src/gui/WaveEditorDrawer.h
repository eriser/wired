// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __WAVE_EDITOR_DRAWER_H__
#define __WAVE_EDITOR_DRAWER_H__

#include <wx/wx.h>
#include "WaveFile.h"


//#define				PAINT_WIDTH	(100000)

#include 			<list>


using namespace std;

typedef struct 
{
   long pos;
   long length;
   WaveFile *from;
} tCopy;

typedef struct 
{
   long start;
   long end;
   long pos;
   WaveFile *from;
} tRegion;

class Greater
{
public:
   bool operator() (tRegion elem1, tRegion elem2)
   {
      if (elem1.start < elem2.start)
         return true;
      return false;
   }
};

class				WaveEditorDrawer
{
 protected:
  long				NumberOfChannels;
  float				**Data;
  long				*DrawData;
  wxSize			size;
  wxBitmap			*Bmp;
  wxMemoryDC		memDC;
  bool				FullDraw;
  bool				UseSettings;

  long				StartWavePos;
  long				EndWavePos;
  wxColour			PenColor;
  wxColour			BrushColor;
  WaveFile			*Wave; 
  long				swidth;
  long				thumbwidth;
  
 

  
  void				OnPaint(wxPaintDC &dc, wxSize s, wxRegionIterator &region);
  
 public:
  WaveEditorDrawer(const wxSize& s, bool fulldraw = false, bool use_settings = true);
  virtual ~WaveEditorDrawer();
  
  virtual void		SetDrawing(wxSize s, long xsrc);
  void				RedrawBitmap(wxSize s);
  virtual void		SetWave(float **data, unsigned long frame_length, long channel_count, wxSize s);
  virtual void		SetWave(WaveFile *w, wxSize s);
  virtual void		SetSize(wxSize s);
  virtual void		SetSize(int x, int y);
//  void				ReadElmtList(float *f,long pos, long xsrc);
//  void				deleteFromList();
//  void				SetTempFile();
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
  
  
  list<tRegion> 	l;
  tCopy				clipboard;
  long				PAINT_WIDTH;
  long				zoomx;
  long				zoomy;
};





#endif/*__WAVE_EDITOR_DRAWER_H__*/
