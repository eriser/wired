// Copyright (C) 2004 by Wired Team
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
  PAINT_WIDTH = 50000;
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

  printf(" [ START ] WaveEditorDrawer::SetWave(%d)\n", w);

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
    printf(" [ ELSE START ] WaveEditorDrawer::SetWave(%d)\n", w);
      Wave = w;
      Data = w->Data;
      NumberOfChannels = w->GetNumberOfChannels();
      StartWavePos = 0;
      EndWavePos = w->GetNumberOfFrames();
      
    }
}


//void			WaveEditorDrawer::ReadElmtList(float *f, long pos, long xsrc)
//{
//  int					len = xsrc+PAINT_WIDTH;
//  
//  cout << "Debut ReadElmtList"<< endl;
//  list<tRegion>::iterator it;
//  for (it = l.begin(); it != l.end(); it++)
//	  if ((pos >= (*it).start) && (pos <= (*it).end))
//	  {
//		(*it).from->Read(f, pos);
//		break;
//	  }
//	  cout << "fin ReadElmtList"<< endl;
//}
//
//
//void			WaveEditorDrawer::deleteFromList()
//{
//
//  cout << "Debut deleteFromList"<< endl;
//  list<tRegion>::iterator it;
//  for (it = l.begin(); it != l.end(); it++)
//	  if (((*it).start == -1) && ((*it).end == -1))
//		l.pop_front();
//  cout << "fin deleteFromList"<< endl;
//}
//
//void					WaveEditorDrawer::SetTempFile()
//{
//  float					**TempBuf;
//  long					width;
//  int 					frames_nbr = 0;
//  f_count_t 			read_frames;
//  long					inc;
//  
//  
//
//  inc = (EndWavePos / s.x);
//  printf(" [  Debut  ] WaveEditorDrawer::SetTempFFile()\n");
//  list<tRegion>::iterator it;
//  for (it = l.begin(); it != l.end(); it++)
//  {
//	width = (*it).end - (*it).start;
//	
//	// buffer destine a recevoir les donnees a copier
//	float * rw_buffer = new float [(*it).from.GetNumberOfChannels() * width];
//	
//	(*it).from.SetCurrentPosition((*it).pos);
//	read_frames = (*it).from.ReadFloatF(rw_buffer, width);
//	temp.WriteFloatF(rw_buffer, read_frames);
//	frames_nbr += read_frames;
//	
//	delete rw_buffer;
//  }
// 
//  temp->sfinfo.frames = frames_nbr;
//  temp->sfinfo.channels = Wave.GetNumberOfChannels();
//  temp->sfinfo.format = Wave.GetFormat();
//  temp->sfinfo.samplerate = Wave.GetSampleRate();
//  
//  printf(" [  END  ] WaveEditorDrawer::SetTempFFile()\n");
//
//}


void					WaveEditorDrawer::SetDrawing(wxSize s, long xsrc)
{
  int					size_x, size_y;
  long					i, j, k, pos, coeff, inc, n, pos_deb, pos_fin;  
  float					cur, val;
  int					len;
  float					f[NumberOfChannels];


  printf(" [ START ] WaveEditorDrawer::SetDrawing()\n");

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
  DrawData = new long[size_x];
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
	  cout << "etape 3 " << endl;
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

  printf(" [  END  ] WaveEditorDrawer::SetDrawing()\n");

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
  //cout << " WaveEditorDrawer::OnPaint"  << endl ;
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
  
  //cout << " WaveEditorDrawer::OnPaint End"  << endl ; 
}
