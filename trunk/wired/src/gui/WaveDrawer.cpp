// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include <iostream>
#include "WaveDrawer.h"
#include "Colour.h"
#include "Settings.h"

using namespace std;

WaveDrawer::WaveDrawer(const wxSize& s, bool fulldraw, bool use_settings) 
  : FullDraw(fulldraw), UseSettings(use_settings)
{
	cout << "[WAVEDRAWER] [NEW] 0" << endl;
  if (!s.x)
  {
  		cout << "[WAVEDRAWER] [NEW] 1" << endl;
    size.x = 1;
  }
  	cout << "[WAVEDRAWER] [NEW] 2" << endl;
  size.y = s.y;
  	cout << "[WAVEDRAWER] [NEW] 3" << endl;
  Data = 0;
  	cout << "[WAVEDRAWER] [NEW] 4" << endl;
  DrawData = 0;
  	cout << "[WAVEDRAWER] [NEW] 5" << endl;
  NumberOfChannels = 0;  
  	cout << "[WAVEDRAWER] [NEW] 6" << endl;
  StartWavePos = 0;
  	cout << "[WAVEDRAWER] [NEW] 7" << endl;
  EndWavePos = 0;
  	cout << "[WAVEDRAWER] [NEW] 8" << endl;
  Bmp = 0;
  	cout << "[WAVEDRAWER] [NEW] 9" << endl;
  PenColor = CL_WAVE_DRAW;
  	cout << "[WAVEDRAWER] [NEW] 10" << endl;
  BrushColor = CL_WAVEDRAWER_BRUSH;
  	cout << "[WAVEDRAWER] [NEW] 11" << endl;
  Transparent = false;
  	cout << "[WAVEDRAWER] [NEW] 12" << endl;
}

WaveDrawer::~WaveDrawer()
{
  if (DrawData) delete [] DrawData;
  if (Bmp) delete Bmp;
  while (NumberOfChannels--)
  	if (Data[NumberOfChannels]) delete Data[NumberOfChannels];
 	if (Data) delete Data;
  if (Wave) delete Wave;
}

void					WaveDrawer::SetWave(float **data, unsigned long frame_length, long channel_count, wxSize s)
{
#ifdef __DEBUG__
  printf(" [ START ] WaveDrawer::SetWave(%x, %d, %d) -- START\n", data, frame_length, channel_count);
#endif
  Data = data;
  StartWavePos = 0;
  EndWavePos = frame_length;
  NumberOfChannels = channel_count;
  SetDrawing(s);
#ifdef __DEBUG__
  printf(" [ START ] WaveDrawer::SetWave(%x, %d, %d) -- OVER\n", data, frame_length, channel_count);
#endif
}

void					WaveDrawer::SetWave(WaveFile *w, wxSize s)
{
#ifdef __DEBUG__
  printf(" [ START ] WaveDrawer::SetWave(%x, sise x %d y %d)\n", w, s.x, s.y);
#endif
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
  SetDrawing(s);
#ifdef __DEBUG__
  printf(" [  END  ] WaveDrawer::SetWave(%d)\n", w);
#endif
}

void					WaveDrawer::SetWave(WaveFile *w, wxSize s, long wstart, long wend)
{
#ifdef __DEBUG__
  printf(" [ START ] WaveDrawer::SetWave(%x, size, wstart = %d, wend = %d)\n", w, wstart, wend);
#endif
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
      StartWavePos = wstart;
      EndWavePos = (wend <= w->GetNumberOfFrames()) ? wend : w->GetNumberOfFrames();
    }
  SetDrawing(s);
#ifdef __DEBUG__
  printf(" [  END  ] WaveDrawer::SetWave(%x)\n", w);
#endif
}

void					WaveDrawer::SetDrawing(wxSize s)
{
  int					size_x, size_y;
  long					i, j, k, pos, coeff, inc;  
  float					cur, val;
  int					end;
  float					f[NumberOfChannels];

  printf(" [ START ] WaveDrawer::SetDrawing(size x %d y %d)\n", s.x, s.y);
  printf(" >>> HERE : StartWavePos %d, EndWavePos %d\n", StartWavePos, EndWavePos);
  size_x = s.x;
  size_y = s.y;
  if (size_x < 2)
    return;
  end = EndWavePos;
  // Coefficient d'amplitude
  coeff = size_y / 2;
  // Cr?ation du buffer contenant les datas a dessiner
  if (DrawData)
  {
  	cout << "[WAVEDRAWER] [SETDRAWING] 0" << endl;
    delete [] DrawData;
  }
  cout << "[WAVEDRAWER] [SETDRAWING] 1" << endl;
  DrawData = new long[size_x];
    	cout << "[WAVEDRAWER] [SETDRAWING] 2" << endl;
  // Coefficient d'incr?mentation
  inc = (EndWavePos - StartWavePos) / size_x;
    	cout << "[WAVEDRAWER] [SETDRAWING] 3" << endl;
  if (UseSettings && WiredSettings->dbWaveRender)
    {
      if (!Data) // Wave sur disque
	{
		  	cout << "[WAVEDRAWER] [SETDRAWING] 4" << endl;
	  for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
	    {
	      for (k = 0, cur = 0; (k < inc) && (pos < end); k++, pos++)
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
	      else
		if (val > 1.f)
		  val = 1.f;
	      DrawData[i] = (long)(val * coeff);
	    }	  
	}
      else // Wave loade? en memoire
	{
		  	cout << "[WAVEDRAWER] [SETDRAWING] 5" << endl;
	  for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
	    {
	      for (k = 0, cur = 0; (k < inc) && (pos < end); k++, pos++)
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
	// Coefficient d'incr?mentation
	if (!Data) // Wave sur disque
	  {
#define WAVEVIEW_TEMP_BUF_SIZE	4096
	    float		**TempBuf;
	    long		buf_pos;
	    
	      	cout << "[WAVEDRAWER] [SETDRAWING] 6" << endl;
	    TempBuf = new float *[2];
	    TempBuf[0] = new float[WAVEVIEW_TEMP_BUF_SIZE];
	    TempBuf[1] = new float[WAVEVIEW_TEMP_BUF_SIZE];      	
	    for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
	      {
		for (k = 0, buf_pos = 0, cur = 0; (k < inc) && (pos < end); k++, pos++, buf_pos++)
		  {		
		    if (!(buf_pos % WAVEVIEW_TEMP_BUF_SIZE))
		      {
			Wave->Read(TempBuf, pos, WAVEVIEW_TEMP_BUF_SIZE);
			buf_pos = 0;
		      }
		    for (j = 0; (j < NumberOfChannels); j++)
		      cur += fabsf(TempBuf[j][buf_pos]);
		  }
		DrawData[i] = (long)(((cur / (NumberOfChannels + inc) * coeff) + 0.5));
	      }
	        	cout << "[WAVEDRAWER] [SETDRAWING] 7" << endl;
	    if (TempBuf[0])
		    delete TempBuf[0];
	    	        	cout << "[WAVEDRAWER] [SETDRAWING] 70" << endl;
      	if (TempBuf[1])
		    delete TempBuf[1];
	    	        	cout << "[WAVEDRAWER] [SETDRAWING] 71" << endl;
	    if (TempBuf)
		    delete TempBuf;
	    	        	cout << "[WAVEDRAWER] [SETDRAWING] 72" << endl;
	  }	  
	else // Wave loade? en memoire
	  {
	  	  	cout << "[WAVEDRAWER] [SETDRAWING] 8" << endl;
	    for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
	      {
		for (k = 0, cur = 0; (k < inc) && (pos < end); k++, pos++)
		  for (j = 0; (j < NumberOfChannels); j++)
		    cur += fabsf(Data[j][pos]);	    
		DrawData[i] = (long)(((cur / (NumberOfChannels + inc) * coeff) + 0.5));
	      }      
	  }
      }
    else 
      {
	if (!Data) // Wave sur disque
	  {
	  	  	cout << "[WAVEDRAWER] [SETDRAWING] 9" << endl;
	    for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
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
	  	  	cout << "[WAVEDRAWER] [SETDRAWING] 10" << endl;
	    for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
	      {
		for (j = 0, cur = 0; (j < NumberOfChannels); j++)
		  cur += fabsf(Data[j][pos++]);
		DrawData[i] = (long)((cur / (NumberOfChannels)) * coeff);
		pos += inc;
	      }
	  }
      }
      cout << "[WAVEDRAWER] [SETDRAWING] 11 s.x: " << s.x << " ; s.y: " << s.y << endl;
  RedrawBitmap(s);
  cout << "[WAVEDRAWER] [SETDRAWING] 12" << endl;
  printf(" [  END  ] WaveDrawer::SetDrawing()\n");
}

void					WaveDrawer::RedrawBitmap(wxSize s)
{
  long					coeff;

cout << "[WAVEDRAWER] [RedrawBitmap] 00 ; " << endl;
cout << "[WAVEDRAWER] [RedrawBitmap] s.x: " << s.x << " s.y: " << s.y << endl;
  coeff = s.y / 2;
  cout << "[WAVEDRAWER] [RedrawBitmap] 01 ; " << endl;
  // Cr?ation de la bitmap
  if (Bmp)
  {
  	cout << "[WAVEDRAWER] [RedrawBitmap] 000" << endl;
    delete Bmp;
  }
  cout << "[WAVEDRAWER] [RedrawBitmap] 02" << endl;
  Bmp = new wxBitmap(5000, s.y);
  cout << "[WAVEDRAWER] [RedrawBitmap] 03" << endl;
  memDC.SelectObject(*Bmp);
  cout << "[WAVEDRAWER] [RedrawBitmap] 1" << endl;
  memDC.SetPen(PenColor);
  cout << "[WAVEDRAWER] [RedrawBitmap] 2" << endl;
  memDC.SetBrush((!Transparent) ? BrushColor : *wxTRANSPARENT_BRUSH);
  cout << "[WAVEDRAWER] [RedrawBitmap] 3" << endl;
  memDC.DrawRectangle(0, 0, s.x, s.y);
  cout << "[WAVEDRAWER] [RedrawBitmap] 4" << endl;
  if (s.x > 2)
    for (int i = 0; i < s.x; i++)
      memDC.DrawLine(i, coeff - DrawData[i], i, coeff + DrawData[i]);
  cout << "[WAVEDRAWER] [RedrawBitmap] 5" << endl;
}

void					WaveDrawer::SetSize(wxSize s)
{
#ifdef __DEBUG__
  printf("SetSize [W = %d] [H = %d]\n", s.x, s.y);
#endif
}

void					WaveDrawer::SetSize(int x, int y)
{
  wxSize				s(x, y);
  SetSize(s);
#ifdef __DEBUG__
  printf("WAVEDRAWER::SETSIZE (NOT TO CALL)\n");
#endif
}

void					WaveDrawer::OnPaint(wxPaintDC &dc, wxSize s, wxRegionIterator &region)
{
  dc.SetPen(PenColor);
  dc.SetBrush((!Transparent) ? BrushColor : *wxTRANSPARENT_BRUSH);
  dc.DrawRectangle(0, 0, s.x, s.y);
  if ((Data || (Wave && !Wave->LoadedInMem)) && Bmp)
    for(; region; region++)
      dc.Blit(region.GetX(), region.GetY(),
	      region.GetW(), region.GetH(),
	      &memDC, region.GetX(), region.GetY(), 
	      wxCOPY, FALSE);      
}

WaveDrawer			WaveDrawer::operator=(const WaveDrawer& right)
{
	if (this != &right)
	{
		NumberOfChannels = right.NumberOfChannels;
		Data = right.Data;
		DrawData = right.DrawData;
		size = right.size;
		Bmp = right.Bmp;
		//TODO memDC = right.memDC;
		FullDraw = right.FullDraw;
		UseSettings = right.UseSettings;
		Transparent = right.Transparent;
		StartWavePos = right.StartWavePos;
		EndWavePos = right.EndWavePos;
		PenColor = right.PenColor;
		BrushColor = right.BrushColor;
		Wave = right.Wave;
	}
	return *this;
}
