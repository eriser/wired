// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include <iostream>
#include "WaveEditorDrawer.h"
#include "Colour.h"
#include "Settings.h"

using namespace std;


WaveEditorDrawer::WaveEditorDrawer(const wxSize& s, bool fulldraw, bool use_settings) 
  : FullDraw(fulldraw), UseSettings(use_settings)
{
  PAINT_WIDTH = 800000;
  zoomx = PAINT_WIDTH;
  zoomy = 1;
  if (!s.x)
    size.x = 1;
  size.y = s.y;
  Data = 0;
  DrawData = 0;
  NumberOfChannels = 0;  
  StartWavePos = 0;
  EndWavePos = 0;
  Bmp = 0;
  PenColor = CL_WAVE_DRAW;
  BrushColor = CL_WAVEDRAWER_BRUSH;
}

WaveEditorDrawer::~WaveEditorDrawer()
{
  if (DrawData)
    delete [] DrawData;
  if (Bmp)
    delete Bmp;
}

void					WaveEditorDrawer::SetWave(float **data, unsigned long frame_length, long channel_count, wxSize s)
{
  
  Data = data;
  StartWavePos = 0;
  EndWavePos = frame_length;
  NumberOfChannels = channel_count;
}

void					WaveEditorDrawer::SetWave(WaveFile *w, wxSize s)
{

  if (!w)
    {
      Wave = 0;
      Data = 0;
      NumberOfChannels = 0;
      StartWavePos = 0;
      EndWavePos = 0;
    }
  else
    {
      Wave = w;
      Data = w->Data;
      NumberOfChannels = w->GetNumberOfChannels();
      StartWavePos = 0;
      EndWavePos = w->GetNumberOfFrames(); 
    }
}



void					WaveEditorDrawer::SetDrawing(wxSize s, long xsrc)
{
  int					size_x, size_y;
  long					i, j, k, pos, coeff, inc, n, pos_deb, pos_fin;  
  float					cur, val;
  int					len;
  float					f[NumberOfChannels];

  size_x = s.x;
  size_y = s.y;
  if (size_x < 2)
    return;
      
  if ((PAINT_WIDTH) <= EndWavePos)
	len = (PAINT_WIDTH);
  else
	len = EndWavePos;
  
  // Coefficient d'amplitude
  coeff = (size_y / 2);
  // Coefficient d'incrémentation
  inc = (long) ceill(len / size_x);  
  
  if (inc == 0) inc = 1;
  
  if ( EndWavePos > PAINT_WIDTH) 
	pos_deb =((xsrc*inc) < (EndWavePos - PAINT_WIDTH))? xsrc*inc : (EndWavePos - PAINT_WIDTH);
  else
	pos_deb = 0;
 
  pos_fin = (len + (xsrc*inc));
  
  if (DrawData)
    delete [] DrawData;
  DrawData = new long[s.x];

  if (UseSettings && WiredSettings->dbWaveRender)
    {
      if (!Data) // Wave sur disque
	{
	  for (i = 0, pos = pos_deb; (i < size_x) && (pos < pos_fin); i++)
	    {
	      for (k = 0, cur = 0; (k < inc) && (pos < pos_fin); k++, pos++)
		  {		
			Wave->Read(f, pos);
			for (j = 0; (j < NumberOfChannels); j++)
			  cur += fabsf(f[j]);
		  }
	      val = cur / (NumberOfChannels + inc);
	      val = 10 * log10(val);
	      // The smallest value we will see is -45.15 (10*log10(1/32768))
	      val = (val + 45.f) / 45.f;
	      if (val < 0.f)
			val = 0.f;
	      else if (val > 1.f)
			val = 1.f;
	      DrawData[i] = (long)(val * coeff);
	    }	  
	}
      else // Wave loadeé en memoire
	{
	  for (i = 0, pos = pos_deb; (i < size_x) && (pos < pos_fin); i++)
	    {
	      for (k = 0, cur = 0; (k < inc) && (pos < pos_fin); k++, pos++)
		for (j = 0; (j < NumberOfChannels); j++)
		  cur += fabsf(Data[j][pos]);
	      val = cur / (NumberOfChannels + inc);
	      val = 10 * log10(val);
	      // The smallest value we will see is -45.15 (10*log10(1/32768))
	      val = (val + 45.f) / 45.f;
	      if (val < 0.f)
		val = 0.f;
	      else if (val > 1.f)
		val = 1.f;
	      DrawData[i] = (long)(val * coeff);
	    }
	}
    }
  else
    if (!UseSettings || !WiredSettings->QuickWaveRender)
      {
	if (!Data) // Wave sur disque
	  {
	   #define WAVEVIEW_TEMP_BUF_SIZE	4096
	    float		**TempBuf;
	    long		buf_pos;

	    TempBuf = new float *[2];
	    TempBuf[0] = new float[WAVEVIEW_TEMP_BUF_SIZE];
	    TempBuf[1] = new float[WAVEVIEW_TEMP_BUF_SIZE];   	
	    for (i = 0, pos = pos_deb; (i < size_x) && (pos < pos_fin); i++)
	      {
			for (k = 0, buf_pos = 0, cur = 0; (k < inc) && (pos < pos_fin); k++, pos++, buf_pos++)
			  {		
				if ((buf_pos % WAVEVIEW_TEMP_BUF_SIZE) == 0)
				{
				  Wave->Read(TempBuf, pos, WAVEVIEW_TEMP_BUF_SIZE);
				  buf_pos = 0;
				}
				for (j = 0; (j < NumberOfChannels); j++)
				{
				  cur += fabsf(TempBuf[j][buf_pos]);
				}
			  }
			DrawData[i] = (long)(((cur / (NumberOfChannels + inc) * coeff) + 0.5));
	      }
	    delete TempBuf[0];
	    delete TempBuf[1];
	    delete TempBuf;
	  }	  
	else // Wave loadeé en memoire
	  {
	  
	    for (i = 0, pos = pos_deb; (i < size_x) && (pos < pos_fin); i++)
	      {
	      
			for (k = 0, cur = 0; (k < inc) && (pos < pos_fin); k++, pos++)
			  for (j = 0; (j < NumberOfChannels); j++)
				cur += fabsf(Data[j][pos]);	    
			DrawData[i] = (long)(((cur / (NumberOfChannels + inc) * coeff) + 0.5));
	
	      }      
	  }
      }
    else 
      {
	//inc = (long) ceill(len / size_x);
	if (!Data) // Wave sur disque
	  {
	    for (i = 0, pos = pos_deb; (i < size_x) && (pos < pos_fin); i++)
	      {
		Wave->Read(f, pos);
		for (j = 0, cur = 0; (j < NumberOfChannels); j++)
		  cur += fabsf(f[j]);
		DrawData[i] = (long)((cur / (NumberOfChannels)) * coeff);
		pos += inc;
	      }
	  }
	else
	  {
	    for (i = 0, pos = pos_deb; (i < size_x) && (pos < pos_fin); i++)
	      {
	      for (j = 0, cur = 0; (j < NumberOfChannels); j++)
		cur += fabsf(Data[j][pos++]);
	      DrawData[i] = (long)((cur / (NumberOfChannels)) * coeff);
	      pos += inc;
	      }
	  }
      }
  RedrawBitmap(s);
}


void					WaveEditorDrawer::RedrawBitmap(wxSize s)
{
  long					coeff;

  coeff = (s.y / 2);
  // Création de la bitmap
  if (Bmp)
    delete Bmp;
  Bmp = new wxBitmap(s.x, s.y);
  memDC.SelectObject(*Bmp);
  memDC.SetPen(PenColor);
  memDC.SetBrush(BrushColor);
  memDC.DrawRectangle(0, 0, s.x, s.y);
  for (int i = 0; i < s.x; i++)
    memDC.DrawLine(i, coeff - DrawData[i], i, coeff + DrawData[i]);
}

void					WaveEditorDrawer::SetSize(wxSize s)
{
#ifdef __DEBUG__
  printf("SetSize [W = %d] [H = %d]\n", s.x, s.y);
#endif
}

void					WaveEditorDrawer::SetSize(int x, int y)
{
  wxSize				s(x, y);
  SetSize(s);
#ifdef __DEBUG__
  printf("WaveEditorDrawer::SETSIZE (NOT TO CALL)\n");
#endif
}


void					WaveEditorDrawer::OnPaint(wxPaintDC &dc, wxSize s, wxRegionIterator &region)
{
   dc.SetPen(PenColor);
  dc.SetBrush(BrushColor);
  //dc.SetBrush(*wxTRANSPARENT_BRUSH);
  dc.DrawRectangle(0, 0, s.x, s.y);
  if ((Data || (Wave && !Wave->LoadedInMem)) && Bmp)
	  for(; region; region++)
		dc.Blit(region.GetX(), region.GetY(),
		    region.GetW(), region.GetH(),
			&memDC, region.GetX(), region.GetY(), 
		  wxCOPY , FALSE);  
}
