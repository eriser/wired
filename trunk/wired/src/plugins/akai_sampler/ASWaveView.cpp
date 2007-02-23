// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "ASWaveView.h"
#include "Settings.h"
#include "ASEnvel.h"

ASWaveView::ASWaveView(wxSize sz) : 
  WaveDrawer(sz, true, false)
{
  env = NULL;
  size = sz;
  DrawData = NULL;
  Wave = NULL;
}

void ASWaveView::SetSize(wxSize sz)
{
  size = sz;
  if (Wave)
    SetDrawing(size);
}

void ASWaveView::SetSample(WaveFile *w)
{
  SetWave(w, size);
}

void ASWaveView::RedrawBitmap(wxSize s)
{
  long					coeff;

  coeff = s.GetHeight() / 2;
  if (Bmp)
    delete Bmp;
  Bmp = new wxBitmap(s.GetWidth(), s.GetHeight());
  memDC.SelectObject(*Bmp);
  memDC.SetPen(PenColor);
  if (!Transparent)
    memDC.SetBrush(BrushColor);
  else
    memDC.SetBrush(*wxTRANSPARENT_BRUSH);
  memDC.DrawRectangle(0, 0, s.GetWidth(), s.GetHeight());
  for (int i = 0; i < s.GetWidth(); i++)
    memDC.DrawLine(i, coeff - DrawData[i], i, coeff + DrawData[i]);
}

float ASWaveView::GetCoef(long pos)
{
  if (!env)
    return 0.0f;
  return 3.0f * env->GetCoef(pos);
}

void ASWaveView::SetDrawing(wxSize s)
{
  int					size_x, size_y;
  long					i, j, k, pos, coeff, inc;  
  float					cur, val;
  int					end;
  float					f[NumberOfChannels];

  size_x = s.x;
  size_y = s.y;
  if (size_x < 2)
    return;
  end = EndWavePos;
  // Coefficient d'amplitude
  coeff = size_y / 2;
  // Création du buffer contenant les datas a dessiner
  if (DrawData)
    delete [] DrawData;
  DrawData = new long[size_x];
  // Coefficient d'incrémentation
  inc = (EndWavePos - StartWavePos) / size_x;
  if (UseSettings && WiredSettings->dbWaveRender)
  {
    if (!Data) // Wave sur disque
    {
      for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
      {
        for (k = 0, cur = 0; (k < inc) && (pos < end); k++, pos++)
        {		
          Wave->Read(f, pos);
          for (j = 0; (j < NumberOfChannels); j++)
            cur += fabsf(f[j] * (1 + GetCoef(pos)));
        }
        val = cur / (NumberOfChannels + inc);
        val = 10 * log10(val);
        // The smallest value we will see is -43.15 (10*log10(1/32768))
        val = (val + 45.f) / 45.f;
        if (val < 0.f)
          val = 0.f;
        else
          if (val > 1.f)
            val = 1.f;
        DrawData[i] = (long)(val * coeff);
      }	  
    }
    else // Wave loadeé en memoire
    {
      for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
      {
        for (k = 0, cur = 0; (k < inc) && (pos < end); k++, pos++)
          for (j = 0; (j < NumberOfChannels); j++)
            cur += fabsf(Data[j][pos] * (1 + GetCoef(pos)));
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
      // Coefficient d'incrémentation
      if (!Data) // Wave sur disque
      {
#define WAVEVIEW_TEMP_BUF_SIZE	4096
        float		**TempBuf;
        long		buf_pos;

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
              cur += fabsf(TempBuf[j][buf_pos] * (1 + GetCoef(pos)));
          }
          DrawData[i] = (long)(((cur / (NumberOfChannels + inc) * coeff) + 0.5));
        }
        delete TempBuf[0];
        delete TempBuf[1];
        delete TempBuf;
      }	  
      else // Wave loadeé en memoire
      {
        for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
        {
          for (k = 0, cur = 0; (k < inc) && (pos < end); k++, pos++)
            for (j = 0; (j < NumberOfChannels); j++)
              cur += fabsf(Data[j][pos] * (1 + GetCoef(pos)));	    
          DrawData[i] = (long)(((cur / (NumberOfChannels + inc) * coeff) + 0.5));
        }      
      }
    }
    else 
    {
      if (!Data) // Wave sur disque
      {
        for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
        {
          Wave->Read(f, pos);
          for (j = 0, cur = 0; (j < NumberOfChannels); j++)
            cur += fabsf(f[j] * (1 + GetCoef(pos)));
          DrawData[i] = (long)((cur / (NumberOfChannels)) * coeff);
          pos += inc;
        }
      }
      else
      {
        for (i = 0, pos = StartWavePos; (i < size_x) && (pos < end); i++)
        {
          for (j = 0, cur = 0; (j < NumberOfChannels); j++)
            cur += fabsf((1 + GetCoef(pos)) * Data[j][pos++]);
          DrawData[i] = (long)((cur / (NumberOfChannels)) * coeff);
          pos += inc;
        }
      }
    }
  RedrawBitmap(s);
}

wxBitmap *ASWaveView::GetBitmap()
{
  return Bmp;
}
