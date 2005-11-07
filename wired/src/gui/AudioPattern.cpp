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
#include "../xml/WiredSessionXml.h"
#include "ColoredBox.h"
#include "AudioPattern.h"
#include "HelpPanel.h"
#include "../sequencer/Track.h"
#include "../gui/SeqTrack.h"
#include "../engine/AudioEngine.h"
#include "../audio/WriteWaveFile.h"
#include "../mixer/Channel.h"

static long				audio_pattern_count = 1;
extern WiredSessionXml	*CurrentXmlSession;

AudioPattern::AudioPattern(double pos, double endpos, long trackindex)
  : Pattern(pos, endpos, trackindex),
    WaveDrawer(Pattern::GetSize())
{
	cout << "[AUDIOPATTERN] [New] 0" << endl;
  Init();
  	cout << "[AUDIOPATTERN] [New] 1" << endl;
  Wave = 0;
  	cout << "[AUDIOPATTERN] [New] 2" << endl;
  SetWave(0);
  	cout << "[AUDIOPATTERN] [New] 3" << endl;
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
  if (InputChan) delete InputChan;
  if (RecordWave) delete RecordWave;
}

void					AudioPattern::Init()
{
  wxString				s;

  Pattern::PenColor = CL_PATTERN_NORM;
  Pattern::BrushColor = CL_WAVEDRAWER_BRUSH;
  s.Printf("T%d A%d", TrackIndex + 1, audio_pattern_count++);
  Name = s.c_str();
  LastBlock = -1;  
  RecordWave = 0;
  Connect(GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnMotion);
  Connect(GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnClick);
  Connect(GetId(), wxEVT_LEFT_UP, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnLeftUp);
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

void					AudioPattern::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s("This is an Audio pattern. Double-click on it to open the Audio editor.");
      HelpWin->SetText(s);
    }
}

void					AudioPattern::Update()
{
#ifdef __DEBUG__
  printf(" [ START ] AudioPattern::Update()\n");
#endif
  Pattern::Update();
  AudioPattern::SetSize(Pattern::GetSize());
  wxWindow::SetPosition(Pattern::GetMPosition() -
			wxPoint((int) floor(SeqPanel->CurrentXScrollPos), (int) SeqPanel->CurrentYScrollPos));
#ifdef __DEBUG__
  printf(" [  END  ] AudioPattern::Update()\n");
#endif
}

void					AudioPattern::OnBpmChange()
{
  Length = Seq->MeasurePerSample * Wave->GetNumberOfFrames();
  EndPosition = Position + Length;
  Update();
}

void					AudioPattern::SetFullWave(WaveFile *w)
{
  if (!w)
    {
      StartWavePos = 0;
      EndWavePos = 0;
      NumberOfChannels = 0;
    }
  else
    FileName = w->Filename;
  WaveDrawer::SetWave(w, GetSize());
}

void					AudioPattern::SetWave(WaveFile *w)
{
#ifdef __DEBUG__
  cout << "WaveDrawer::StartWavePos = " << WaveDrawer::StartWavePos<< " WaveDrawer::EndWavePos = " << WaveDrawer::EndWavePos << endl;
#endif
  if (!w)
    {
      StartWavePos = 0;
      EndWavePos = 0;
      NumberOfChannels = 0;
    }
  else
    FileName = w->Filename;
  WaveDrawer::SetWave(w, GetSize(), StartWavePos, EndWavePos);
}

void					AudioPattern::SetDrawing()
{
  WaveDrawer::SetPenColor(Pattern::PenColor);
  WaveDrawer::SetBrushColor(Pattern::BrushColor);
  WaveDrawer::SetDrawing(GetSize());
  Refresh();
}

float					**AudioPattern::GetBlock(long block)
{
  float					**buf;
  long					size;
  long					pos;

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

void					AudioPattern::SetSelected(bool sel)
{
  Pattern::SetSelected(sel);
  if (sel)
    Pattern::BrushColor = CL_WAVEDRAWER_BRUSH_SEL;
  else
    Pattern::BrushColor = CL_WAVEDRAWER_BRUSH;
  WaveDrawer::PenColor = Pattern::PenColor;
  WaveDrawer::BrushColor = Pattern::BrushColor;
  WaveDrawer::RedrawBitmap(GetSize());
  Refresh();
}

bool					AudioPattern::PrepareRecord(int type)
{
  wxString				s;
  bool					done = false;
  int					i = 1;

  cout << "Preparing record for pattern " << this 
       << " with audio dir : " << CurrentXmlSession->GetAudioDir().c_str() << endl;
  while (!done)
    {
      s.Printf("%s/wired_audio%d.wav", CurrentXmlSession->GetAudioDir().c_str(), i);
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

void					AudioPattern::StopRecord()
{
  string				s = RecordWave->Filename;
  WaveFile				*w;
  WriteWaveFile				*recw;

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
      SetFullWave(w);
      Refresh();
    }
  /*
  SeqMutex.Lock();
  Wave = w;
  SeqMutex.Unlock();
  */
}

void					AudioPattern::GetRecordBuffer()
{
  float					*f;

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

Pattern					*AudioPattern::CreateCopy(double pos)
{
  Pattern				*p;

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

void					AudioPattern::OnClick(wxMouseEvent &e)
{
  Pattern::OnClick(e);
  if (SeqPanel->Tool == ID_TOOL_SPLIT_SEQUENCER)
    Split((double) ((Pattern::GetMPosition().x + e.m_x)
		    / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor)));
  else
    if (SeqPanel->Tool == ID_TOOL_PAINT_SEQUENCER)
      SetDrawColour(SeqPanel->ColorBox->GetColor());
}

void					AudioPattern::OnLeftUp(wxMouseEvent &e)
{
  Pattern::OnLeftUp(e);
}

void					AudioPattern::Split(double pos)
{
  AudioPattern				*p;

  if ((Position < pos) && (pos < EndPosition))
    {
      SeqMutex.Lock();
#ifdef __DEBUG__
      cout << " >>> HERE OLD:\n\t Position = " << Position << "\n\t Length = " << Length << "\n\t EndPosition = " << EndPosition << endl;
      cout << "new pos: " << pos << endl;
#endif
      p = new AudioPattern(pos, EndPosition, TrackIndex);
#ifdef __DEBUG__
      cout << " >>> HERE NEW :\n\t p->Position = " << p->Position << "\n\t p->Length = " << p->Length << "\n\t p->EndPosition = " << p->EndPosition << endl;
#endif
      p->StartWavePos = StartWavePos + (long) floor((pos - Position) * Seq->SamplesPerMeasure);
      p->EndWavePos = p->StartWavePos + (long) floor(p->Length * Seq->SamplesPerMeasure);
      p->SetDrawColour(WaveDrawer::PenColor);
      p->SetWave(Wave);
      p->SetCursor(GetCursor());
      if (IsSelected())
	p->SetSelected(false);
      p->Update();
      EndWavePos = p->StartWavePos;
      Length = (EndPosition = pos) - Position;
      SetDrawing();
      Update();
      SeqMutex.Unlock();
      Seq->Tracks[TrackIndex]->AddColoredPattern((Pattern *) p);
    }
  else
    cout << "C QUOI CE DELIRE DE POS ?? " << pos << endl;
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
  OnClick(e);
  if (SeqPanel->Tool == ID_TOOL_MOVE_SEQUENCER)
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

AudioPattern			AudioPattern::operator=(const AudioPattern& right)
{
	if (this != &right)
	{
		//TODO xdrag = right.xdrag (When used)
		//TODO ydrag = right.ydrag (When used)
		Position = right.Position;
		EndPosition = right.EndPosition;
		Length = right.Length;
		TrackIndex = right.TrackIndex;
		StateMask = right.StateMask;
		m_pos = right.m_pos;
		m_size = right.m_size;
		m_click = right.m_click;
		Name = right.Name;
		//TODO PenColor = right.PenColor;
		//TODO BrushColor = right.BrushColor;

		InputChan = right.InputChan;
		LastBlock = right.LastBlock;
		FileName = right.FileName;
		RecordWave = right.RecordWave;
	}
	return *this;
}
