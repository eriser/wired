// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include <math.h>
#include <wx/filename.h>
#include "SequencerGui.h"
#include "Sequencer.h"
#include "Colour.h"
#include "OptionPanel.h"
#include "Mixer.h"
#include "AudioCenter.h"
#include "WiredSession.h"
#include "ColoredBox.h"
#include "AudioPattern.h"
#include "HelpPanel.h"
#include "../sequencer/Track.h"
#include "../gui/SeqTrack.h"

static long		audio_pattern_count = 1;
extern WiredSession	*CurrentSession;

AudioPattern::AudioPattern(double pos, double endpos, long trackindex)
  : Pattern(pos, endpos, trackindex),
    WaveDrawer(Pattern::GetSize())
{
  Init();
  Wave = 0;
  SetWave(0);
}

AudioPattern::AudioPattern(double pos, WaveFile *w, long trackindex)
  : Pattern(pos, pos + Seq->MeasurePerSample * w->GetNumberOfFrames(), trackindex),
    WaveDrawer(Pattern::GetSize())
{
#ifdef __DEBUG__
  cout << " ### NEW AUDIO PATTERN ###\n\t Position: "<< Position << "; EndPosition: " << EndPosition << "; Length: " << Length
       << "; StartWavePos: " << StartWavePos << "; EndWavePos: " << EndWavePos << endl;
#endif
  Init();
  Wave = w;
  wxSize s = GetSize();
  SetSize(s);
  WaveDrawer::SetWave(w, s);
  FileName = w->Filename;
#ifdef __DEBUG__
  cout << " ---  OK AUDIO PATTERN ---\n\t Position: "<< Position << "; EndPosition: " << EndPosition << "; Length: " << Length
       << "; StartWavePos: " << StartWavePos << "; EndWavePos: " << EndWavePos << endl;
#endif
}

AudioPattern::~AudioPattern()
{
  
}

void				AudioPattern::Init()
{
  wxString			s;

  Pattern::PenColor = CL_PATTERN_NORM;
  s.Printf("T%d A%d", TrackIndex + 1, audio_pattern_count++);
  Name = s.c_str();
  LastBlock = -1;  
  RecordWave = 0;
  Connect(GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnMotion);
  Connect(GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnClick);
  Connect(GetId(), wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnRightClick);
  Connect(GetId(), wxEVT_LEFT_DCLICK, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnDoubleClick);
  Connect(GetId(), wxEVT_PAINT, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnPaint);
  Connect(GetId(), wxEVT_SIZE, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnSize);
  Connect(GetId(), wxEVT_ENTER_WINDOW, (wxObjectEventFunction)(wxEventFunction)
	  (wxMouseEventFunction) &AudioPattern::OnHelp);
  SeqPanel->PutCursorsOnTop();
}

void				AudioPattern::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("This is an Audio pattern. Double-click on it to open the Audio editor.");
      HelpWin->SetText(s);
    }
}

void				AudioPattern::Update()
{
#ifdef __DEBUG__
  printf(" [ START ] AudioPattern::Update()\n");
#endif
  Pattern::Update();
  AudioPattern::SetSize(Pattern::GetSize());
  //Pattern::SetMPosition(Pattern::GetMPosition());
  wxWindow::SetPosition(Pattern::GetMPosition());
#ifdef __DEBUG__
  printf(" [  END  ] AudioPattern::Update()\n");
#endif
}

void				AudioPattern::OnBpmChange()
{
  Length = Seq->MeasurePerSample * Wave->GetNumberOfFrames();
  EndPosition = Position + Length;
  Update();
}

void				AudioPattern::SetWave(WaveFile *w)
{
#ifdef __DEBUG__
  cout << "WaveDrawer::StartWavePos = " << WaveDrawer::StartWavePos<< " WaveDrawer::EndWavePos = " << WaveDrawer::EndWavePos << endl;
#endif
  WaveDrawer::SetWave(w, GetSize(), StartWavePos, EndWavePos);
}

void				AudioPattern::SetDrawing()
{
  WaveDrawer::SetPenColor(Pattern::PenColor);
  WaveDrawer::SetBrushColor(Pattern::BrushColor);
  WaveDrawer::SetDrawing(GetSize());
  Refresh();
}

float				**AudioPattern::GetBlock(long block)
{
  float				**buf;
  long				size;
  long				pos;

  if (!Wave)
    return (0x0);
  pos = (block * Audio->SamplesPerBuffer) + StartWavePos;
  size = EndWavePos - pos;
  if (size > 0)
    {
      buf = new float *[2];
      if (size > Audio->SamplesPerBuffer)
	size = Audio->SamplesPerBuffer; 
      buf[0] = new float[Audio->SamplesPerBuffer];
      buf[1] = new float[Audio->SamplesPerBuffer];	
      Wave->Read(buf, pos, size);
      return (buf);
    }
  else
    return (0x0);
}

void				AudioPattern::SetSelected(bool sel)
{
  Pattern::SetSelected(sel);
  if (sel)
    WaveDrawer::BrushColor = CL_WAVEDRAWER_BRUSH_SEL;
  else
    WaveDrawer::BrushColor = CL_WAVEDRAWER_BRUSH;
  WaveDrawer::PenColor = Pattern::PenColor;
  WaveDrawer::RedrawBitmap(GetSize());
  Refresh();
}

bool				AudioPattern::PrepareRecord(int type)
{
  wxString			s;
  bool				done = false;
  int				i = 1;

  cout << "Preparing record for pattern " << this 
       << " with audio dir : " << CurrentSession->AudioDir << endl;
  while (!done)
    {
      s.Printf("%s/wired_audio%d.wav", CurrentSession->AudioDir.c_str(), i);
      wxFileName f(s);
      if (!f.FileExists())
	done = true;
      else
	i++;
    }
  if (!s.IsEmpty())
    {
      try
	{
	  RecordWave = new WriteWaveFile(s.c_str(), (int)Audio->SampleRate, 1, type);
	  FileName = s;
	  InputChan = Mix->OpenInput(Seq->Tracks[TrackIndex]->TrackOpt->DeviceId);
	  Mix->FlushInput(Seq->Tracks[TrackIndex]->TrackOpt->DeviceId);
	  cout << "[AUDIOPATTERN] Recording on input: " 
	       << Seq->Tracks[TrackIndex]->TrackOpt->DeviceId << endl;
	  return (true);
	}
      catch (...)
	{
	  cout << "[AUDIOPATTERN] Error : could not create audio file" << endl; ; // FIXME
	}
    }
  else
    cout << "[AUDIOPATTERN] Error : could not create audio file" << endl;    
  return (false);
}

void				AudioPattern::StopRecord()
{
  string			s = RecordWave->Filename;
  WaveFile			*w;
  WriteWaveFile			*recw;

  SeqMutex.Lock();
  Mix->RemoveChannel(InputChan);
  InputChan = 0x0;
  recw = RecordWave;
  RecordWave = 0x0;
  SeqMutex.Unlock();
  delete recw;
  // Affichage du wave
  w = WaveCenter.AddWaveFile(s);  
  if (w)
    {
      SetWave(w);
      Refresh();
    }
  SeqMutex.Lock();
  Wave = w;
  SeqMutex.Unlock();
}

void				AudioPattern::GetRecordBuffer()
{
  float				*f;

  if (InputChan)
    {
      int i;
      for ( i = 0; i <= InputChan->CurBuf; i++ )
	{
	  f = InputChan->PopBuffer(i);
	  if (f)
	    {
	      RecordWave->WriteFloat(f, Audio->SamplesPerBuffer);
	    }
	}
      /*
	f = InputChan->PopBuffer(i);
	if (f)
	RecordWave->WriteFloat(f, InputChan->OffSet);
      */
    }
}

Pattern				*AudioPattern::CreateCopy(double pos)
{
  Pattern			*p;

#ifdef __DEBUG__
  printf(" [ START ] AudioPattern::CreateCopy(%f) on track %d\n", pos, TrackIndex);
#endif
  p = new AudioPattern(pos, Wave, TrackIndex);
  Seq->Tracks[TrackIndex]->AddPattern(p);
  //p = new AudioPattern(pos, Wave, TrackIndex);
  //p = Seq->Tracks[TrackIndex]->AddPattern(Wave, pos);
  //printf("AudioPattern::CreateCopy(%d) new pat %d -- OVER\n", pos, p);
#ifdef __DEBUG__
  printf(" [  END  ] AudioPattern::CreateCopy(%f) on track %d\n", pos, TrackIndex);
#endif
  return (p);
}

void				AudioPattern::OnClick(wxMouseEvent &e)
{
  AudioPattern			*p;
  double			d;

  Pattern::OnClick(e);
#ifdef __DEBUG__
  cout << " >>> CLICKED :\n\t Position = " << Position << "\n\t Length = " << Length << "\n\t EndPosition = " << EndPosition;
  printf(" >>> HERE : StartWavePos %d, EndWavePos %d\n", StartWavePos, EndWavePos);
  printf(" >>> HERE : s.x %d s.y %d\n", GetSize().x, GetSize().y);
#endif
  if (SeqPanel->Tool == ID_TOOL_SPLIT_SEQUENCER)
    {
      d = (double) ((SeqPanel->CurrentXScrollPos 
		     + Pattern::GetMPosition().x + e.m_x)
		    / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor));
#ifdef __DEBUG__
      cout << " >>> HERE OLD:\n\t Position = " << Position << "\n\t Length = " << Length << "\n\t EndPosition = " << EndPosition << endl;
      cout << "new pos: " << d << endl;
#endif
      p = new AudioPattern(d, EndPosition, TrackIndex);
#ifdef __DEBUG__
      cout << " >>> HERE NEW :\n\t p->Position = " << p->Position << "\n\t p->Length = " << p->Length << "\n\t p->EndPosition = " << p->EndPosition << endl;
#endif
      p->StartWavePos = (long)(StartWavePos + (d - Position) * Seq->SamplesPerMeasure);
      p->EndWavePos = (long)(p->StartWavePos + (p->Length) * Seq->SamplesPerMeasure);
      p->SetWave(Wave);
      p->SetDrawing();
      p->Update();
      SeqMutex.Lock();
      EndWavePos = p->StartWavePos;
      Length = (EndPosition = d) - Position;
      SetDrawing();
      Update();
      SeqMutex.Unlock();
      Seq->Tracks[TrackIndex]->AddPattern(p);
    }
  else
    if (SeqPanel->Tool == ID_TOOL_PAINT_SEQUENCER)
      SetDrawColour(SeqPanel->ColorBox->GetColor());
}

void					AudioPattern::SetDrawColour(wxColour c)
{ 
  Pattern::SetDrawColour(c);
  WaveDrawer::PenColor = c;
  RedrawBitmap(GetSize());
  Refresh();
}

void					AudioPattern::OnDoubleClick(wxMouseEvent &e)
{
  Pattern::OnDoubleClick(e);
  OnClick(e);
  OptPanel->ShowWave(this);
  /*printf("audio (%d) me (%d)\n", audio, me);*/
}

void					AudioPattern::OnRightClick(wxMouseEvent &e)
{
  Pattern::OnRightClick(e);
}

void					AudioPattern::OnMotion(wxMouseEvent &e)
{
  Pattern::OnMotion(e);
}

void					AudioPattern::OnPaint(wxPaintEvent &e)
{
  wxPaintDC				dc(this);
  wxRegionIterator			region(GetUpdateRegion());

  WaveDrawer::OnPaint(dc, GetSize(), region);
  Pattern::DrawName(dc, GetSize());
}

void					AudioPattern::OnSize(wxSizeEvent &e)
{
  if (FullDraw && (Data || (Wave && !Wave->LoadedInMem)))
    {
      WaveDrawer::SetDrawing(GetSize());
      Refresh();
    }  
}

void					AudioPattern::SetSize(wxSize s)
{
  //  printf(" [ START ] AudioPattern::SetSize(wxSize s) >> [ %d ] [ %d ]\n", s.x, s.y);
  if (s == wxWindow::GetSize())
    return;
  wxWindow::SetSize(s);
  if (Data || (Wave && !Wave->LoadedInMem))
    {
      WaveDrawer::SetDrawing(s);
      Refresh();
    }
  //  printf(" [  END  ] AudioPattern::SetSize(wxSize s) >> [ %d ] [ %d ]\n", s.x, s.y);
}


/*
BEGIN_EVENT_TABLE(AudioPattern, wxWindow)
  EVT_LEFT_DOWN(AudioPattern::OnClick)
  EVT_RIGHT_DOWN(AudioPattern::OnRightClick)
  EVT_MENU(Audio_Delete, AudioPattern::OnDeleteClick)
END_EVENT_TABLE()
*/
