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
  BrushColor = CL_SEQ_BACKGROUND;
}

WaveDrawer::~WaveDrawer()
{
  if (DrawData)
    delete [] DrawData;
  if (Bmp)
    delete Bmp;
}

void					WaveDrawer::SetWave(float **data, unsigned long frame_length, long channel_count, wxSize s)
{
  //printf("WaveDrawer::SetWave(%d, %d, %d) -- START\n", data, frame_length, channel_count);
  Data = data;
  StartWavePos = 0;
  EndWavePos = frame_length;
  NumberOfChannels = channel_count;
  SetDrawing(s);
  //printf("WaveDrawer::SetWave(%d, %d, %d) -- OVER\n", data, frame_length, channel_count);  
}

void					WaveDrawer::SetWave(WaveFile *w, wxSize s)
{
#ifdef __DEBUG__
  printf(" [ START ] WaveDrawer::SetWave(%d)\n", w);
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

void					WaveDrawer::SetDrawing(wxSize s)
{
  int					size_x, size_y;
  long					i, j, k, pos, coeff, inc;  
  float					cur, val;
  int					len;
  float					f[NumberOfChannels];

#ifdef __DEBUG__
  printf(" [ START ] WaveDrawer::SetDrawing()\n");
#endif
  /*  if (win)
    win->GetSize(&size_x, &size_y);
    else*/
  //  GetSize(&size_x, &size_y);
  size_x = s.x;
  size_y = s.y;
  //  printf("GRRRRRRRRRRRR [W = %d] [H = %d]\n", size_x, size_y);
  /*  size_x = size.x;
      size_y = size.y;*/
  if (size_x < 2)
    return;
  len = EndWavePos;// * NumberOfChannels;
  // Coefficient d'amplitude
  coeff = size_y / 2;
  // Cr�ation du buffer contenant les datas a dessiner
  //printf("WaveDrawer::SetDrawing() - STEP 1\n");
  if (DrawData)
    delete [] DrawData;
  DrawData = new long[size_x];
  //printf("WaveDrawer::SetDrawing() - STEP 2\n");

  if (UseSettings && WiredSettings->dbWaveRender)
    {
      // Coefficient d'incr�mentation
      inc = (EndWavePos / size_x);  
      //printf("WaveDrawer::SetDrawing() - STEP 3\n");

      if (!Data) // Wave sur disque
	{
	  //printf("WaveDrawer::SetDrawing() - STEP 4\n");

	  for (i = 0, pos = StartWavePos; (i < size_x) && (pos < len); i++)
	    {
	      for (k = 0, cur = 0; (k < inc) && (pos < len); k++, pos++)
		{		
		  Wave->Read(f, pos);
		  for (j = 0; (j < NumberOfChannels); j++)
		    cur += fabsf(f[j]);
		}
	      //printf("WaveDrawer::SetDrawing() - STEP 5\n");

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
      else // Wave loade� en memoire
	{
	  //printf("WaveDrawer::SetDrawing() - STEP 13\n");

	for (i = 0, pos = StartWavePos; (i < size_x) && (pos < len); i++)
	  {
	    for (k = 0, cur = 0; (k < inc) && (pos < len); k++, pos++)
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
	//printf("WaveDrawer::SetDrawing() - STEP 14\n");
	
	}
    }
  else if (!UseSettings || !WiredSettings->QuickWaveRender)
    {
      // Coefficient d'incr�mentation
      inc = (EndWavePos / size_x);  
      //printf("WaveDrawer::SetDrawing() - STEP 21\n");

      if (!Data) // Wave sur disque
	{
#define WAVEVIEW_TEMP_BUF_SIZE	4096
	  float **TempBuf;
	  long  buf_pos;
	  TempBuf = new float *[2];
	  TempBuf[0] = new float[WAVEVIEW_TEMP_BUF_SIZE];
	  TempBuf[1] = new float[WAVEVIEW_TEMP_BUF_SIZE];      	

	  for (i = 0, pos = StartWavePos; (i < size_x) && (pos < len); i++)
	    {
	      for (k = 0, buf_pos = 0, cur = 0; (k < inc) && (pos < len); k++, pos++, buf_pos++)
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
	  delete TempBuf[0];
	  delete TempBuf[1];
	  delete TempBuf;
	}	  
      else // Wave loade� en memoire
	{
	  //printf("WaveDrawer::SetDrawing() - STEP 23\n");

	for (i = 0, pos = StartWavePos; (i < size_x) && (pos < len); i++)
	  {
	    for (k = 0, cur = 0; (k < inc) && (pos < len); k++, pos++)
	      for (j = 0; (j < NumberOfChannels); j++)
		cur += fabsf(Data[j][pos]);	    
	    DrawData[i] = (long)(((cur / (NumberOfChannels + inc) * coeff) + 0.5));
	  }      
	//printf("WaveDrawer::SetDrawing() - STEP 24\n");

	}
    }
  else 
    {
      inc = (EndWavePos / size_x);
      //printf("WaveDrawer::SetDrawing() - STEP 31\n");

      if (!Data) // Wave sur disque
	{
	  //printf("WaveDrawer::SetDrawing() - STEP 32\n");

	  for (i = 0, pos = StartWavePos; (i < size_x) && (pos < len); i++)
	    {
	      Wave->Read(f, pos);
	      for (j = 0, cur = 0; (j < NumberOfChannels); j++)
		cur += fabsf(f[j]);
	      DrawData[i] = (long)((cur / (NumberOfChannels)) * coeff);
	      pos += inc;
	    }
	  //printf("WaveDrawer::SetDrawing() - STEP 33\n");

	}
      else
	{
	  //printf("WaveDrawer::SetDrawing() - STEP 34\n");

	for (i = 0, pos = StartWavePos; (i < size_x) && (pos < len); i++)
	  {
	    for (j = 0, cur = 0; (j < NumberOfChannels); j++)
	      cur += fabsf(Data[j][pos++]);
	    DrawData[i] = (long)((cur / (NumberOfChannels)) * coeff);
	    pos += inc;
	  }
	//printf("WaveDrawer::SetDrawing() - STEP 35\n");
	}

    }
  //printf("WaveDrawer::SetDrawing() - STEP 42\n");

  //cout << "[WAVEVIEW] Position after parse: " << i << endl;
  // Cr�ation de la bitmap
  if (Bmp)
    delete Bmp;
  Bmp = new wxBitmap(size_x, size_y);
  memDC.SelectObject(*Bmp);

  memDC.SetPen(PenColor);
  memDC.SetBrush(BrushColor);
  memDC.DrawRectangle(0, 0, size_x, size_y);
  //printf("WaveDrawer::SetDrawing() - STEP 51\n");
  for (i = 0; i < size_x; i++)
    memDC.DrawLine(i, coeff - DrawData[i], i, coeff + DrawData[i]);
  //printf("WaveDrawer::SetDrawing() - STEP 69\n");

  //  memDC.SetPen(*wxLIGHT_GREY_PEN);
  //  memDC.DrawLine(0, coeff, size_x, coeff);
#ifdef __DEBUG__
  printf(" [  END  ] WaveDrawer::SetDrawing()\n");
#endif
}

void					WaveDrawer::SetSize(wxSize s)
{
#ifdef __DEBUG__
  printf("GRRRRR 42 RRRRRRRRR [W = %d] [H = %d]\n", s.x, s.y);
#endif
  /*  if (s == win->GetSize())
    return;
  win->SetSize(s);
  if (Data || (Wave && !Wave->LoadedInMem))
    {
      SetDrawing();
      win->Refresh();
    }
  */
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
  if ((Data || (Wave && !Wave->LoadedInMem)) && Bmp)
    {
      for(; region; region++)
	dc.Blit(region.GetX(), region.GetY(),
		region.GetW(), region.GetH(),
		&memDC, region.GetX(), region.GetY(), 
		wxCOPY, FALSE);      
      dc.SetBrush(*wxTRANSPARENT_BRUSH);
    }
  else
    dc.SetBrush(BrushColor);
  dc.DrawRectangle(0, 0, s.x, s.y);
}
